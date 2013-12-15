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
 * THE SOFTWARE. */

#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "gol.h"
#include "ansi_escape.h"


/* Structure that holds information about printing a Game of Life to the
 * Terminal (yeay!) */
typedef struct _gol_printer {
    ANSICOLOR color_alive;
    ANSICOLOR color_dead;
    int max_width;
    int max_height;
} gol_printer_t;

/* Print a Game of Life to the Terminal window (from the current position, the
 * cursor should at least be positioned in the first column. */
void gol_printer_print(const gol_printer_t* printer, const game_of_life_t* game) {
    /* Calculate the actual iteration range. */
    int width = game->width;
    int height = game->height;
    if (printer->max_width > 0 && printer->max_width < width) {
        width = printer->max_width;
    }
    if (printer->max_height > 0 && printer->max_height < height) {
        height = printer->max_height;
    }

    /* We don't want to flood the Terminal with characters and avoid
     * superflous characters, so we save the state of the last cell we
     * printed. if it didn't change, we don't have to change our color. */
    bool prev_state = false;
    bool prev_error = false;

    /* Iterate over each cell, lines first. */
    int i, j;
    for (j=0; j < height; j++) {
        for (i=0; i < width; i++) {
            /* Retrieve the current cell. */
            cell_t* cell = game_of_life_cell(game, i, j);

            if (cell == NULL) {
                prev_error = true;
                ansiescape_setgraphics("b", ANSICOLOR_RED);
            }
            else if (cell->state != prev_state || (i == 0 && j == 0) || prev_error) {
                ANSICOLOR color = (cell->state ? printer->color_alive : printer->color_dead);
                ansiescape_setgraphics("b", color);
            }
            prev_state = cell->state;
            prev_error = false;

            printf(" ");
        }
        printf("\n\r");
    }

    ansiescape_setgraphics("");
}


int main() {
    /* Retrieve the width and height of the Terminal. */
    int width, height;
    if (!ansiescape_winsize(&height, &width)) {
        fprintf(stderr, "Could not retrieve Terminal size.\n");
        return -1;
    }

    if (width < 40) width = 40;
    if (height < 20) height = 20;
    height-=2;

    /* Create a new Game of Life. */
    game_of_life_t* game = game_of_life_create(width, height, true);
    if (!game) {
        fprintf(stderr, "Game of Life could not be allocated.\n");
        return -1;
    }

    game_of_life_draw_glider(game, 10, 10, false, true);
    game_of_life_draw_lwss(game, 30, 10, 'r');

    /* Create a printer. */
    gol_printer_t printer;
    printer.color_alive = ANSICOLOR_BLUE;
    printer.color_dead = ANSICOLOR_WHITE;

    int i;
    bool running = true;
    while (running) {
        ansiescape_winsize(&height, &width);
        if (height > 2) height -= 2;

        printer.max_width = width;

        ansiescape_setcursor(0, 0);
        gol_printer_print(&printer, game);
        printf("Generation: %llu\n", game->generation);
        game_of_life_next_generation(game);
        usleep(50 * 1000);
    }

    game_of_life_destroy(game);
    game = NULL;
    return 0;
}

