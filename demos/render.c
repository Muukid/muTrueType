/*
============================================================
                        DEMO INFO

DEMO NAME:          render.c
DEMO WRITTEN BY:    Muukid
CREATION DATE:      2024-07-12
LAST UPDATED:       2024-07-21

============================================================
                        DEMO PURPOSE

This demo tests the rendering capability of mutt.

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

/* Global variables */
	
	// Font information holder:
	muttFont font;

	// Result value:
	muttResult result = MUTT_SUCCESS;

	// Test characters:
	uint32_m test_chars[] = {
		'A','B','C','D','E','F','G','H','I','J','K','L','M',
		'N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
		'a','b','c','d','e','f','g','h','i','j','k','l','m',
		'n','o','p','q','r','s','t','u','v','w','x','y','z',
	};

	// Test character length
	#define TEST_CHAR_LEN (sizeof(test_chars)/sizeof(uint32_m))

	// To-be-filled glyph IDs for test characters
	uint16_m glyph_ids[TEST_CHAR_LEN] = {0};

	// Point size to be rendered at
	float point_size = 500.f;

	// Pixels per inch
	float ppi = 72.f;

	// Rendering format
	muttRenderFormat format = MUTT_BW_FULL_PIXEL_BI_LEVEL_R;

int main(void) {

/* Load font */

{
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
}

/* Grab characters */

printf("chars {");

for (uint16_m i = 0; i < TEST_CHAR_LEN; i++)
{	
	// Print new lines and tabs occasionally
	if (i%3 == 0) {
		printf("\n\t");
	}

	// Get glyph ID for the character
	glyph_ids[i] = mutt_codepoint_to_glyph_id(&font, test_chars[i]);

	// Print result
	printf("[0x%04x] - %05" PRIu16 ", ", test_chars[i], glyph_ids[i]);
}

printf("\n}\n");

/* Define pixels */

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

/* Render each glyph and output it */

// Allocate memory for the glyph stuff
uint32_m glyph_mem_len;
muByte* glyph_mem;

// - Simple/Composite memory:
{
	glyph_mem_len = mutt_simple_glyph_get_max_size(&font);
	uint32_m composite_max = mutt_composite_glyph_get_max_size(&font);
	if (composite_max > glyph_mem_len) {
		glyph_mem_len = composite_max;
	}
}

// - Allocate
glyph_mem = (muByte*)malloc(glyph_mem_len);
if (!glyph_mem) {
		printf("Unable to allocate glyph memory\n");
		goto fail_glyph_mem;
}

// Loop through each glyph
for (uint32_m i = 0; i < TEST_CHAR_LEN; i++) {
	// Render glyph to pixels
	result = mutt_render_glyph_id(&font, glyph_ids[i], point_size, ppi, format, pixels, width, height, glyph_mem);
	if (result != MUTT_SUCCESS) {
			printf("[Warning] Character #%" PRIu32 " failed to render: %s\n", i, mutt_result_get_name(result));
	} else {
		// Output as image
		// - Generate filename based on index
		char name[24] = {0};
		sprintf(name, "%" PRIu32 ".png", i);
		printf("Written \"%" PRIu32 ".png\"\n", i);
		// - Output file using stb_image_write
		stbi_write_png(name, width, height, 1, pixels, width);
	}
}

/* Deload */

{
	// Free glyph memory
	free(glyph_mem);
	fail_glyph_mem:

	// Free pixels
	free(pixels);
	fail_pixels:

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

