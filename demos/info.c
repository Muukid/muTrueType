/*
============================================================
                        DEMO INFO

DEMO NAME:          info.c
DEMO WRITTEN BY:    Muukid
CREATION DATE:      2024-07-02
LAST UPDATED:       2024-07-02

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

int main(void) {

/* Load font */

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

/* Print table directory info */

	{
		printf("== Table directory ==\n");

		// Print fixed variables

		printf("numTables: %" PRIu16 "\n", font.directory->num_tables);
		printf("searchRange: %" PRIu16 "\n", font.directory->search_range);
		printf("entrySelector: %" PRIu16 "\n", font.directory->entry_selector);
		printf("range_shift: %" PRIu16 "\n", font.directory->range_shift);

		// Print all table records

		for (uint16_m i = 0; i < font.directory->num_tables; i++) {
			muttTableRecord* record = &font.directory->table_records[i];

			// (tableTag)
			printf("[%c%c%c%c] - ", 
				record->table_tag[0], record->table_tag[1], record->table_tag[2], record->table_tag[3]
			);

			printf("checksum=%" PRIu32 ", ", record->checksum);
			printf("offset=%" PRIu32 " bytes, ", record->offset);
			printf("length=%" PRIu32 " bytes\n", record->length);
		}

		printf("\n");
	}

/* Deload */

	// Deload font
	mutt_deload(&font);

	// Print success
	printf("Successful\n");

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

