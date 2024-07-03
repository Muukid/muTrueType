/*
============================================================
                        DEMO INFO

DEMO NAME:          info.c
DEMO WRITTEN BY:    Muukid
CREATION DATE:      2024-07-02
LAST UPDATED:       2024-07-03

============================================================
                        DEMO PURPOSE

This demo tests all of the low-level API, printing out a
bunch of low-level information about the TrueType font.

Note that the file 'font.ttf' should exist within the
directory.

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
	
	// For printing and file loading:
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

/* Print functions */

	// Prints a value in binary.
	void print_binary(muByte* mem, size_m size) {
		for (size_m byte = 0; byte < size; byte++) {
			for (size_m bit = 0; bit < 8; bit++) {
				uint8_m val = (mem[byte] >> bit) & 1;
				printf("%" PRIu8 "", val);
			}
		}
	}

int main(void) {

/* Load font */

{
	// Open file in binary

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

	result = mutt_load(data, fptr_size, &font, MUTT_LOAD_ALL);
	free(data); // (We no longer need the original raw data)

	if (result != MUTT_SUCCESS) {
		printf("mutt_load returned %s\n", mutt_result_get_name(result));
		return -1;
	}
}

/* Print table directory info */

{
	printf("== Table directory ==\n");

	// Print fixed variables

	printf("numTables: %" PRIu16 "\n",     font.directory->num_tables);
	printf("searchRange: %" PRIu16 "\n",   font.directory->search_range);
	printf("entrySelector: %" PRIu16 "\n", font.directory->entry_selector);
	printf("range_shift: %" PRIu16 "\n",   font.directory->range_shift);

	// Print all table records

	for (uint16_m i = 0; i < font.directory->num_tables; i++) {
		muttTableRecord* record = &font.directory->table_records[i];

		// (tableTag)
		printf("[%c%c%c%c] - ", 
			record->table_tag[0], record->table_tag[1], record->table_tag[2], record->table_tag[3]
		);

		printf("checksum=%" PRIu32 ", ",     record->checksum);
		printf("offset=%" PRIu32 " bytes, ", record->offset);
		printf("length=%" PRIu32 " bytes\n", record->length);
	}

	printf("\n");
}

/* Print maxp info */

printf("== Maxp ==\n");

if (font.maxp)
{
	printf("version: %" PRIu16 ".%" PRIu16 "\n",   font.maxp->version_high, font.maxp->version_low);
	printf("numGlyphs: %" PRIu16 "\n",             font.maxp->num_glyphs);
	printf("maxPoints: %" PRIu16 "\n",             font.maxp->max_points);
	printf("maxContours: %" PRIu16 "\n",           font.maxp->max_contours);
	printf("maxCompositePoints: %" PRIu16 "\n",    font.maxp->max_composite_points);
	printf("maxCompositeContours: %" PRIu16 "\n",  font.maxp->max_composite_contours);
	printf("maxZones: %" PRIu16 "\n",              font.maxp->max_zones);
	printf("maxTwilightPoints: %" PRIu16 "\n",     font.maxp->max_twilight_points);
	printf("maxStorage: %" PRIu16 "\n",            font.maxp->max_storage);
	printf("maxFunctionDefs: %" PRIu16 "\n",       font.maxp->max_function_defs);
	printf("maxInstructionDefs: %" PRIu16 "\n",    font.maxp->max_instruction_defs);
	printf("maxStackElements: %" PRIu16 "\n",      font.maxp->max_stack_elements);
	printf("maxSizeOfInstructions: %" PRIu16 "\n", font.maxp->max_size_of_instructions);
	printf("maxComponentElements: %" PRIu16 "\n",  font.maxp->max_component_elements);
	printf("maxComponentDepth: %" PRIu16 "\n",     font.maxp->max_component_depth);
}
else
{
	printf("Failed to load: %s\n", mutt_result_get_name(font.maxp_res));
}

printf("\n");

/* Print head info */

printf("== Head ==\n");

if (font.head)
{
	printf("fontRevision: %" PRIu16 ".%" PRIu16 "\n", font.head->font_revision_high, font.head->font_revision_low);
	printf("checksumAdjustment: %" PRIu32 "\n", font.head->checksum_adjustment);

	printf("flags: ");
	print_binary((muByte*)&font.head->flags, sizeof(font.head->flags));
	printf("\n");

	printf("unitsPerEm: %" PRIu16 "\n",         font.head->units_per_em);
	printf("created: %" PRIi64 "\n",            font.head->created);
	printf("modified: %" PRIi64 "\n",           font.head->modified);
	printf("xMin: %" PRIi16 "\n",               font.head->x_min);
	printf("yMin: %" PRIi16 "\n",               font.head->y_min);
	printf("xMax: %" PRIi16 "\n",               font.head->x_max);
	printf("yMax: %" PRIi16 "\n",               font.head->y_max);

	printf("macStyle: ");
	print_binary((muByte*)&font.head->mac_style, sizeof(font.head->mac_style));
	printf("\n");

	printf("lowestRecPPEM: %" PRIu16 "\n",      font.head->lowest_rec_ppem);
	printf("fontDirectionHint: %" PRIi16 "\n",  font.head->font_direction_hint);
	printf("indexToLocFormat: %" PRIi16 "\n",   font.head->index_to_loc_format);
	printf("glyphDataFormat: %" PRIi16 "\n",    font.head->glyph_data_format);
}
else
{
	printf("Failed to load: %s\n", mutt_result_get_name(font.head_res));
}

printf("\n");

/* Deload */

{
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

