/*
============================================================
                        DEMO INFO

DEMO NAME:          info.c
DEMO WRITTEN BY:    Muukid
CREATION DATE:      2024-07-02
LAST UPDATED:       2024-07-07

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
	#define MUTT_PRINT_MEMORY_USAGE
	#define MUTT_IMPLEMENTATION
	#include "muTrueType.h"
	
	// For printing and file loading:
	#include <stdio.h>

	// For print types:
	#include <inttypes.h>

	// For allocation:
	#include <stdlib.h>

	// For string comparison:
	#include <string.h>

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

	// Prints a UTF16-BE string.
	void print_utf16_be(muByte* str, size_m size) {
		wchar_t ws[2];
		ws[1] = 0;

		for (size_m i = 0; i < size; i++) {
			ws[0] = (wchar_t)mu_rbe_uint16(((char*)&str[i]));
			printf("%ls", ws);
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

/* Print hhea info */

printf("== Hhea ==\n");

if (font.hhea)
{
	printf("ascender: %"            PRIi16 "\n", font.hhea->ascender);
	printf("descender: %"           PRIi16 "\n", font.hhea->descender);
	printf("lineGap: %"             PRIi16 "\n", font.hhea->line_gap);
	printf("advanceWidthMax: %"     PRIu16 "\n", font.hhea->advance_width_max);
	printf("minLeftSideBearing: %"  PRIi16 "\n", font.hhea->min_left_side_bearing);
	printf("minRightSideBearing: %" PRIi16 "\n", font.hhea->min_right_side_bearing);
	printf("xMaxExtent: %"          PRIi16 "\n", font.hhea->x_max_extent);
	printf("caretSlopeRise: %"      PRIi16 "\n", font.hhea->caret_slope_rise);
	printf("caretSlopeRun: %"       PRIi16 "\n", font.hhea->caret_slope_run);
	printf("caretOffset: %"         PRIi16 "\n", font.hhea->caret_offset);
	printf("metricDataFormat: %"    PRIi16 "\n", font.hhea->metric_data_format);
	printf("numberOfHMetrics: %"    PRIu16 "\n", font.hhea->number_of_hmetrics);
}
else
{
	printf("Failed to load: %s\n", mutt_result_get_name(font.hhea_res));
}

printf("\n");

/* Print hmtx info */

printf("== Hmtx ==\n");

if (font.hmtx)
{
	// Print various hMetrics
	if (font.hhea->number_of_hmetrics > 0) {
		printf("[Various hMetrics]\n");

		for (uint16_m h = 0; h < font.hhea->number_of_hmetrics && h < (uint16_m)((h+1)*2); h *= 2) {
			printf("%" PRIu16 ": ", h);
			printf("advanceWidth: %" PRIu16 ", ", font.hmtx->hmetrics[h].advance_width);
			printf("lsb: %"          PRIi16 "\n", font.hmtx->hmetrics[h].lsb);

			if (h == 0) {
				h = 1;
			}
		}
	}

	// Print various left side bearings
	uint16_m lsb_count = font.maxp->num_glyphs - font.hhea->number_of_hmetrics;
	if (lsb_count > 0) {
		printf("[Various leftSideBearings]\n");

		for (uint16_m l = 0; l < lsb_count && l < (uint16_m)((l+1)*2); l *= 2) {
			printf("%" PRIu16 ": ", l);
			printf("leftSideBearing: %" PRIi16 "\n", font.hmtx->left_side_bearings[l]);

			if (l == 0) {
				l = 1;
			}
		}
	}
}
else
{
	printf("Failed to load: %s\n", mutt_result_get_name(font.hmtx_res));
}

printf("\n");

/* Print loca info */

printf("== Loca ==\n");

