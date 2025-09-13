#include "GameState.h"

bool isActive(GameState *gs) {
    return gs->isActive;
}

void setIsActive(GameState *gs, const bool newIsActive) {
    gs->isActive = newIsActive;
}

Point *getSnakePosition(GameState *gs) {
    return gs->snake;
}

size_t getSnakeLength(GameState *gs) {
    return gs->snakeLength;
}

void setSnakeLength(GameState *gs, const size_t length) {
    gs->snakeLength = length;
}

Point *getFoodPosition(GameState *gs) {
    return &gs->food;
}

size_t getPoints(GameState *gs) {
    return gs->points;
}

void setPoints(GameState *gs, const size_t newPoints) {
    gs->points = newPoints;
}

void setFoodPosition(GameState *gs, const Point newFoodPosition) {
    gs->food = newFoodPosition;
}

Direction getDirection(const GameState *gs) {
    return gs->direction;
}

void setDirection(GameState *gs, const Direction direction) {
    gs->direction = direction;
}