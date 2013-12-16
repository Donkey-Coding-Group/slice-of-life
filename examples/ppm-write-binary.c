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

#include <stdio.h>
#include "ppm.h"

int main() {
    /* Create a PPM Output Stream to a file object. */
    ppm_outstream_t* stream = ppm_outstream_create_fromfilename(
            "ppm-write-binary-result.ppm");
    if (stream == NULL) {
        fprintf(stderr, "File could not be opened or outstream not allocated\n.");
        return -1;
    }

    /* Create a PPM Write Session. */
    ppm_writesession_t session;
    int r = ppm_writesession_init(&session, stream, PPM_MODE_BINARY,
                                  255, 255, 255); // width, height, maxvalue
    if (r != 0) {
        fprintf(stderr, "Write Session could not be initialized.\n");
        ppm_outstream_destroy(stream);
        return -1;
    }

    /* Write the header of the PPM Image. */
    ppm_write_header(&session);

    /* Write the pixels to the PPM Image. */
    int x, y;
    for (y=0; y < session.height; y++) {
        for (x=0; x < session.width; x++) {
            ppm_write_pixel(&session, x, y, 100);
        }
    }

    /* Close the file stream again. */
    ppm_outstream_destroy(stream);
    return 0;
}

