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

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "ansi_escape.h"

static int* _get_fill_values(const char* sequence, va_list* args) {
    /* Count the number of variable arguments. */
    int count = 0;
    const char* c = sequence;
    while (*c != 0) {
        if (*c++ == '#') count++;
    }

    /* Fill an array of the arguments. */
    int* array = malloc(sizeof(int) * count);
    if (array == NULL) return NULL;
    int i;
    for (i=0; i < count; i++) {
        array[i] = va_arg(*args, int);
    }

    return array;
}

static int* _get_graphics_values(const char* sequence, va_list* args) {
    /* Count the number of variable arguments. */
    int count = 0;
    const char* c = sequence;
    while (*c != 0) {
        switch (*c++) {
            case 'f':
            case 'b':
            case 'a':
                count++;
        }
    }

    /* Fill an array of the arguments. */
    int* array = malloc(sizeof(int) * count);
    if (array == NULL) return NULL;
    int i;
    for (i=0; i < count; i++) {
        array[i] = va_arg(*args, int);
    }

    return array;
}


void ansiescape_vfill(char* buffer, const char* sequence, const int* values) {
    char* b = buffer;
    const char* c = sequence;
    while (*c != 0) {
        /* Replace # (hash) signs by the specified argument (integers). */
        if (*c == '#') {
            /* Print the character to the buffer. */
            b += sprintf(b, "%d", *values++);
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

    /* Convert the variable arguments into an array of integer values. */
    va_list args;
    va_start(args, sequence);
    int* array = _get_fill_values(sequence, &args);
    va_end(args);
    if (array == NULL) return;

    /* Call the array-based function.*/
    ansiescape_vfill(buffer, sequence, array);
    free(array);
}



void ansiescape_vgraphics(char* buffer, const char* sequence, const int* values) {
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
                number = 30 + *values++;
                break;
            case 'b':
                number = 40 + *values++;
                break;
            case 'a':
                number = *values++;
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
    int* array = _get_graphics_values(sequence, &args);
    va_end(args);
    if (array == NULL) return;

    /* Call the array-based function.*/
    ansiescape_vgraphics(buffer, sequence, array);
    free(array);
}

