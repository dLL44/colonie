#ifndef COLOR_H
#define COLOR_H

#include <stdio.h>

// Reset
#define CCOLOR_RESET "\x1B[0m"

// Regular Colors
#define CCOLOR_RED "\x1B[31m"
#define CCOLOR_GREEN "\x1B[32m"
#define CCOLOR_YELLOW "\x1B[33m"
#define CCOLOR_BLUE "\x1B[34m"
#define CCOLOR_MAGENTA "\x1B[35m"
#define CCOLOR_CYAN "\x1B[36m"
#define CCOLOR_WHITE "\x1B[37m"

// Background Colors
#define CBG_RED "\x1B[41m"
#define CBG_GREEN "\x1B[42m"
#define CBG_YELLOW "\x1B[43m"
#define CBG_BLUE "\x1B[44m"
#define CBG_MAGENTA "\x1B[45m"
#define CBG_CYAN "\x1B[46m"
#define CBG_WHITE "\x1B[47m"

// Bold Colors
#define CBOLD_RED "\x1B[1;31m"
#define CBOLD_GREEN "\x1B[1;32m"
#define CBOLD_YELLOW "\x1B[1;33m"
#define CBOLD_BLUE "\x1B[1;34m"
#define CBOLD_MAGENTA "\x1B[1;35m"
#define CBOLD_CYAN "\x1B[1;36m"
#define CBOLD_WHITE "\x1B[1;37m"

// Function to change color
void setColor(const char *colorCode)
{
    printf("%s", colorCode);
}

#endif