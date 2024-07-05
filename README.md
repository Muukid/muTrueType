

# muTrueType v1.0.0

muTrueType (abbreviated to 'mutt') is a public domain header-only single-file C library for retrieving data from the TrueType file format, rendering glyphs to a bitmap, and handling the layout/spacing of multiple glyphs in sequence.

***WARNING!*** This library is still under heavy development, has no official releases, and won't be stable until its first public release v1.0.0.

To use it, download the `muTrueType.h` file, add it to your include path, and include it like so:

```c
#define MUTT_IMPLEMENTATION
#include "muTrueType.h"
```

More information about the general structure of a mu library is provided at [the mu library information GitHub repository.](https://github.com/Muukid/mu-library-information)

# Demos

The documentation for this library is rather explicit/hard to read and get a good overview of the library in the process. For this, demos that quickly show the gist of the library and how it works are available in the `demos` folder.

# Licensing

mutt is licensed under public domain or MIT, whichever you prefer. More information is provided in the accompanying file `license.md` and at the bottom of `muTrueType.h`.

# TrueType documentation

mutt has the ability to work fairly low-level in the details of TrueType, meaning that more deep usage of mutt's API (beyond using high-level functions that process the TrueType data) necessitates an understanding of the TrueType documentation. Terms from the TrueType documentation will be used with the assumption that the user has read it and understands these terms.

mutt is developed primarily off of these sources of documentation:

* [OpenType spec](https://learn.microsoft.com/en-us/typography/opentype/spec/).

* [TrueType reference manual](https://developer.apple.com/fonts/TrueType-Reference-Manual/).


# Other library dependencies

mutt has a dependency on:

* [muUtility v1.1.0](https://github.com/Muukid/muUtility/releases/tag/v1.1.0).

Note that mu libraries store their dependencies within their files, so you don't need to import these dependencies yourself.

Note that the libraries listed may also have other dependencies that they also include that aren't listed here.

# Result enumerator

mutt uses the `muttResult` enumerator to represent how a function went. It has the following possible values:

* `MUTT_SUCCESS` - the task succeeded.

* `MUTT_FAILED_MALLOC` - a call to malloc failed; memory was insufficient to perform the operation

* `MUTT_FAILED_REALLOC` - a call to realloc failled; memory was insufficient to perform the operation.

* `MUTT_UNFOUND_TABLE` - the table could not be located within the data.

* `MUTT_DUPLICATE_TABLE` - another table with the same tag was found.

* `MUTT_INVALID_TABLE_DIRECTORY_LENGTH` - the length of the given TrueType data is not enough for the table directory. Likely the length is incorrect or the data given is not TrueType data.

* `MUTT_INVALID_TABLE_DIRECTORY_SFNT_VERSION` - the value for "sfntVersion" in the table directory was invalid. Since this is the first value read when loading TrueType data, this most likely means that rather the data given is corrupt, not TrueType data, or is under another incompatible wrapper (such as fonts that use CFF data).

* `MUTT_INVALID_TABLE_DIRECTORY_SEARCH_RANGE` - the value for "searchRange" in the table directory was invalid.

* `MUTT_INVALID_TABLE_DIRECTORY_ENTRY_SELECTOR` - the value for "entrySelector" in the table directory was invalid.

* `MUTT_INVALID_TABLE_DIRECTORY_RANGE_SHIFT` - the value for "rangeShift" in the table directory was invalid.

* `MUTT_INVALID_TABLE_RECORD_OFFSET` - the value for "offset" in a table record was out of range.

* `MUTT_INVALID_TABLE_RECORD_LENGTH` - the value for "length" in a table record was out of range.

* `MUTT_INVALID_TABLE_RECORD_CHECKSUM` - the value for "checksum" in a table record was invalid.

* `MUTT_INVALID_MAXP_LENGTH` - the value for the table length of the maxp table was invalid. This could mean that an unsupported version of the table is being used.

* `MUTT_INVALID_MAXP_VERSION` - the version value in the maxp table was invalid/unsupported.

* `MUTT_INVALID_MAXP_MAX_ZONES` - the value for "maxZones" in the maxp table was invalid.

* `MUTT_INVALID_HEAD_LENGTH` - the value for the table length of the head table was invalid. This could mean that an unsupported version of the table is being used.

* `MUTT_INVALID_HEAD_VERSION` - the version value in the head table was invalid/unsupported.

* `MUTT_INVALID_HEAD_MAGIC_NUMBER` - the value for "magicNumber" in the head table was invalid.

* `MUTT_INVALID_HEAD_UNITS_PER_EM` - the value for "unitsPerEm" in the head table was invalid.

* `MUTT_INVALID_HEAD_X_MIN_MAX` - the values for "xMin" and "xMax" in the head table were invalid.

* `MUTT_INVALID_HEAD_Y_MIN_MAX` - the values for "yMin" and "yMax" in the head table were invalid.

* `MUTT_INVALID_HEAD_INDEX_TO_LOC_FORMAT` - the value for "indexToLocFormat" in the head table was invalid.

* `MUTT_INVALID_HEAD_GLYPH_DATA_FORMAT` - the value for "glyphDataFormat" in the head table was invalid/unsupported.

* `MUTT_INVALID_HHEA_LENGTH` - the value for the table length of the hhea table was invalid. This could mean that an unsupported version of the table is being used.

* `MUTT_INVALID_HHEA_VERSION` - the version value in the hhea table was invalid/unsupported.

* `MUTT_INVALID_HHEA_METRIC_DATA_FORMAT` - the value for "metricDataFormat" in the hhea table was invalid/unsupported.

* `MUTT_INVALID_HHEA_NUMBER_OF_HMETRICS` - the value for "numberOfHMetrics" in the hhea table was invalid/unsupported.

* `MUTT_INVALID_HMTX_LENGTH` - the value for the table length of the hmtx table was invalid.

* `MUTT_INVALID_LOCA_LENGTH` - the value for the table length of the loca table was invalid.

* `MUTT_INVALID_POST_LENGTH` - the value for the table length of the post table was invalid.

* `MUTT_INVALID_POST_VERSION` - the value "version" in the post table was invalid/unsupported.

* `MUTT_INVALID_POST_GLYPH_NAME_INDEX` - an index in the array "glyphNameIndex" in the version 2.0 post subtable was out of range.

* `MUTT_INVALID_NAME_LENGTH` - the value for the table length of the name table was invalid. This could mean that an unsupported version of the table is being used.

* `MUTT_INVALID_NAME_VERSION` - the value "version" in the name table was invalid/unsupported.

* `MUTT_INVALID_NAME_STORAGE_OFFSET` - the value "storageOffset" in the name table was out of range.

* `MUTT_INVALID_NAME_PLATFORM_ID` - the value "platformID" in a NameRecord within the "nameRecord" array in the name table was invalid/unsupported.

* `MUTT_INVALID_NAME_ENCODING_ID` - the value "encodingID" in a NameRecord within the "nameRecord" array in the name table was invalid/unsupported.

* `MUTT_INVALID_NAME_LANGUAGE_ID` - the value "languageID" in a NameRecord within the "nameRecord" array in the name table was invalid/unsupported.

* `MUTT_INVALID_NAME_STRING_OFFSET` - the value "stringOffset" and "length" in a NameRecord within the "nameRecord" array in the name table were out of range.

* `MUTT_INVALID_NAME_LANG_TAG_OFFSET` - the value "langTagOffset" and "length" in a LangTagRecord within the "langTagRecord" array in the name table were out of range.

* `MUTT_HHEA_REQUIRES_MAXP` - the hhea table failed to load becuase maxp is rather not being loaded or failed to load, and hhea relies on maxp.

* `MUTT_HMTX_REQUIRES_MAXP` - the hmtx table failed to load because maxp is rather not being loaded or failed to load, and hmtx relies on mxap.

* `MUTT_HMTX_REQUIRES_HHEA` - the hmtx table failed to load because hhea is rather not being loaded or failed to load, and hmtx relies on hhea.

* `MUTT_LOCA_REQUIRES_HEAD` - the loca table failed to load because head is rather not being loaded or failed to load, and loca relies on head.

* `MUTT_LOCA_REQUIRES_MAXP` - the loca table failed to load because maxp is rather not being loaded or failed to load, and loca relies on maxp.

Most of these errors getting triggered imply that rather the data is corrupt (especially in regards to checksum errors), uses some extension or format not supported by this library (such as OpenType), has accidental incorrect values, or is purposely malformed to attempt to get out of the memory region of the file data.

## Result name function

The function `mutt_result_get_name` converts a `muttResult` value into a `const char*` representation, defined below: 

```c
MUDEF const char* mutt_result_get_name(muttResult result);
```


Note that this function is only defined if `MUTT_NAMES` is defined before the inclusion of the header file.

This function returns `"MUTT_UNKNOWN"` if a respective name could not be found.

# Loading a TrueType font

## Loading and deloading functions

The function `mutt_load` loads information about TrueType data into a `muttFont` struct, defined below: 

```c
MUDEF muttResult mutt_load(muByte* data, size_m datalen, muttFont* font, uint32_m load_flags);
```


The function `mutt_deload` deloads information retrieved from TrueType data within a `muttFont` struct, defined below: 

```c
MUDEF void mutt_deload(muttFont* font);
```


`mutt_deload` must be called at *some* point on every successfully loaded font. Once `mutt_deload` is called on a `muttFont` struct, its contents are undefined.

If `mutt_load` doesn't return `MUTT_SUCCESS`, the font failed to load entirely, the contents of `font` are undefined, and `mutt_deload` should not be called on it.

`mutt_load` checks if all of the values within each requested table are correct according to the specification, and doesn't load a given table if one is found, which helps prevent bad values and attack vectors.

## Loading flags

The function `mutt_load` takes in a parameter, `load_flags`, which determines what information about the TrueType font does and doesn't get loaded. The value `0` loads no information, and a bit equaling `1` means to load it.

### Exact bit values

The following macros are defined for certain bits indicating what information to load:

* [0x00000001] `MUTT_LOAD_DIRECTORY` - load the directory and permanently store the results. The directory is loaded no matter what, but this bit ensures that the directory data isn't wiped after loading.

* [0x00000002] `MUTT_LOAD_MAXP` - load the maxp table.

* [0x00000004] `MUTT_LOAD_HEAD` - load the head table.

* [0x00000008] `MUTT_LOAD_HHEA` - load the hhea table.

* [0x00000010] `MUTT_LOAD_HMTX` - load the hmtx table.

* [0x00000020] `MUTT_LOAD_LOCA` - load the loca table.

* [0x00000040] `MUTT_LOAD_POST` - load the post table.

* [0x00000080] `MUTT_LOAD_NAME` - load the name table.

### Group bit values

The following macros are defined for loading groups of tables:

* [0x000003FE] `MUTT_LOAD_REQUIRED_TABLES` - loads all tables required by TrueType standards (cmap, glyf, head, hhea, hmtx, loca, maxp, name, and post).

* [0xFFFFFFFF] `MUTT_LOAD_ALL` - loads everything; sets all flags.

# Reading information from a TrueType font

## The `muttFont` struct

A TrueType font is represented by the struct `muttFont`. Once successfully loaded, the data stored within a `muttFont` struct is entirely separate from the user-allocated TrueType data.

Inside the `muttFont` struct is all of the loaded information from when it was loaded. The actual full list of members is:

* `uint32_m load_flags` - the load flags that were provided to the load function.

* `muttDirectory* directory` - a pointer to a directory listing all of the tables provided by the given font.

* `muttMaxp* maxp` - a pointer to the maxp table.

* `muttResult maxp_res` - the result of loading the member `maxp`.

* `muttHead* head` - a pointer to the head table.

* `muttResult head_res` - the result of loading the member `head`.

* `muttHhea* hhea` - a pointer to the hhea table.

* `muttResult hhea_res` - the result of loading the member `hhea`.

* `muttHmtx* hmtx` - a pointer to the hmtx table.

* `muttResult hmtx_res` - the result of loading the member `hmtx`.

* `muttLoca* loca` - a pointer to the loca table.

* `muttResult loca_res` - the result of loading the member `loca`.

* `muttPost* post` - a pointer to the post table.

* `muttResult post_res` - the result of loading the member `post`.

* `muttName* name` - a pointer to the name table.

* `muttResult name_res` - the result of loading the member `name`.

* `muByte* mem` - the inner allocated memory used for holding necessary data.

* `size_m memlen` - the length of the allocated memory, in bytes.

* `size_m memcur` - offset to the latest unused memory in `mem`, in bytes.

Most of the members are in pairs of pointers and result values. If a requested pointer is 0, it could not be loaded, and its corresponding result value will indicate the result enumerator indicating what went wrong.

The contents of a pointer and result pair for information not included in the load flags are undefined.

Note that if the directory fails to load, the entire loading function fails, and what went wrong is returned in the loading function; this is why there is no respective result for the member `directory`.

Note that if an array in a table or directory is of length 0, the value for the pointer within the respective struct is 0 unless stated otherwise.

## Directory information

The struct `muttDirectory` is used to list all of the tables provided by a TrueType font. It is stored in the struct `muttFont` as `muttFont->directory`, and is similar to TrueType's table directory. It is loaded permanently with the flag `MUTT_LOAD_DIRECTORY`.

Its members are:

* `uint16_m num_tables` - equivalent to "numTables" in the table directory.

* `uint16_m search_range` - equivalent to "searchRange" in the table directory.

* `uint16_m entry_selector` - equivalent to "entrySelector" in the table directory.

* `uint16_m range_shift` - equivalent to "rangeShift" in the table directory.

* `muttTableRecord* table_records` - equivalent to "tableRecords" in the table directory.

The struct `muttTableRecord` is similar to TrueType's table record, and has the following members:

* `uint8_m table_tag[4]` - equivalent to "tableTag" in the table record.

* `uint32_m checksum` - equivalent to "checksum" in the table record.

* `uint32_m offset` - equivalent to "offset" in the table record.

* `uint32_m length` - equivalent to "length" in the table record.

## Maxp information

The struct `muttMaxp` is used to represent the maxp table provided by a TrueType font, stored in the struct `muttFont` as `muttFont->maxp`, and loaded with the flag `MUTT_LOAD_MAXP`.

Its members are:

* `uint16_m version_high` - equivalent to the high bytes of "version" in the maxp table.

* `uint16_m version_low` - equivalent to the low bytes "version" in the maxp table.

* `uint16_m num_glyphs` - equivalent to "numGlyphs" in the maxp table.

* `uint16_m max_points` - equivalent to "maxPoints" in the maxp table.

* `uint16_m max_contours` - equivalent to "maxContours" in the maxp table.

* `uint16_m max_composite_points` - equivalent to "maxCompositePoints" in the maxp table.

* `uint16_m max_composite_contours` - equivalent to "maxCompositeContours" in the maxp table.

* `uint16_m max_zones` - equivalent to "maxZones" in the maxp table.

* `uint16_m max_twilight_points` - equivalent to "maxTwilightPoints" in the maxp table.

* `uint16_m max_storage` - equivalent to "maxStorage" in the maxp table.

* `uint16_m max_function_defs` - equivalent to "maxFunctionDefs" in the maxp table.

* `uint16_m max_instruction_defs` - equivalent to "maxInstructionDefs" in the maxp table.

* `uint16_m max_stack_elements` - equivalent to "maxStackElements" in the maxp table.

* `uint16_m max_size_of_instructions` - equivalent to "maxSizeOfInstructions" in the maxp table.

* `uint16_m max_component_elements` - equivalent to "maxComponentElements" in the maxp table.

* `uint16_m max_component_depth` - equivalent to "maxComponentDepth" in the maxp table.

## Head information

The struct `muttHead` is used to represent the head table provided by a TrueType font, stored in the struct `muttFont` as `muttFont->head`, and loaded with the flag `MUTT_LOAD_HEAD`.

Its members are:

* `uint16_m font_revision_high` - equivalent to the high bytes of "fontRevision" in the head table.

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

* `int16_m glyph_data_format` - equivalent to "glyphDataFormat" in the head table.

### Head mac style macros

The following macros are defined to make bit-masking the `mac_style` member of the `muttHead` struct easier:

* [0x0001] `MUTT_MAC_STYLE_BOLD` - bold.

* [0x0002] `MUTT_MAC_STYLE_ITALIC` - italic.

* [0x0004] `MUTT_MAC_STYLE_UNDERLINE` - underlined.

* [0x0008] `MUTT_MAC_STYLE_OUTLINE` - outlined.

* [0x0010] `MUTT_MAC_STYLE_SHADOW` - shadow.

* [0x0020] `MUTT_MAC_STYLE_CONDENSED` - condensed.

* [0x0040] `MUTT_MAC_STYLE_EXTENDED` - extended.

### Head index to loc format macros

The following macros are defined to make the value of the `index_to_loc_format` member of the `muttHead` struct easier to interpret:

* [0x0000] `MUTT_LOCA_FORMAT_OFFSET16` - short offsets (offset16).

* [0x0001] `MUTT_LOCA_FORMAT_OFFSET32` - long offsets (offset32).

## Hhea information

The struct `muttHhea` is used to represent the hhea table provided by a TrueType font, stored in the struct `muttFont` as `muttFont->hhea`, and loaded with the flag `MUTT_LOAD_HHEA` (flag `MUTT_LOAD_MAXP` also needs to be set for loca to load successfully).

Its members are:

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

* `int16_m metric_data_format` - equivalent to "metricDataFormat" in the hhea table.

* `uint16_m number_of_hmetrics` - equivalent to "numberOfHMetrics" in the hhea table.

## Hmtx information

The struct `muttHmtx` is used to represent the hmtx table provided by a TrueType font, stored in the struct `muttFont` as `muttFont->hmtx`, and loaded with the flag `MUTT_LOAD_HMTX` (flags `MUTT_LOAD_MAXP` and `MUTT_LOAD_HHEA` also need to be set for loca to load successfully).

Its members are:

* `muttLongHorMetric* hmetrics` - equivalent to "hMetrics" in the hmtx table.

* `int16_m* left_side_bearings` - equivalent to "leftSideBearings" in the hmtx table.

The struct `muttLongHorMetric` is similar to TrueType's LongHorMetric record, and has the following members:

* `uint16_m advance_width` - equivalent to "advanceWidth" in the LongHorMetric record.

* `int16_m lsb` - equivalent to "lsb" in the LongHorMetric record.

## Loca information

The union `muttLoca` is used to represent the loca table provided by a TrueType font, stored in the struct `muttFont` as `muttFont->loca`, and loaded with the flag `MUTT_LOAD_LOCA` (flags `MUTT_LOAD_HEAD` and `MUTT_LOAD_MAXP` also need to be set for loca to load successfully).

Its members are:

* `uint16_m* offsets16` - equivalent to "offsets" in the short format of the loca table.

* `uint32_m* offsets32` - equivalent to "offsets" in the long format of the loca table.

## Post information

### Version 2.0 post subtable

The struct `muttPost20` represents a version 2.0 post subtable. It has the following members:

* `uint16_m num_glyphs` - equivalent to "numGlyphs" in version 2.0 of the post subtable. If this value is equal to 0, the contents of all other members are undefined.

* `uint16_m* glyph_name_index` - equivalent to "glyphNameIndex" in version 2.0 of the post subtable.

* `uint8_m* string_data` - equivalent to "stringData" in version 2.0 of the post subtable.

### Version 2.5 post subtable

The struct `muttPost25` represents a version 2.5 post subtable. It has the following members:

* `uint16_m num_glyphs` - equivalent to "numGlyphs" in version 2.5 of the post subtable. If this value is equal to 0, the contents of all other members are undefined.

* `int8_m* offset` - equivalent to "offset" in version 2.5 of the post subtable.

### Post subtable

The union `muttPostSubtable` represents the possible subtables offered by different versions of the post table. It has the following members:

* `muttPost20 v20` - version 2.0 post subtable.

* `muttPost25 v25` - version 2.5 post subtable.

### Post struct

The struct `muttPost` is used to represent the post table provided by a TrueType font, stored in the struct `muttFont` as `muttFont->post`, and loaded with the flag `MUTT_LOAD_POST`.

Its members are:

* `uint16_m version_high` - equivalent to the high bytes of "version" in the post table.

* `uint16_m version_low` - equivalent to the low bytes of "version" in the post table.

* `int32_m italic_angle` - equivalent to "italicAngle" in the post table.

* `int16_m underline_position` - equivalent to "underlinePosition" in the post table.

* `int16_m underline_thickness` - equivalent to "underlineThickness" in the post table.

* `uint32_m is_fixed_pitch` - equivalent to "isFixedPitch" in the post table.

* `uint32_m min_mem_type42` - equivalent to "minMemType42" in the post table.

* `uint32_m max_mem_type42` - equivalent to "maxMemType42" in the post table.

* `uint32_m min_mem_type1` - equivalent to "minMemType1" in the post table.

* `uint32_m max_mem_type1` - equivalent to "maxMemType1" in the post table.

* `muttPostSubtable subtable` - the subtable offered by the version of the post table; the contents of this member are undefined if the version is not 2.0 or 2.5.

## Name information

The struct `muttName` is used to represent the name table provided by a TrueType font, stored in the struct `muttFont` as `muttFont->name`, and loaded with the flag `MUTT_LOAD_NAME`.

Its members are:

* `uint16_m version` - equivalent to "version" in the name table.

* `uint16_m count` - equivalent to "count" in the name table. If this value is 0, the contents of all members listed after this are undefined.

* `uint16_m storage_offset` - equivalent to "storageOffset" in the name table.

* `muttNameRecord* name_record` - equivalent to "nameRecord" in the name table.

* `uint16_m lang_tag_count` - equivalent to "langTagCount" in the name table (version 1). The value of this member is undefined if `version` is 0.

* `muttLangTagRecord* lang_tag_record` - equivalent to "langTagRecord" in the name table (version 1). The value of this member is undefined if `version` is 0.

* `muByte* storage` - the raw storage data.

### Name record

The struct `muttNameRecord` represents a NameRecord in the name table. It has the following members:

* `uint16_m platform_id` - equivalent to "platformID" in the NameRecord subtable of the name table.

* `uint16_m encoding_id` - equivalent to "encodingID" in the NameRecord subtable of the name table.

* `uint16_m language_id` - equivalent to "languageID" in the NameRecord subtable of the name table.

* `uint16_m name_id` - equivalent to "nameID" in the NameRecord subtable of the name table.

* `uint16_m length` - equivalent to "length" in the NameRecord subtable of the name table.

* `uint16_m string_offset` - equivalent to "stringOffset" in the NameRecord subtable of the name table.

The raw data for the string can be accessed via `&muttName.storage[muttNameRecord.string_offset]`.

### Lang tag record

The struct `muttLangTagRecord` represents a LangTagRecord in the name table. It has the following members:

* `uint16_m length` - equivalent to "length" in the LangTagRecord subtable of the name table.

* `uint16_m lang_tag_offset` - equivalent to "langTagOffset" in the LangTagRecord subtable of the name table.

The raw data for the language tag string can be accessed via `&muttName.storage[muttLangTagRecord.lang_tag_offset]`.

### Name ID macros

The following macros are defined for getting the meaning of some name IDs:

* [00] `MUTT_NAME_ID_COPYRIGHT`

* [01] `MUTT_NAME_ID_FONT_FAMILY`

* [02] `MUTT_NAME_ID_FONT_SUBFAMILY`

* [03] `MUTT_NAME_ID_UNIQUE`

* [04] `MUTT_NAME_ID_FONT_NAME`

* [05] `MUTT_NAME_ID_VERSION`

* [06] `MUTT_NAME_ID_POSTSCRIPT_NAME`

* [07] `MUTT_NAME_ID_TRADEMARK`

* [08] `MUTT_NAME_ID_MANUFACTURER`

* [09] `MUTT_NAME_ID_DESIGNER`

* [10] `MUTT_NAME_ID_DESCRIPTION`

* [11] `MUTT_NAME_ID_VENDOR_URL`

* [12] `MUTT_NAME_ID_DESIGNER_URL`

* [13] `MUTT_NAME_ID_LICENSE_DESCRIPTION`

* [14] `MUTT_NAME_ID_LICENSE_URL`

* [16] `MUTT_NAME_ID_TYPOGRAPHIC_FAMILY`

* [17] `MUTT_NAME_ID_TYPOGRAPHIC_SUBFAMILY`

* [18] `MUTT_NAME_ID_COMPATIBLE`

* [19] `MUTT_NAME_ID_SAMPLE_TEXT`

* [20] `MUTT_NAME_ID_FINDFONT`

* [21] `MUTT_NAME_ID_WWS_FAMILY`

* [22] `MUTT_NAME_ID_WWS_SUBFAMILY`

* [23] `MUTT_NAME_ID_LIGHT_BACKGROUND`

* [24] `MUTT_NAME_ID_DARK_BACKGROUND`

* [25] `MUTT_NAME_ID_VARIATIONS_PREFIX`

The exact meaning of these values can be found in the TrueType and OpenType documentation. Note that name IDs are not limited to just the values defined above.

The function `mutt_name_id_get_name` returns a stringified version of a given name ID value, directly converting its macro equivalent to a string version (for example, `MUTT_NAME_ID_COPYRIGHT` turns into `"MUTT_NAME_ID_COPYRIGHT"`), defined below: 

```c
MUDEF const char* mutt_name_id_get_name(uint16_m name_id);
```


The function `mutt_name_id_get_nice_name` returns a readable string version of a given name ID value, directly converting its macro equivalent to a readable string version (for example, `MUTT_NAME_ID_COPYRIGHT` turns into `"Copyright notice"`), defined below: 

```c
MUDEF const char* mutt_name_id_get_nice_name(uint16_m name_id);
```


Both above-listed functions returns `MUTT_UNKNOWN` if there is no defined macro equivalent for the given name ID value. Both functions are only defined if `MUTT_NAMES` is defined.

### Platform ID macros

The following macros are defined for getting the meaning of some name IDs:

* [0] `MUTT_PLATFORM_UNICODE`

* [1] `MUTT_PLATFORM_MACINTOSH`

* [2] `MUTT_PLATFORM_ISO`

* [3] `MUTT_PLATFORM_WINDOWS`

* [4] `MUTT_PLATFORM_CUSTOM`

All of the platform ID values defined above are the ones defined by TrueType and OpenType as of the writing of this, and are the only platform ID values compatible with mutt, and an error will be thrown for a table if a platform ID value is given that is not one of the values defined above.

Note that `MUTT_PLATFORM_ISO` and `MUTT_PLATFORM_CUSTOM` are invalid values for a platform ID in a name table; they will not be a given value in the name table if it successfully loaded.

More information about these values is available in the TrueType standard.

The function `mutt_platform_id_get_name` returns a stringified version of a given platform ID value, directly converting its macro equivalent to a string version (for example, `MUTT_PLATFORM_UNICODE` turns into `"MUTT_PLATFORM_UNICODE"`), defined below: 

```c
MUDEF const char* mutt_platform_id_get_name(uint16_m platform_id);
```


The function `mutt_platform_id_get_nice_name` returns a readable string version of a given platform ID value, directly converting its macro equivalent to a readable string version (for example, `MUTT_PLATFORM_UNICODE` turns into `"Unicode"`), defined below: 

```c
MUDEF const char* mutt_platform_id_get_nice_name(uint16_m platform_id);
```


Both above-listed functions returns `MUTT_UNKNOWN` if there is no defined macro equivalent for the given platform ID value. Both functions are only defined if `MUTT_NAMES` is defined.

### Unicode encoding ID macros

The following macros are defined for interpreting Unicode encoding IDs:

* [0] `MUTT_UNICODE_ENCODING_1_0`

* [1] `MUTT_UNICODE_ENCODING_1_1`

* [2] `MUTT_UNICODE_ENCODING_ISO_IEC_10646`

* [3] `MUTT_UNICODE_ENCODING_2_0_BMP`

* [4] `MUTT_UNICODE_ENCODING_2_0_FULL`

* [5] `MUTT_UNICODE_ENCODING_VARIATION`

* [6] `MUTT_UNICODE_ENCODING_FULL`

All of the Unicode encoding ID values defined above are the ones defined by TrueType and OpenType as of writing this, and are the only Unicode encoding ID values compatible with mutt, and an error will be thrown for a table if a Unicode encoding ID value is given that is not one of the values defined above.

Note that `MUTT_UNICODE_ENCODING_VARIATION` and `MUTT_UNICODE_ENCODING_FULL` are invalid values for a Unicode encoding ID in a name table; they will not be a given value in the name table if it successfully loaded.

More information about these values is available in the TrueType standard.

The function `mutt_unicode_encoding_id_get_name` returns a stringified version of a given Unicode encoding ID value, directly converting its macro equivalent to a string version (for example, `MUTT_UNICODE_ENCODING_1_0` turns into `"MUTT_UNICODE_ENCODING_1_0"`), defined below: 

```c
MUDEF const char* mutt_unicode_encoding_id_get_name(uint16_m encoding_id);
```


The function `mutt_unicode_encoding_id_get_nice_name` returns a readable string version of a given Unicode encoding ID value, directly converting its macro equivalent to a readable string version (for example, `MUTT_UNICODE_ENCODING_1_0` turns into `"Unicode 1.0"`), defined below: 

```c
MUDEF const char* mutt_unicode_encoding_id_get_nice_name(uint16_m encoding_id);
```


Both above-listed functions returns `MUTT_UNKNOWN` if there is no defined macro equivalent for the given Unicode encoding ID value. Both functions are only defined if `MUTT_NAMES` is defined.

### Macintosh encoding ID macros

The following macros are defined for interpreting Macintosh encoding IDs:

* [00] `MUTT_MACINTOSH_ENCODING_ROMAN`

* [01] `MUTT_MACINTOSH_ENCODING_JAPANESE`

* [02] `MUTT_MACINTOSH_ENCODING_CHINESE_TRADITIONAL`

* [03] `MUTT_MACINTOSH_ENCODING_KOREAN`

* [04] `MUTT_MACINTOSH_ENCODING_ARABIC`

* [05] `MUTT_MACINTOSH_ENCODING_HEBREW`

* [06] `MUTT_MACINTOSH_ENCODING_GREEK`

* [07] `MUTT_MACINTOSH_ENCODING_RUSSIAN`

* [08] `MUTT_MACINTOSH_ENCODING_RSYMBOL`

* [09] `MUTT_MACINTOSH_ENCODING_DEVANAGARI`

* [10] `MUTT_MACINTOSH_ENCODING_GURMUKHI`

* [11] `MUTT_MACINTOSH_ENCODING_GUJARATI`

* [12] `MUTT_MACINTOSH_ENCODING_ODIA`

* [13] `MUTT_MACINTOSH_ENCODING_BANGLA`

* [14] `MUTT_MACINTOSH_ENCODING_TAMIL`

* [15] `MUTT_MACINTOSH_ENCODING_TELUGU`

* [16] `MUTT_MACINTOSH_ENCODING_KANNADA`

* [17] `MUTT_MACINTOSH_ENCODING_MALAYALAM`

* [18] `MUTT_MACINTOSH_ENCODING_SINHALESE`

* [19] `MUTT_MACINTOSH_ENCODING_BURMESE`

* [20] `MUTT_MACINTOSH_ENCODING_KHMER`

* [21] `MUTT_MACINTOSH_ENCODING_THAI`

* [22] `MUTT_MACINTOSH_ENCODING_LAOTIAN`

* [23] `MUTT_MACINTOSH_ENCODING_GEORGIAN`

* [24] `MUTT_MACINTOSH_ENCODING_ARMENIAN`

* [25] `MUTT_MACINTOSH_ENCODING_CHINESE_SIMPLIFIED`

* [26] `MUTT_MACINTOSH_ENCODING_TIBETAN`

* [27] `MUTT_MACINTOSH_ENCODING_MONGOLIAN`

* [28] `MUTT_MACINTOSH_ENCODING_GEEZ`

* [29] `MUTT_MACINTOSH_ENCODING_SLAVIC`

* [30] `MUTT_MACINTOSH_ENCODING_VIETNAMESE`

* [31] `MUTT_MACINTOSH_ENCODING_SINDHI`

* [32] `MUTT_MACINTOSH_ENCODING_UNINTERPRETED`

All of the Macintosh encoding ID values defined above are the ones defined by TrueType and OpenType as of writing this, and are the only Macintosh encoding ID values compatible with mutt, and an error will be thrown for a table if a Macintosh encoding ID value is given that is not one of the values defined above.

More information about these values is available in the TrueType standard.

The function `mutt_macintosh_encoding_id_get_name` returns a stringified version of a given Macintosh encoding ID value, directly converting its macro equivalent to a string version (for example, `MUTT_MACINTOSH_ENCODING_ROMAN` turns into `"MUTT_MACINTOSH_ENCODING_ROMAN"`), defined below: 

```c
MUDEF const char* mutt_macintosh_encoding_id_get_name(uint16_m encoding_id);
```


The function `mutt_macintosh_encoding_id_get_nice_name` returns a readable string version of a given Macintosh encoding ID value, directly converting its macro equivalent to a readable string version (for example, `MUTT_MACINTOSH_ENCODING_ROMAN` turns into `"Roman"`), defined below: 

```c
MUDEF const char* mutt_macintosh_encoding_id_get_nice_name(uint16_m encoding_id);
```


Both above-listed functions returns `MUTT_UNKNOWN` if there is no defined macro equivalent for the given Macintosh encoding ID value. Both functions are only defined if `MUTT_NAMES` is defined.

### Macintosh language ID macros

The following macros are defined for interpreting Macintosh language IDs:

* [000] `MUTT_MACINTOSH_LANGUAGE_ENGLISH`

* [001] `MUTT_MACINTOSH_LANGUAGE_FRENCH`

* [002] `MUTT_MACINTOSH_LANGUAGE_GERMAN`

* [003] `MUTT_MACINTOSH_LANGUAGE_ITALIAN`

* [004] `MUTT_MACINTOSH_LANGUAGE_DUTCH`

* [005] `MUTT_MACINTOSH_LANGUAGE_SWEDISH`

* [006] `MUTT_MACINTOSH_LANGUAGE_SPANISH`

* [007] `MUTT_MACINTOSH_LANGUAGE_DANISH`

* [008] `MUTT_MACINTOSH_LANGUAGE_PORTUGUESE`

* [009] `MUTT_MACINTOSH_LANGUAGE_NORWEGIAN`

* [010] `MUTT_MACINTOSH_LANGUAGE_HEBREW`

* [011] `MUTT_MACINTOSH_LANGUAGE_JAPANESE`

* [012] `MUTT_MACINTOSH_LANGUAGE_ARABIC`

* [013] `MUTT_MACINTOSH_LANGUAGE_FINNISH`

* [014] `MUTT_MACINTOSH_LANGUAGE_GREEK`

* [015] `MUTT_MACINTOSH_LANGUAGE_ICELANDIC`

* [016] `MUTT_MACINTOSH_LANGUAGE_MALTESE`

* [017] `MUTT_MACINTOSH_LANGUAGE_TURKISH`

* [018] `MUTT_MACINTOSH_LANGUAGE_CROATIAN`

* [019] `MUTT_MACINTOSH_LANGUAGE_CHINESE_TRADITIONAL`

* [020] `MUTT_MACINTOSH_LANGUAGE_URDU`

* [021] `MUTT_MACINTOSH_LANGUAGE_HINDI`

* [022] `MUTT_MACINTOSH_LANGUAGE_THAI`

* [023] `MUTT_MACINTOSH_LANGUAGE_KOREAN`

* [024] `MUTT_MACINTOSH_LANGUAGE_LITHUANIAN`

* [025] `MUTT_MACINTOSH_LANGUAGE_POLISH`

* [026] `MUTT_MACINTOSH_LANGUAGE_HUNGARIAN`

* [027] `MUTT_MACINTOSH_LANGUAGE_ESTONIAN`

* [028] `MUTT_MACINTOSH_LANGUAGE_LATVIAN`

* [029] `MUTT_MACINTOSH_LANGUAGE_SAMI`

* [030] `MUTT_MACINTOSH_LANGUAGE_FAROESE`

* [031] `MUTT_MACINTOSH_LANGUAGE_FARSI_PERSIAN`

* [032] `MUTT_MACINTOSH_LANGUAGE_RUSSIAN`

* [033] `MUTT_MACINTOSH_LANGUAGE_CHINESE_SIMPLIFIED`

* [034] `MUTT_MACINTOSH_LANGUAGE_FLEMISH`

* [035] `MUTT_MACINTOSH_LANGUAGE_IRISH_GAELIC`

* [036] `MUTT_MACINTOSH_LANGUAGE_ALBANIAN`

* [037] `MUTT_MACINTOSH_LANGUAGE_ROMANIAN`

* [038] `MUTT_MACINTOSH_LANGUAGE_CZECH`

* [039] `MUTT_MACINTOSH_LANGUAGE_SLOVAK`

* [040] `MUTT_MACINTOSH_LANGUAGE_SLOVENIAN`

* [041] `MUTT_MACINTOSH_LANGUAGE_YIDDISH`

* [042] `MUTT_MACINTOSH_LANGUAGE_SERBIAN`

* [043] `MUTT_MACINTOSH_LANGUAGE_MACEDONIAN`

* [044] `MUTT_MACINTOSH_LANGUAGE_BULGARIAN`

* [045] `MUTT_MACINTOSH_LANGUAGE_UKRAINIAN`

* [046] `MUTT_MACINTOSH_LANGUAGE_BYELORUSSIAN`

* [047] `MUTT_MACINTOSH_LANGUAGE_UZBEK`

* [048] `MUTT_MACINTOSH_LANGUAGE_KAZAKH`

* [049] `MUTT_MACINTOSH_LANGUAGE_AZERBAIJANI_CYRILLIC`

* [050] `MUTT_MACINTOSH_LANGUAGE_AZERBAIJANI_ARABIC`

* [051] `MUTT_MACINTOSH_LANGUAGE_ARMENIAN`

* [052] `MUTT_MACINTOSH_LANGUAGE_GEORGIAN`

* [053] `MUTT_MACINTOSH_LANGUAGE_MOLDAVIAN`

* [054] `MUTT_MACINTOSH_LANGUAGE_KIRGHIZ`

* [055] `MUTT_MACINTOSH_LANGUAGE_TAJIKI`

* [056] `MUTT_MACINTOSH_LANGUAGE_TURKMEN`

* [057] `MUTT_MACINTOSH_LANGUAGE_MONGOLIAN`

* [058] `MUTT_MACINTOSH_LANGUAGE_MONGOLIAN_CYRILLIC`

* [059] `MUTT_MACINTOSH_LANGUAGE_PASHTO`

* [060] `MUTT_MACINTOSH_LANGUAGE_KURDISH`

* [061] `MUTT_MACINTOSH_LANGUAGE_KASHMIRI`

* [062] `MUTT_MACINTOSH_LANGUAGE_SINDHI`

* [063] `MUTT_MACINTOSH_LANGUAGE_TIBETAN`

* [064] `MUTT_MACINTOSH_LANGUAGE_NEPALI`

* [065] `MUTT_MACINTOSH_LANGUAGE_SANSKRIT`

* [066] `MUTT_MACINTOSH_LANGUAGE_MARATHI`

* [067] `MUTT_MACINTOSH_LANGUAGE_BENGALI`

* [068] `MUTT_MACINTOSH_LANGUAGE_ASSAMESE`

* [069] `MUTT_MACINTOSH_LANGUAGE_GUJARATI`

* [070] `MUTT_MACINTOSH_LANGUAGE_PUNJABI`

* [071] `MUTT_MACINTOSH_LANGUAGE_ORIYA`

* [072] `MUTT_MACINTOSH_LANGUAGE_MALAYALAM`

* [073] `MUTT_MACINTOSH_LANGUAGE_KANNADA`

* [074] `MUTT_MACINTOSH_LANGUAGE_TAMIL`

* [075] `MUTT_MACINTOSH_LANGUAGE_TELUGU`

* [076] `MUTT_MACINTOSH_LANGUAGE_SINHALESE`

* [077] `MUTT_MACINTOSH_LANGUAGE_BURMESE`

* [078] `MUTT_MACINTOSH_LANGUAGE_KHMER`

* [079] `MUTT_MACINTOSH_LANGUAGE_LAO`

* [080] `MUTT_MACINTOSH_LANGUAGE_VIETNAMESE`

* [081] `MUTT_MACINTOSH_LANGUAGE_INDONESIAN`

* [082] `MUTT_MACINTOSH_LANGUAGE_TAGALOG`

* [083] `MUTT_MACINTOSH_LANGUAGE_MALAY_ROMAN`

* [084] `MUTT_MACINTOSH_LANGUAGE_MALAY_ARABIC`

* [085] `MUTT_MACINTOSH_LANGUAGE_AMHARIC`

* [086] `MUTT_MACINTOSH_LANGUAGE_TIGRINYA`

* [087] `MUTT_MACINTOSH_LANGUAGE_GALLA`

* [088] `MUTT_MACINTOSH_LANGUAGE_SOMALI`

* [089] `MUTT_MACINTOSH_LANGUAGE_SWAHILI`

* [090] `MUTT_MACINTOSH_LANGUAGE_KINYARWANDA_RUANDA`

* [091] `MUTT_MACINTOSH_LANGUAGE_RUNDI`

* [092] `MUTT_MACINTOSH_LANGUAGE_NYANJA_CHEWA`

* [093] `MUTT_MACINTOSH_LANGUAGE_MALAGASY`

* [094] `MUTT_MACINTOSH_LANGUAGE_ESPERANTO`

* [128] `MUTT_MACINTOSH_LANGUAGE_WELSH`

* [129] `MUTT_MACINTOSH_LANGUAGE_BASQUE`

* [130] `MUTT_MACINTOSH_LANGUAGE_CATALAN`

* [131] `MUTT_MACINTOSH_LANGUAGE_LATIN`

* [132] `MUTT_MACINTOSH_LANGUAGE_QUECHUA`

* [133] `MUTT_MACINTOSH_LANGUAGE_GUARANI`

* [134] `MUTT_MACINTOSH_LANGUAGE_AYMARA`

* [135] `MUTT_MACINTOSH_LANGUAGE_TATAR`

* [136] `MUTT_MACINTOSH_LANGUAGE_UIGHUR`

* [137] `MUTT_MACINTOSH_LANGUAGE_DZONGKHA`

* [138] `MUTT_MACINTOSH_LANGUAGE_JAVANESE`

* [139] `MUTT_MACINTOSH_LANGUAGE_SUNDANESE`

* [140] `MUTT_MACINTOSH_LANGUAGE_GALICIAN`

* [141] `MUTT_MACINTOSH_LANGUAGE_AFRIKAANS`

* [142] `MUTT_MACINTOSH_LANGUAGE_BRETON`

* [143] `MUTT_MACINTOSH_LANGUAGE_INUKTITUT`

* [144] `MUTT_MACINTOSH_LANGUAGE_SCOTTISH_GAELIC`

* [145] `MUTT_MACINTOSH_LANGUAGE_MANX_GAELIC`

* [146] `MUTT_MACINTOSH_LANGUAGE_IRISH_GAELIC_DOT`

* [147] `MUTT_MACINTOSH_LANGUAGE_TONGAN`

* [148] `MUTT_MACINTOSH_LANGUAGE_GREEK_POLYTONIC`

* [149] `MUTT_MACINTOSH_LANGUAGE_GREENLANDIC`

* [150] `MUTT_MACINTOSH_LANGUAGE_AZERBAIJANI`

All of the Macintosh language ID values defined above are the ones defined by TrueType and OpenType as of writing this, and are the only Macintosh language ID values compatible with mutt, and an error will be thrown for a table if a Macintosh language ID value is given that is not one of the values defined above.

More information about these values is available in the TrueType standard.

The function `mutt_macintosh_language_id_get_name` returns a stringified version of a given Macintosh language ID value, directly converting its macro equivalent to a string version (for example, `MUTT_MACINTOSH_LANGUAGE_ENGLISH` turns into `"MUTT_MACINTOSH_LANGUAGE_ENGLISH"`), defined below: 

```c
MUDEF const char* mutt_macintosh_language_id_get_name(uint16_m language_id);
```


The function `mutt_macintosh_language_id_get_nice_name` returns a readable string version of a given Macintosh language ID value, directly converting its macro equivalent to a readable string version (for example, `MUTT_MACINTOSH_LANGUAGE_ENGLISH` turns into `"English"`), defined below: 

```c
MUDEF const char* mutt_macintosh_language_id_get_nice_name(uint16_m language_id);
```


Both above-listed functions returns `MUTT_UNKNOWN` if there is no defined macro equivalent for the given Macintosh language ID value. Both functions are only defined if `MUTT_NAMES` is defined.

### ISO encoding ID macros

The following macros are defined for interpreting ISO encoding IDs:

* [0] `MUTT_ISO_ENCODING_7_BIT_ASCII`

* [1] `MUTT_ISO_ENCODING_10646`

* [2] `MUTT_ISO_ENCODING_8859_1`

All of the ISO encoding ID values defined above are the ones defined by TrueType and OpenType as of writing this, and are the only ISO encoding ID values compatible with mutt, and an error will be thrown for a table if an ISO encoding ID value is given that is not one of the values defined above.

More information about these values is available in the TrueType standard.

The function `mutt_iso_encoding_id_get_name` returns a stringified version of a given ISO encoding ID value, directly converting its macro equivalent to a string version (for example, `MUTT_ISO_ENCODING_7_BIT_ASCII` turns into `"MUTT_ISO_ENCODING_7_BIT_ASCII"`), defined below: 

```c
MUDEF const char* mutt_iso_encoding_id_get_name(uint16_m encoding_id);
```


The function `mutt_iso_encoding_id_get_nice_name` returns a readable string version of a given ISO encoding ID value, directly converting its macro equivalent to a readable string version (for example, `MUTT_ISO_ENCODING_7_BIT_ASCII` turns into `"7-bit ASCII"`), defined below: 

```c
MUDEF const char* mutt_iso_encoding_id_get_nice_name(uint16_m encoding_id);
```


Both above-listed functions returns `MUTT_UNKNOWN` if there is no defined macro equivalent for the given ISO encoding ID value. Both functions are only defined if `MUTT_NAMES` is defined.

### Windows encoding ID macros

The following macros are defined for interpreting Windows encoding IDs:

* [00] `MUTT_WINDOWS_ENCODING_SYMBOL`

* [01] `MUTT_WINDOWS_ENCODING_UNICODE_BMP`

* [02] `MUTT_WINDOWS_ENCODING_SHIFT_JIS`

* [03] `MUTT_WINDOWS_ENCODING_PRC`

* [04] `MUTT_WINDOWS_ENCODING_BIG5`

* [05] `MUTT_WINDOWS_ENCODING_WANSUNG`

* [06] `MUTT_WINDOWS_ENCODING_JOHAB`

* [10] `MUTT_WINDOWS_ENCODING_UNICODE_FULL`

All of the Windows encoding ID values defined above are the ones defined by TrueType and OpenType as of writing this, and are the only Windows encoding ID values compatible with mutt, and an error will be thrown for a table if a Windows encoding ID value is given that is not one of the values defined above.

More information about these values is available in the OpenType standard.

The function `mutt_windows_encoding_id_get_name` returns a stringified version of a given Windows encoding ID value, directly converting its macro equivalent to a string version (for example, `MUTT_WINDOWS_ENCODING_SYMBOL` turns into `"MUTT_WINDOWS_ENCODING_SYMBOL"`), defined below: 

```c
MUDEF const char* mutt_windows_encoding_id_get_name(uint16_m encoding_id);
```


The function `mutt_windows_encoding_id_get_nice_name` returns a readable string version of a given Windows encoding ID value, directly converting its macro equivalent to a readable string version (for example, `MUTT_WINDOWS_ENCODING_SYMBOL` turns into `"Symbol"`), defined below: 

```c
MUDEF const char* mutt_windows_encoding_id_get_nice_name(uint16_m encoding_id);
```


Both above-listed functions returns `MUTT_UNKNOWN` if there is no defined macro equivalent for the given Windows encoding ID value. Both functions are only defined if `MUTT_NAMES` is defined.

# Version macros

mutt defines three macros to define the version of mutt: `MUTT_VERSION_MAJOR`, `MUTT_VERSION_MINOR`, and `MUTT_VERSION_PATCH`, following the format of `vMAJOR.MINOR.PATCH`.

# C standard library dependencies

mutt has several C standard library dependencies not provided by its other library dependencies, all of which are overridable by defining them before the inclusion of its header. This is a list of all of those dependencies.

## `stdlib.h` dependencies

* `mu_malloc` - equivalent to `malloc`.

* `mu_free` - equivalent to `free`.

* `mu_realloc` - equivalent to `realloc`.

## `string.h` dependencies

* `mu_memcpy` - equivalent to `memcpy`.

## `math.h` dependencies

* `mu_pow` - equivalent to `pow`.
