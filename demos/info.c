/*
============================================================
                        DEMO INFO

DEMO NAME:          info.c
DEMO WRITTEN BY:    Muukid
CREATION DATE:      2024-08-18
LAST UPDATED:       2024-08-28

============================================================
                        DEMO PURPOSE

This demo tests all of the low-level API, printing out a
bunch of low-level information about the TrueType font.

Note that this demo is dependent on the 'resources' folder
within the demos folder.

============================================================
                        LICENSE INFO

All code is licensed under MIT License or public domain, 
whichever you prefer.
More explicit license information at the end of file.

============================================================
*/

/* Macros */
	
	// Comment/Uncomment this to not print or print glyph coordinates
	#define PRINT_COORDS

/* Inclusion */
	
	// Include muTrueType
	#define MUTT_NAMES // (for name functions)
	#define MUTT_IMPLEMENTATION // (for source code)
	#include "muTrueType.h"

	// For printing and file loading:
	#include <stdio.h>
	
	// For print types:
	#include <inttypes.h>

/* Global variables */
	
	// Font information holder
	muttFont font;

	// Result value holder
	muttResult result = MUTT_SUCCESS;

/* Functions */

	// Prints a value in binary
	void print_binary(muByte* mem, size_m size) {
		// Loop through each byte
		for (size_m byte = 0; byte < size; ++byte) {
			// Loop through each bit
			for (size_m bit = 0; bit < 8; ++bit) {
				// Shift and mask for the bit in this byte
				uint8_m val = (mem[byte] >> bit) & 1;
				// Print bit value
				printf("%" PRIu8 "", val);
			}
		}
	}

	// Prints a UTF16-BE string with the given tab level
	void print_utf16_be(muByte* str, size_m size, uint16_m tab) {
		// Create a null-terminated wide string with one character
		wchar_t ws[2];
		ws[1] = 0;

		// Loop through each character in str:
		for (size_m i = 0; i < size; ++i) {
			// Set character in wide string to the value in str
			ws[0] = (wchar_t)MU_RBEU16(&str[i]);
			// Print character
			printf("%ls", ws);
			// Do tabs if it's a newline character
			if (ws[0] == (wchar_t)'\n') {
				for (uint16_m t = 0; t < tab; ++t) {
					printf("\t");
				}
			}
		}
	}

