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

size_t ppm_write_method_file(void* fp_, const char* buffer, size_t size) {
    FILE* fp = (FILE*) fp_;
    return fwrite(buffer, 1, size, fp);
}

size_t ppm_write_pixel(
        const ppm_outstream_t* outstream, PPM_MODE mode, int maxvalue,
        int r, int g, int b) {
    int size = 0;
    char buffer[50];
    switch (mode) {
        case PPM_MODE_PLAIN:
            size = snprintf(buffer, sizeof(buffer), "%d %d %d", r, g, b);
            ppm_outstream_write(outstream, buffer, size);
            break;
        case PPM_MODE_BINARY:
            if (maxvalue < 256) size = 1;
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
            ppm_outstream_write(outstream, buffer, size);
            break;
    }
    return size;
}


