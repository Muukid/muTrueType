/*
============================================================
                        DEMO INFO

DEMO NAME:          render.c
DEMO WRITTEN BY:    Muukid
CREATION DATE:      2024-07-12
LAST UPDATED:       2024-07-26

============================================================
                        DEMO PURPOSE

This demo tests the rendering capability of mutt by
rendering a handful of glyphs from a given font.

Note that the file 'font.ttf' should exist within the
directory, and the file 'stb_image_write.h' should exist
within the include directory, which is also available in the
demos folder.

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

	// Include stb_image_write for writing results
	#define STB_IMAGE_WRITE_IMPLEMENTATION
	#include "stb_image_write.h"
		
	// For printing:
	#include <stdio.h>

	// For print types:
	#include <inttypes.h>

	// For allocation:
	#include <stdlib.h>

/* Macros */
	
	// Define this if you want to clamp each glyph image to
	// the exact constraints for that glyph, meaning that
	// each glyph image is stored with no pixels to spare.
	// This is defined to allow testing for if the high-
	// level function "mutt_render_glyph_id" works.
	// Undefine it if you want the opposite effect.
	#define CLAMP_GLYPHS

/* Global variables */
	
	// Font information holder:
	muttFont font;

	// Result value:
	muttResult result = MUTT_SUCCESS;

	// Amount of glyphs to render
	uint16_m glyph_count = 65535;

	// Point size to be rendered at
	float point_size = 500.f;

	// Pixels per inch
	float ppi = 96.f;

	// Rendering format
	muttRenderFormat format = MUTT_BW_FULL_PIXEL_BI_LEVEL_R;

int main(void) {

/* Load font */

{
	// Load file

	FILE* fptr = fopen("font.ttf", "rb");

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
		printf("Couldn't allocate data for file\n");
		fclose(fptr);
		return -1;
	}

	fread(data, fptr_size, 1, fptr);

	// Close file

	fclose(fptr);

	// Load everything in the font

	result = mutt_load(data, fptr_size, &font, MUTT_LOAD_REQUIRED_TABLES & ~(
		// (All the flags we don't need in the required table set)
		MUTT_LOAD_NAME | MUTT_LOAD_POST | MUTT_LOAD_HMTX
	));
	free(data); // (We no longer need the original raw data)

	// Make sure result of loading font was good
	if (result != MUTT_SUCCESS) {
		printf("mutt_load returned %s\n", mutt_result_get_name(result));
		return -1;
	}

	// Make sure all tables we requested loaded successfully
	else if (mutt_get_load_results(&font) != 0) {
		printf("not all requested tables loaded successfully\n");
		return -1;
	}

	// Cap glyph_count at the number of glyphs provided by the font
	if (glyph_count > font.maxp->num_glyphs) {
		glyph_count = font.maxp->num_glyphs;
	}
}

/* Allocate pixel memory for non-clamp rendering */

#ifndef CLAMP_GLYPHS

	// Get maximum dimensions needed for any glyph as our width and height
	uint32_m width, height;
	mutt_maximum_glyph_render_dimensions(&font, point_size, ppi, &width, &height);
	printf("Image dimensions: %" PRIu32 "x%" PRIu32 "\n", width, height);

	// Allocate pixels based on dimensions
	uint8_m* pixels = (uint8_m*)malloc(width*height);
	if (!pixels) {
		printf("Unable to allocate pixels\n");
		goto fail_pixels;
	}

#endif

/* Render each glyph and output it */

