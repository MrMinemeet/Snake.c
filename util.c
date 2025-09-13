#include "util.h"

#include <stdio.h>

void putNumber(int number) {
    if (number == 0) {
        putchar('0');
        return;
    }
    if (number < 0) {
        putchar('-');
        number = -number;
    }
    if (number >= 10) {
        putNumber(number / 10);
    }
    putchar((number % 10) + '0');
}
