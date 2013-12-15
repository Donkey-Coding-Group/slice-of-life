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
 * file: gol.c
 * description: Simple implementation of Conway's Game of Life
 * author: Niklas Rosenstein <rosensteinniklas@gmail.com> */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gol.h"


/* This utility function implements a cyclic modular calculation. This means,
 * for instance, that if a value of -1 is passed for *x*, the result will
 * be ``v - 1``. */
static int _casemod(int x, int v) {
    int r = abs(x % v);
    if (x < 0) r = v - r;
    return r;
}


game_of_life_t* game_of_life_create(
        uint32_t width, uint32_t height, bool adjacency) {
    /* Validate the parameters. */
    if (width < 1 || height < 1) return NULL;

    /* Allocate the Game of Life structure. */
    game_of_life_t* game = malloc(sizeof(game_of_life_t));
    if (game == NULL) {
        return NULL;
    }

    /* Allocate the array of the Cells. */
    size_t cells_size = sizeof(cell_t) * width * height;
    cell_t* cells = malloc(cells_size);
    if (cells == NULL) {
        free(game);
        return NULL;
    }

    /* Initialize all cells to zero. */
    memset(cells, cells_size, 0);

    game->width = width;
    game->height = height;
    game->generation = 0;
    game->cells = cells;
    game->adjacency = adjacency;

    return game;
}

void game_of_life_destroy(game_of_life_t* game) {
    if (game) {
        if (game->cells) free(game->cells);
        game->cells = NULL;
        free(game);
    }
}

cell_t* game_of_life_cell(const game_of_life_t* game, int32_t x, int32_t y) {
    if (game->adjacency) {
        x = _casemod(x, game->width);
        y = _casemod(y, game->height);
    }
    else if (x < 0 || y < 0 || x >= game->width || y >= game->height) {
        return NULL;
    }
    return &game->cells[x + y * game->width];
}

void game_of_life_cell_set(
        const game_of_life_t* game, int32_t x, int32_t y, bool state) {
    cell_t* cell = game_of_life_cell(game, x, y);
    if (cell) {
        cell->state = state;
    }
}

/* This utility function returns true if the cell at the specified cell
 * exists (ie. the index is not out of the grid's bounds) and is alive,
 * false in any other case. */
static bool game_of_life_cell_state(
        const game_of_life_t* game, int32_t x, int32_t y) {
    cell_t* cell = game_of_life_cell(game, x, y);
    if (cell) return cell->state;
    else return 0;
}

int game_of_life_neighbour_count(
        const game_of_life_t* game, int32_t x, int32_t y) {
    int count = 0;
    if (game_of_life_cell_state(game, x - 1, y - 1)) count++;
    if (game_of_life_cell_state(game, x    , y - 1)) count++;
    if (game_of_life_cell_state(game, x + 1, y - 1)) count++;
    if (game_of_life_cell_state(game, x + 1, y    )) count++;
    if (game_of_life_cell_state(game, x + 1, y + 1)) count++;
    if (game_of_life_cell_state(game, x    , y + 1)) count++;
    if (game_of_life_cell_state(game, x - 1, y + 1)) count++;
    if (game_of_life_cell_state(game, x - 1, y    )) count++;
    return count;
}

void game_of_life_next_generation(game_of_life_t* game) {
    game->generation++;
    int i, j;

    /* Iterate over the complete game cell and fill the "prev_state"
     * attribute of each cell temporarily. This value will be moved
     * to the "current" attribute later on. */
    for (i=0; i < game->width; i++) {
        for (j=0; j < game->height; j++) {
            cell_t* cell = game_of_life_cell(game, i, j);

            int neighbours = game_of_life_neighbour_count(game, i, j);
            bool new_state;
            if (cell->state) {
                new_state = (neighbours == 2 || neighbours == 3);
            }
            else {
                new_state = (neighbours == 3);
            }

            /* Temporarily fill the "prev_state" attribute. */
            cell->prev_state = new_state;
        }
    }

    /* Update the states of each cell. */
    for (i=0; i < game->width; i++) {
        for (j=0; j < game->height; j++) {
            cell_t* cell = game_of_life_cell(game, i, j);

            bool value = cell->prev_state;
            cell->prev_state = cell->state;
            cell->state = value;
        }
    }
}

void game_of_life_draw_block(
        const game_of_life_t* game, int32_t x, int32_t y, int32_t w, int32_t h,
        bool state) {
    int i, j;
    for (i=0; i < w; i++) {
        for (j=0; j < h; j++) {
            game_of_life_cell_set(game, x + i, y + j, state);
        }
    }
}

void game_of_life_draw_pattern(
        const game_of_life_t* game, const char* pattern, int32_t x, int32_t y,
        int32_t w, int32_t h, GOL_ROT rotation, GOL_FLIP flip, bool reset) {
    int i, j;
    for (i=0; i < w; i++) {
        for (j=0; j < h; j++) {
            int pi = i;
            int pj = j;
            int gi = i;
            int gj = j;

            switch (rotation) {
                case GOL_ROT_270:
                    gi = pj;
                    gj = w - 1 - pi;
                    break;
                case GOL_ROT_180:
                    pi = w - 1 - pi;
                    pj = h - 1 - pj;
                    break;
                case GOL_ROT_90:
                    gi = h - 1 - pj;
                    gj = pi;
                    break;
                case GOL_ROT_0:
                default:
                    break;
            }

            if (flip & GOL_FLIP_H) pi = w - 1 - pi;
            if (flip & GOL_FLIP_V) pj = h - 1 - pj;

            char c = pattern[pi + pj * w];
            bool living = (c == 'x' || c == 'X');
            if (!reset && !living) continue;
            game_of_life_cell_set(game, x + gi, y + gj, living);
        }
    }
}

void game_of_life_draw_glider(
            const game_of_life_t* game, int32_t x, int32_t y,
            bool flip_h, bool flip_v) {
    game_of_life_draw_block(game, x, y, 4, 3, false);

    int h = (flip_h ? -1 : 1);
    int v = (flip_v ? -1 : 1);

    /* Draw the Glider pattern. */
    game_of_life_cell_set(game, x + 1 * h, y        , 1);
    game_of_life_cell_set(game, x + 2 * h, y + 1 * v, 1);
    game_of_life_cell_set(game, x + 2 * h, y + 2 * v, 1);
    game_of_life_cell_set(game, x + 1 * h, y + 2 * v, 1);
    game_of_life_cell_set(game, x        , y + 2 * v, 1);
}

void game_of_life_draw_lwss(
        const game_of_life_t* game, int32_t x, int32_t y, GOL_ROT rotation,
        GOL_FLIP flip) {
    static const int w = 5;
    static const int h = 4;
    static const char pattern[] =
            " XXXX"
            "X   X"
            "    X"
            "X  X";

    game_of_life_draw_pattern(game, pattern, x, y, w, h, rotation, flip, true);
}

