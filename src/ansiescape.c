/* Copyright (c) 2013  Niklas Rosenstein
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * file: ansi_escape.c
 * description: ANSI Escape Sequence library
 * author: Niklas Rosenstein <rosensteinniklas@gmail.com> */

/* Unix system library */
#include <sys/ioctl.h>
#include <unistd.h>

/* C standard library */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ansiescape.h"

void ansiescape_vfill(char* buffer, const char* sequence, va_list args) {
    char* b = buffer;
    const char* c = sequence;
    while (*c != 0) {
        /* Replace # (hash) signs by the specified argument (integers). */
        if (*c == '#') {
            /* Print the character to the buffer. */
            b += sprintf(b, "%d", va_arg(args, int));
        }
        else {
            /* Otherwise, write the current value of the sequence into the
             * buffer. */
            *b++ = *c;
        }
        /* Increase the character of the buffer. */
        c++;
    }

    /* Null-terminator. */
    *b = 0;
}

void ansiescape_fill(char* buffer, const char* sequence, ...) {
    *buffer = 0;

    va_list args;
    va_start(args, sequence);

    /* Call the array-based function.*/
    ansiescape_vfill(buffer, sequence, args);
    va_end(args);
}



void ansiescape_vgraphics(char* buffer, const char* sequence, va_list args) {
    /* Copy the first parts of the ANSI Escape Code. */
    char* b = buffer;
    memcpy(b, "\033[", 2);
    b += 2;

    int count = 0;
    const char* c = sequence;
    while (*c != 0) {
        /* Decide which number to put into the buffer. If the number stays
         * negative, we won't do anything. */
        int number = -1;
        switch (*c++) {
            case 'f':
                number = 30 + va_arg(args, int);
                break;
            case 'b':
                number = 40 + va_arg(args, int);
                break;
            case 'a':
                number = va_arg(args, int);
                break;
        }

        /* Print the number into the buffer. */
        if (number >= 0) {
            count++;
            b += sprintf(b, "%d;", number);
        }
    }

    /* If there is no semicolon, we'll increase the buffer pointer. */
    if (count > 0) b--;
    *b++ = 'm';
    *b = 0;
}

void ansiescape_graphics(char* buffer, const char* sequence, ...) {
    *buffer = 0;

    /* Convert the variable arguments into an array of integer values. */
    va_list args;
    va_start(args, sequence);

    /* Call the array-based function.*/
    ansiescape_vgraphics(buffer, sequence, args);
    va_end(args);
}


void ansiescape_setgraphics(const char* sequence, ...) {
    va_list args;
    va_start(args, sequence);

    char buffer[80];
    ansiescape_vgraphics(buffer, sequence, args);
    va_end(args);

    printf("%s", buffer);
}

void ansiescape_setcursor(int line, int column) {
    char buffer[20];
    ansiescape_fill(buffer, ANSIESCAPE_CURSOR_POSITION, line, column);
    printf("%s", buffer);
}

bool ansiescape_winsize(int* rows, int* columns) {
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) { /* no error */
        if (rows)    *rows = w.ws_row;
        if (columns) *columns = w.ws_col;
        return true;
    }
    return false;
}

void ansiescape_clear() {
    int i, j;
    int rows, columns;
    if (!ansiescape_winsize(&rows, &columns)) return;
    if (rows > 1) rows-=1; /* TOOD: Is this on every Terminal ok? */

    ansiescape_setgraphics("");
    ansiescape_setcursor(0, 0);
    for (i=0; i < rows; i++) {
        for (j=0; j < columns; j++) {
            printf(" ");
        }
        printf("\n\r");
    }
    ansiescape_setcursor(0, 0);
}



