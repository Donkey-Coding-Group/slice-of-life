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
 * file: ppm.c
 * description: Simple PPM writer.
 * author: Niklas Rosenstein <rosensteinniklas@gmail.com> */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include <arpa/inet.h>  // htonl
#include "ppm.h"


char _count_decimal_places(int64_t value) {
    char count = 1;
    while (value > 9) {
        value /= 10;
        count++;
    }
    return count;
}


/* Callbacks for FILE streams. */

static size_t _ppm_outstream_fp_write(
        const ppm_outstream_t* stream, const char* buffer, size_t size) {
    FILE* fp = (FILE*) stream->object;
    return fwrite(buffer, 1, size, fp);
}

static void _ppm_outstream_fp_destroy(ppm_outstream_t* stream) {
    if (stream->object) {
        fclose((FILE*) stream->object);
        stream->object = NULL;
    }
}


/* Stream creation. */

ppm_outstream_t* ppm_outstream_create_fromfile(FILE* fp) {
    if (fp == NULL) return NULL;
    ppm_outstream_t* stream = malloc(sizeof(ppm_outstream_t));
    if (stream == NULL) return NULL;
    stream->object = fp;
    stream->write = _ppm_outstream_fp_write;
    stream->destroy = _ppm_outstream_fp_destroy;
    return stream;
}

ppm_outstream_t* ppm_outstream_create_fromfilename(const char* filename) {
    FILE* fp = fopen(filename, "wb");
    return ppm_outstream_create_fromfile(fp);
}

void ppm_outstream_destroy(ppm_outstream_t* stream) {
    if (stream->destroy) {
        stream->destroy(stream);
    }
    free(stream);
}


/* Generic Stream interaction. */

size_t ppm_outstream_write(
        const ppm_outstream_t* stream, const char* buffer, size_t size) {
    return stream->write(stream, buffer, size);
}

size_t ppm_outstream_printf(
        const ppm_outstream_t* stream, const char* format, ...) {
    va_list args;
    va_start(args, format);

    char* string = NULL;
    int size = vasprintf(&string, format, args);
    va_end(args);
    if (string == NULL) return 0;

    size_t result = ppm_outstream_write(stream, string, size);
    free(string);
    return result;
}


/* PPM Writing. */

int ppm_write_init(
        ppm_writesession_t* session, const ppm_outstream_t* stream,
        PPM_MODE mode, uint16_t width, uint16_t height, uint16_t maxvalue) {
    if (!session || !stream) return 1;
    if (width <= 0 || height <= 0 || maxvalue <= 0) return 2;

    /* The P6 (plain) format can only be used with single-byte colours. */
    if (maxvalue > 255 && mode == PPM_MODE_PLAIN) {
        return 3;
    }

    session->stream = stream;
    session->mode = mode;
    session->width = width;
    session->height = height;
    session->maxvalue = maxvalue;

    session->pixelcount = 0;
    session->line = 0;
    session->column = 0;

    session->characters_in_line = 0;
    /* Count the number if places each digit requires. */
    switch (mode) {
        case PPM_MODE_BINARY:
            session->pixelwidth = (maxvalue < 256 ? 1 : 2);
            session->pixelformat[0] = 0;
            break;
        case PPM_MODE_PLAIN:
            session->pixelwidth = _count_decimal_places(maxvalue) + 1;
            snprintf(session->pixelformat, sizeof(session->pixelformat),
                    "%%%d%s %%%d%s %%%d%s",
                    session->pixelwidth - 1, PRIu16,
                    session->pixelwidth - 1, PRIu16,
                    session->pixelwidth - 1, PRIu16);
            session->pixelwidth *= 3;
            break;
    }
    return 0;
}

size_t ppm_write_header(ppm_writesession_t* session) {
    /* printf() format string for the header. */
    static const char* whm_format = "%"PRIu16" %"PRIu16"\n%"PRIu16"\n";

    char buffer[100];
    char* b = buffer;

    /* Blit the magic number into the buffer. */
    switch (session->mode) {
        case PPM_MODE_PLAIN:
            b += sprintf(buffer, "P3\n");
            break;
        case PPM_MODE_BINARY:
            b += sprintf(buffer, "P6\n");
            break;
        default:
            fprintf(stderr, "Invalid PPM_MODE supplied to ppm_write_header().");
            return 0;
    }

    /* Blit the width, height and maxvalue into the buffer. */
    b += sprintf(b, whm_format, session->width, session->height,
                 session->maxvalue);

    /* Write the buffer to the output stream. */
    return ppm_outstream_write(session->stream, buffer, (int) (b - buffer));
}