// Loop through each glyph
for (uint32_m i = 0; i < glyph_count; i++) {
	// Calculate corresponding glyph ID
	uint16_m glyph_id = (uint16_m)((float)i / ((float)glyph_count / (float)font.maxp->num_glyphs));

	// Non-clamped glyph rendering handling:
	#ifndef CLAMP_GLYPHS

		// Render glyph to pixels
		result = mutt_render_glyph_id(&font, glyph_id, point_size, ppi, format, pixels, width, height);
		if (result != MUTT_SUCCESS) {
				printf("[Warning] Glyph #%" PRIu16 " failed to render: %s\n", glyph_id, mutt_result_get_name(result));
				continue;
		}

	// Clamped glyph rendering handling:
	#else

		// Get glyph header
		muttGlyphHeader header;
		result = mutt_glyph_get_header(&font, glyph_id, &header);
		if (result != MUTT_SUCCESS) {
			printf("[Warning] Failed to retrieve header for glyph #%" PRIu16 ": %s\n", glyph_id, mutt_result_get_name(result));
			continue;
		}

		// Don't draw if it's a non-outlined contour
		if (header.length == 0) {
			printf("Glyph #%" PRIu16 " has no outline, skipping...\n", glyph_id);
			continue;
		}

		// Allocate pixel memory for glyph
		uint32_m width, height;
		mutt_glyph_render_dimensions(&font, &header, point_size, ppi, &width, &height);
		uint8_m* pixels = (uint8_m*)malloc(width*height);
		if (!pixels) {
			printf("[Warning] Failed to allocate pixel data for glyph #%" PRIu16 " (%" PRIu32 "x%" PRIu32 ")\n", glyph_id, width, height);
			continue;
		}

		// Allocate and retrieve glyph memory

		muttSimpleGlyph sglyph;
		muttCompositeGlyph cglyph;
		muByte* data;

		// - Simple
		if (header.number_of_contours > -1) {
			// Retrieve amount of memory needed
			uint32_m written;
			result = mutt_simple_glyph_get_data(&font, &header, &sglyph, 0, &written);
			if (result != MUTT_SUCCESS) {
				printf("[Warning] Failed to retrieve the memory requirements for glyph #%" PRIu16 ": %s\n", glyph_id, mutt_result_get_name(result));
				mu_free(pixels);
				continue;
			}

			// Allocate memory
			data = (muByte*)malloc(written);
			if (!data) {
				printf("[Warning] Failed to allocate glyph memory for glyph #%" PRIu16 " (%" PRIu32 " bytes)\n", glyph_id, written);
				mu_free(pixels);
				continue;
			}

			// Get data
			result = mutt_simple_glyph_get_data(&font, &header, &sglyph, data, &written);
			if (result != MUTT_SUCCESS) {
				printf("[Warning] Failed to retrieve the data for glyph #%" PRIu16 ": %s\n", glyph_id, mutt_result_get_name(result));
				mu_free(pixels);
				mu_free(data);
				continue;
			}
		}

		// - Composite
		else {
			// Retrieve amount of memory needed
			uint32_m written;
			result = mutt_composite_glyph_get_data(&font, &header, &cglyph, 0, &written);
			if (result != MUTT_SUCCESS) {
				printf("[Warning] Failed to retrieve the memory requirements for glyph #%" PRIu16 ": %s\n", glyph_id, mutt_result_get_name(result));
				mu_free(pixels);
				continue;
			}

			// Allocate memory
			data = (muByte*)malloc(written);
			if (!data) {
				printf("[Warning] Failed to allocate glyph memory for glyph #%" PRIu16 " (%" PRIu32 " bytes)\n", glyph_id, written);
				mu_free(pixels);
				continue;
			}

			// Get data
			result = mutt_composite_glyph_get_data(&font, &header, &cglyph, data, 0);
			if (result != MUTT_SUCCESS) {
				printf("[Warning] Failed to retrieve the data for glyph #%" PRIu16 ": %s\n", glyph_id, mutt_result_get_name(result));
				mu_free(pixels);
				mu_free(data);
				continue;
			}
		}

		// Render glyph and output result

		// - Simple
		if (header.number_of_contours > -1) {
			// Render glyph
			result = mutt_render_simple_glyph(&font, &header, &sglyph, point_size, ppi, format, pixels, width, height);
			if (result != MUTT_SUCCESS) {
				if (result == MUTT_EMPTY_SIMPLE_GLYPH) {
					printf("Glyph #%" PRIu16 " was empty\n", glyph_id);
				} else {
					printf("[Warning] Failed to render glyph #%" PRIu16 ": %s\n", glyph_id, mutt_result_get_name(result));
				}
				mu_free(pixels);
				mu_free(data);
				continue;
			}
		}

		// - Composite
		else {
			// Render glyph
			result = mutt_render_composite_glyph(&font, &header, &cglyph, point_size, ppi, format, pixels, width, height);
			if (result != MUTT_SUCCESS) {
				if (result == MUTT_EMPTY_COMPOSITE_GLYPH) {
					printf("Glyph #%" PRIu16 " was empty\n", glyph_id);
				} else {
					printf("[Warning] Failed to render glyph #%" PRIu16 ": %s\n", glyph_id, mutt_result_get_name(result));
				}
				mu_free(pixels);
				mu_free(data);
				continue;
			}
		}

	#endif

	// Output result as PNG
	// - Generate filename based on glyph ID
	char name[24] = {0};
	sprintf(name, "f/%" PRIu16 ".png", glyph_id);
	// - Output file using stb_image_write
	if (stbi_write_png(name, width, height, 1, pixels, width) == 0) {
		printf("[Warning] Failed to write \"%" PRIu16 ".png\"\n", glyph_id);
	} else {
		printf("Written \"%" PRIu16 ".png\"\n", glyph_id);
	}

	// Free data on clamped glyph rendering
	#ifdef CLAMP_GLYPHS
		mu_free(pixels);
		mu_free(data);
	#endif
}

/* Deload */

{
	// Global memory that non-glyph clamping uses

	#ifndef CLAMP_GLYPHS
		// Free pixels
		free(pixels);
		fail_pixels:
	#endif

	// Deload font
	mutt_deload(&font);

	// Print success
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