if (font.loca)
{
	// Print offset types
	printf("[Various offsets ");
	if (font.head->index_to_loc_format == MUTT_LOCA_FORMAT_OFFSET16) {
		printf("(16-bit)]\n");
	} else {
		printf("(32-bit)]\n");
	}

	// Print various offsets
	for (uint32_m o = 0; o < (uint32_m)(font.maxp->num_glyphs+1) && o < (uint64_m)((o+1)*2); o *= 2) {
		printf("%" PRIu32 ": ", o);

		if (font.head->index_to_loc_format == MUTT_LOCA_FORMAT_OFFSET16) {
			printf("%" PRIu16 ", ", font.loca->offsets.o16[o]);
		} else {
			printf("%" PRIu32 ", ", font.loca->offsets.o32[o]);
		}

		if (o == 0) {
			o = 1;
		}
	}
}
else
{
	printf("Failed to load: %s\n", mutt_result_get_name(font.loca_res));
}

printf("\n");

/* Print post info */

printf("== Post ==\n");

if (font.post)
{
	printf("version: %" PRIu16 ".%" PRIu16 "\n", font.post->version_high, font.post->version_low);

	printf("italicAngle: %"        PRIi32 "\n", font.post->italic_angle);
	printf("underlinePosition: %"  PRIi16 "\n", font.post->underline_position);
	printf("underlineThickness: %" PRIi16 "\n", font.post->underline_thickness);
	printf("isFixedPitch: %"       PRIu32 "\n", font.post->is_fixed_pitch);
	printf("minMemType42: %"       PRIu32 "\n", font.post->min_mem_type42);
	printf("maxMemType42: %"       PRIu32 "\n", font.post->max_mem_type42);
	printf("minMemType1: %"        PRIu32 "\n", font.post->min_mem_type1);
	printf("maxMemType1: %"        PRIu32 "\n", font.post->max_mem_type1);

	// 2.0 subtable
	if (font.post->version_high == 2 && font.post->version_low == 0) {
		printf("[2.0]\n");

		printf("numGlyphs: %" PRIu16 "\n", font.post->subtable.v20.num_glyphs);

		// Print various glyph name indexes
		for (uint16_m i = 0; i < font.post->subtable.v20.num_glyphs && i < (uint32_m)((i+1)*2); i *= 2) {
			uint16_m name_index = font.post->subtable.v20.glyph_name_index[i];
			printf("#%" PRIu16 ": %" PRIu16 "", i, name_index);

			// Print stringData if exists
			if (name_index >= 258) {
				name_index -= 258;

				// Increment by length until string is reached
				uint8_m* string_data = font.post->subtable.v20.string_data;
				for (uint32_m c = 0; c < name_index; c++) {
					string_data += string_data[0] + 1;
				}
				uint8_m length = string_data[0];

				// Print each character
				printf(" (");
				for (uint16_m i = 1; i <= length; i++) {
					printf("%c", string_data[i]);
				}
				printf(")\n");
			} else {
				printf("\n");
			}

			if (i == 0) {
				i = 1;
			}
		}
	}

	// 2.5 subtable
	else if (font.post->version_high == 2 && font.post->version_low == 5) {
		printf("[2.5]\n");

		printf("numGlyphs: %" PRIu16 "\n", font.post->subtable.v25.num_glyphs);

		// Print various glyph offsets
		for (uint16_m i = 0; i < font.post->subtable.v25.num_glyphs && i < (uint32_m)((i+1)*2); i *= 2) {
			printf("#%" PRIu16 ": %" PRIi8 "\n", i, font.post->subtable.v25.offset[i]);

			if (i == 0) {
				i = 1;
			}
		}
	}
}
else
{
	printf("Failed to load: %s\n", mutt_result_get_name(font.post_res));
}

printf("\n");

/* Print name info */

printf("== Name ==\n");

