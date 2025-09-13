#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "GameState.h"
#include "util.h"

constexpr size_t DEFAULT_START_LENGTH = 2;

void drawHeader(GameState *gs);
bool keyboardHit();
void enableRawMode(GameState *gs);
void disableRawMode(GameState *gs);
void spawnFood(Point *food);
void draw(GameState *gs);
void tick(GameState *gs);
void initGameState(GameState *gs);

// ----------------------------------------------------------------------------
int main(void) {
    srand(time(nullptr)); // NOLINT(*-msc51-cpp)

    GameState gs;
    initGameState(&gs);

    enableRawMode(&gs);
    while (isActive(&gs)) {
        if (keyboardHit()) {
            char key;
            read(STDIN_FILENO, &key, 1);
            const Direction dir = getDirection(&gs);
            if (key == 'w' && dir != DOWN) {
                setDirection(&gs, UP);
            } else if (key == 's' && dir != UP) {
                setDirection(&gs, DOWN);
            } else if (key == 'a' && dir != RIGHT) {
                setDirection(&gs, LEFT);
            } else if (key == 'd' && dir != LEFT) {
                setDirection(&gs, RIGHT);
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
    food->x = rand() % WIDTH; // NOLINT(*-narrowing-conversions)
    food->y = rand() % HEIGHT; // NOLINT(*-narrowing-conversions)
}

void drawHeader(GameState *gs) {
    // Print header
    constexpr char scoreMsg[] = "Score: ";
    for (size_t i = 0; scoreMsg[i] != '\0'; i++) {
        putchar(scoreMsg[i]);
    }
    putNumber(getPoints(gs));
    putchar('\n');

    // Print seperator
    for (size_t i = 0; i < WIDTH; i++) {
        printf("=");
    }
    putchar('\n');
}

void draw(GameState *gs) {
    if (!gs->isActive) {
        return;
    }

   system("clear");

    drawHeader(gs);

    // Print game field
    for (size_t y = 0; y < HEIGHT; y++) {
        for (size_t x = 0; x < WIDTH; x++) {
            bool printed = false;

            const Point *food = getFoodPosition(gs);
            if (food->x == x && food->y == y) {
                putchar('F');
                continue;
            }

            const size_t snakeLength = getSnakeLength(gs);
            for (size_t i = 0; i < snakeLength; i++) {
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
    const size_t snakeLength = getSnakeLength(gs);
    for (size_t i = 0; i < snakeLength; i++) {
        if (gs->snake[i].x == newHead.x && gs->snake[i].y == newHead.y) {
            setIsActive(gs,false);
            return;
        }
    }

    // Shift body
    for (size_t i = snakeLength; 0 < i; i--) {
        gs->snake[i] = gs->snake[i - 1];
    }
    gs->snake[0] = newHead;

    // Check for food
    const Point *food = getFoodPosition(gs);
    if (newHead.y == food->y && newHead.x == food->x) {
        setSnakeLength(gs, getSnakeLength(gs) + 1);
        setPoints(gs, getPoints(gs) + 1);
        spawnFood(&gs->food);
    }
}

void initGameState(GameState *gs) {
    gs->isActive = true;

    spawnFood(&gs->food);

    setDirection(gs, RIGHT);

    setSnakeLength(gs, DEFAULT_START_LENGTH);
    setPoints(gs, 0);

    for (size_t i = 0; i < gs->snakeLength; i++) {
        gs->snake[i] = (Point){WIDTH / 2 - i, HEIGHT / 2};
    }
}

bool keyboardHit() {
    fd_set set;
    struct timeval tv = {0};
    FD_ZERO(&set);
    FD_SET(STDIN_FILENO, &set);
    return select(STDIN_FILENO + 1, &set, nullptr, nullptr, &tv) == 1;
}

void enableRawMode(GameState *gs) {
    tcgetattr(STDIN_FILENO, &gs->origTerm);
    struct termios rawTerm = gs->origTerm;
    rawTerm.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &rawTerm);
}

void disableRawMode(GameState *gs) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &gs->origTerm);
}