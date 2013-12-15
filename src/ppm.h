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
 * file: ppm.h
 * description: Simple PPM writer.
 * author: Niklas Rosenstein <rosensteinniklas@gmail.com>
 *
 * This C header defines functions and types for writing a PMM image
 * to an output stream. */

#ifndef NIKLASROSENSTEIN_PPM
#define NIKLASROSENSTEIN_PPM

#include <stdlib.h>

/* This method type is used to write data to an object from a buffer. The
 * first parameter is the user-object as specified in the
 * :attr:`ppm_outstream_t.object` field. The scond is a buffer and the third
 * is the number of bytes that are supposed to be written. The function
 * returns the number of bytes that have actually been written to the
 * output stream. */
typedef size_t (*ppm_writemethod_t)(void*, const char*, size_t);

/* This structure contains callbacks for writing data to an output object.
 * It contains a callback that will be invoked with the data to write and
 * its user-data. */
typedef struct _ppm_outstream {
    /* The custom object that is capable to recieving data based on
     * char buffers. The :attr:`write` callback recieves this object
     * as its first parameter. */
    void* object;

    /* The callback that is able to write the char buffer to the
     * output *object*. Returns the number of bytes that have actually
     * been written. */
    ppm_writemethod_t write;
} ppm_outstream_t;

/* Invoke the :attr:`ppm_outstream_t.write` callback. */
size_t ppm_outstream_write(
        const ppm_outstream_t* outstream, const char* buffer, size_t size);

/* Printf to a ppm output stream. */
size_t ppm_outstream_printf(
        const ppm_outstream_t* outstream, const char* format, ...);

/* This is a pre-implemented callback for using the ppm_outstream_t with
 * ``FILE\*`` objects. */
size_t ppm_writemethod_file(void* fp, const char* buffer, size_t size);

/* Mode specifier. */
typedef enum PPM_MODE {
    PPM_MODE_BINARY,
    PPM_MODE_PLAIN,
} PPM_MODE;

/* Writes the header of a PPM file. */
size_t ppm_write_header(
        const ppm_outstream_t* outstream, PPM_MODE mode, uint16_t width,
        uint16_t height, uint16_t maxvalue);

/* Writes a pixel in the specified PPM mode to the outstream. */
size_t ppm_write_pixel(
        const ppm_outstream_t* outstream, PPM_MODE mode, uint16_t maxvalue,
        uint16_t r, uint16_t g, uint16_t b);

#endif /* NIKLASROSENSTEIN_PPM */