if (font.name)
{
	printf("version: %"       PRIu16 "\n", font.name->version);
	printf("count: %"         PRIu16 "\n", font.name->count);
	if (font.name->count == 0) {
		goto out_name;
	}
	printf("storageOffset: %" PRIu16 "\n", font.name->storage_offset);

	// Loop through each name record
	if (font.name->count > 0) {
		printf("[Name records]\n");
	}
	for (uint16_m r = 0; r < font.name->count; r++) {
		muttNameRecord record = font.name->name_record[r];

		// Name ID
		const char* name_id = mutt_name_id_get_nice_name(record.name_id);
		if (strcmp(name_id, "MUTT_UNKNOWN") == 0) {
			continue;
		}

		printf("* %s\n", name_id);

		// Platform
		printf("\t[%s", mutt_platform_id_get_nice_name(record.platform_id));

		// Unicode
		if (record.platform_id == MUTT_PLATFORM_UNICODE) {
			printf(", %s", mutt_unicode_encoding_id_get_nice_name(record.encoding_id));
		}
		// Macintosh
		else if (record.platform_id == MUTT_PLATFORM_MACINTOSH) {
			printf(", %s", mutt_macintosh_encoding_id_get_nice_name(record.encoding_id));
			printf(", %s", mutt_macintosh_language_id_get_nice_name(record.language_id));
		}
		// Windows
		else {
			printf(", %s", mutt_windows_encoding_id_get_nice_name(record.encoding_id));
			printf(", lang=%" PRIu16 "", record.language_id);
		}

		printf("]\n");

		// UTF16-BE
		if (
			(record.platform_id == MUTT_PLATFORM_UNICODE) ||
			(record.platform_id == MUTT_PLATFORM_WINDOWS && (
				record.encoding_id == MUTT_WINDOWS_ENCODING_UNICODE_BMP ||
				record.encoding_id == MUTT_WINDOWS_ENCODING_UNICODE_FULL
			))
		) {
			printf("\tUTF16-BE string - ");
			print_utf16_be(&font.name->storage[record.string_offset], record.length);
			printf("\n");
		}
	}

	// Loop through each lang tag
	if (font.name->version == 1 && font.name->lang_tag_count > 0) {
		printf("[Lang tags]\n");

		for (uint16_m l = 0; l < font.name->lang_tag_count; l++) {
			muttLangTagRecord record = font.name->lang_tag_record[l];

			printf("%" PRIu16 " - ", l);
			print_utf16_be(&font.name->storage[record.lang_tag_offset], record.length);
			printf("\n");
		}
	}
}
else
{
	printf("Failed to load: %s\n", mutt_result_get_name(font.name_res));
}

out_name:

printf("\n");

/* Print glyf info */

printf("== Glyf ==\n");