size_t ppm_write_pixel(
        ppm_writesession_t* session, uint16_t r, uint16_t g, uint16_t b) {
    /* Limit the r, g, b values. */
    if (r > session->maxvalue) r = session->maxvalue;
    if (g > session->maxvalue) g = session->maxvalue;
    if (b > session->maxvalue) b = session->maxvalue;

    /* The characters that will be used to separate the pixels from each
     * other. Either a space or a newline. */
    char c = ' ';
    int size = 0;
    char buffer[50];

    /* Check if we should do a line-break instead of simple whitespace. */
    bool do_linebreak = false;
    if ( session->column >= session->width) {
        session->line++;
        session->column = 0;
    }
    else {
        session->column++;
    }
    session->pixelcount++;

    if ((session->characters_in_line + session->pixelwidth) > 70) {
        do_linebreak = true;
        session->characters_in_line = 0;
    }
    if (do_linebreak) {
        c = '\n';
    }

    /* Pack the r, g, b values into the buffer depending on the PPM mode. */
    switch (session->mode) {
        case PPM_MODE_PLAIN:
            size = snprintf(buffer, sizeof(buffer), session->pixelformat,
                    r, g, b);
            buffer[size++] = c;
            buffer[size] = 0;
            break;
        case PPM_MODE_BINARY:
            if (session->maxvalue < 256) size = 1;
            else size = 2;

            {
                uint32_t vr, vg, vb;
                vr = htonl(r);
                vg = htonl(g);
                vb = htonl(b);
                size_t off = sizeof(uint32_t) - size;

                memcpy(buffer + 0 * size, ((const char*) &vr) + off, size);
                memcpy(buffer + 1 * size, ((const char*) &vg) + off, size);
                memcpy(buffer + 2 * size, ((const char*) &vb) + off, size);
            }

            size = size * 3;
            break;
    }

    ppm_outstream_write(session->stream, buffer, size);
    session->characters_in_line += size;
    return size;
}


/* PPM Pixel Buffer. */

ppm_pixel_buffer_t* ppm_pixel_buffer_create(
        uint16_t width, uint16_t height, uint16_t maxvalue) {
    /* Validate the parameters. */
    if (width < 1 || height < 1 || maxvalue < 1) return NULL;

    /* Allocate a Pixel array. */
    ppm_pixel_t* pixels = malloc(sizeof(ppm_pixel_t) * width * height);
    if (pixels == NULL) return NULL;

    ppm_pixel_buffer_t* buffer = malloc(sizeof(ppm_pixel_buffer_t));
    if (buffer == NULL) {
        free(pixels);
        return NULL;
    }

    buffer->pixels = pixels;
    buffer->width = width;
    buffer->height = height;
    buffer->maxvalue = maxvalue;
    return buffer;
}

void ppm_pixel_buffer_destroy(ppm_pixel_buffer_t* buffer) {
    if (buffer->pixels) {
        free(buffer->pixels);
        buffer->pixels = NULL;
    }
    free(buffer);
}

ppm_pixel_t* ppm_pixel_buffer_get(
        const ppm_pixel_buffer_t* buffer, uint16_t x, uint16_t y) {
    if (x >= buffer->width || y >= buffer->height) return NULL;
    return &buffer->pixels[x + y * buffer->width];
}

int ppm_write_pixel_buffer(
        const ppm_pixel_buffer_t* buffer, const ppm_outstream_t* stream,
        PPM_MODE mode) {
    /* Create the PPM Write Session. */
    ppm_writesession_t session;
    if (ppm_write_init(&session, stream, mode, buffer->width, buffer->height,
                       buffer->maxvalue) != 0) {
        return 1;
    }

    /* Write header and the pixels. */
    ppm_write_header(&session);
    int i, j;
    for (j=0; j < buffer->height; j++) {
        for (i=0; i < buffer->width; i++) {
            const ppm_pixel_t* p = ppm_pixel_buffer_get(buffer, i, j);
            ppm_write_pixel(&session, p->r, p->g, p->b);
        }
    }

    return 0;
}

int ppm_write_pixel_buffer_to_file(
        const ppm_pixel_buffer_t* buffer, PPM_MODE mode, FILE* fp) {
    ppm_outstream_t* stream = ppm_outstream_create_fromfile(fp);
    if (stream == NULL) {
        return 1;
    }

    int result = ppm_write_pixel_buffer(buffer, stream, mode);
    ppm_outstream_destroy(stream);

    if (result != 0) {
        return 2;
    }
    return 0;
}



