#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <time.h>

#include "util.h"

/*
 * Horizontal - Width  - X
 * Vertical   - Height - Y
 */
constexpr size_t WIDTH = 60;
constexpr size_t HEIGHT = 28;

constexpr size_t DEFAULT_START_LENGTH = 2;

typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction;

typedef struct {
    int x;
    int y;
} Point;

typedef struct {
    bool isActive;
    Point snake[WIDTH * HEIGHT];
    Point food;
    size_t snakeLength;
    Direction direction;
    struct termios origTerm;
} GameState;

void drawHeader(const GameState *gs);
bool keyboardHit();
void enableRawMode(GameState *gs);
void disableRawMode(GameState *gs);
void spawnFood(Point *food);
void draw(const GameState *gs);
void tick(GameState *gs);
void initGameState(GameState *gs);

// ----------------------------------------------------------------------------
int main(void) {
    srand(time(0));

    GameState gs;
    initGameState(&gs);

    enableRawMode(&gs);
    while (gs.isActive) {
        if (keyboardHit()) {
            char key;
            read(STDIN_FILENO, &key, 1);

            if (key == 'w' && gs.direction != DOWN) {
                gs.direction = UP;
            } else if (key == 's' && gs.direction != UP) {
                gs.direction = DOWN;
            } else if (key == 'a' && gs.direction != RIGHT) {
                gs.direction = LEFT;
            } else if (key == 'd' && gs.direction != LEFT) {
                gs.direction = RIGHT;
            }
        }
        tick(&gs);
        draw(&gs);
        usleep(60'000);
    }
    disableRawMode(&gs);
    return 0;
}

// ----------------------------------------------------------------------------
void spawnFood(Point *food) {
    food->x = rand() % WIDTH;
    food->y = rand() % HEIGHT;
}

void drawHeader(const GameState *gs) {
    // Print header
    constexpr char scoreMsg[] = "Score: ";
    for (size_t i = 0; scoreMsg[i] != '\0'; i++) {
        putchar(scoreMsg[i]);
    }
    printNumber(gs->snakeLength);
    putchar('\n');

    // Print seperator
    for (size_t i = 0; i < WIDTH; i++) {
        printf("=");
    }
}

void draw(const GameState *gs) {
    system("clear");

    drawHeader(gs);

    // Print game field
    for (size_t y = 0; y < HEIGHT; y++) {
        for (size_t x = 0; x < WIDTH; x++) {
            bool printed = false;

            if (gs->food.x == x && gs->food.y == y) {
                putchar('F');
                continue;
            }

            for (size_t i = 0; i < gs->snakeLength; i++) {
                if (gs->snake[i].x == x && gs->snake[i].y == y) {
                    printed = true;
                    putchar(i == 0 ? '@' : 'o');
                    break;
                }
            }
            if (!printed) {
                putchar(' ');
            }
        }
        putchar('\n');
    }
}

void tick(GameState *gs) {
    Point newHead = gs->snake[0];
    switch (gs->direction) {
        case UP: newHead.y--; break;
        case DOWN: newHead.y++; break;
        case LEFT: newHead.x--; break;
        case RIGHT: newHead.x++; break;
    }

    // Wrap head. I.e. Move from one screen edge to the other
    if (newHead.x < 0) {
        newHead.x = WIDTH - 1;
    }
    if (newHead.x >= WIDTH) {
        newHead.x = 0;
    }
    if (newHead.y < 0) {
        newHead.y = HEIGHT - 1;
    }
    if (newHead.y >= HEIGHT) {
        newHead.y = 0;
    }

    // Check for collision with itself
    for (size_t i = 0; i < gs->snakeLength; i++) {
        if (gs->snake[i].x == newHead.x && gs->snake[i].y == newHead.y) {
            gs->isActive = false;
            return;
        }
    }

    // Shift body
    for (size_t i = gs->snakeLength; 0 < i; i--) {
        gs->snake[i] = gs->snake[i - 1];
    }
    gs->snake[0] = newHead;

    // Check for food
    if (newHead.y == gs->food.y && newHead.x == gs->food.x) {
        gs->snakeLength++;
        spawnFood(&gs->food);
    }
}

void initGameState(GameState *gs) {
    gs->isActive = true;

    spawnFood(&gs->food);

    gs->direction = RIGHT;

    gs->snakeLength = DEFAULT_START_LENGTH;

    for (size_t i = 0; i < gs->snakeLength; i++) {
        gs->snake[i] = (Point){WIDTH / 2 - i, HEIGHT / 2};
    }
}

bool keyboardHit() {
    fd_set set;
    struct timeval tv = {0};
    FD_ZERO(&set);
    FD_SET(STDIN_FILENO, &set);
    return select(STDIN_FILENO + 1, &set, nullptr, 0, &tv) == 1;
}

void enableRawMode(GameState *gs) {
    tcgetattr(STDIN_FILENO, &gs->origTerm);
    atexit((void (*))disableRawMode);
    struct termios rawTerm = gs->origTerm;
    rawTerm.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &rawTerm);
}

void disableRawMode(GameState *gs) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &gs->origTerm);
}