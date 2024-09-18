/*
============================================================
                        DEMO INFO

DEMO NAME:          full_raster.c
DEMO WRITTEN BY:    Muukid
CREATION DATE:      2024-09-16
LAST UPDATED:       2024-09-18

============================================================
                        DEMO PURPOSE

This demo tests the rasterization of TrueType glyphs via
loading a font and performing rasterization on all defined
glyphs within it, exporting the output as a PNG file.

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

	// Point size of rasterization
	float point_size = 100.f;
	// PPI of display for rasterization
	float PPI = 96.f;

/* Functions */

	// Returns if a number is a power of 2
	// https://stackoverflow.com/a/600306
	muBool is_power_of_two(uint32_m n) {
		return (n & (n - 1)) == 0;
	}

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

	// Allocate rglyph data
	uint32_m rdata_len = mutt_header_rglyph_max(&font);
	printf("rdata_len = %" PRIu32 "\n\n", rdata_len);
	muByte* rdata = (muByte*)malloc(rdata_len);

	// Loop through each glyph ID
	for (uint16_m g = 0; g < font.maxp->num_glyphs; ++g) {
		// Get header
		muttGlyphHeader header;
		result = mutt_glyph_header(&font, g, &header);
		if (mutt_result_is_fatal(result)) {
			printf("%" PRIu16 " (header) - %s\n\n", g, mutt_result_get_name(result));
			continue;
		}

		// Leave if zero contours
		if (header.number_of_contours == 0) {
			printf("%" PRIu16 " - no contours\n\n", g);
			continue;
		}

		// Get rglyph
		muttRGlyph glyph;
		uint32_m written;
		result = mutt_header_rglyph(&font, &header, &glyph, point_size, PPI, rdata, &written);
		if (result != MUTT_SUCCESS) {
			printf("%" PRIu16 " (rglyph) - %s", g, mutt_result_get_name(result));
			if (mutt_result_is_fatal(result)) {
				printf(" (fatal)\n\n");
				continue;
			} else {
				printf(" (non-fatal)\n\n");
			}
		}
		// Print memory usage (only if glyph ID is power of 2)
		if (is_power_of_two(g)) {
			// - Amount of bytes used
			printf("%" PRIu16 " - %" PRIu32 " / %" PRIu32 " bytes used (", g, written, rdata_len);
			// - Percentage of allocated memory
			printf("%f%% of maximum glyph memory)\n", (((float)written) / ((float)rdata_len)) * 100.f);
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
			printf("%" PRIu16 " - failed to allocate %" PRIu32 "x%" PRIu32 " pixels\n\n", g, bitmap.width, bitmap.height);
			continue;
		}
		// Print allocated pixel amount (only if glyph ID is power of 2)
		if (is_power_of_two(g)) {
			printf("%" PRIu16 " - %" PRIu32 "x%" PRIu32 " pixels allocated\n", g, bitmap.width, bitmap.height);
		}

		// Rasterize glyph to pixels
		result = mutt_raster_glyph(&glyph, &bitmap, MUTTR_FULL_PIXEL_AA8X8);
		if (mutt_result_is_fatal(result)) {
			printf("%" PRIu16 " (rasterizing) - %s\n", g, mutt_result_get_name(result));
			free(bitmap.pixels);
			continue;
		}

		// Create filename based on codepoint
		char name[24];
		// Simple glyph name:
		if (header.number_of_contours > 0) {
			sprintf(name, "output/%" PRIu16 " (s).png", g);
		} else {
			sprintf(name, "output/%" PRIu16 " (c).png", g);
		}
		// Output result to PNG file
		if (!stbi_write_png(name, bitmap.width, bitmap.height, 1, bitmap.pixels, bitmap.stride)) {
			printf("%" PRIu16 " (outputting) - stbi_write_png failed to write '%s'\n", g, name);
			printf("(You may need to create the 'output' directory)\n\n");
		}
		else if (is_power_of_two(g)) {
			printf("Written '%s'\n\n", name);
		}

		// Deallocate pixels
		free(bitmap.pixels);
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

