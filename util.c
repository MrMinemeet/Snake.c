#include "util.h"

#include <stdio.h>

void printNumber(int number) {
    if (number == 0) {
        putchar('0');
        return;
    }
    if (number < 0) {
        putchar('-');
        number = -number;
    }
    if (number >= 10) {
        printNumber(number / 10);
    }
    putchar((number % 10) + '0');
}
