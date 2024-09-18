

# muTrueType v1.0.0

muTrueType (acronymized to "mutt") is a public domain single-file C library for retrieving data from the TrueType file format via its tables (the "[low-level API](#low-level-api)"), rasterizing glyphs to a bitmap (the "[raster API](#raster-api)"), and handling the layout/placement/spacing of multiple glyphs in a graphical context (the "[layout API](#layout-api)"). Its header is automatically defined upon inclusion if not already included (`MUTT_H`), and the source code is defined if `MUTT_IMPLEMENTATION` is defined, following the internal structure of:

```c
#ifndef MUTT_H
#define MUTT_H
// (Header code)
#endif

#ifdef MUTT_IMPLEMENTATION
// (Source code)
#endif
```

Therefore, a standard inclusion of the file to get all automatic functionality looks like:

```c
#define MUTT_IMPLEMENTATION
#include "muTrueType.h"
```

More information about the general structure of a mu library is provided at [the mu library information GitHub repository](https://github.com/Muukid/mu-library-information).

# Demos

Demos are designed for mutt to both test its functionality and to allow users to get the basic idea of the structure of the library quickly without having to read the documentation in full. These demos are available in the `demos` folder.

## Demo resources

The demos use other files to operate correctly. These other files can be found in the `resources` folder within `demos`, and this folder is expected to be in the same location that the program is executing from. For example, if a user compiles a demo into `main.exe`, and decides to run it, the `resources` folder from `demos` should be in the same directory as `main.exe`.

The resources' licenses may differ from the licensing of mutt itself; see the [license section that covers demo resources](#licensing-of-demo-resources).

# Licensing

mutt is licensed under public domain or MIT, whichever you prefer. More information is provided in the accompanying file `license.md` and at the bottom of `muTrueType.h`.

## Licensing of demo resources

The resources used by the demos may differ from licensing of the demos themselves; in that context, their licenses apply, with licensing of each file available as a separate file with the same name, but with no filename extension.

# Known bugs and limitations

This section covers all of the known bugs and limitations of mutt.

## Instruction support

Currently, mutt does not have any built-in way to execute any TrueType instructions.

## Limited table support

mutt is meant to be fairly simplistic for now, so it only supports reading information from all of the 9 required tables (besides post). Support for other tables may be added in future versions.

## Support for post table

mutt currently does not have support for reading values from the post table, which is one of the 9 required tables in the TrueType specification.

## Limited cmap format support

Currently, mutt only supports loading cmap formats 0, 4, and 12. This should be okay for most cases, with Apple's TrueType spec. saying, "Modern font generation tools might not need to be able to write general-purpose cmaps in formats other than 4 and 12."

## Optimal cmap conversions

mutt does not use the most efficient algorithms to convert codepoints to glyph IDs and vice versa for cmap formats.

## Fairly slow rendering

mutt is not particularly optimized extremely well in its rendering techniques, but could be optimized with fairly minimal effort in the future. It scans horizontal line by horizontal line, converting the glyph to a series of line segments and counting intersections. It considers each line segment for each horizontal line, which could be optimized by sorting the line segments in such a way that once a line is no longer being considered, every line before it is also not being considered. This optimization has yet to be implemented.

# TrueType documentation

Involved usage of the low-level API of mutt necessitates an understanding of the TrueType documentation. Terms from the TrueType documentation will be used with the assumption that the user has read it and understands these terms.

mutt is developed primarily off of these sources of documentation:

* [OpenType spec](https://learn.microsoft.com/en-us/typography/opentype/spec/).

* [TrueType reference manual](https://developer.apple.com/fonts/TrueType-Reference-Manual/).


# Other library dependencies

mutt has a dependency on:

* [muUtility v2.0.1](https://github.com/Muukid/muUtility/releases/tag/v2.0.1).

> Note that mu libraries store their dependencies within their files, so you don't need to import these dependencies yourself; this section is purely to provide more information about the contents that this file defines. The libraries listed may also have other dependencies that they also include that aren't explicitly listed here.

# Loading a TrueType font

All major parts of the mutt API rely on loading a TrueType font and then reading data from it, which is encapsulated in the `muttFont` struct, which is described [later in the lower-level API section](#font-struct). Most casual usage of the mutt API only needs to treat `muttFont` as a handle to the font itself.

## Loading a font

To load a TrueType font into a `muttFont` struct, the function `mutt_load` is used, defined below: 

```c
MUDEF muttResult mutt_load(muByte* data, uint64_m datalen, muttFont* font, muttLoadFlags load_flags);
```


`data` and `datalen` should be the raw binary data of the font file, and should be loaded by the user themselves. `font` is the `muttFont` struct to be filled in with information after loading. `load_flags` is a value whose bits indicate which tables to load; more elaboration on this is given in the [load flags section](#font-load-flags).

If the result returned by mutt is fatal, the contents of `font` are undefined. If the result returned by mutt isn't fatal, the font has been successfully loaded, and must be deloaded at some point.

Once this function has finished executing, there are no internal dependencies on the pointer to the data given, and can be safely freed.

## Deloading a font

To deload a font, the function `mutt_deload` is used, defined below: 

```c
MUDEF void mutt_deload(muttFont* font);
```


This function must be called on every successfully loaded font at some point. The contents of `font` are undefined after `mutt_deload` has been called on it.

## Font load flags

To customize what tables are loaded when loading a TrueType font, the type `muttLoadFlags` exists (typedef for `uint32_m`) whose bits indicate what tables should be loaded. It has the following defined values:

* [0x00000001] `MUTT_LOAD_MAXP` - load the [maxp table](#maxp-table).

* [0x00000002] `MUTT_LOAD_HEAD` - load the [head table](#head-table).

* [0x00000004] `MUTT_LOAD_HHEA` - load the [hhea table](#hhea-table).

* [0x00000008] `MUTT_LOAD_HMTX` - load the [hmtx table](#hmtx-table).

* [0x00000010] `MUTT_LOAD_LOCA` - load the [loca table](#loca-table).

* [0x00000020] `MUTT_LOAD_POST` - load the [post table](#post-table).

* [0x00000040] `MUTT_LOAD_NAME` - load the [name table](#name-table).

* [0x00000080] `MUTT_LOAD_GLYF` - load the [glyf table](#glyf-table).

* [0x00000100] `MUTT_LOAD_CMAP` - load the [cmap table](#cmap-table).

To see which tables successfully loaded, see the [section covering the font struct](#font-struct).

### Font load flag groups

For most users, it is unnecessary or confusing to specify all the tables they want manually, so several macros are defined that set the bits for several tables. These are the defined flag groups:

* [0x000001FF] `MUTT_LOAD_REQUIRED` - load the tables required by the TrueType specification (maxp, head, hhea, hmtx, loca, post, name, glyf, and cmap).

* [0xFFFFFFFF] `MUTT_LOAD_ALL` - loads all tables that could be supported by mutt.

# Low-level API

The low-level API of mutt is designed to support reading information from the tables provided by TrueType. It is used internally by all other parts of the mutt API. All values provided by the low-level API have been checked to be valid, and are guaranteed to be valid once given to the user, unless explicitly stated otherwise.

## Font struct

The font struct, `muttFont`, is the primary way of reading information from TrueType tables, holding pointers to each table's defined data, and is automatically filled using the function [`mutt_load`](#loading-a-font). It has the following members:

* `muttLoadFlags load_flags` - flags indicating which requested tables successfully loaded.

* `muttLoadFlags fail_load_flags` - flags indicating which requested tables did not successfully load.

* `muttDirectory* directory` - a pointer to the [font directory](#font-directory).

* `muttMaxp* maxp` - a pointer to the [maxp table](#maxp-table).

* `muttResult maxp_res` - the result of attempting to load the maxp table.

* `muttHead* head` - a pointer to the [head table](#head-table).

* `muttResult head_res` - the result of attempting to load the head table.

* `muttHhea* hhea` - a pointer to the [hhea table](#hhea-table).

* `muttResult hhea_res` - the result of attempting to load the hhea table.

* `muttHmtx* hmtx` - a pointer to the [hmtx table](#hmtx-table).

* `muttResult hmtx_res` - the result of attempting to load the hmtx table.

* `muttLoca* loca` - a pointer to the [loca table](#loca-table).

* `muttResult loca_res` - the result of attempting to load the loca table.

* `muttPost* post` - a pointer to the post table.

* `muttResult post_res` - the result of attempting to load the post table.

* `muttName* name` - a pointer to the [name table](#name-table).

* `muttResult name_res` - the result of attempting to load the name table.

* `muttGlyf* glyf` - a pointer to the [glyf table](#glyf-table).

* `muttResult glyf_res` - the result of attempting to load the glyf table.

* `muttCmap* cmap` - a pointer to the [cmap table](#cmap-table).

* `muttResult cmap_res` - the result of attempting to load the cmap table.

For each optionally-loadable table within the `muttFont` struct, there exists two members: one that exists as a pointer to the table, and a result value storing the result of attempting to load the table itself. If the respective result value is fatal, or the user never requested for the table to be loaded, the pointer to the table will be 0. Otherwise, the member will be a valid pointer to the table information.

## Font directory

The struct `muttDirectory` is used to list all of the tables provided by a TrueType font. It is stored in the struct `muttFont` as `muttFont.directory` and is similar to TrueType's table directory. It has the following members:

* `uint16_m num_tables` - the amount of tables within the font; equivalent to "numTables" in the table directory.

* `muttTableRecord* records` - pointer to an array of each [table record](#table-record); similar to "tableRecords" in the table directory. This array is of length `num_tables`.

### Table record

The struct `muttTableRecord` represents a table record in the table directory. It has the following members:

* `uint8_m table_tag_u8[4]` - the table tag, represented by four consecutive unsigned 8-bit values representing each character of the table tag.

* `uint32_m table_tag_u32`- the table tag, represented by an unsigned 32-bit value representing the characters of the table tag read as big-endian.

* `uint32_m checksum` - equivalent to "checksum" in the table record.

* `uint32_m offset` - equivalent to "offset" in the table record.

* `uint32_m length` - equivalent to "length" in the table record.

The checksum value is not validated for the head table, as the head table itself includes a checksum value.

## Maxp table

The struct `muttMaxp` is used to represent the maxp table provided by a TrueType font, stored in the struct `muttFont` as the pointer member "`maxp`", and loaded with the flag `MUTT_LOAD_MAXP`. It has the following members:

* `uint16_m version_high` - the high bytes of "version" in the version 1.0 maxp table.

* `uint16_m version_low` - the low bytes of "version" in the version 1.0 maxp table.

* `uint16_m num_glyphs` - equivalent to "numGlyphs" in the version 1.0 maxp table.

* `uint16_m max_points` - equivalent to "maxPoints" in the version 1.0 maxp table.

* `uint16_m max_contours` - equivalent to "maxContours" in the version 1.0 maxp table.

* `uint16_m max_composite_points` - equivalent to "maxCompositePoints" in the version 1.0 maxp table.

* `uint16_m max_composite_contours` - equivalent to "maxCompositeContours" in the version 1.0 maxp table.

* `uint16_m max_zones` - equivalent to "maxZones" in the version 1.0 maxp table.

* `uint16_m max_twilight_points` - equivalent to "maxTwilightPoints" in the version 1.0 maxp table.

* `uint16_m max_storage` - equivalent to "maxStorage" in the version 1.0 maxp table.

* `uint16_m max_function_defs` - equivalent to "maxFunctionDefs" in the version 1.0 maxp table.

* `uint16_m max_instruction_defs` - equivalent to "maxInstructionDefs" in the version 1.0 maxp table.

* `uint16_m max_stack_elements` - equivalent to "maxStackElements" in the version 1.0 maxp table.

* `uint16_m max_size_of_instructions` - equivalent to "maxSizeOfInstructions" in the version 1.0 maxp table.

* `uint16_m max_component_elements` - equivalent to "maxComponentElements" in the version 1.0 maxp table.

* `uint16_m max_component_depth` - equivalent to "maxComponentDepth" in the version 1.0 maxp table.

Since most values given in this table are just maximums, there are only checks performed for the version, numGlyph, and maxZones values. All other values dictate maximums that other tables must follow, and checks will be performed on said tables to ensure they stay within the maximums dictated by maxp.

## Head table

The struct `muttHead` is used to represent the head table provided by a TrueType font, stored in the struct `muttFont` as the pointer member "`head`", and loaded with the flag `MUTT_LOAD_HEAD`. It has the following members:

* `int16_m font_revision_high` - equivalent to the high bytes of "fontRevision" in the head table.

* `uint16_m font_revision_low` - equivalent to the low bytes of "fontRevision" in the head table.

* `uint32_m checksum_adjustment` - equivalent to "checksumAdjustment" in the head table.

* `uint16_m flags` - equivalent to "flags" in the head table.

* `uint16_m units_per_em` - equivalent to "unitsPerEm" in the head table.

* `int64_m created` - equivalent to "created" in the head table.

* `int64_m modified` - equivalent to "modified" in the head table.

* `int16_m x_min` - equivalent to "xMin" in the head table.

* `int16_m y_min` - equivalent to "yMin" in the head table.

* `int16_m x_max` - equivalent to "xMax" in the head table.

* `int16_m y_max` - equivalent to "yMax" in the head table.

* `uint16_m mac_style` - equivalent to "macStyle" in the head table.

* `uint16_m lowest_rec_ppem` - equivalent to "lowestRecPPEM" in the head table.

* `int16_m font_direction_hint` - equivalent to "fontDirectionHint" in the head table.

* `int16_m index_to_loc_format` - equivalent to "indexToLocFormat" in the head table.

Currently, the values for "checksumAdjustment" and "fontDirectionHint" are not checked.

### indexToLocFormat macros

The macros `MUTT_OFFSET_16` (0) and `MUTT_OFFSET_32` (1) are defined to make reading the value of "indexToLocFormat" easier.

## Hhea table

The struct `muttHhea` is used to represent the hhea table provided by a TrueType font, stored in the struct `muttFont` as the pointer member "`hhea`", and loaded with the flag `MUTT_LOAD_HHEA` (`MUTT_LOAD_MAXP` must also be defined). It has the following members:

* `int16_m ascender` - equivalent to "ascender" in the hhea table.

* `int16_m descender` - equivalent to "descender" in the hhea table.

* `int16_m line_gap` - equivalent to "lineGap" in the hhea table.

* `uint16_m advance_width_max` - equivalent to "advanceWidthMax" in the hhea table.

* `int16_m min_left_side_bearing` - equivalent to "minLeftSideBearing" in the hhea table.

* `int16_m min_right_side_bearing` - equivalent to "minRightSideBearing" in the hhea table.

* `int16_m x_max_extent` - equivalent to "xMaxExtent" in the hhea table.

* `int16_m caret_slope_rise` - equivalent to "caretSlopeRise" in the hhea table.

* `int16_m caret_slope_run` - equivalent to "caretSlopeRun" in the hhea table.

* `int16_m caret_offset` - equivalent to "caretOffset" in the hhea table.

* `uint16_m number_of_hmetrics` - equivalent to "numberOfHMetrics" in the hhea table.

All values provided in the `muttHhea` struct are not checked (besides numberOfHMetrics, since it must be less than or equal to `maxp->num_glyphs` in order to generate a valid array length for "leftSideBearings" within hmtx), as virtually all of them have no technically "incorrect" values (from what I'm aware).

## Hmtx table

The struct `muttHmtx` is used to represent the hmtx table provided by a TrueType font, stored in the struct `muttFont` as the pointer member "`hmtx`", and loaded with the flag `MUTT_LOAD_HMTX` (`MUTT_LOAD_MAXP` and `MUTT_LOAD_HHEA` must also be defined). It has the following members:

* `muttLongHorMetric* hmetrics` - an array of horizontal metric records; equiavlent to "hMetrics" in the hmtx table. Its length is equivalent to `hhea->number_of_hmetrics`.

* `int16_m* left_side_bearings` - equivalent to "leftSideBearings" in the hmtx table. Its length is equivalent to `maxp->num_glyphs - hhea->number_of_hmetrics`.

The struct `muttLongHorMetrics` has the following members:

* `uint16_m advance_width` - equivalent to "advanceWidth" in the LongHorMetric record.

* `int16_m lsb` - equivalent to "lsb" in the LongHorMetric record.

All values provided in the `muttHmtx` struct (AKA the values in `muttLongHorMetrics`) are not checked, as virtually all of them have no technically "incorrect" values (from what I'm aware).

## Loca table

The union `muttLoca` is used to represent the loca table provided by a TrueType font, stored in the struct `muttFont` as the pointer member "`loca`", and loaded with the flag `MUTT_LOAD_LOCA` (`MUTT_LOAD_MAXP`, `MUTT_LOAD_HEAD`, and `MUTT_LOAD_GLYF` must also be defined). It has the following members:

* `uint16_m* offsets16` - equivalent to the short-format offsets array in the loca table. This member is to be read from if `head->index_to_loc_format` is equal to `MUTT_OFFSET_16`.

* `uint32_m* offsets32` - equivalent to the long-format offsets array in the loca table. This member is to be read from if `head->index_to_loc_format` is equal to `MUTT_OFFSET_32`.

The offsets are verified to be within range of the glyf table, along with all of the other rules within the specification.

## Name table

The struct `muttName` is used to represent the name table provided by a TrueType font, stored in the struct `muttFont` as the pointer member "`name`", and loaded with the flag `MUTT_LOAD_NAME`. It has the following members:

* `uint16_m version` - equivalent to "version" in the naming table header.

* `uint16_m count` - the amount of name records specified; equivalent to "count" in the naming table header.

* `muttNameRecord* name_records` - all [name records](#name-record) provided (length `count`); equivalent to "nameRecord" in the naming table header.

* `uint16_m lang_tag_count` - the amount of language tags specified; equivalent to "langTagCount" in the naming table header.

* `muttLangTagRecord* lang_tag_records` - all [language tag records](#lang-tag-record) provided (length `lang_tag_count`); equivalent to "langTagRecord" in the naming table header.

* `muByte* string_data` - the raw string data provided by the name table. All pointers to strings provided by the name table are pointers to parts of this data.

### Name record

The struct `muttNameRecord` represents a name record in TrueType. It has the following members:

* `uint16_m platform_id` - the [platform ID](#platform-id); equivalent to "platformID" in the name record.

* `uint16_m encoding_id` - the [encoding ID](#encoding-id); equivalent to "encodingID" in the name record.

* `uint16_m language_id` - the [language ID](#language-id); equivalent to "languageID" in the name record.

* `uint16_m name_id` - the [name ID](#name-id); equivalent to "nameID" in the name record.

* `uint16_m length` - the length of the string, in bytes; equivalent to "length" in the name record.

* `muByte* string` - a pointer to the string data stored within `muttName->string_data` for this given name record.

No platform, encoding, language, or name IDs give bad result values unless the specification explicitly states that the range of values that it's within will never be supported. The provided pointer for `string` is checked to be a pointer to valid data for the given length.

### Lang tag record

The struct `muttLangTagRecord` represents a language tag in TrueType. It has the following members:

* `uint16_m length` - the length of the string, in bytes; equivalent to "length" in the lang tag record.

* `muByte* lang_tag` - a pointer to the string data stored within `muttName->string_data` for this given name record.

The provided pointer for `lang_tag` is checked to be a pointer to valid data for the given length.

## Glyf table

The struct `muttGlyf` is used to represent the glyf table provided by a TrueType font, stored in the struct `muttFont` as the pointer member "`glyf`", and loaded with the flag `MUTT_LOAD_GLYF` (`MUTT_LOAD_MAXP`, `MUTT_LOAD_HEAD`, and `MUTT_LOAD_LOCA` must also be defined). It has the following members:

* `uint32_m len` - the length of the glyf table, in bytes.

* `muByte* data` - the raw byte data of the glyf table (length `len`).

Unlike most low-level table structs, `muttGlyf` provides virtually no information about any glyphs upfront. This is because expanding every single glyph's information can be taxing, so instead, an API is provided to load individual glyph information using the `muttGlyf` struct, which is described below.

### Glyph header

Every glyph, simple or composite, is described initially by its header, which is represented in mutt with the struct `muttGlyphHeader`, which has the following members:

* `int16_m number_of_contours` - equivalent to "numberOfContours" in the glyph header; if this value is negative, the glyph is composite, and if positive or zero, it is simple.

* `int16_m x_min` - equivalent to "xMin" in the glyph header; minimum for x-coordinate data.

* `int16_m y_min` - equivalent to "yMin" in the glyph header; minimum for y-coordinate data.

* `int16_m x_max` - equivalent to "xMax" in the glyph header; maximum for x-coordinate data.

* `int16_m y_max` - equivalent to "yMax" in the glyph header; maximum for y-coordinate data.

* `muByte* data` - a pointer to byte data in `glyf->data` after the header. This is primarily used internally by mutt.

* `uint32_m length` - the length of the data after the header in bytes. If this member is equal to 0, the given glyph has no outline, and should not be called with any functions.

The minimums and maximums for x- and y-coordinates within the glyph header are not checked initially (besides making sure the minimums are less than or equal to the maximums, and that they're within range of the values provided by the head table); if the actual glyph coordinates are not confined within the given minimums and maximums, a bad result will be provided upon loading the simple glyph data.

#### Get glyph header

In order to load a glyph header for a given glyph ID, the function `mutt_glyph_header` is used, defined below: 

```c
MUDEF muttResult mutt_glyph_header(muttFont* font, uint16_m glyph_id, muttGlyphHeader* header);
```


Upon a non-fatal result, `header` is filled with valid header information for the given glyph ID. Upon a fatal result, the contents of `header` are undefined. The given header information is only valid for as long as `font` is not deloaded.

`glyph_id` must be a valid glyph ID for the given font (AKA less than `font->head->num_glyphs`).

### Simple glyph

The struct `muttSimpleGlyph` represents a simple glyph in mutt, and has the following members:

* `uint16_m* end_pts_of_contours` - equivalent to "endPtsOfContours" in the simple glyph table.

* `uint16_m instruction_length` - equivalent to "instructionLength" in the simple glyph table; the length of `instructions`, in bytes.

* `muByte* instructions` - equivalent to "instructions" in the simple glyph table; the instructions for the given glyph.

* `muttGlyphPoint* points` - each point for the simple glyph. The number of points is equal to `end_pts_of_contours[muttGlyphHeader->number_of_contours-1]+1` if `muttGlyphHeader->number_of_contours` is over 0; if `muttGlyphHeader->number_of_contours` is equal to 0, `points` will be equal to 0 as well.

The struct `muttGlyphPoint` represents a point in a simple glyph, and has the following members:

* `uint8_m flags` - equivalent to a value within the "flags" array in the simple glyph table; the [flags of the given point](#glyph-point-flags).

* `int16_m x` - the x-coordinate of the point, in FUnits.

* `int16_m y` - the y-coordinate of the point, in FUnits.

#### Glyph point flags

The following macros are defined for bitmasking a glyph point's flags:

* [0x01] `MUTT_ON_CURVE_POINT`

* [0x02] `MUTT_X_SHORT_VECTOR`

* [0x04] `MUTT_Y_SHORT_VECTOR`

* [0x08] `MUTT_REPEAT_FLAG`

* [0x10] `MUTT_X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR`

* [0x20] `MUTT_Y_IS_SAME_OR_POSITIVE_Y_SHORT_VECTOR`

* [0x40] `MUTT_OVERLAP_SIMPLE`

Note that since the value of `flags` is directly copied from the raw TrueType data, usage of these macros is optional, and the user can bitmask as they please in accordance to the TrueType specification.

#### Load simple glyph

In order to load a simple glyph, the function `mutt_simple_glyph` is used, defined below: 

```c
MUDEF muttResult mutt_simple_glyph(muttFont* font, muttGlyphHeader* header, muttSimpleGlyph* glyph, muByte* data, uint32_m* written);
```


Upon a non-fatal result, `glyph` is filled with valid simple glyph information for the given glyph ID using memory from `data`. Upon a fatal result, the contents of `glyph` and `data` are undefined. The given glyph information is only valid for as long as `font` is not deloaded, and as long as `data` goes unmodified.

This function follows the format of a user-allocated function. For an explanation of how `data` and `written` are supposed to be used within this function, see [the user-allocated function section](#user-allocated-functions).

The x/y min/max values within `header` are overwritten upon a call to this function with correct values. If the values provided in `header` were invalid before mutt overwrites them, the non-fatal result values `MUTT_INVALID_GLYF_SIMPLE_X_COORD` or `MUTT_INVALID_GLYF_SIMPLE_X_COORD` will be given.

> This function checks if all of the values are compliant with information from other tables (especially maxp) and compliant with TrueType's specification with a few exceptions: as far as I'm aware, it's invalid to have a flag that uses values from a prior point (such as X_IS_SAME...) when the current flag is the first flag specified, since in that case, there's no "previous value" to repeat from. This is done in several common fonts, however, so mutt permits this, setting the value to 0 in this case.

> It's also invalid (from what I'm aware) to have the first point be off-curve, but in the case that such happens, mutt permits this, pretending that the previous point was an on-curve point at (0,0). It's also invalid (from what I'm aware) to have a repeat flag count that exceeds the amount of points, but since it's easy to internally make sure to simply not go over the point count, mutt permits this.

#### Simple glyph memory maximum

The maximum amount of memory that will be needed for loading a simple glyph, in bytes, is provided by the function `mutt_simple_glyph_max_size`, defined below: 

```c
MUDEF uint32_m mutt_simple_glyph_max_size(muttFont* font);
```


#### Simple glyph point count

Getting just the amount of points within a simple glyph based on its header is a fairly cheap operating requiring no manual allocation. The function `mutt_simple_glyph_points` calculates the amount of points that a simple glyph contains, defined below: 

```c
MUDEF muttResult mutt_simple_glyph_points(muttFont* font, muttGlyphHeader* header, uint16_m* num_points);
```


Upon a non-fatal result, `num_points` is dereferenced and set to the amount of points within the simple glyph indicated by `header`.

### Composite glyph

The struct `muttCompositeGlyph` represents a composite glyph in mutt, and has the following members:

* `uint16_m component_count` - the number of components within the composite glyph.

* `muttComponentGlyph* components` - an array of each component within the composite glyph.

* `uint16_m instruction_length` - the length of the instructions for the composite glyph, in bytes.

* `muByte* instructions` - the instructions for the composite glyph.

The struct `muttComponentGlyph` represents a component in a composite glyph, and has the following members:

* `uint16_m flags` - equivalent to "flags" in the component glyph record; the [flags for the given component glyph](#glyph-component-flags).

* `uint16_m glyph_index` - equivalent to "glyphIndex" in the component glyph record; the glyph ID of the given component.

* `int32_m argument1` - equivalent to "argument1" in the component glyph record.

* `int32_m argument2` - equivalent to "argument2" in the component glyph record.

* `float scales[4]` - the transform data of the component.

The data of `scales` depends on the value of `flags` (see TrueType/OpenType documentation for more information on how this data works); the following conditions exist:

* If the `MUTT_WE_HAVE_A_SCALE` bit is 1, `scales[0]` is the scale; the contents of all other float indexes are undefined.

* If the `MUTT_WE_HAVE_AN_X_AND_Y_SCALE` bit is 1, `scales[0]` and `scales[1]` are the x- and y-scales respectively; the contents of all other float indexes are undefined.

* If the `MUTT_WE_HAVE_A_TWO_BY_TWO` bit is 1, `scales[0]`, `scales[1]`, `scales[2]`, and `scales[3]` are the 2-by-2 affine transformation values (xscale, scale01, scale10, and yscale respectively).

* If none of the bits mentioned above are 1, the values of `scales` are undefined.

The value for `glyph_index` is not verified to be a non-infinite loop of composite glyphs, and must be manually checked for by the user, unless being converted to a pixel glyph, in which case the conversion checks for this case.

#### Glyph component flags

The following macros are defined for bitmasking a glyph component's flags:

* [0x0001] `MUTT_ARG_1_AND_2_ARE_WORDS`

* [0x0002] `MUTT_ARGS_ARE_XY_VALUES`

* [0x0004] `MUTT_ROUND_XY_TO_GRID`

* [0x0008] `MUTT_WE_HAVE_A_SCALE`

* [0x0020] `MUTT_MORE_COMPONENTS`

* [0x0040] `MUTT_WE_HAVE_AN_X_AND_Y_SCALE`

* [0x0080] `MUTT_WE_HAVE_A_TWO_BY_TWO`

* [0x0100] `MUTT_WE_HAVE_INSTRUCTIONS`

* [0x0200] `MUTT_USE_MY_METRICS`

* [0x0400] `MUTT_OVERLAP_COMPOUND`

* [0x0800] `MUTT_SCALED_COMPONENT_OFFSET`

* [0x1000] `MUTT_UNSCALED_COMPONENT_OFFSET`

Note that since the value of `flags` is retrieved from the TrueType data, usage of these macros is optional, and the user can bitmask as they please in accordance to the TrueType specification.

#### Load composite glyph

In order to load a composite glyph, the function `mutt_composite_glyph` is used, defined below: 

```c
MUDEF muttResult mutt_composite_glyph(muttFont* font, muttGlyphHeader* header, muttCompositeGlyph* glyph, muByte* data, uint32_m* written);
```


Upon a non-fatal result, `glyph` is filled with valid composite glyph information for the given glyph ID using memory from `data`. Upon a fatal result, the contents of `glyph` and `data` are undefined. The given glyph information is only valid for as long as `font` is not deloaded, and as long as `data` goes unmodified.

This function follows the format of a user-allocated function. For an explanation of how `data` and `written` are supposed to be used within this function, see [the user-allocated function section](#user-allocated-functions).

This function performs no checks on the validity of the components' range within the minimum/maximum coordinate ranges specified for the glyph in the respective header. Therefore, this function does allow composite glyphs to successfully load that have points that are out of range. This is due to the fact that properly verifying the points' coordinates would entail fully decompressing the composite glyph's components, which is not performed in the lower-level API of mutt.

#### Composite glyph memory maximum

The maximum amount of memory that will be needed for loading a composite glyph, in bytes, is provided by the function `mutt_composite_glyph_max_size`, defined below: 

```c
MUDEF uint32_m mutt_composite_glyph_max_size(muttFont* font);
```


#### Composite glyph component retrieval

A composite glyph can be processed component-by-component using the function `mutt_composite_component`, defined below: 

```c
MUDEF muttResult mutt_composite_component(muttFont* font, muttGlyphHeader* header, muByte** prog, muttComponentGlyph* component, muBool* no_more);
```


The upside of this is that it requires no upfront manual allocation. This function performs the same checks for validity as `mutt_composite_glyph` besides the component count being valid, which must be tracked by the user themself.

`prog` is a pointer to a pointer holding where mutt is reading TrueType data from, and should be initialized to `header->data` on the first call to `mutt_composite_component`.

Upon a non-fatal result, `component` will be dereferenced & set to the next component within the composite glyph (or the first component upon the first call to `mutt_composite_component`). `prog` will be incremented to the location of the next component, and if there *isn't* a next component, `no_more` will be dereferenced and set to `MU_TRUE`.

#### Composite glyph glyph indexes

The ability to go through each component within a composite glyph individually to get their glyph indexes with limited error checking is provided by the function `mutt_composite_component_glyph`, defined below: 

```c
MUDEF muttResult mutt_composite_component_glyph(muttFont* font, muttGlyphHeader* header, muByte** prog, uint16_m* glyph_index, muBool* no_more);
```


`prog` is a pointer to a pointer holding where mutt is reading TrueType data from, and should be initialized to `header->data` on the first call to `mutt_composite_component_glyph`.

Upon a non-fatal result, `glyph_index` will be dereferenced & set to the glyph index of the current component, `prog` will be incremented to the next component, and if there *isn't* a next component, `no_more` will be dereferenced and set to `MU_TRUE`.

Limited error checking is performed on the composite glyph with this function. For example, the amount of components is not checked to be valid, nor the flag exclusivity of the transform flags when moving past them. The user should make sure that, when calling this function, they make sure that the number of components being scanned is valid when compared to the maximums listed within the maxp table. The glyph index is checkd to be valid before being given to the user.

### Max glyph size

The maximum amount of memory that will be needed for loading a glyph, simple or composite, in bytes, is provided by the function `mutt_glyph_max_size`, defined below: 

```c
MUDEF uint32_m mutt_glyph_max_size(muttFont* font);
```


This function returns the largest value between `mutt_simple_glyph_max_size` and `mutt_composite_glyph_max_size`.

## Cmap table

The struct `muttCmap` is used to represent the cmap table provided by a TrueType font, stored in the struct `muttFont` as the pointer member "`cmap`", and loaded with the flag `MUTT_LOAD_CMAP` (`MUTT_LOAD_MAXP` must also be defined). It has the following members:

* `uint16_m num_tables` - equivalent to "numTables" in the cmap header; the number of encoding records in the `encoding_records` array.

* `muttEncodingRecord* encoding_records` - equivalent to "encodingRecords" in the cmap header; an array of each encoding record in the cmap table.

The union `muttCmapFormat` represents a cmap format for a cmap encoding record. It has the following members:

* `muttCmap0* f0` - [format 0](#format-0).

* `muttCmap4* f4` - [format 4](#format-4).

* `muttCmap12* f12` - [format 12](#format-12).

The struct `muttEncodingRecord` represents an encoding record in the cmap table. It has the following members:

* `uint16_m platform_id` - equivalent to "platformID" in the cmap encoding record; the platform ID of the encoding record.

* `uint16_m encoding_id` - equivalent to "encodingID" in the cmap encoding record; the encoding ID of the encoding record.

* `uint16_m format` - equivalent to "format" in a given cmap subtable format; the format of the encoding record.

* `muttCmapFormat encoding` - a union holding a pointer to the information for the cmap subtable format.

* `muttResult result` - the result of attempting to load the cmap subtable format.

If `result` is a fatal result (most commonly due to the format not being supported (`MUTT_INVALID_CMAP_ENCODING_RECORD_FORMAT`)), the relevant member of `encoding` is 0, unless the format is unsupported, in which case the value of `encoding` is undefined.

The following sections detail how to convert codepoint values to glyph ID values and vice versa, using (at the highest level) the cmap table as a whole, and (at the lowest level) reading data from each format's struct. ***WARNING:*** The glyph ID values returned by a cmap format subtable are *not* checked to be valid upon loading; they are checked to be valid once a function is called for conversion. This means that if the user is using the structs of each format to get glyph ID values instead of using the respective functions, they need to check if the glyph ID values that they retrieve are valid or not, since they are unchecked. Normal checks for all values being within data range are still performed; for example, the loading of format 4 checks if all values within its segments generate a valid index into its glyphIdArray, it just doesn't check if the values within glyphIdArray are valid glyph IDs, that it performed in `mutt_cmap4_get_glyph`.

Codepoint values passed into functions for conversion can be invalid; 0 shall just be returned. Codepoint values returned by a conversion function may not be valid codepoint values for the relevant encoding; the user must check these values themselves. Invalid glyph ID values should not be passed into conversion functions, and conversion functions should not return invalid glyph ID values.

### Top-level cmap

Every implemented cmap format in mutt can retrieve a glyph ID based on a given codepoint and vice versa.

The function `mutt_get_glyph` searches each cmap encoding record specified for the given font and attempts to convert the given codepoint value to a valid glyph ID, defined below: 

```c
MUDEF uint16_m mutt_get_glyph(muttFont* font, uint32_m codepoint);
```


The function `mutt_get_codepoint` searches each cmap encoding record specified for the given font and attempts to convert the given glyph ID to a codepoint value, defined below: 

```c
MUDEF uint32_m mutt_get_codepoint(muttFont* font, uint16_m glyph_id);
```


Both functions return 0 if no equivalent could be found in the conversion process for any cmap encoding record.

### Cmap encoding

The function `mutt_cmap_encoding_get_glyph` converts a given codepoint value to a glyph ID value using the given cmap encoding record, defined below: 

```c
MUDEF uint16_m mutt_cmap_encoding_get_glyph(muttFont* font, muttEncodingRecord* record, uint32_m codepoint);
```


The function `mutt_cmap_encoding_get_codepoint` converts a given glyph ID to a codepoint value using the given cmap encoding record, defined below: 

```c
MUDEF uint32_m mutt_cmap_encoding_get_codepoint(muttFont* font, muttEncodingRecord* record, uint16_m glyph_id);
```


Both functions return 0 if no equivalent could be found in the conversion process for the format of the given encoding record.

### Format 0

The struct `muttCmap0` represents a cmap format 0 subtable, and has the following members:

* `uint16_m language` - equivalent to "language" in the cmap format 0 subtable.

* `uint8_m glyph_ids[256]` - equivalent to "glyphIdArray" in the cmap format 0 subtable.

The function `mutt_cmap0_get_glyph` converts a given codepoint value to a glyph ID value using the given format 0 cmap subtable, defined below: 

```c
MUDEF uint16_m mutt_cmap0_get_glyph(muttFont* font, muttCmap0* f0, uint8_m codepoint);
```


The function `mutt_cmap0_get_codepoint` converts a given glyph ID to a codepoint value using the given format 0 cmap subtable, defined below: 

```c
MUDEF uint8_m mutt_cmap0_get_codepoint(muttFont* font, muttCmap0* f0, uint16_m glyph);
```


Both functions return 0 if no equivalent could be found in the conversion process.

### Format 4

The struct `muttCmap4` represents a cmap format 4 subtable, and has the following members:

* `uint16_m language` - equivalent to "language" in the cmap format 4 subtable.

* `uint16_m seg_count` - equivalent to half of "segCountX2" in the cmap format 4 subtable; the amount of segments in the `seg` array.

* `muttCmap4Segment* seg` - an array of each segment within the cmap format 4 subtable.

* `uint16_m* glyph_ids` - equivalent to "glyphIdArray" in the cmap format 4 subtable; the glyph index array that each segment should return indexes into.

Internally, mutt does not verify or use the values for "searchRange", "entrySelector", or "rangeShift".

The struct `muttCmap4Segment` represents a segment in the cmap format 4 subtable, and has the following members:

* `uint16_m end_code` - equivalent to the value for the given segment in the "endCode" array in the cmap format 4 subtable; the end character code for the given segment.

* `uint16_m start_code` - equivalent to the value for the given segment in the "startCode" array in the cmap format 4 subtable; the start character code for the given segment.

* `int16_m id_delta` - equivalent to the value for the given segment in the "idDelta" array in the cmap format 4 subtable; the delta for the character codes of the given segment.

* `uint16_m id_range_offset` - equivalent to the value for the given segment in the "idRangeOffset" array in the cmap format 4 subtable, but divided by 2 and with (`muttCmap4->seg_count` - the index for the given segment) subtracted; the start code index offset into `muttCmap4->glyph_ids`.

* `uint16_m start_glyph_id` - the calculated first glyph ID of the segment. This is not checked to be a valid glyph ID, and is used when converting glyph IDs into codepoints.

* `uint16_m end_glyph_id` - the calculated last glyph ID of the segment. This is not checked to be a valid glyph ID, and is used when converting glyph IDs into codepoints.

> In TrueType, idRangeOffset is stored as an offset into the glyphIdArray for the start code, to which every code in the segment adds an offset to get the next value (`codepoint-start_code` incrementally offsets, beginning at the start code as 0). However, this offset is stored relative to `&idRangeOffset[segment]`, so this offset is accounted for when loading the format in mutt by subtracting `seg_count-segment` for each value in idRangeOffset. The value is also first divided by 2 (before the subtraction) since, internally, the value is accessed by directly indexing into `glyph_ids`, but idRangeOffset values store actual byte offsets, so the 2 bytes per glyph ID must be accounted for. Glyph IDs are then internally retrieved (after being verified to be valid offsets) via `f4->glyph_ids[seg->id_range_offset + (codepoint - seg->start_code)]` (followed by delta logic), and this is how the user should also retrieve them when manually reading from the `muttCmap4` struct.

The function `mutt_cmap4_get_glyph` converts a given codepoint value to a glyph ID value using the given format 4 cmap subtable, defined below: 

```c
MUDEF uint16_m mutt_cmap4_get_glyph(muttFont* font, muttCmap4* f4, uint16_m codepoint);
```


The function `mutt_cmap4_get_codepoint` converts a given glyph ID to a codepoint value using the given format 4 cmap subtable, defined below: 

```c
MUDEF uint16_m mutt_cmap4_get_codepoint(muttFont* font, muttCmap4* f4, uint16_m glyph);
```


Both functions return 0 if no equivalent could be found in the conversion process.

### Format 12

The struct `muttCmap12` represents a cmap format 12 subtable, and has the following members:

* `uint32_m language` - equivalent to "language" in the cmap format 12 subtable.

* `uint32_m num_groups` - equivalent to "numGroups" in the cmap format 12 subtable; the amount of groups in the `groups` array.

* `muttCmap12Group* groups` - equivalent to "groups" in the cmap format 12 subtable; an array of each map group.

The struct `muttCmap12Group` represents a sequential map group in the cmap format 12 subtable, and has the following members:

* `uint32_m start_char_code` - equivalent to "startCharCode" in the sequential map group record; the first character code for the given group.

* `uint32_m end_char_code` - equivalent to "endCharCode" in the sequential map group record; the last character code for the given group.

* `uint32_m start_glyph_id` - equivalent to "startGlyphID" in the sequential map group record; the glyph ID for the first character code.

The function `mutt_cmap12_get_glyph` converts a given codepoint value to a glyph ID value using the given format 12 cmap subtable, defined below: 

```c
MUDEF uint16_m mutt_cmap12_get_glyph(muttFont* font, muttCmap12* f12, uint32_m codepoint);
```


The function `mutt_cmap12_get_codepoint` converts a given glyph ID to a codepoint value using the given format 12 cmap subtable, defined below: 

```c
MUDEF uint32_m mutt_cmap12_get_codepoint(muttFont* font, muttCmap12* f12, uint16_m glyph);
```


Both functions return 0 if no equivalent could be found in the conversion process.

## User allocated functions

"User-allocated functions" are functions used in mutt to allow the user to handle allocation of memory necessary to perform certain low-level operations. These functions usually have two distinct members that make this possible, `muByte* data` and `uint32_m* written`, with the functions usually following the format of:

```c
MUDEF muttResult mutt_whatever(muttFont* font, ... muByte* data, uint32_m* written);
```

The way these functions work depends on the value of `data`:

* If `data` is 0, `written` is dereferenced and set to how many bytes are needed within `data` in order to perform the operation.

* If `data` is not 0, `data` is used as allocated memory to perform whatever task is needed, and assumes that `data` is large enough to perform the operation. If `written` is not 0 in this instance, `written` is dereferenced and set to how much data was used in the operation.

These functions are setup so that the user has full control over the allocation of the data for the given process.

If these functions are ever used to fill information about something (in particular, if it's filling up a struct like `mutt_simple_glyph` or `mutt_composite_glyph`), the filled-out structs use `data` as memory for the information provided by them. This means that the information with the struct is only valid for as long as `data` goes unmodified by the user.

User-allocated functions do not expect the given memory to be zero'd out, and the contents of `data`, once filled in, are undefined.

User-allocated functions usually also have provided maximums for the maximum amount of memory that will be needed to perform the operation for any given variables, which can be used to pre-allocate and use the same memory for multiple passes of the operation. For example, `mutt_simple_glyph` has a memory maximum provided by the function `mutt_simple_glyph_max_size`, which means that a user can allocate memory of byte-length `mutt_simple_glyph_max_size(...)` and use that same memory for a call to `mutt_simple_glyph` for any glyphs that they need to load, avoiding the need to reallocate any memory when processing simple glyphs, although at the cost of only being able to process one simple glyph at a time.

## String macros

This section covers macros defined for platform, encoding, language, and name IDs. Note that values may be given that don't fit into any of the given macros.

### Platform ID

The following macros are defined for platform IDs:

* [0x0000] `MUTT_PLATFORM_UNICODE` - [Unicode platform](#unicode-encoding).

* [0x0001] `MUTT_PLATFORM_MACINTOSH` - [Macintosh platform](#macintosh-encoding).

* [0x0002] `MUTT_PLATFORM_ISO` - [ISO platform](#iso-encoding).

* [0x0003] `MUTT_PLATFORM_WINDOWS` - [Windows platform](#windows-encoding).

* [0x0004] `MUTT_PLATFORM_CUSTOM` - custom encoding.

#### Platform ID names

The name function `mutt_platform_get_name` returns a `const char*` representation of a given platform ID (for example, `MUTT_PLATFORM_UNICODE` returns "MUTT_PLATFORM_UNICODE"), defined below: 

```c
MUDEF const char* mutt_platform_get_name(uint16_m platform_id);
```


This function returns "MU_UNKNOWN" in the case that `platform_id` is an unrecognized value.

The name function `mutt_platform_get_nice_name` does the same thing, but returns a more readable version of it (for example, `MUTT_PLATFORM_UNICODE` returns "Unicode"), defined below: 

```c
MUDEF const char* mutt_platform_get_nice_name(uint16_m platform_id);
```


This function returns "Unknown" in the case that `platform_id` is an unrecognized value.

> Note that these are name functions, and are only defined if `MUTT_NAMES` is also defined.

### Encoding ID

The following macros are defined for various platform encoding IDs:

#### Unicode encoding

* [0x0000] `MUTT_UNICODE_1_0` - Unicode 1.0.

* [0x0001] `MUTT_UNICODE_1_1` - Unicode 1.1.

* [0x0002] `MUTT_UNICODE_ISO_IEC_10646` - ISO/IEC 10646.

* [0x0003] `MUTT_UNICODE_2_0_BMP` - Unicode 2.0, BMP only.

* [0x0004] `MUTT_UNICODE_2_0` - Unicode 2.0.

* [0x0005] `MUTT_UNICODE_VARIATION` - Unicode variation sequences.

* [0x0006] `MUTT_UNICODE_FULL` - Unicode "full repertoire".

##### Unicode encoding names

The name function `mutt_unicode_encoding_get_name` returns a `const char*` representation of a given Unicode encoding ID (for example, `MUTT_UNICODE_1_0` returns "MUTT_UNICODE_1_0"), defined below: 

```c
MUDEF const char* mutt_unicode_encoding_get_name(uint16_m encoding_id);
```


This function returns "MU_UNKNOWN" in the case that `encoding_id` is an unrecognized value.

The name function `mutt_unicode_encoding_get_nice_name` does the same thing, but returns a more readable version of it (for example, `MUTT_UNICODE_1_0` returns "Unicode 1.0"), defined below: 

```c
MUDEF const char* mutt_unicode_encoding_get_nice_name(uint16_m encoding_id);
```


This function returns "Unknown" in the case that `encoding_id` is an unrecognized value.

> Note that these are name functions, and are only defined if `MUTT_NAMES` is also defined.

#### Macintosh encoding

* [0x0000] `MUTT_MACINTOSH_ROMAN` - Roman.

* [0x0001] `MUTT_MACINTOSH_JAPANESE` - Japanese.

* [0x0002] `MUTT_MACINTOSH_CHINESE_TRADITIONAL` - Chinese (Traditional).

* [0x0003] `MUTT_MACINTOSH_KOREAN` - Korean.

* [0x0004] `MUTT_MACINTOSH_ARABIC` - Arabic.

* [0x0005] `MUTT_MACINTOSH_HEBREW` - Hebrew.

* [0x0006] `MUTT_MACINTOSH_GREEK` - Greek.

* [0x0007] `MUTT_MACINTOSH_RUSSIAN` - Russian.

* [0x0008] `MUTT_MACINTOSH_RSYMBOL` - RSymbol.

* [0x0009] `MUTT_MACINTOSH_DEVANAGARI` - Devanagari.

* [0x000A] `MUTT_MACINTOSH_GURMUKHI` - Gurmukhi.

* [0x000B] `MUTT_MACINTOSH_GUJARATI` - Gujarati.

* [0x000C] `MUTT_MACINTOSH_ODIA` - Odia.

* [0x000D] `MUTT_MACINTOSH_BANGLA` - Bangla.

* [0x000E] `MUTT_MACINTOSH_TAMIL` - Tamil.

* [0x000F] `MUTT_MACINTOSH_TELUGU` - Telugu.

* [0x0010] `MUTT_MACINTOSH_KANNADA` - Kannada.

* [0x0011] `MUTT_MACINTOSH_MALAYALAM` - Malayalam.

* [0x0012] `MUTT_MACINTOSH_SINHALESE` - Sinhalese.

* [0x0013] `MUTT_MACINTOSH_BURMESE` - Burmese.

* [0x0014] `MUTT_MACINTOSH_KHMER` - Khmer.

* [0x0015] `MUTT_MACINTOSH_THAI` - Thai.

* [0x0016] `MUTT_MACINTOSH_LAOTIAN` - Laotian.

* [0x0017] `MUTT_MACINTOSH_GEORGIAN` - Georgian.

* [0x0018] `MUTT_MACINTOSH_ARMENIAN` - Armenian.

* [0x0019] `MUTT_MACINTOSH_CHINESE_SIMPLIFIED` - Chinese (Simplified).

* [0x001A] `MUTT_MACINTOSH_TIBETAN` - Tibetan.

* [0x001B] `MUTT_MACINTOSH_MONGOLIAN` - Mongolian.

* [0x001C] `MUTT_MACINTOSH_GEEZ` - Geez.

* [0x001D] `MUTT_MACINTOSH_SLAVIC` - Slavic.

* [0x001E] `MUTT_MACINTOSH_VIETNAMESE` - Vietnamese.

* [0x001F] `MUTT_MACINTOSH_SINDHI` - Sindhi.

* [0x0020] `MUTT_MACINTOSH_UNINTERPRETED` - Uninterpreted.

##### Macintosh encoding names

The name function `mutt_macintosh_encoding_get_name` returns a `const char*` representation of a given Macintosh encoding ID (for example, `MUTT_MACINTOSH_ROMAN` returns "MUTT_MACINTOSH_ROMAN"), defined below: 

```c
MUDEF const char* mutt_macintosh_encoding_get_name(uint16_m encoding_id);
```


This function returns "MU_UNKNOWN" in the case that `encoding_id` is an unrecognized value.

The name function `mutt_macintosh_encoding_get_nice_name` does the same thing, but returns a more readable version of it (for example, `MUTT_MACINTOSH_ROMAN` returns "Roman"), defined below: 

```c
MUDEF const char* mutt_macintosh_encoding_get_nice_name(uint16_m encoding_id);
```


This function returns "Unknown" in the case that `encoding_id` is an unrecognized value.

> Note that these are name functions, and are only defined if `MUTT_NAMES` is also defined.

#### ISO encoding

* [0x0000] `MUTT_ISO_7_BIT_ASCII` - 7-bit ASCII.

* [0x0001] `MUTT_ISO_10646` - ISO 10646.

* [0x0002] `MUTT_ISO_8859_1` - ISO 8859-1.

#### Windows encoding

* [0x0000] `MUTT_WINDOWS_SYMBOL` - Symbol.

* [0x0001] `MUTT_WINDOWS_UNICODE_BMP` - Unicode BMP.

* [0x0002] `MUTT_WINDOWS_SHIFTJIS` - ShiftJIS.

* [0x0003] `MUTT_WINDOWS_PRC` - PRC.

* [0x0004] `MUTT_WINDOWS_BIG5` - Big5.

* [0x0005] `MUTT_WINDOWS_WANSUNG` - Wansung.

* [0x0006] `MUTT_WINDOWS_JOHAB` - Johab.

* [0x000A] `MUTT_WINDOWS_UNICODE` - Unicode full repertoire.

##### Windows encoding names

The name function `mutt_windows_encoding_get_name` returns a `const char*` representation of a given Windows encoding ID (for example, `MUTT_WINDOWS_SYMBOL` returns "MUTT_WINDOWS_SYMBOL"), defined below: 

```c
MUDEF const char* mutt_windows_encoding_get_name(uint16_m encoding_id);
```


This function returns "MU_UNKNOWN" in the case that `encoding_id` is an unrecognized value.

The name function `mutt_windows_encoding_get_nice_name` does the same thing, but returns a more readable version of it (for example, `MUTT_WINDOWS_SYMBOL` returns "Symbol"), defined below: 

```c
MUDEF const char* mutt_windows_encoding_get_nice_name(uint16_m encoding_id);
```


This function returns "Unknown" in the case that `encoding_id` is an unrecognized value.

> Note that these are name functions, and are only defined if `MUTT_NAMES` is also defined.

### Language ID

The following macros are defined for various language IDs:

#### Macintosh language

* [0x0000] `MUTT_MACINTOSH_LANG_ENGLISH` - English.

* [0x0001] `MUTT_MACINTOSH_LANG_FRENCH` - French.

* [0x0002] `MUTT_MACINTOSH_LANG_GERMAN` - German.

* [0x0003] `MUTT_MACINTOSH_LANG_ITALIAN` - Italian.

* [0x0004] `MUTT_MACINTOSH_LANG_DUTCH` - Dutch.

* [0x0005] `MUTT_MACINTOSH_LANG_SWEDISH` - Swedish.

* [0x0006] `MUTT_MACINTOSH_LANG_SPANISH` - Spanish.

* [0x0007] `MUTT_MACINTOSH_LANG_DANISH` - Danish.

* [0x0008] `MUTT_MACINTOSH_LANG_PORTUGUESE` - Portuguese.

* [0x0009] `MUTT_MACINTOSH_LANG_NORWEGIAN` - Norwegian.

* [0x000A] `MUTT_MACINTOSH_LANG_HEBREW` - Hebrew.

* [0x000B] `MUTT_MACINTOSH_LANG_JAPANESE` - Japanese.

* [0x000C] `MUTT_MACINTOSH_LANG_ARABIC` - Arabic.

* [0x000D] `MUTT_MACINTOSH_LANG_FINNISH` - Finnish.

* [0x000E] `MUTT_MACINTOSH_LANG_GREEK` - Greek.

* [0x000F] `MUTT_MACINTOSH_LANG_ICELANDIC` - Icelandic.

* [0x0010] `MUTT_MACINTOSH_LANG_MALTESE` - Maltese.

* [0x0011] `MUTT_MACINTOSH_LANG_TURKISH` - Turkish.

* [0x0012] `MUTT_MACINTOSH_LANG_CROATIAN` - Croatian.

* [0x0013] `MUTT_MACINTOSH_LANG_CHINESE_TRADITIONAL` - Chinese (traditional).

* [0x0014] `MUTT_MACINTOSH_LANG_URDU` - Urdu.

* [0x0015] `MUTT_MACINTOSH_LANG_HINDI` - Hindi.

* [0x0016] `MUTT_MACINTOSH_LANG_THAI` - Thai.

* [0x0017] `MUTT_MACINTOSH_LANG_KOREAN` - Korean.

* [0x0018] `MUTT_MACINTOSH_LANG_LITHUANIAN` - Lithuanian.

* [0x0019] `MUTT_MACINTOSH_LANG_POLISH` - Polish.

* [0x001A] `MUTT_MACINTOSH_LANG_HUNGARIAN` - Hungarian.

* [0x001B] `MUTT_MACINTOSH_LANG_ESTONIAN` - Estonian.

* [0x001C] `MUTT_MACINTOSH_LANG_LATVIAN` - Latvian.

* [0x001D] `MUTT_MACINTOSH_LANG_SAMI` - Sami.

* [0x001E] `MUTT_MACINTOSH_LANG_FAROESE` - Faroese.

* [0x001F] `MUTT_MACINTOSH_LANG_FARSI_PERSIAN` - Farsi/Persian.

* [0x0020] `MUTT_MACINTOSH_LANG_RUSSIAN` - Russian.

* [0x0021] `MUTT_MACINTOSH_LANG_CHINESE_SIMPLIFIED` - Chinese (simplified).

* [0x0022] `MUTT_MACINTOSH_LANG_FLEMISH` - Flemish.

* [0x0023] `MUTT_MACINTOSH_LANG_IRISH_GAELIC` - Irish Gaelic.

* [0x0024] `MUTT_MACINTOSH_LANG_ALBANIAN` - Albanian.

* [0x0025] `MUTT_MACINTOSH_LANG_ROMANIAN` - Romanian.

* [0x0026] `MUTT_MACINTOSH_LANG_CZECH` - Czech.

* [0x0027] `MUTT_MACINTOSH_LANG_SLOVAK` - Slovak.

* [0x0028] `MUTT_MACINTOSH_LANG_SLOVENIAN` - Slovenian.

* [0x0029] `MUTT_MACINTOSH_LANG_YIDDISH` - Yiddish.

* [0x002A] `MUTT_MACINTOSH_LANG_SERBIAN` - Serbian.

* [0x002B] `MUTT_MACINTOSH_LANG_MACEDONIAN` - Macedonian.

* [0x002C] `MUTT_MACINTOSH_LANG_BULGARIAN` - Bulgarian.

* [0x002D] `MUTT_MACINTOSH_LANG_UKRANIAN` - Ukrainian.

* [0x002E] `MUTT_MACINTOSH_LANG_BYELORUSSIAN` - Byelorussian.

* [0x002F] `MUTT_MACINTOSH_LANG_UZBEK` - Uzbek.

* [0x0030] `MUTT_MACINTOSH_LANG_KAZAKH` - Kazakh.

* [0x0031] `MUTT_MACINTOSH_LANG_AZERBAIJANI_CYRILLIC` - Azerbaijani (Cyrillic script).

* [0x0032] `MUTT_MACINTOSH_LANG_AZERBAIJANI_ARABIC` - Azerbaijani (Arabic script).

* [0x0033] `MUTT_MACINTOSH_LANG_ARMENIAN` - Armenian.

* [0x0034] `MUTT_MACINTOSH_LANG_GEORGIAN` - Georgian.

* [0x0035] `MUTT_MACINTOSH_LANG_MOLDAVIAN` - Moldavian.

* [0x0036] `MUTT_MACINTOSH_LANG_KIRGHIZ` - Kirghiz.

* [0x0037] `MUTT_MACINTOSH_LANG_TAJIKI` - Tajiki.

* [0x0038] `MUTT_MACINTOSH_LANG_TURKMEN` - Turkmen.

* [0x0039] `MUTT_MACINTOSH_LANG_MONGOLIAN` - Mongolian (Mongolian script).

* [0x003A] `MUTT_MACINTOSH_LANG_MONGOLIAN_CYRILLIC` - Mongolian (Cyrillic script).

* [0x003B] `MUTT_MACINTOSH_LANG_PASHTO` - Pashto.

* [0x003C] `MUTT_MACINTOSH_LANG_KURDISH` - Kurdish.

* [0x003D] `MUTT_MACINTOSH_LANG_KASHMIRI` - Kashmiri.

* [0x003E] `MUTT_MACINTOSH_LANG_SINDHI` - Sindhi.

* [0x003F] `MUTT_MACINTOSH_LANG_TIBETAN` - Tibetan.

* [0x0040] `MUTT_MACINTOSH_LANG_NEPALI` - Nepali.

* [0x0041] `MUTT_MACINTOSH_LANG_SANSKIRT` - Sanskrit.

* [0x0042] `MUTT_MACINTOSH_LANG_MARATHI` - Marathi.

* [0x0043] `MUTT_MACINTOSH_LANG_BENGALI` - Bengali.

* [0x0044] `MUTT_MACINTOSH_LANG_ASSAMESE` - Assamese.

* [0x0045] `MUTT_MACINTOSH_LANG_GUJARATI` - Gujarati.

* [0x0046] `MUTT_MACINTOSH_LANG_PUNJABI` - Punjabi.

* [0x0047] `MUTT_MACINTOSH_LANG_ORIYA` - Oriya.

* [0x0048] `MUTT_MACINTOSH_LANG_MALAYALAM` - Malayalam.

* [0x0049] `MUTT_MACINTOSH_LANG_KANNADA` - Kannada.

* [0x004A] `MUTT_MACINTOSH_LANG_TAMIL` - Tamil.

* [0x004B] `MUTT_MACINTOSH_LANG_TELUGU` - Telugu.

* [0x004C] `MUTT_MACINTOSH_LANG_SINHALESE` - Sinhalese.

* [0x004D] `MUTT_MACINTOSH_LANG_BURMESE` - Burmese.

* [0x004E] `MUTT_MACINTOSH_LANG_KHMER` - Khmer.

* [0x004F] `MUTT_MACINTOSH_LANG_LAO` - Lao.

* [0x0050] `MUTT_MACINTOSH_LANG_VIETNAMESE` - Vietnamese.

* [0x0051] `MUTT_MACINTOSH_LANG_INDONESIAN` - Indonesian.

* [0x0052] `MUTT_MACINTOSH_LANG_TAGALOG` - Tagalog.

* [0x0053] `MUTT_MACINTOSH_LANG_MALAY_ROMAN` - Malay (Roman script).

* [0x0054] `MUTT_MACINTOSH_LANG_MALAY_ARABIC` - Malay (Arabic script).

* [0x0055] `MUTT_MACINTOSH_LANG_AMHARIC` - Amharic.

* [0x0056] `MUTT_MACINTOSH_LANG_TIGRINYA` - Tigrinya.

* [0x0057] `MUTT_MACINTOSH_LANG_GALLA` - Galla.

* [0x0058] `MUTT_MACINTOSH_LANG_SOMALI` - Somali.

* [0x0059] `MUTT_MACINTOSH_LANG_SWAHILI` - Swahili.

* [0x005A] `MUTT_MACINTOSH_LANG_KINYARWANDA_RUANDA` - Kinyarwanda/Ruanda.

* [0x005B] `MUTT_MACINTOSH_LANG_RUNDI` - Rundi.

* [0x005C] `MUTT_MACINTOSH_LANG_NYANJA_CHEWA` - Nyanja/Chewa.

* [0x005D] `MUTT_MACINTOSH_LANG_MALAGASY` - Malagasy.

* [0x005E] `MUTT_MACINTOSH_LANG_ESPERANTO` - Esperanto.

* [0x0080] `MUTT_MACINTOSH_LANG_WELSH` - Welsh.

* [0x0081] `MUTT_MACINTOSH_LANG_BASQUE` - Basque.

* [0x0082] `MUTT_MACINTOSH_LANG_CATALAN` - Catalan.

* [0x0083] `MUTT_MACINTOSH_LANG_LATIN` - Latin.

* [0x0084] `MUTT_MACINTOSH_LANG_QUECHUA` - Quechua.

* [0x0085] `MUTT_MACINTOSH_LANG_GUARANI` - Guarani.

* [0x0086] `MUTT_MACINTOSH_LANG_AYMARA` - Aymara.

* [0x0087] `MUTT_MACINTOSH_LANG_TATAR` - Tatar.

* [0x0088] `MUTT_MACINTOSH_LANG_UIGHUR` - Uighur.

* [0x0089] `MUTT_MACINTOSH_LANG_DZONGKHA` - Dzongkha.

* [0x008A] `MUTT_MACINTOSH_LANG_JAVANESE_ROMAN` - Javanese (Roman script).

* [0x008B] `MUTT_MACINTOSH_LANG_SUNDANESE_ROMAN` - Sundanese (Roman script).

* [0x008C] `MUTT_MACINTOSH_LANG_GALICIAN` - Galician.

* [0x008D] `MUTT_MACINTOSH_LANG_AFRIKAANS` - Afrikaans.

* [0x008E] `MUTT_MACINTOSH_LANG_BRETON` - Breton.

* [0x008F] `MUTT_MACINTOSH_LANG_INUKTITUT` - Inuktitut.

* [0x0090] `MUTT_MACINTOSH_LANG_SCOTTISH_GAELIC` - Scottish Gaelic.

* [0x0091] `MUTT_MACINTOSH_LANG_MANX_GAELIC` - Manx Gaelic.

* [0x0092] `MUTT_MACINTOSH_LANG_IRISH_GAELIC_DOT_ABOVE` - Irish Gaelic (with dot above).

* [0x0093] `MUTT_MACINTOSH_LANG_TONGAN` - Tongan.

* [0x0094] `MUTT_MACINTOSH_LANG_GREEK_POLYTONIC` - Greek (polytonic).

* [0x0095] `MUTT_MACINTOSH_LANG_GREENLANDIC` - Greenlandic.

* [0x0096] `MUTT_MACINTOSH_LANG_AZERBAIJANI_ROMAN` - Azerbaijani (Roman script).

### Name ID

The follwing macros are defined for various name IDs:

* [0x0000] `MUTT_NAME_COPYRIGHT_NOTICE` - "Copyright notice."

* [0x0001] `MUTT_NAME_FONT_FAMILY` - "Font Family name."

* [0x0002] `MUTT_NAME_FONT_SUBFAMILY` - "Font Subfamily name."

* [0x0003] `MUTT_NAME_FONT_IDENTIFIER` - "Unique font identifier."

* [0x0004] `MUTT_NAME_FONT_FULL` - "Full font name that reflects all family and relevant subfamily descriptors".

* [0x0005] `MUTT_NAME_VERSION` - "Version string."

* [0x0006] `MUTT_NAME_POSTSCRIPT` - "PostScript name for the font."

* [0x0007] `MUTT_NAME_TRADEMARK` - "Trademark."

* [0x0008] `MUTT_NAME_MANUFACTURER` - "Manufacturer Name."

* [0x0009] `MUTT_NAME_DESIGNER` - "Designer."

* [0x000A] `MUTT_NAME_DESCRIPTION` - "Description."

* [0x000B] `MUTT_NAME_VENDOR_URL` - "URL of Vendor."

* [0x000C] `MUTT_NAME_DESIGNER_URL` - "URL of Designer."

* [0x000D] `MUTT_NAME_LICENSE` - "License Description."

* [0x000E] `MUTT_NAME_LICENSE_URL` - "License Info URL."

* [0x0010] `MUTT_NAME_TYPOGRAPHIC_FAMILY` - "Typographic Family name."

* [0x0011] `MUTT_NAME_TYPOGRAPHIC_SUBFAMILY` - "Typographic Subfamily name."

* [0x0012] `MUTT_NAME_COMPATIBLE_FULL` - "Compatible Full (Macintosh only)."

* [0x0013] `MUTT_NAME_SAMPLE_TEXT` - "Sample text."

* [0x0014] `MUTT_NAME_POSTSCRIPT_CID_FINDFONT` - "PostScript CID findfont name."

* [0x0015] `MUTT_NAME_WWS_FAMILY` - "WWS Family Name."

* [0x0016] `MUTT_NAME_WWS_SUBFAMILY` - "WWS Subfamily Name."

* [0x0017] `MUTT_NAME_LIGHT_BACKGROUND_PALETTE` - "Light Background Palette."

* [0x0018] `MUTT_NAME_DARK_BACKGROUND_PALETTE` - "Dark Background Palette."

#### Name ID names

The name function `mutt_name_id_get_name` returns a `const char*` representation of a given name ID (for example, `MUTT_NAME_COPYRIGHT_NOTICE` returns "MUTT_NAME_COPYRIGHT_NOTICE"), defined below: 

```c
MUDEF const char* mutt_name_id_get_name(uint16_m name_id);
```


This function returns "MU_UNKNOWN" in the case that `name_id` is an unrecognized value.

The name function `mutt_name_id_get_nice_name` does the same thing, but returns a more readable version of it (for example, `MUTT_NAME_COPYRIGHT_NOTICE` returns "Copyright notice"), defined below: 

```c
MUDEF const char* mutt_name_id_get_nice_name(uint16_m name_id);
```


This function returns "Unknown" in the case that `name_id` is an unrecognized value.

> Note that these are name functions, and are only defined if `MUTT_NAMES` is also defined.

## Internally used low-level functionality

mutt uses several internally-defined low-level things to make certain things easier to perform. This section is a list of them.

### Reading F2DOT14 values

The macro function `MUTT_F2DOT14` creates an expression for a float equivalent of a given array that stores 2 bytes representing a big-endian F2DOT14, defined below: 

```c
#define MUTT_F2DOT14(b) (((float)((*(int8_m*)&b[1]) & 0xC0)) + (((float)(MU_RBEU16(b) & 0xFFFF)) / 16384.f))
```


### Delta logic

Some cmap formats use fairly weird logic when using "idDelta" values. The function `mutt_id_delta` figures this logic out automatically, defined below: 

```c
MUDEF uint16_m mutt_id_delta(uint16_m character_code, int16_m delta);
```


# Raster API

The raster API has the ability to [rasterize](#rasterize-glyph) [TrueType-like glyphs](#raster-glyph) onto [a bitmap](#raster-bitmap).

## Raster glyph

A "raster glyph" (often shortened to "rglyph", respective struct [`muttRGlyph`](#rglyph-struct)) is a glyph described for rasterization in the raster API, being similar to how a simple glyph is defined in the low-level API, and is heavily based on how glyphs are specified in TrueType.

The most common usage of rglyphs is for [rasterizing given glyphs in a TrueType font](#rasterization-of-truetype-glyphs). This can be achieved via converting a simple or composite glyph retrieved from the low-level API to an rglyph equivalent, which mutt has built-in support for, and can do [automatically via rendering a glyph purely based on its glyph ID](#rasterize-glyph-id).

Rglyphs don't necessarily need to come from a simple or composite glyph, however. The user can pass in their own rglyphs, and as long as they use the struct correctly, the raster API will rasterize it correctly.

### Rglyph struct

An rglyph is represented via the struct `muttRGlyph`, which has the following members:

* `uint16_m num_points` - the number of points in the `points` array. This value must be at least 1.

* `muttRPoint* points` - each point for the glyph.

* `uint16_m num_contours` - the number of contours in the glyph.

* `uint16_m* contour_ends` - the last point index of each contour, in increasing order. `contour_ends[num_contours-1]+1` must equal `num_points`.

* `float x_max` - the greatest x-coordinate value of any point within the glyph.

* `float y_max` - the greatest y-coordinate value of any point within the glyph.

A point in an rglyph is represented with the struct `muttRPoint`, which has the following members:

* `float x` - the x-coordinate of the point, in [pixel units](#raster-bitmap).

* `float y` - the y-coordinate of the point, in [pixel units](#raster-bitmap).

* `muttRFlags flags` - the [flags](#rglyph-flags) of the point.

No coordinate values in any point within an rglyph should be negative, or exceed the values indicated by `x_max` and `y_max`.

The ordering of points should follow the non-zero winding number rule that TrueType glyphs also follow: "[Points that have a non-zero winding number are inside the glyph. All other points are outside the glyph.](https://developer.apple.com/fonts/TrueType-Reference-Manual/RM02/Chap2.html#distinguishing)"

All contours must start with an on-curve point.

Some rendering methods have the possibility of "[bleeding](#raster-bleeding)" over pixels that mathematically are completely outside of the glyph, but are one pixel away from another pixel who is at least partially inside of the glyph. For this reason, it is recommended to have each points' coordinates offset by at least 1 pixel, so that no pixel coordinate is exactly at 0. This is automatically performed when converting simple and composite glyphs from the low-level API to an rglyph, and should be done by the user when creating/modifying rglyphs.

#### Rglyph flags

The type `muttRFlags` (typedef for `uint8_m`) represents the flags of a given point in an rglyph. It has the following defined values for bitmasking:

* [0x01] `MUTTR_ON_CURVE` - represents whether or not the point is on (1) or off (0) the curve; equivalent to "ON_CURVE_POINT" for simple glyphs in TrueType.

No other bits other than the ones defined above are read for any point in an rglyph.

## Raster bitmap

Rasterization of an rglyph is performed on a bitmap. The information about the bitmap is provided by the struct `muttRBitmap`, which has the following members:

* `uint32_m width` - the width of the bitmap, in pixels.

* `uint32_m height` - the height of the bitmap, in pixels.

* `muttRChannels channels` - the [channels](#raster-channels) of the bitmap.

* `uint32_m stride` - the amount of bytes to move by for each horizontal row of pixels.

* `uint8_m* pixels` - the pixel data for the bitmap to be filled in, stored from left to right, top to bottom. All values within the pixel data are expected to be pre-initialized to the appropriate out-of-glyph color indicated by `io_color`.

* `muttRIOColor io_color` - the [in/out color](#raster-in-out-color) of the bitmap.

### Raster channels

The type `muttRChannels` (typedef for `uint16_m`) represents the channels of a bitmap. It has the following defined values:

* [0x0000] `MUTTR_R` - one color channel per pixel, corresponding to one value representing how far a pixel is *in* or *out* of the glyph.

* [0x0002] `MUTTR_RGB` - three color channels: red, green, and blue in that order per pixel.

* [0x0003] `MUTTR_RGBA` - four color channels: red, green, blue, and alpha in that order per pixel.

How non-singular channel values represent how far a pixel is *in* or *out* of the glyph is dependent on the [raster method](#raster-method).

### Raster in out color

The type `muttRIOColor` (typedef for `uint8_m`) represents what values indicate whether or not a pixel is *inside* of the glyph or *outside* of the glyph (and the corresponding possible mixing between the two values). It has the following defined values:

* [0x00] `MUTTR_BW` - a smaller value indicates being more outside the glyph, and a larger value indicates being more inside the glyph.

* [0x01] `MUTTR_WB` - a larger value indicates being more outside the glyph, and a smaller value indicates being more inside the glyph.

The rules of these values applies to all channels, including alpha.

## Rasterize glyph

Rasterizing a glyph is performed with the function `mutt_raster_glyph`, defined below: 

```c
MUDEF muttResult mutt_raster_glyph(muttRGlyph* glyph, muttRBitmap* bitmap, muttRMethod method);
```


### Raster method

The type `muttRMethod` (typedef for `uint16_m`) represents what rasterization method to use when rasterizing a glyph. It has the following defined values:

* [0x0000] `MUTTR_FULL_PIXEL_BI_LEVEL` - [full-pixel](#full-pixel) [bi-level](#bi-level) rasterization.

* [0x0001] `MUTTR_FULL_PIXEL_AA2X2` - [full-pixel](#full-pixel) two-by-two [anti-aliased](#anti-aliasing) rasterization.

* [0x0002] `MUTTR_FULL_PIXEL_AA4X4` - [full-pixel](#full-pixel) four-by-four [anti-aliased](#anti-aliasing) rasterization.

* [0x0003] `MUTTR_FULL_PIXEL_AA8X8` - [full-pixel](#full-pixel) eight-by-eight [anti-aliased](#anti-aliasing) rasterization.

Most of the terms used to describe these rendering methods are taken from terms used in [The Raster Tragedy](http://rastertragedy.com).

### Full-pixel

The term "full-pixel" means that each pixel is used as one value indicating how much a pixel is *inside* or *outside* of the glyph. Each pixel is treated pixel-coordinate-wise as being directly in the center of a pixel in the pixel coordinate grid; for example, the coordinates of the top-leftest pixel in a bitmap is (0.5, 0.5) when internally calculating how much a pixel is inside or outside of the glyph.

### Bi-level

The term "bi-level" means that each pixel is rather fully inside or outside of the glyph, with no possibility of intermediate values.

### Anti-aliasing

Anti-aliasing is used in rasterization to smooth jagged edges, taking multiple samples per pixel, calculating whether or not each one is inside or outside of the glyph, and averaging all of those values for the calculated value of a given pixel. This allows for pixels to exist that are *partially* inside or outside of the glyph, and whose pixel values indicate as such, which is the opposite of [bi-level rasterization](#bi-level).

The amount of samples per pixel in the x- and y-direction is controlled by its dimensions, splitting up the pixel into multiple sub-pixels to then be individually calculated. For example, two-by-two anti-aliasing implies taking two samples on the x- and y-axis per pixel, so the top-leftest pixel (coordinates (0.5, 0.5)) would be split up into coordinates (0.25, 0.25), (0.75, 0.25), (0.25, 0.75), and (0.75, 0.75), in no particular order, and individually calculated & averaged for the final pixel value.

### Raster bleeding

Some rasterization methods have a possibility of setting pixels as (at least partially) inside of the glyph that aren't mathematically inside of the glyph to any degree, but are one pixel away from another pixel that *is* (at least partially) inside of the glyph. This effect is called "bleeding", and can cause pixels to be inside of the glyph that are outside of the range of the glyph's coordinates.

In terms of rglyphs, this is prevented by offsetting the coordinates of each point by 1 pixel, ensuring that there is at least a single pixel to the left and top and of any pixel that is mathematically inside of the glyph, and thus can catch any theoretical bleeding. This is automatically performed when converting simple and composite glyphs from the low-level API to an rglyph, and should be done by the user when creating/modifying rglyphs.

In terms of rasterization, this is prevented by increasing the width and height of the bitmap to be 1 pixel greater than the maximum x- and y-coordinates within the glyph (`glyph->x_max` and `glyph->y_max`). The conversion from the decimal values of `x_max` and `y_max` to an integer width and height should be performed via a ceiling of the final result.

## Rasterization of TrueType glyphs

The raster API gives access to rasterizing TrueType glyphs by converting them to an rglyph, which can then be [rasterized directly](#rasterize-glyph). This conversion can be done rather by the user directly [giving a simple glyph](#simple-glyph-to-rglyph), [giving a composite glyph](#composite-glyph-to-rglyph), or by [giving the header of a simple or composite glyph](#glyph-header-to-rglyph).

This conversion can also automatically be performed internally via [rasterizing the glyph based on a given glyph ID](#rasterize-glyph-id), handling all of the allocation and conversions. This can be inefficient to call on large groups of glyphs, as new memory has to be repeatedly allocated and deallocated.

### Font units to pixel units

The rasterization of any TrueType glyph involves converting the Truetype "font units" (FUnits) to pixel units (which is what a raster glyph uses). This conversion requires a [point size](https://en.wikipedia.org/wiki/Point_(typography)) and the [pixels per inch](https://en.wikipedia.org/wiki/Pixel_density), or PPI, of the display (usually 72 or 96). These two variables allow the coordinates of an rglyph to be rasterized at a predictable and calculatable physical size when displayed.

The function `mutt_funits_to_punits` performs the conversion described above for a given font based on its unitsPerEm value (stored in the head table), defined below: 

```c
MUDEF float mutt_funits_to_punits(muttFont* font, float funits, float point_size, float ppi);
```


Although the font unit range in TrueType can be expressed with a signed 16-bit integer, `funits` is a `float` for the sake of being able to perform the conversion on transformed coordinates in composite glyphs, which can result in decimal numbers.

### Simple glyph to rglyph

The function `mutt_simple_rglyph` converts a simple glyph to an rglyph, defined below: 

```c
MUDEF muttResult mutt_simple_rglyph(muttFont* font, muttGlyphHeader* header, muttSimpleGlyph* glyph, muttRGlyph* rglyph, float point_size, float ppi, muByte* data, uint32_m* written);
```


Upon a non-fatal result, `rglyph` is filled with valid raster glyph information for the given simple glyph using memory from `data`. Upon a fatal result, the contents of `rglyph` and `data` are undefined. The given rglyph information is only valid for as long as `font` is not deloaded, and as long as `data` goes unmodified.

The given simple glyph must have at least one contour, and that one contour must have points. The simple glyph given must be valid.

This function follows the format of a user-allocated function. For an explanation of how `data` and `written` are supposed to be used within this function, see [the user-allocated function section](#user-allocated-functions).

#### Simple glyph to rglyph memory maximum

The maximum amount of memory that will be needed for converting a simple glyph to a raster glyph for a given font, in bytes, is provided by the function `mutt_simple_rglyph_max`, defined below: 

```c
MUDEF uint32_m mutt_simple_rglyph_max(muttFont* font);
```


### Composite glyph to rglyph

The function `mutt_composite_rglyph` converts a composite glyph to an rglyph, defined below: 

```c
MUDEF muttResult mutt_composite_rglyph(muttFont* font, muttGlyphHeader* header, muttCompositeGlyph* glyph, muttRGlyph* rglyph, float point_size, float ppi, muByte* data);
```


Upon a non-fatal result, `rglyph` is filled with valid raster glyph information for the given composite glyph using memory from `data`. Upon a fatal result, the contents of `rglyph` and `data` are undefined. The given rglyph information is only valid for as long as `font` is not deloaded, and as long as `data` goes unmodified.

`data` should be a pointer to user-allocated memory of size `mutt_composite_rglyph_max` (in bytes). This makes it ***not*** follow the conventional format of a user-allocated function within mutt: the required amount of memory needed for converting a composite glyph to an rglyph is fixed across each font, and thus, has no `written` parameter.

The given composite glyph must have at least one contour, and that one contour must have points. The composite glyph given must be valid.

This function does ***not*** currently work with composite glyphs that use ***phantom points***, as the table required for processing them (gvar) is not currently supported.

#### Composite glyph to rglyph memory maximum

The maximum amount of memory that will be needed for converting a composite glyph to a raster glyph for a given font, in bytes, is provided by the function `mutt_composite_rglyph_max`, defined below: 

```c
MUDEF uint32_m mutt_composite_rglyph_max(muttFont* font);
```


### Glyph header to rglyph

The function `mutt_header_rglyph` converts a glyph header to a glyph, defined below: 

```c
MUDEF muttResult mutt_header_rglyph(muttFont* font, muttGlyphHeader* header, muttRGlyph* rglyph, float point_size, float ppi, muByte* data, uint32_m* written);
```


Upon a non-fatal result, `rglyph` is filled with valid raster glyph information for the given glyph based on its header, using memory from `data`. Upon a fatal result, the contents of `rglyph` and `data` are undefined. The given rglyph information is only valid for as long as `font` is not deloaded, and as long as `data` goes unmodified.

The given glyph must have at least one contour, and that one contour must have points. The glyph header given must be valid.

This function follows the format of a user-allocated function. For an explanation of how `data` and `written` are supposed to be used within this function, see [the user-allocated function section](#user-allocated-functions). However, since the conversion of a composite glyph to an rglyph requires a fixed amount of memory per font, if `written` is ever dereferenced and set by this function relative to a composite glyph, it will be set to `mutt_composite_rglyph_max`.

#### Glyph header to rglyph memory maximum

The maximum amount of memory that will be needed for converting a glyph header to a raster glyph for a given font, in bytes, is provided by the function `mutt_header_rglyph_max`, defined below: 

```c
MUDEF uint32_m mutt_header_rglyph_max(muttFont* font);
```


This function rather returns (the sum of `mutt_simple_glyph_max_size` and `mutt_simple_rglyph_max`) or (the sum of `mutt_composite_glyph_max_size` and `mutt_composite_rglyph_max`), whichever is greater. All the table loading requirements of these functions apply.

# Result

The type `muttResult` (typedef for `uint32_m`) is defined to represent how a task went. Result values can be "fatal" (meaning that the task completely failed to execute, and the program will continue as if the task had never been attempted), "non-fatal" (meaning that the task partially failed, but was still able to complete the task), and "successful" (meaning that the task fully succeeded).

## Result values

The following values are defined for `muttResult` (all values not explicitly stated as being fatal, non-fatal, or successful are assumed to be fatal):

### General result values

* `MUTT_SUCCESS` - the task was successfully completed; real value 0.

* `MUTT_FAILED_MALLOC` - a call to malloc failed, implying insufficient available memory to perform the task.

* `MUTT_FAILED_FIND_TABLE` - the table could not be located, and is likely not included in the font file.

### Directory result values

* `MUTT_INVALID_DIRECTORY_LENGTH` - the length of the table directory was invalid. This is the first check performed on the length of the font file data, meaning that if this result is given, it is likely that the data given is not font file data.

* `MUTT_INVALID_DIRECTORY_SFNT_VERSION` - the value of "sfntVersion" in the table directory was invalid/unsupported. This is the first check performed on the values within the font file data, meaning that if this result is given, it is likely that the data given is not TrueType font file data. This can also be triggered by the TrueType font using CFF data, which is currently unsupported.

* `MUTT_INVALID_DIRECTORY_NUM_TABLES` - the value of "numTables" in the table directory was invalid; the number of tables must be at least 9 to store all tables required in TrueType.

* `MUTT_INVALID_DIRECTORY_RECORD_OFFSET` - the value of "offset" in a table record within the table directory was out of range.

* `MUTT_INVALID_DIRECTORY_RECORD_LENGTH` - the value of "length" in a table record within the table directory was out of range.

* `MUTT_INVALID_DIRECTORY_RECORD_CHECKSUM` - the value of "checksum" in a table record within the table directory was invalid, implying that the table data was incorrect.

* `MUTT_INVALID_DIRECTORY_RECORD_TABLE_TAG` - the table tag of a table within the table directory was a duplicate of a previous one.

* `MUTT_MISSING_DIRECTORY_RECORD_TABLE_TAGS` - one or more tables required by TrueType standards could not be found in the table directory.

### Maxp result values

* `MUTT_INVALID_MAXP_LENGTH` - the length of the maxp table was invalid.

* `MUTT_INVALID_MAXP_VERSION` - the version of the maxp table given was invalid/unsupported. This likely means that the font has a CFF/CFF2 font outline, which is currently unsupported.

* `MUTT_INVALID_MAXP_NUM_GLYPHS` - the value for "numGlyphs" given in the maxp table was invalid.

* `MUTT_INVALID_MAXP_MAX_ZONES` - the value for "maxZones" given in the maxp table was invalid.

### Head result values

* `MUTT_INVALID_HEAD_LENGTH` - the length of the head table was invalid.

* `MUTT_INVALID_HEAD_VERSION` - the version indicated for the head table was invalid/unsupported.

* `MUTT_INVALID_HEAD_MAGIC_NUMBER` - the value for the magic number in the head table was invalid.

* `MUTT_INVALID_HEAD_UNITS_PER_EM` - the value for the units per em in the head table was not within the correct range of 16 to 16384.

* `MUTT_INVALID_HEAD_X_MIN_COORDINATES` - the value "xMin" in the head table was not within the valid TrueType grid coordinate range of -16384 to 16383.

* `MUTT_INVALID_HEAD_Y_MIN_COORDINATES` - the value "yMin" in the head table was not within the valid TrueType grid coordinate range of -16384 to 16383.

* `MUTT_INVALID_HEAD_X_MAX_COORDINATES` - the value "xMax" in the head table was not within the valid TrueType grid coordinate range of -16384 to 16383.

* `MUTT_INVALID_HEAD_Y_MAX_COORDINATES` - the value "yMax" in the head table was not within the valid TrueType grid coordinate range of -16384 to 16383.

* `MUTT_INVALID_HEAD_X_MIN_MAX` - the value established minimum x-value within the head table was greater than the established maximum x-value.

* `MUTT_INVALID_HEAD_Y_MIN_MAX` - the value established minimum y-value within the head table was greater than the established maximum y-value.

* `MUTT_INVALID_HEAD_INDEX_TO_LOC_FORMAT` - the value for "indexToLocFormat" within the head table was invalid/unsupported; it was not one of the expected values 0 (Offset16) or 1 (Offset32).

* `MUTT_INVALID_HEAD_GLYPH_DATA_FORMAT` - the value for "glyphDataFormat" within the head table was invalid/unsupported; it was not the expected value 0.

### Hhea result values

* `MUTT_INVALID_HHEA_LENGTH` - the length of the hhea table was invalid.

* `MUTT_INVALID_HHEA_VERSION` - the version indicated for the hhea table was invalid/unsupported.

* `MUTT_INVALID_HHEA_METRIC_DATA_FORMAT` - the value for "metricDataFormat" within the hhea table was invalid/unsupported; it was not the expected value 0.

* `MUTT_INVALID_HHEA_NUMBER_OF_HMETRICS` - the value for "numberOfHMetrics" within the hhea table was invalid; numberOfHMetrics must be less than or equal to "numGlyphs" in order to generate a valid array length for "leftSideBearings" within hmtx.

* `MUTT_HHEA_REQUIRES_MAXP` - the maxp table rather failed to load or was not requested for loading, and hhea requires maxp to be loaded.

### Hmtx result values

* `MUTT_INVALID_HMTX_LENGTH` - the length of the hmtx table was invalid.

* `MUTT_HMTX_REQUIRES_MAXP` - the maxp table rather failed to load or was not requested for loading, and hmtx requires maxp to be loaded.

* `MUTT_HMTX_REQUIRES_HHEA` - the hhea table rather failed to load or was not requested for loading, and hmtx requires hhea to be loaded.

### Loca result values

* `MUTT_INVALID_LOCA_LENGTH` - the length of the loca table was invalid.

* `MUTT_INVALID_LOCA_OFFSET` - an offset in the loca table was invalid. This could mean that the offset's range was invalid for the glyf table, or that the rule of incremental offsets was violated.

* `MUTT_LOCA_REQUIRES_MAXP` - the maxp table rather failed to load or was not requested for loading, and loca requires maxp to be loaded.

* `MUTT_LOCA_REQUIRES_HEAD` - the head table rather failed to load or was not requested for loading, and loca requires head to be loaded.

* `MUTT_LOCA_REQUIRES_GLYF` - the glyf table rather failed to load or was not requested for loading, and loca requires glyf to be loaded.

### Name result values

* `MUTT_INVALID_NAME_LENGTH` - the length of the name table was invalid.

* `MUTT_INVALID_NAME_VERSION` - the version of the name table was invalid/unsupported.

* `MUTT_INVALID_NAME_STORAGE_OFFSET` - the offset given for the storage of string data within the name table was invalid / out of range.

* `MUTT_INVALID_NAME_LENGTH_OFFSET` - the length and offset given for a name record within the name table was invalid / out of range.

### Glyf result values

* `MUTT_INVALID_GLYF_HEADER_LENGTH` - the glyph header length given from values by the loca table were invalid; they were above 0, implying an outline, yet the length given was insufficient to store a glyph header.

* `MUTT_INVALID_GLYF_HEADER_NUMBER_OF_CONTOURS` - the number of contours within the glyph header exceeded the maximum set by the maxp table.

* `MUTT_INVALID_GLYF_HEADER_X_MIN` - the glyph header's xMin value was not in range of the head table's listed corresponding value.

* `MUTT_INVALID_GLYF_HEADER_Y_MIN` - the glyph header's yMin value was not in range of the head table's listed corresponding value.

* `MUTT_INVALID_GLYF_HEADER_X_MAX` - the glyph header's xMax value was not in range of the head table's listed corresponding value.

* `MUTT_INVALID_GLYF_HEADER_Y_MAX` - the glyph header's yMax value was not in range of the head table's listed corresponding value.

* `MUTT_INVALID_GLYF_HEADER_X_MIN_MAX` - the glyph header's xMin value was greater than its xMax value or vice versa, which does not make sense.

* `MUTT_INVALID_GLYF_HEADER_Y_MIN_MAX` - the glyph header's yMin value was greater than its yMax value or vice versa, which does not make sense.

* `MUTT_INVALID_GLYF_SIMPLE_LENGTH` - the length of the simple glyph description is invalid/insufficient to describe the simple glyph.

* `MUTT_INVALID_GLYF_SIMPLE_END_PTS_OF_CONTOURS` - a value within the endPtsOfContours array of the simple glyph was invalid; rather the value was non-incremental, or the last index was the invalid value 0xFFFF.

* `MUTT_INVALID_GLYF_SIMPLE_POINT_COUNT` - the amount of points specified within the simple glyph exceeded the maximum set by the maxp table.

* `MUTT_INVALID_GLYF_SIMPLE_INSTRUCTION_LENGTH` - the instruction length given by the simple glyph exceeded the maximum set by the maxp table.

* `MUTT_INVALID_GLYF_SIMPLE_X_COORD` - an x-coordinate within the simple glyph was out of range for its listed minimum/maximum values. This is non-fatal, as mutt automatically overwrites the existing min/max values within the header.

* `MUTT_INVALID_GLYF_SIMPLE_Y_COORD` - a y-coordinate within the simple glyph was out of range for its minimum/maximum values. This is non-fatal, as mutt automatically overwrites the existing min/max values within the header.

* `MUTT_INVALID_GLYF_COMPOSITE_LENGTH` - the length of the composite glyph description is invalid/insufficient to describe the composite glyph.

* `MUTT_INVALID_GLYF_COMPOSITE_INSTRUCTION_LENGTH` - the instruction length given by the composite glyph exceeded the maximum set by the maxp table.

* `MUTT_INVALID_GLYF_COMPOSITE_COMPONENT_COUNT` - the amount of components given in the composite glyph exceeded the maximum set by the maxp table.

* `MUTT_INVALID_GLYF_COMPOSITE_GLYPH_INDEX` - the value for "glyphIndex" in a component within the composite glyph was an invalid glyph index (out of range for the number of glyphs specified in maxp).

* `MUTT_INVALID_GLYF_COMPOSITE_FLAGS` - the flags in a component within the composite glyph were invalid (multiple mutually exclusive transform data flags were set).

### Cmap result values

* `MUTT_INVALID_CMAP_LENGTH` - the length of the cmap table was invalid.

* `MUTT_INVALID_CMAP_VERSION` - the version of the cmap table was invalid/unsupported.

* `MUTT_INVALID_CMAP_ENCODING_RECORD_OFFSET` - an encoding record's subtable offset was invalid (AKA out of range for the cmap table).

* `MUTT_INVALID_CMAP_ENCODING_RECORD_LENGTH` - an encoding record's subtable length was invalid (AKA the offset's distance from the end of the cmap table was not long enough to figure out its format).

* `MUTT_INVALID_CMAP_ENCODING_RECORD_FORMAT` - the encoding record's format was invalid/unsupported.

* `MUTT_INVALID_CMAP0_LENGTH` - the length of the cmap format 0 subtable was invalid.

* `MUTT_INVALID_CMAP4_LENGTH` - the length of the cmap format 4 subtable was invalid/insufficient to define the data needed.

* `MUTT_INVALID_CMAP4_SEG_COUNT_X2` - the value given for "segCountX2" in the cmap format 4 subtable was not divisible by 2, and was therefore an invalid value.

* `MUTT_INVALID_CMAP4_END_CODE` - an "endCode" value for a segment in the cmap format 4 subtable broke the incremental order of the endCode values.

* `MUTT_INVALID_CMAP4_LAST_END_CODE` - the last "endCode" value in the cmap format 4 subtable was not 0xFFFF, which is required in the TrueType specification.

* `MUTT_INVALID_CMAP4_START_CODE` - a "startCode" value for a segment in the cmap format 4 subtable was greater than its endCode.

* `MUTT_INVALID_CMAP4_ID_RANGE_OFFSET` - an "idRangeOffset" value for a segment in the cmap format 4 subtable was rather out of range for indexes into glyphIdArray, or was not divisible by 2 (which it must be, since it's a byte-offset into a 2-byte-integer array, starting from a 2-byte-integer array).

* `MUTT_INVALID_CMAP12_LENGTH` - the length of the cmap format 12 subtable was invalid/in sufficient to define the data needed.

* `MUTT_INVALID_CMAP12_START_CHAR_CODE` - a "startCharCode" value for a map group in the cmap format 12 subtable was not incremental compared to the previous group.

* `MUTT_INVALID_CMAP12_END_CHAR_CODE` - an "endCharCode" value for a map group in the cmap format 12 subtable was not less than the startCharCode value of the next group.

* `MUTT_CMAP_REQUIRES_MAXP` - the maxp table rather failed to load or was not requested for loading, and cmap requires maxp to be loaded.

### Rasterization result values

* `MUTT_UNKNOWN_RASTER_METHOD` - the given raster method value was unrecognized.

* `MUTT_INVALID_RGLYPH_COMPOSITE_CONTOUR_COUNT` - the process of converting a composite glyph to an rglyph failed because the given composite glyph had more contours than the maximum contour count indicated in the maxp table.

* `MUTT_INVALID_RGLYPH_COMPOSITE_POINT_COUNT` - the process of converting a composite glyph to an rglyph failed because the given composite glyph had more points than the maximum point count indicated in the maxp table.

* `MUTT_INVALID_RGLYPH_COMPOSITE_DEPTH` - the process of converting a composite glyph to an rglyph failed because the given composite glyph had a larger component depth than the maximum component depth indicated in the maxp table.

* `MUTT_INVALID_RGLYPH_COMPOSITE_COMPONENT_COUNT` - the process of converting a composite glyph to an rglyph failed because the given composite glyph (or a composite component within it) had more components than the maximum component count indicated in the maxp table.

* `MUTT_INVALID_RGLYPH_COMPOSITE_COMPONENT_ARGUMENT1` - the process of converting a composite glyph to an rglyph failed because a simple glyph had an argument1 value giving a point number that was out of range for the parent glyph.

* `MUTT_INVALID_RGLYPH_COMPOSITE_COMPONENT_ARGUMENT2` - the process of converting a composite glyph to an rglyph failed because a simple glyph had an argument2 value giving a point number that was out of range for the child glyph.

## Check if result is fatal

The function `mutt_result_is_fatal` returns whether or not a given `muttResult` value is fatal, defined below: 

```c
MUDEF muBool mutt_result_is_fatal(muttResult result);
```


This function returns `MU_TRUE` if the value of `result` is invalid.

## Result name

The function `mutt_result_get_name` returns a `const char*` representation of a given result value (for example, `MUTT_SUCCESS` returns "MUTT_SUCCESS"), defined below: 

```c
MUDEF const char* mutt_result_get_name(muttResult result);
```


This function returns "MU_UNKNOWN" in the case that `result` is an invalid result value.

> This function is a "name" function, and therefore is only defined if `MUTT_NAMES` is also defined.

# C standard library dependencies

mutt has several C standard library dependencies, all of which are overridable by defining them before the inclusion of its header. The following is a list of those dependencies.

## `stdlib.h` dependencies

* `mu_malloc` - equivalent to `malloc`.

* `mu_free` - equivalent to `free`.

* `mu_realloc` - equivalent to `realloc`.

* `mu_qsort` equivalent to `qsort`.

## `string.h` dependencies

* `mu_memcpy` - equivalent to `memcpy`.

* `mu_memset`- equivalent to `memset`.

## `math.h` dependencies

* `mu_fabsf` - equivalent to `fabsf`.

* `mu_roundf` - equivalent to `roundf`.
