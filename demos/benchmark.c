/*
============================================================
                        DEMO INFO

DEMO NAME:          benchmark.c
DEMO WRITTEN BY:    Muukid
CREATION DATE:      2024-12-19
LAST UPDATED:       2024-12-19

============================================================
                        DEMO PURPOSE

This demo tests the performance of rasterization by
rasterizing a given font at several different sizes with
several different rasterization methods.

This demo is dependent on the 'resources' folder within the
demos folder, and will, upon success, generate several files
within a created directory labeled 'output'.

This demo is also dependent on 'nanotime.h' being in the
include directory, which is provided in the demos folder.

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

	// For getting the time
	#define NANOTIME_IMPLEMENTATION
	#include "nanotime.h"

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

	// Point sizes to test
	float point_sizes[] = {
		25.f, 50.f, 100.f, 500.f
	};
	#define POINT_SIZE_COUNT (sizeof(point_sizes) / sizeof(float))

	// PPI of display for rasterization
	float PPI = 96.f;

	// Raster methods to test
	muttRMethod methods[] = {
		MUTTR_FULL_PIXEL_BI_LEVEL,
		MUTTR_FULL_PIXEL_AA2X2,
		MUTTR_FULL_PIXEL_AA4X4,
		MUTTR_FULL_PIXEL_AA8X8
	};
	#define METHOD_COUNT (sizeof(methods) / sizeof(muttRMethod))

	// Max number of glyphs to test each pass
	#define MAX_GLYPH_COUNT 32

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
		free(data); // (We no longer need the orig raw data)

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

	// Calculate amount of glyphs to render
	uint32_m glyph_count = MAX_GLYPH_COUNT;
	if (glyph_count > font.maxp->num_glyphs) {
		glyph_count = font.maxp->num_glyphs;
	}

	// Loop through each point size
	for (uint32_m p = 0; p < POINT_SIZE_COUNT; ++p) {
		float point_size = point_sizes[p];
		printf("== %f point size ==\n", point_size);

		// Loop through each raster method
		for (uint32_m m = 0; m < METHOD_COUNT; ++m) {
			muttRMethod method = methods[m];
			printf("\t== %i raster method ==\n", (int)method);

			uint32_m successful_count = 0;
			double prog_time = 0.0;

			// Loop through each glyph
			for (uint16_m gi = 0; gi < glyph_count; ++gi) {
				// Calculate glyph ID
				uint16_m g = (((float)gi) / ((float)glyph_count)) * font.maxp->num_glyphs;
				printf("\t\t[%" PRIu16 "]\t ", g);

				uint64_t start = nanotime_now();

				// Get header
				muttGlyphHeader header;
				result = mutt_glyph_header(&font, g, &header);
				if (mutt_result_is_fatal(result)) {
					printf("(header) - %s\n", mutt_result_get_name(result));
					continue;
				}

				// Leave if zero contours
				if (header.number_of_contours == 0) {
					printf("no contours\n");
					continue;
				}

				// Get rglyph
				muttRGlyph glyph;
				uint32_m written;
				result = mutt_header_rglyph(&font, &header, &glyph, point_size, PPI, rdata, &written);
				if (mutt_result_is_fatal(result)) {
					printf("(rheader) - %s\n", mutt_result_get_name(result));
					continue;
				}

				// Describe bitmap to be rasterized onto
				muttRBitmap bitmap;
				bitmap.width = glyph.x_max + 2;
				bitmap.height = glyph.y_max + 2;
				bitmap.channels = MUTTR_R;
				bitmap.stride = bitmap.width;
				bitmap.io_color = MUTTR_BW;
				bitmap.pixels = (muByte*)malloc(bitmap.width * bitmap.height);
				if (!bitmap.pixels) {
					printf("failed to allocate pixels\n");
					continue;
				}

				// Rasterize glyph to pixels
				result = mutt_raster_glyph(&glyph, &bitmap, method);
				if (mutt_result_is_fatal(result)) {
					printf("failed to raster\n");
					free(bitmap.pixels);
					continue;
				}

				free(bitmap.pixels);
				uint64_t end = nanotime_now();

				// Calculate and print time
				double raster_time = nanotime_interval(start, end, nanotime_now_max()) / (double)NANOTIME_NSEC_PER_SEC;
				++successful_count;
				prog_time += raster_time;
				printf("%f seconds", raster_time);

				printf("\n");
			}

			if (successful_count == 0) {
				printf("\tAverage time: 0 seconds (none successful)\n");
			}
			else {
				double avg = prog_time / ((double)successful_count);
				printf("\tAverage time: %f seconds\n", avg);
			}
		}

		printf("\n");
	}

	// Free rglyph data
	free(rdata);

	/* Deload font */
	{
		mutt_deload(&font);
		printf("Successful\n");
	}

	return 0;
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

