#pragma once
#include <stddef.h>
#include <termios.h>


/*
 * Horizontal - Width  - X
 * Vertical   - Height - Y
 */
constexpr size_t WIDTH = 60;
constexpr size_t HEIGHT = 20;

typedef struct {
    int x;
    int y;
} Point;

typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction;

typedef struct {
    bool isActive;
    Point snake[WIDTH * HEIGHT];
    Point food;
    size_t snakeLength;
    size_t points;
    Direction direction;
    struct termios origTerm;
} GameState;

// ----------------------------------------------------------------------------
bool isActive(GameState *gs);
void setIsActive(GameState *gs, bool newIsActive);

Point *getSnakePosition(GameState *gs);
size_t getSnakeLength(GameState *gs);
void setSnakeLength(GameState *gs, size_t length);

size_t getPoints(GameState *gs);
void setPoints(GameState *gs, size_t newPoints);

Point *getFoodPosition(GameState *gs);
void setFoodPosition(GameState *gs, Point newFoodPosition);

Direction getDirection(const GameState *gs);
void setDirection(GameState *gs, Direction direction);
