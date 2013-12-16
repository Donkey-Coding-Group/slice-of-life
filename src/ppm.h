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


typedef struct _ppm_outstream ppm_outstream_t;

/* This method type is used to write data to an object from a buffer. The
 * first parameter is the user-object as specified in the
 * :attr:`ppm_outstream_t.object` field. The scond is a buffer and the third
 * is the number of bytes that are supposed to be written. The function
 * returns the number of bytes that have actually been written to the
 * output stream. */
typedef size_t (*ppm_outstream_write_t)(const ppm_outstream_t*, const char*, size_t);

/* Called when a PPM Outstream is being destroyed. */
typedef void (*pmm_outstream_on_destroy_t)(ppm_outstream_t*);

/* This structure contains callbacks for writing data to an output object.
 * It contains a callback that will be invoked with the data to write and
 * its user-data. */
struct _ppm_outstream {
    /* The custom object that is capable to recieving data based on
     * char buffers. The :attr:`write` callback recieves this object
     * as its first parameter. */
    void* object;

    /* The callback that is able to write the char buffer to the
     * output *object*. Returns the number of bytes that have actually
     * been written. */
    ppm_outstream_write_t write;

    /* This function is called when the stream is being destroyed. */
    pmm_outstream_on_destroy_t destroy;
};


/* Create a ppm_outstream_t from a FILE object. */
ppm_outstream_t* ppm_outstream_create_fromfile(FILE* fp);

/* Create a PPM Outstream from a Filename. The file will be opened in
 * binary mode. NULL is returned when the file could not be opened. */
ppm_outstream_t* ppm_outstream_create_fromfilename(const char* filename);

/* Destroy a ppm_outstream_t object allocated with one of the create
 * functions. */
void ppm_outstream_destroy(ppm_outstream_t* stream);

/* Invoke the :attr:`ppm_outstream_t.write` callback. */
size_t ppm_outstream_write(
        const ppm_outstream_t* stream, const char* buffer, size_t size);

/* Printf to a ppm output stream. */
size_t ppm_outstream_printf(
        const ppm_outstream_t* stream, const char* format, ...);


/* Mode specifier, plain or binary. */
typedef enum PPM_MODE {
    PPM_MODE_BINARY,
    PPM_MODE_PLAIN,
} PPM_MODE;

/* This class contains data that is required throughout the process
 * of writing a PPM file. PPM files are written pixelline by pixelline. */
typedef struct _ppm_writesession {
    const ppm_outstream_t* stream;
    PPM_MODE mode;
    uint16_t width;
    uint16_t height;
    uint16_t maxvalue;

    uint16_t pixelcount;
    uint16_t line;
    uint16_t column;

    uint16_t characters_in_line;
    char pixelwidth;
    char pixelformat[40];
} ppm_writesession_t;


/* Initialize a :class:`ppm_write_session_t` object. Returns a non-zero
 * value on failure. The width, height and maxvalue must not be zero. */
int ppm_write_init(
        ppm_writesession_t* session, const ppm_outstream_t* stream,
        PPM_MODE mode, uint16_t width, uint16_t height, uint16_t maxvalue);

/* Writes the header of a PPM file. */
size_t ppm_write_header(ppm_writesession_t* session);

/* Writes a pixel in the specified PPM mode to the stream. */
size_t ppm_write_pixel(
        ppm_writesession_t* session, uint16_t r, uint16_t g, uint16_t b);


/* This structure represents an R G B pixel of a PPM Image. */
typedef struct _ppm_pixel {
    uint16_t r, g, b;
} ppm_pixel_t;

/* A PPM Pixel buffer. */
typedef struct _ppm_pixel_buffer {
    ppm_pixel_t* pixels;
    uint16_t width;
    uint16_t height;
    uint16_t maxvalue;
} ppm_pixel_buffer_t;

/* Create a PPM Pixel Buffer from the specified parameters. */
ppm_pixel_buffer_t* ppm_pixel_buffer_create(
        uint16_t width, uint16_t height, uint16_t maxvalue);

/* Destroys a PPM Pixel Buffer allocated with ppm_pixel_buffer_create(). */
void ppm_pixel_buffer_destroy(ppm_pixel_buffer_t* buffer);

/* Get a pixel from the PPM Pixel Buffer, or NULL if the indecies are
 * out of range. */
ppm_pixel_t* ppm_pixel_buffer_get(
        const ppm_pixel_buffer_t* buffer, uint16_t x, uint16_t y);

/* Writes a PPM Image to the specified outstream. */
int ppm_write_pixel_buffer(
        const ppm_pixel_buffer_t* buffer, const ppm_outstream_t* stream,
        PPM_MODE mode);

/* Writes a PPM Image to the specified file. */
int ppm_write_pixel_buffer_to_file(
        const ppm_pixel_buffer_t* buffer, PPM_MODE mode, FILE* fp);

#endif /* NIKLASROSENSTEIN_PPM */