int main(void)
{
	/* Load font */
	{
		// Open font file
		FILE* fptr = fopen("resources/font.ttf", "rb");
		if (!fptr) {
			printf("Couldn't open 'resources/font.ttf'\n");
			return -1;
		}

		// Get size
		fseek(fptr, 0L, SEEK_END);
		size_m fptr_size = ftell(fptr);
		fseek(fptr, 0L, SEEK_SET);

		// Allocate data
		muByte* data = (muByte*)malloc(fptr_size);
		if (!data) {
			printf("Insufficient memory to open file\n");
			fclose(fptr);
			return -1;
		}

		// Read data into buffer
		fread(data, fptr_size, 1, fptr);

		// Close file
		fclose(fptr);

		// Load font with the data
		result = mutt_load(data, (uint64_m)fptr_size, &font, MUTT_LOAD_ALL);
		free(data); // (No longer need original raw data)

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

	/* Print table directory info */
	{
		printf("== Table directory ==\n");

		// Print number of tables
		printf("numTables: %" PRIu16 "\n", font.directory->num_tables);

		// Loop through each table record
		for (uint16_m r = 0; r < font.directory->num_tables; ++r) {
			// Get table record
			muttTableRecord record = font.directory->records[r];

			// Print table tag
			printf("[%c%c%c%c]\n",
				record.table_tag_u8[0], record.table_tag_u8[1],
				record.table_tag_u8[2], record.table_tag_u8[3]
			);

			// Print other information
			printf("\tchecksum = %" PRIu32 "\n", record.checksum);
			printf("\toffset   = %" PRIu32 " bytes\n", record.offset);
			printf("\tlength   = %" PRIu32 " bytes\n", record.length);
		}

		printf("\n");
	}

	/* Print maxp */
	{
		printf("== Maxp ==\n");

		// Case for if maxp failed to load
		if (!font.maxp) {
			printf("maxp failed to load: %s\n\n", mutt_result_get_name(font.maxp_res));
			goto end_of_maxp;
		}

		// Print values
		printf("version               = %" PRIu16 ".%" PRIu16 "\n", font.maxp->version_high, font.maxp->version_low);
		printf("numGlyphs             = %" PRIu16 "\n", font.maxp->num_glyphs);
		printf("maxPoints             = %" PRIu16 "\n", font.maxp->max_points);
		printf("maxContours           = %" PRIu16 "\n", font.maxp->max_contours);
		printf("maxCompositePoints    = %" PRIu16 "\n", font.maxp->max_composite_points);
		printf("maxCompositeContours  = %" PRIu16 "\n", font.maxp->max_composite_contours);
		printf("maxZones              = %" PRIu16 "\n", font.maxp->max_zones);
		printf("maxTwilightPoints     = %" PRIu16 "\n", font.maxp->max_twilight_points);
		printf("maxStorage            = %" PRIu16 "\n", font.maxp->max_storage);
		printf("maxFunctionDefs       = %" PRIu16 "\n", font.maxp->max_function_defs);
		printf("maxInstructionDefs    = %" PRIu16 "\n", font.maxp->max_instruction_defs);
		printf("maxStackElements      = %" PRIu16 "\n", font.maxp->max_stack_elements);
		printf("maxSizeOfInstructions = %" PRIu16 "\n", font.maxp->max_size_of_instructions);
		printf("maxComponentElements  = %" PRIu16 "\n", font.maxp->max_component_elements);
		printf("maxComponentDepth     = %" PRIu16 "\n", font.maxp->max_component_depth);

		printf("\n");
	}
	end_of_maxp:

	/* Print head */
	{
		printf("== Head ==\n");

		// Case for if head failed to load
		if (!font.head) {
			printf("head failed to load: %s\n\n", mutt_result_get_name(font.head_res));
			goto end_of_head;
		}

		// Print values
		printf("fontRevision         = %" PRIi16 ".%" PRIu16 "\n", font.head->font_revision_high, font.head->font_revision_low);
		printf("checksumAdjustment   = %" PRIu32 "\n", font.head->checksum_adjustment);
		printf("flags                = "); print_binary((muByte*)&font.head->flags, sizeof(font.head->flags)); printf("\n");
		printf("unitsPerEm           = %" PRIu16 "\n", font.head->units_per_em);
		printf("created              = %" PRIi64 "\n", font.head->created);
		printf("modified             = %" PRIi64 "\n", font.head->modified);
		printf("xMin, xMax           = [%" PRIi16 ", %" PRIi16 "]\n", font.head->x_min, font.head->x_max);
		printf("yMin, yMax           = [%" PRIi16 ", %" PRIi16 "]\n", font.head->y_min, font.head->y_max);
		printf("macStyle             = "); print_binary((muByte*)&font.head->mac_style, sizeof(font.head->mac_style)); printf("\n");
		printf("lowestRecPPEM        = %" PRIu16 "\n", font.head->lowest_rec_ppem);
		printf("fontDirectionHint    = %" PRIi16 "\n", font.head->font_direction_hint);
		printf("indexToLocFormat     = %" PRIi16 "\n", font.head->index_to_loc_format);

		printf("\n");
	}
	end_of_head:

	/* Print hhea */
	{
		printf("== Hhea ==\n");

		// Case for if hhea failed to load
		if (!font.hhea) {
			printf("hhea failed to load: %s\n\n", mutt_result_get_name(font.hhea_res));
			goto end_of_hhea;
		}

		// Print values
		printf("ascender            = %" PRIi16 "\n", font.hhea->ascender);
		printf("descender           = %" PRIi16 "\n", font.hhea->descender);
		printf("lineGap             = %" PRIi16 "\n", font.hhea->line_gap);
		printf("advanceWidthMax     = %" PRIu16 "\n", font.hhea->advance_width_max);
		printf("minLeftSideBearing  = %" PRIi16 "\n", font.hhea->min_left_side_bearing);
		printf("minRightSideBearing = %" PRIi16 "\n", font.hhea->min_right_side_bearing);
		printf("xMaxExtent          = %" PRIi16 "\n", font.hhea->x_max_extent);
		printf("caretSlopeRise      = %" PRIi16 "\n", font.hhea->caret_slope_rise);
		printf("caretSlopeRun       = %" PRIi16 "\n", font.hhea->caret_slope_run);
		printf("caretOffset         = %" PRIi16 "\n", font.hhea->caret_offset);
		printf("numberOfHMetrics    = %" PRIu16 "\n", font.hhea->number_of_hmetrics);

		printf("\n");
	}
	end_of_hhea:

	/* Print hmtx */
	{
		printf("== Hmtx ==\n");

		// Case for if hmtx failed to load
		if (!font.hmtx) {
			printf("hmtx failed to load: %s\n\n", mutt_result_get_name(font.hmtx_res));
			goto end_of_hmtx;
		}

		printf("hMetrics[%" PRIu16 "]\n", font.hhea->number_of_hmetrics);
		// Loop through each power of 2 in indexes for hmetrics array
		{
			uint16_m prev_h = 0;
			for (uint16_m h = 0; h < font.hhea->number_of_hmetrics && prev_h <= h; (h==0) ?(h+=1) :(h*=2)) {
				prev_h = h;

				// Print info
				// - Index
				printf("\thMetrics[%" PRIu16 "]\t = { ", h);
				// - advanceWidth
				printf("advanceWidth=%" PRIu16 ", \t", font.hmtx->hmetrics[h].advance_width);
				// - lsb
				printf("lsb=%" PRIu16 " }\n", font.hmtx->hmetrics[h].lsb);
			}
		}

		// Calculate number of elements in leftSideBearings array
		uint16_m lsb_len = font.maxp->num_glyphs - font.hhea->number_of_hmetrics;
		printf("leftSideBearings[%" PRIu16 "]\n", lsb_len);
		// Loop through each power of 2 in indexes for left side bearings array
		{
			uint16_m prev_l = 0;
			for (uint16_m l = 0; l < lsb_len && prev_l <= l; (l==0) ?(l+=1) :(l*=2)) {
				prev_l = l;

				// Print number
				printf("\tlsb[%" PRIu16 "]\t = %" PRIi16 "\n", l, font.hmtx->left_side_bearings[l]);
			}
		}

		printf("\n");
	}
	end_of_hmtx:

	/* Print loca */
	{
		printf("== Loca ==\n");

		// Case for if loca failed to load
		if (!font.loca) {
			printf("loca failed to load: %s\n\n", mutt_result_get_name(font.loca_res));
			goto end_of_loca;
		}

		// Calculate number of offsets
		uint32_m offsets = ((uint32_m)font.maxp->num_glyphs) + 1;

		// Offset16:
		if (font.head->index_to_loc_format == MUTT_OFFSET_16) {
			printf("Offset16[%" PRIu32 "]\n", offsets);

			// Loop through each power of 2 in offset indexes
			uint32_m prev_o = 0;
			for (uint32_m o = 0; o < offsets && prev_o <= o; (o==0) ?(o+=1) :(o*=2)) {
				prev_o = o;
				// Print index
				printf("\toffsets[%" PRIu32 "]\t = %" PRIu16 "\n", o, font.loca->offsets16[o]);
			}
		}
		// Offset 32:
		else {
			printf("Offset32[%" PRIu32 "]\n", offsets);

			// Loop through each power of 2 in offset indexes
			uint32_m prev_o = 0;
			for (uint32_m o = 0; o < offsets && prev_o <= o; (o==0) ?(o+=1) :(o*=2)) {
				prev_o = o;
				// Print index
				printf("offsets[%" PRIu32 "]\t = %" PRIu32 "\n", o, font.loca->offsets32[o]);
			}
		}

		printf("\n");
	}
	end_of_loca:

	/* Print name */
	{
		printf("== Name ==\n");

		// Case for if name failed to load
		if (!font.name) {
			printf("name failed to load: %s\n\n", mutt_result_get_name(font.name_res));
			goto end_of_name;
		}

		// Loop through each name
		printf("nameRecord[%" PRIu16 "]\n", font.name->count);
		for (uint32_m r = 0; r < font.name->count; ++r) {
			muttNameRecord rp = font.name->name_records[r];
			// Print index
			printf("\tnameRecord[%" PRIu16 "]\t = {\n", r);

			// Print platform ID
			printf("\t\tplatformID = %s (%" PRIu16 ")\n", mutt_platform_get_nice_name(rp.platform_id), rp.platform_id);

			// Print encoding ID
			switch (rp.platform_id) {
				// - Unknown
				default: {
					printf("\t\tencodingID = Unknown (%" PRIu16 ")\n", rp.encoding_id);
				} break;
				// - Unicode
				case MUTT_PLATFORM_UNICODE: {
					printf("\t\tencodingID = %s (%" PRIu16 ")\n", mutt_unicode_encoding_get_nice_name(rp.encoding_id), rp.encoding_id);
				} break;
				// - Macintosh
				case MUTT_PLATFORM_MACINTOSH: {
					printf("\t\tencodingID = %s (%" PRIu16 ")\n", mutt_macintosh_encoding_get_nice_name(rp.encoding_id), rp.encoding_id);
				} break;
				// - Windows
				case MUTT_PLATFORM_WINDOWS: {
					printf("\t\tencodingID = %s (%" PRIu16 ")\n", mutt_windows_encoding_get_nice_name(rp.encoding_id), rp.encoding_id);
				} break;
			}

			// Print language ID
			printf("\t\tlanguageID = %" PRIu16 "\n", rp.language_id);
			// Print name ID
			printf("\t\tnameID = %" PRIu16 " (%s)\n", rp.name_id, mutt_name_id_get_nice_name(rp.name_id));

			// Print name as UTF16-BE
			// This will likely produce garbage in many cases
			printf("\t\tname (read as UTF16-BE, may be garbage) {\n\t\t\t");
			print_utf16_be(rp.string, rp.length, 3);
			printf("\n\t\t}\n");

			printf("\t}\n");
		}

		printf("\n");
	}
	end_of_name:

	/* Print glyf */
	{
		printf("== Glyf ==\n");

		// Case for if glyf failed to load
		if (!font.glyf) {
			printf("glyf failed to load: %s\n\n", mutt_result_get_name(font.glyf_res));
			goto end_of_glyf;
		}

		// Allocate data for glyphs
		uint32_m glyfsize = mutt_glyph_max_size(&font);
		printf("Memory allocated for loading a glyph (maximum): %" PRIu32 "\n", glyfsize);
		muByte* glyfdata = 0;
		if (glyfsize > 0) {
			glyfdata = (muByte*)mu_malloc(glyfsize);
			// Failure to allocate:
			if (!glyfdata) {
				printf("Unable to allocate enough memory\n");
				goto end_of_glyf;
			}
		}

		// Loop through each power of 2 in glyph indexes
		uint16_m prev_g = 0;
		for (uint16_m g = 0; g < font.maxp->num_glyphs && prev_g <= g; (g==0) ?(g+=1) :(g*=2)) {
			prev_g = g;
			// Print index
			printf("glyphID %" PRIu16 ":\n", g);

			// Get glyph header
			muttGlyphHeader header;
			result = mutt_glyph_header(&font, g, &header);
			// - Fail case:
			if (mutt_result_is_fatal(result)) {
				printf("\tFailed to load header: %s\n", mutt_result_get_name(result));
				continue;
			}

			// Print number of contours
			printf("\tnumberOfContours\t = %" PRIi16 " (", header.number_of_contours);
			// + Simple/Composite
			if (header.number_of_contours < 0) {
				printf("composite)\n");
			} else {
				printf("simple)\n");
			}

			// Print xMin, xMax
			printf("\txMin, xMax\t = [%" PRIi16 ", %" PRIi16 "]\n", header.x_min, header.x_max);
			// Print yMin, yMax
			printf("\tyMin, yMax\t = [%" PRIi16 ", %" PRIi16 "]\n", header.y_min, header.y_max);

			// Print if it does not have an outline
			if (header.length == 0) {
				printf("\tGlyph has no outline, and thus no glyph data\n");
				continue;
			}

			// For simple glyph:
			if (header.number_of_contours >= 0)
			{
				// Get simple glyph
				uint32_m written;
				muttSimpleGlyph glyph;
				result = mutt_simple_glyph(&font, &header, &glyph, glyfdata, &written);
				// - Fail case:
				if (mutt_result_is_fatal(result)) {
					printf("\tFailed to load simple glyph data: %s\n", mutt_result_get_name(result));
					continue;
				}

				// Print memory usage info
				// - Amount of bytes used
				printf("\t%" PRIu32 " / %" PRIu32 " bytes used (", written, glyfsize);
				// - Percentage of allocated memory
				printf("%f%% of maximum glyph memory)\n", (((float)written) / ((float)glyfsize)) * 100.f);

				// Print instruction length
				printf("\tinstructionLength\t = %" PRIu16 "\n", glyph.instruction_length);

				// Print coordinate info (if we should)
				#ifdef PRINT_COORDS

				// Print contours array init
				printf("\tcontours[%" PRIu16 "] = {", header.number_of_contours);

				// Don't go further if no contours (since we have no coordinates to specify)
				if (header.number_of_contours == 0) {
					printf(" }\n");
					continue;
				}
 
				// Print initializer for this contour
				printf("\n\t\t{ ");

				// Loop through each point
				uint16_m contour_id = 0;
				uint16_m point_count = glyph.end_pts_of_contours[header.number_of_contours-1]+1;
				for (uint16_m p = 0; p < point_count; ++p) {
					// Increment contour ID if we've gone past the current contour's end point
					if (p > glyph.end_pts_of_contours[contour_id]) {
						++contour_id;
						// + Reprint initializer
						printf(" },\n\t\t{ ");
					}
					// If not, print comma for coordinates (if not first coordinates)
					else if (p != 0) {
						printf(",");
					}

					// Print point coordinates
					printf("(%" PRIi16 ",%" PRIi16 ")", glyph.points[p].x, glyph.points[p].y);
				}

				// + Print finisher
				printf("\n\t}\n");

				#endif /* PRINT_COORDS */
			}

			// For composite glyph:
			else
			{
				// Get composite glyph
				uint32_m written;
				muttCompositeGlyph glyph;
				result = mutt_composite_glyph(&font, &header, &glyph, glyfdata, &written);
				// - Fail case:
				if (mutt_result_is_fatal(result)) {
					printf("\tFailed to load composite glyph data: %s\n", mutt_result_get_name(result));
					continue;
				}

				// Print memory usage info
				// - Amount of bytes used
				printf("\t%" PRIu32 " / %" PRIu32 " bytes used (", written, glyfsize);
				// - Percentage of allocated memory
				printf("%f%% of maximum glyph memory)\n", (((float)written) / ((float)glyfsize)) * 100.f);

				// Print instruction length
				printf("\tinstructionLength\t = %" PRIu16 "\n", glyph.instruction_length);
				// Print component count
				printf("\tcomponentCount\t = %" PRIu16 "\n", glyph.component_count);
			}
		}

		// Free glyf memory if needed
		if (glyfdata) {
			mu_free(glyfdata);
		}

		printf("\n");
	}
	end_of_glyf:

	/* Print cmap */
	{
		printf("== Cmap ==\n");

		// Case for if cmap failed to load
		if (!font.cmap) {
			printf("cmap failed to load: %s\n\n", mutt_result_get_name(font.cmap_res));
			goto end_of_cmap;
		}

		// Print # of tables
		printf("encodingRecords[%" PRIu16 "]\n", font.cmap->num_tables);

		// Loop through each table
		for (uint16_m t = 0; t < font.cmap->num_tables; ++t) {
			muttEncodingRecord r = font.cmap->encoding_records[t];
			printf("#%" PRIu16 ":\n", t);

			// platformID
			printf("\tplatformID\t = %s (%" PRIu16 ")\n", mutt_platform_get_nice_name(r.platform_id), r.platform_id);

			// Print encoding ID
			switch (r.platform_id) {
				// Unknown
				default: {
					printf("\tencodingID\t = Unknown (%" PRIu16 ")\n", r.encoding_id);
				} break;
				// Unicode
				case MUTT_PLATFORM_UNICODE: {
					printf("\tencodingID\t = %s (%" PRIu16 ")\n", mutt_unicode_encoding_get_nice_name(r.encoding_id), r.encoding_id);
				} break;
				// Macintosh
				case MUTT_PLATFORM_MACINTOSH: {
					printf("\tencodingID\t = %s (%" PRIu16 ")\n", mutt_macintosh_encoding_get_nice_name(r.encoding_id), r.encoding_id);
				} break;
				// Windows
				case MUTT_PLATFORM_WINDOWS: {
					printf("\tencodingID\t = %s (%" PRIu16 ")\n", mutt_windows_encoding_get_nice_name(r.encoding_id), r.encoding_id);
				} break;
			}

			// Print format
			printf("\tformat\t = %" PRIu16 "\n", r.format);
			// Print result of loading
			printf("\tresult\t = %s\n", mutt_result_get_name(r.result));
		}

		printf("\n");
	}
	end_of_cmap:

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