if (font.glyf)
{	
	// Allocation

	uint32_m simple_max = mutt_simple_glyph_get_maximum_size(&font);
	printf("Maximum bytes for simple glyph: %" PRIu32 "\n", simple_max);
	uint32_m composite_max = mutt_composite_glyph_get_maximum_size(&font);
	printf("Maximum bytes for composite glyph: %" PRIu32 "\n", composite_max);

	muByte* simple_mem = (muByte*)malloc(simple_max);
	if (!simple_mem) {
		goto out_glyf;
	}

	muByte* composite_mem = (muByte*)malloc(composite_max);
	if (!composite_mem) {
		free(simple_mem);
		goto out_glyf;
	}

	// Go through various glyph IDs
	for (uint16_m i = 0; i < font.post->subtable.v20.num_glyphs && i < (uint32_m)((i+1)*2); i *= 2) {
		printf("[%" PRIu16 "]\n", i);

		// Get header
		muttGlyphHeader header;
		result = mutt_glyph_get_header(&font, i, &header);
		if (result != MUTT_SUCCESS) {
			printf("Failed to load: %s\n", mutt_result_get_name(result));
			goto end_glyph_loop;
		}

		// Print header info
		printf("\t{ numberOfContours = %" PRIi16 ", ", header.number_of_contours);
		printf("xMin = %" PRIi16 ", ", header.x_min);
		printf("yMin = %" PRIi16 ", ", header.y_min);
		printf("xMax = %" PRIi16 ", ", header.x_max);
		printf("yMax = %" PRIi16 " }\n", header.y_max);

		if (header.length) {
			// Simple glyph handling:
			if (header.number_of_contours > -1) {
				// Get glyph data
				muttSimpleGlyph glyph;
				uint32_m written;
				result = mutt_simple_glyph_get_data(&font, &header, &glyph, simple_mem, &written);
				if (result != MUTT_SUCCESS) {
					printf("Failed to get data: %s\n", mutt_result_get_name(result));
					goto end_glyph_loop;
				}
				printf("\tData = %" PRIu32 " bytes\n", written);

				// Print end points of contours
				if (header.number_of_contours != 0) {
					printf("\tendPtsOfContours[%" PRIi16 "] = { ", header.number_of_contours);

					for (uint16_m c = 0; c < header.number_of_contours; c++) {
						if (c != 0) {
							printf(", ");
						}
						printf("%" PRIu16 "", glyph.end_pts_of_contours[c]);
					}

					printf(" }\n");

					// Go through and print each point, separated by contour
					uint16_m points = glyph.end_pts_of_contours[header.number_of_contours-1] + 1;
					uint16_m contour = 0;

					printf("\tcontour[0] { ");
					for (uint16_m p = 0; p < points; p++) {
						// Print contour
						if (p-1 == glyph.end_pts_of_contours[contour]) {
							contour += 1;
							printf(" \n\t}\n\tcontour[%" PRIu16 "] {\n\t\t", contour);
						}

						// Print a new line for every 8 points
						if (contour == 0) {
							if (p % 8 == 0) {
								printf("\n\t\t");
							}
						} else {
							if ((p-glyph.end_pts_of_contours[contour-1]) % 8 == 0) {
								printf("\n\t\t");
							}
						}

						// Print coordinate
						printf("(%" PRIi16 ", %" PRIi16 ")", glyph.points[p].x, glyph.points[p].y);
						if (p+1 < points && p != glyph.end_pts_of_contours[contour]) {
							printf(", ");
						}
					}
					printf(" \n\t}\n");
				}

			}
			// Composite glyph handling:
			else {
				// Get glyph data
				muttCompositeGlyph glyph;
				uint32_m written;
				result = mutt_composite_glyph_get_data(&font, &header, &glyph, composite_mem, &written);
				if (result != MUTT_SUCCESS) {
					printf("Failed to get data: %s\n", mutt_result_get_name(result));
					goto end_glyph_loop;
				}
				printf("\tData = %" PRIu32 " bytes\n", written);

				// Print instruction length
				printf("\tInstruction length = %" PRIu16 " bytes\n", glyph.instruction_length);

				// Loop through each component
				for (uint16_m c = 0; c < glyph.component_count; c++) {
					muttComponentGlyph* component = &glyph.components[c];

					// Component number
					printf("\tcomponent[%" PRIu16 "] {\n", c);

					// flags
					printf("\t\tflags -  ");
					print_binary((muByte*)&component->flags, sizeof(component->flags));
					printf("\n");

					// glyphIndex
					printf("\t\tglyphIndex - %" PRIu16 "\n", component->glyph_index);

					// argument1
					printf("\t\targument1 - %" PRIi32 "", component->argument1);
					if (component->flags & MUTT_ARGS_ARE_XY_VALUES) {
						printf(" (x-coordinate offset)\n");
					} else {
						printf(" (x-coordinate point number)\n");
					}

					// argument2
					printf("\t\targument2 - %" PRIi32 "", component->argument2);
					if (component->flags & MUTT_ARGS_ARE_XY_VALUES) {
						printf(" (y-coordinate offset)\n");
					} else {
						printf(" (y-coordinate point number)\n");
					}

					// Scale
					if (component->flags & MUTT_WE_HAVE_A_SCALE) {
						printf("Scale - %f\n", component->scales[0]);
					}
					// X-Y scale
					else if (component->flags & MUTT_WE_HAVE_AN_X_AND_Y_SCALE) {
						printf("Scale - [%f, %f]\n", component->scales[0], component->scales[1]);
					}
					// 2x2 scale
					else if (component->flags & MUTT_WE_HAVE_A_TWO_BY_TWO) {
						printf("Scale - [%f, %f, %f, %f]\n", component->scales[0], component->scales[1], component->scales[2], component->scales[3]);
					}

					printf("\t}\n");
				}
			}
		} else {
			printf("No data\n");
		}

		end_glyph_loop:
		if (i == 0) {
			i = 1;
		}
	}

	free(simple_mem);
	free(composite_mem);
}
else
{
	printf("Failed to load: %s\n", mutt_result_get_name(font.glyf_res));
}

printf("\n");

out_glyf:

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

