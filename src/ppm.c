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

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <inttypes.h>
#include "ppm.h"

size_t ppm_outstream_write(
        const ppm_outstream_t* outstream, const char* buffer, size_t size) {
    return outstream->write(outstream->object, buffer, size);
}

size_t ppm_outstream_printf(
        const ppm_outstream_t* outstream, const char* format, ...) {
    va_list args;
    va_start(args, format);

    char* string = NULL;
    int size = vasprintf(&string, format, args);
    va_end(args);
    if (string == NULL) return 0;

    size_t result = ppm_outstream_write(outstream, string, size);
    free(string);
    return result;
}

size_t ppm_writemethod_file(void* fp_, const char* buffer, size_t size) {
    FILE* fp = (FILE*) fp_;
    return fwrite(buffer, 1, size, fp);
}

int ppm_writesession_init(
        ppm_writesession_t* session, const ppm_outstream_t* stream,
        PPM_MODE mode, uint16_t width, uint16_t height, uint16_t maxvalue) {
    if (!session || !stream) return 1;
    if (width <= 0 || height <= 0 || maxvalue <= 0) return 2;

    session->stream = stream;
    session->mode = mode;
    session->width = width;
    session->height = height;
    session->maxvalue = maxvalue;

    session->pixelcount = 0;
    session->line = 0;
    session->column = 0;

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
    /* printf() format string for the r, g, b values used in plain mode. */
    static const char* rgb_format = "%"PRIu16" %"PRIu16" %"PRIu16" %c";

    char c = ' ';
    int size = 0;
    char buffer[50];

    if (session->column >= session->width) {
        c = '\n';
        session->line++;
        session->column = 0;
    }
    else {
        session->column++;
    }
    session->pixelcount++;

    /* Pack the r, g, b values into the buffer depending on the PPM mode. */
    switch (session->mode) {
        case PPM_MODE_PLAIN:
            size = snprintf(buffer, sizeof(buffer), rgb_format, r, g, b, c);
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
    return size;
}



