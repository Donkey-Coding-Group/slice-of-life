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
 * file: ansi_escape.h
 * description: ANSI Escape Sequence library
 * author: Niklas Rosenstein <rosensteinniklas@gmail.com>
 *
 * This C header defines macros and functions for working with ANSI Escape
 * Sequences. Some of the Escape Sequences require parameters which can be
 * inserted using the :func:`ansiescape_fill(char*, const char*, ...)`
 * function. Parameters in a ANSI Escape Sequence are denored by # (hash)
 * signs. */

#ifndef NIKLASROSENSTEIN_ANSI_ESCAPE
#define NIKLASROSENSTEIN_ANSI_ESCAPE

#include <stdarg.h>
#include <stdbool.h>

/* Moves the cursor to the specified position (coordinates). Requires two
 * parameters (line and column). */
#define ANSIESCAPE_CURSOR_POSITION              "\033[#;#H"

/* Moves the cursor up by the specified number of lines without changing
 * columns. If the cursor is already on the top line, ANSI.SYS ignores this
 * sequence. */
#define ANSIESCAPE_CURSOR_UP                    "\033[#A"

/* Moves the cursor down by the specified number of lines without changing
 * columns. If the cursor is already on the bottom line, ANSI.SYS ignores
 * this sequence. */
#define ANSIESCAPE_CURSOR_DOWN                  "\033[#B;"

/* Moves the cursor forward by the specified number of columns without
 * changing lines. If the cursor is already in the rightmost column, ANSI.SYS
 * ignores this sequence. */
#define ANSIESCAPE_CURSOR_FORWARD               "\033[#C;"

/* Moves the cursor back by the specified number of columns without
 * changing lines. If the cursor is already in the leftmost column, ANSI.SYS
 * ignores this sequence. */
#define ANSIESCAPE_CURSOR_BACKWARD              "\033[#D;"

/* Saves the current cursor position. You can move the cursor to the
 * saved cursor position by using the Restore Cursor Position sequence.
 * See :macro:`ANSIESCAPE_CURSOR_RESTORE`. */
#define ANSIESCAPE_CURSOR_SAVE                  "\033[s"

/* Returns the cursor to the position stored by the Save Cursor
 * Position sequence. See :macro:`ANSIESCAPE_CURSOR_SAVE`. */
#define ANSIESCAPE_CURSOR_RESTORE               "\033[u"

/* Clears the screen and moves the cursor to the home position
 * (line 0, column 0). */
#define ANSIESCAPE_ERASE                        "\033[2J\033[0;0H"

/* Clears all characters from the cursor position to the end of the line
 * (including the character at the cursor position). */
#define ANSIESCAPE_ERASE_LINE                   "\033[K"

/* Changes the screen width or type to the mode specified by one of the
 * following values:
 *
 * - ANSIMODE_40_25_MONO
 * - ANSIMODE_20_25_COLOR
 * - ANSIMODE_80_25_MONO
 * - ANSIMODE_80_25_COLOR
 * - ANSIMODE_320_200_COLOR (4-color)
 * - ANSIMODE_230_200_MONO
 * - ANSIMODE_640_200_MONO
 * - ANSIMODE_LINEWRAPPING
 * - ANSIMODE_320_200_COLOR
 * - ANSIMODE_640_200_COLOR (16-color)
 * - ANSIMODE_640_350_MONO
 * - ANSIMODE_640_350_COLOR (16-color)
 * - ANSIMODE_640_480_MONO
 * - ANSIMODE_640_480_COLOR (16-color)
 * - ANSIMODE_320_200_COLOR (256-color)
 */
#define ANSIESCAPE_MODE_SET                     "\033[=#h"

/* Resets the mode by using the same values that Set Mode uses, except for
 * 7, which disables line wrapping (the last character in this escape sequence
 * is a lowercase L). */
#define ANSIESCAPE_MODE_RESET                   "\033[=#l"

/* Resets the applied graphics settings. */
#define ANSIESCAPE_GRAPHICS_RESET               "\033[m"

/* Dislay modes for :macro:`ANSIESCAPE_MODE_SET` and
 * :macro:`ANSIESCAPE_MODE_RESET`. */
typedef enum ANSIMODE {
    ANSIMODE_40_25_MONO = 0,
    ANSIMODE_20_25_COLOR = 1,
    ANSIMODE_80_25_MONO = 2,
    ANSIMODE_80_25_COLOR = 3,
    ANSIMODE_320_200_4COLOR = 4,
    ANSIMODE_230_200_MONO = 5,
    ANSIMODE_640_200_MONO = 6,
    ANSIMODE_LINEWRAPPING = 7,
    ANSIMODE_320_200_COLOR = 13,
    ANSIMODE_640_200_COLOR = 14,
    ANSIMODE_640_350_MONO = 15,
    ANSIMODE_640_350_COLOR = 16,
    ANSIMODE_640_480_MONO = 17,
    ANSIMODE_640_480_COLOR = 18,
    ANSIMODE_320_200_256COLOR = 19,
} ANSIMODE;

/* Attributes for the ANSI Graphics Mode. */
typedef enum ANSIATTR {
    ANSIATTR_NONE = 0,
    ANSIATTR_BOLD = 1,
    ANSIATTR_UNDERLINE = 4,
    ANSIATTR_BLINK = 5,
    ANSIATTR_REVERSEVIDEO = 6,
    ANSIATTR_CONCEALED = 7,
} ANSIATTR;

/* Colors for the ANSI Graphics Mode. */
typedef enum ANSICOLOR {
    ANSICOLOR_NONE = -1,
    ANSICOLOR_BLACK = 0,
    ANSICOLOR_RED = 1,
    ANSICOLOR_GREEN = 2,
    ANSICOLOR_YELLOW = 3,
    ANSICOLOR_BLUE = 4,
    ANSICOLOR_MAGENTA = 5,
    ANSICOLOR_CYAN = 6,
    ANSICOLOR_WHITE = 7,
} ANSICOLOR;

/* Parse and substitute an ANSI Escape Sequence Macro. # (hash) signs are
 * replaced by the passed values and filled into the buffer. */
void ansiescape_fill(char* buffer, const char* sequence, ...);
void ansiescape_vfill(char* buffer, const char* sequence, va_list args);

/* The ANSI Escape Sequence for specifying the Graphics Mode accepts a
 * variable number of arguments and is therefore treated specialized with
 * its own function instead of a macro. The *sequence* string must contain
 * the following characters only: f, b and a. *f* stands for foreground color,
 * *b* for the background color and *a* for an attribute. Multiple *a*s may
 * contained in the sequence. Invalid characters are simply ignored. The
 * specified variable arguments must match the *sequence* string. */
void ansiescape_graphics(char* buffer, const char* sequence, ...);
void ansiescape_vgraphics(char* buffer, const char* sequence, va_list args);

/* Print the specified graphics to stdout. */
void ansiescape_setgraphics(const char* sequence, ...);

/* Set the cursor position to the specified column and row. */
void ansiescape_setcursor(int line, int column);

/* Returns the size of the Terminal Window (in columns and lines). True on
 * success, false on failure. */
bool ansiescape_winsize(int* width, int* height);

/* Clears the Terminal window, but different from :macro:`ANSIESCAPE_ERASE`.
 * Instead of just skipping the characters that remain, the current view
 * is overwritten. */
void ansiescape_clear();

#endif /* NIKLASROSENSTEIN_ANSI_ESCAPE */