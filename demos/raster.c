/*
============================================================
                        DEMO INFO

DEMO NAME:          raster.c
DEMO WRITTEN BY:    Muukid
CREATION DATE:      2024-09-04
LAST UPDATED:       2024-09-06

============================================================
                        DEMO PURPOSE

This demo tests the rasterization of TrueType glyphs via
loading a font and performing rasterization on several
glyphs, exporting the output as a PNG file.

This demo is dependent on the 'resources' folder within the
demos folder, and will, upon success, generate several files
within a created directory labeled 'output'.

============================================================
                        LICENSE INFO

All code is licensed under MIT License or public domain, 
whichever you prefer.
More explicit license information at the end of file.

============================================================
*/

/* Including */
    
    // Include muTrueType
    #define MUTT_NAMES // (for name functions)
    #define MUTT_IMPLEMENTATION
    #include "muTrueType.h"

    // For outputting images:
    #define STB_IMAGE_WRITE_IMPLEMENTATION
    #include "stb_image_write.h"
    
    // For printing and file loading:
    #include <stdio.h>

    // For print types:
    #include <inttypes.h>

    // For allocation:
    #include <stdlib.h>

/* Global variables */
    
    // Font information holder
    muttFont font;

    // Result value
    muttResult result = MUTT_SUCCESS;

    // Test characters
    uint32_m test_chars[] = {
        // English alphabet (uppercase)
        0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A,
        // English alphabet (lowercase)
        0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
        0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A,
        // Various English symbols
        0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
    };
    // Test character length
    #define TEST_CHAR_LEN (sizeof(test_chars)/sizeof(uint32_m))

    // To-be-filled glyph IDs for test characters
    uint16_m glyph_ids[TEST_CHAR_LEN] = { 0 };

    // Point size of rasterization
    float point_size = 10.f;
    // PPI of display for rasterization
    float PPI = 96.f;

int main(void)
{
    /* Load font */
    {
        // Open file in binary
        FILE* fptr = fopen("resources/font.ttf", "rb");
        if (!fptr) {
            printf("Couldn't open 'font.ttf'\n");
            return -1;
        }

        // Get size
        fseek(fptr, 0L, SEEK_END);
        size_m fptr_size = ftell(fptr);
        fseek(fptr, 0L, SEEK_SET);

        // Load data into buffer
        muByte* data = (muByte*)malloc(fptr_size);
        if (!data) {
            printf("Couldn't allocate data for loading file\n");
            fclose(fptr);
            return -1;
        }
        fread(data, fptr_size, 1, fptr);

        // Close file
        fclose(fptr);

        // Load everything in the font
        result = mutt_load(data, (uint64_m)fptr_size, &font, MUTT_LOAD_ALL);
        free(data); // (We no longer need the original raw data)

        // Print if not successful
        if (result != MUTT_SUCCESS) {
            printf("'mutt_load' returned non-success value: %s\n", mutt_result_get_name(result));
            // Exit if result is fatal
            if (mutt_result_is_fatal(result)) {
                printf("Result is fatal, exiting...\n");
                return -1;
            }
        }
        printf("Successfully loaded font file 'resources/font.ttf'\n");

        // Print if some tables failed to load
        if (font.fail_load_flags) {
            printf("Some tables failed to load\n");
        } else {
            printf("All tables loaded successfully\n");
        }

        printf("\n");
    }

    // Fill in glyph IDs
    {
        // Print glyph ID array initializer
        printf("glyphs[%" PRIu32 "] = {\n", (uint32_m)TEST_CHAR_LEN);

        // Loop through each glyph
        for (uint32_m g = 0; g < TEST_CHAR_LEN; ++g) {
            // Get glyph ID corresponding to codepoint
            glyph_ids[g] = mutt_get_glyph(&font, test_chars[g]);
            // Print conversion
            printf("\t%" PRIu32 "\t -> %" PRIu16 "\n", test_chars[g], glyph_ids[g]);
        }

        // Print glyph ID array end
        printf("}\n\n");
    }

    // Allocate rglyph data
    uint32_m rdata_len = mutt_header_rglyph_max(&font);
    printf("rdata_len = %" PRIu32 "\n\n", rdata_len);
    muByte* rdata = (muByte*)malloc(rdata_len);

    // Loop through each glyph
    for (uint32_m g = 0; g < TEST_CHAR_LEN; ++g) {
        // Get header
        muttGlyphHeader header;
        result = mutt_glyph_header(&font, glyph_ids[g], &header);
        if (mutt_result_is_fatal(result)) {
            printf("%" PRIu16 " (header) - %s\n", glyph_ids[g], mutt_result_get_name(result));
            continue;
        }

        // Leave if zero contours
        if (header.number_of_contours == 0) {
            printf("%" PRIu16 " - no contours\n", glyph_ids[g]);
            continue;
        }

        // Get rglyph
        muttRGlyph glyph;
        uint32_m written;
        result = mutt_header_rglyph(&font, &header, &glyph, point_size, PPI, rdata, &written);
        if (mutt_result_is_fatal(result)) {
            printf("%" PRIu16 " (rglyph) - %s\n", glyph_ids[g], mutt_result_get_name(result));
            continue;
        }

        // Describe bitmap to be rasterized onto
        muttRBitmap bitmap;
        bitmap.width = glyph.x_max + 2; // (width; +2 for bleeding)
        bitmap.height = glyph.y_max + 2; // (height; +2 for bleeding)
        bitmap.channels = MUTTR_R; // (single-channel)
        bitmap.stride = bitmap.width; // (stride per horizontal row)
        bitmap.io_color = MUTTR_BW; // (black = outside glyph, white = inside glyph)
        // - Pixels and their allocation
        bitmap.pixels = (muByte*)malloc(bitmap.width * bitmap.height); // (pixels)
        if (!bitmap.pixels) {
            printf("%" PRIu16 " - failed to allocate %" PRIu32 "x%" PRIu32 " pixels\n", glyph_ids[g], bitmap.width, bitmap.height);
            continue;
        }
        printf("%" PRIu16 " - %" PRIu32 "x%" PRIu32 " pixels allocated\n", glyph_ids[g], bitmap.width, bitmap.height);

        // Rasterize glyph to pixels
        result = mutt_raster_glyph(&glyph, &bitmap, MUTTR_FULL_PIXEL_BI_LEVEL);
        if (mutt_result_is_fatal(result)) {
            printf("%" PRIu16 " (rasterizing) - %s\n", glyph_ids[g], mutt_result_get_name(result));
            free(bitmap.pixels);
            continue;
        }

        // Create filename based on codepoint
        char name[24];
        sprintf(name, "output/%" PRIu32 ".png", test_chars[g]);
        // Output result to PNG file
        if (!stbi_write_png(name, bitmap.width, bitmap.height, 1, bitmap.pixels, bitmap.stride)) {
            printf("%" PRIu16 " (outputting) - stbi_write_png failed to write '%s'\n", glyph_ids[g], name);
            printf("(You may need to create the 'output' directory)\n");
        }
        else {
            printf("Written '%s'\n", name);
        }

        // Deallocate pixels
        free(bitmap.pixels);

        // Print newline for next glyph
        printf("\n");
    }

    // Free rgylph data
    free(rdata);

    /* Deload font */
    {
        mutt_deload(&font);
        printf("Successful\n");
    }
}

/*
------------------------------------------------------------------------------
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2024 Hum
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------
*/

