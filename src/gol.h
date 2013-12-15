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
 * file: gol.h
 * description: Simple implementation of Conway's Game of Life
 * author: Niklas Rosenstein <rosensteinniklas@gmail.com>
 *
 * This C header defines types and functions for creating and processing
 * Conway's Game of Life. */

#ifndef NIKLASROSENSTEIN_GAME_OF_LIFE
#define NIKLASROSENSTEIN_GAME_OF_LIFE

#include <stdint.h>
#include <stdbool.h>

/* This structure represents a cell in a Game of Life grid. It has two
 * members representing the current state of the cell and the previous
 * state of the cell. During calculation of the next generation, the
 * member "prev_state" is used to temporarily store the new state of the
 * cell. */
typedef struct _cell {
    bool state;
    bool prev_state;
} cell_t;

/* This structure represents a session of the Game of Life. */
typedef struct _game_of_life {
    /* The width and height of the grid. */
    uint32_t width;
    uint32_t height;

    /* The number of generations that have been passed since the creation
     * of the game. */
    uint64_t generation;

    /* A linear array of the 2D grid of cells. */
    cell_t* cells;

    /* This field defines whether the boundaries of the field are directly
     * adjacent to their opposite edges and corners. */
    bool adjacency;
} game_of_life_t;

/* Create a new Game of Life from the specified parameters. Returns NULL
 * if memory allocation failed or the parameters have invalid values. The
 * specified width and height must both have at least a value of 1. */
game_of_life_t* game_of_life_create(
        uint32_t width, uint32_t height, bool adjacency);

/* Destroy a Game of Life created with :meth:`game_of_life_create`. */
void game_of_life_destroy(game_of_life_t* game);

/* Access a Cell at the specified X and Y coordinate. If the game's
 * :attr:`game_of_life_t.adjacency` attribute is true, the indecies
 * may exceed or underpass the size of the grid. If otherwise they are out
 * of the grid's bounds, NULL is returned. */
cell_t* game_of_life_cell(const game_of_life_t* game, int32_t x, int32_t y);

/* Set the state of a Cell. Nothing happens if the specified cell does not
 * exist in the grid. */
void game_of_life_cell_set(
        const game_of_life_t* game, int32_t x, int32_t y, bool state);

/* Returns the number of living Cells around the specified cell. */
int game_of_life_neighbour_count(
        const game_of_life_t* game, int32_t x, int32_t y);

/* Bring the Game of Life into its next generation. */
void game_of_life_next_generation(game_of_life_t* game);

/* Draws a block with the specified dimension and state into the
 * game's grid. */
void game_of_life_draw_block(
        const game_of_life_t* game, int32_t x, int32_t y, int32_t w, int32_t h,
        bool state);

/* Draws Glider at the specified position (top-left corner) into the game's
 * grid. A Glider has a dimension of 4 columns and 3 rows. The default
 * orientation is to the bottom left. Use the *flip_h* and *flip_v* parameters
 * to change its heading.
 *
 * Initial pattern:
 *
 *        X
 *         X
 *       XXX
 */
void game_of_life_draw_glider(
            const game_of_life_t* game, int32_t x, int32_t y,
            bool flip_h, bool flip_v);

/* Draws a Lightweis Spaceship (LWSS) at the specified position (top-left
 * corner). A LWSS has a dimension of 5 columsna and 4 rows. The direction
 * parameter can be either 'l' for left, 'r' for right, 'u' and 't' for upwards
 * and 'd' and 'b' for downwards. If an unrecognized value is passed, right
 * heading is used.
 *
 * Inital pattern:
 *
 *     XXXX
 *    X   X
 *        X
 *    X  X
 */
void game_of_life_draw_lwss(
        const game_of_life_t* game, int32_t x, int32_t y, char direction);

#endif /* NIKLASROSENSTEIN_GAME_OF_LIFE */