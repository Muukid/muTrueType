

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

* `MUTT_SUCCESS`: the task succeeded.

* `MUTT_FAILED_MALLOC`: a call to malloc failed; memory was insufficient to perform the operation

* `MUTT_FAILED_REALLOC`: a call to realloc failled; memory was insufficient to perform the operation.

* `MUTT_UNFOUND_TABLE`: the table could not be located within the data.

* `MUTT_DUPLICATE_TABLE`: another table with the same tag was found.

* `MUTT_INVALID_TABLE_DIRECTORY_LENGTH`: the length of the given TrueType data is not enough for the table directory. Likely the length is incorrect or the data given is not TrueType data.

* `MUTT_INVALID_TABLE_DIRECTORY_SFNT_VERSION`: the value for "sfntVersion" in the table directory was invalid. Since this is the first value read when loading TrueType data, this most likely means that rather the data given is corrupt, not TrueType data, or is under another incompatible wrapper (such as fonts that use CFF data).

* `MUTT_INVALID_TABLE_DIRECTORY_SEARCH_RANGE`: the value for "searchRange" in the table directory was invalid.

* `MUTT_INVALID_TABLE_DIRECTORY_ENTRY_SELECTOR`: the value for "entrySelector" in the table directory was invalid.

* `MUTT_INVALID_TABLE_DIRECTORY_RANGE_SHIFT`: the value for "rangeShift" in the table directory was invalid.

* `MUTT_INVALID_TABLE_RECORD_OFFSET`: the value for "offset" in a table record was out of range.

* `MUTT_INVALID_TABLE_RECORD_LENGTH`: the value for "length" in a table record was out of range.

* `MUTT_INVALID_TABLE_RECORD_CHECKSUM`: the value for "checksum" in a table record was invalid.

* `MUTT_INVALID_MAXP_LENGTH`: the value for the table length of maxp was invalid. This could mean that an unsupported version of the table is being used.

* `MUTT_INVALID_MAXP_VERSION`: the version value in the maxp table was invalid/unsupported.

* `MUTT_INVALID_MAXP_MAX_ZONES`: the value for "maxZones" in the maxp table was invalid.

* `MUTT_INVALID_HEAD_LENGTH`: the value for the table length of head was invalid. This could mean that an unsupported version of the table is being used.

* `MUTT_INVALID_HEAD_VERSION`: the version value in the head table was invalid/unsupported.

* `MUTT_INVALID_HEAD_MAGIC_NUMBER`: the value for "magicNumber" in the head table was invalid.

* `MUTT_INVALID_HEAD_UNITS_PER_EM`: the value for "unitsPerEm" in the head table was invalid.

* `MUTT_INVALID_HEAD_X_MIN_MAX`: the values for "xMin" and "xMax" in the head table were invalid.

* `MUTT_INVALID_HEAD_Y_MIN_MAX`: the values for "yMin" and "yMax" in the head table were invalid.

* `MUTT_INVALID_HEAD_INDEX_TO_LOC_FORMAT`: the value for "indexToLocFormat" in the head table was invalid.

* `MUTT_INVALID_HEAD_GLYPH_DATA_FORMAT`: the value for "glyphDataFormat" in the head table was invalid/unsupported.

* `MUTT_INVALID_HHEA_LENGTH`: the value for the table length of hhea was invalid. This could mean that an unsupported version of the table is being used.

* `MUTT_INVALID_HHEA_VERSION`: the version value in the hhea table was invalid/unsupported.

* `MUTT_INVALID_HHEA_METRIC_DATA_FORMAT`: the value for "metricDataFormat" in the hhea table was invalid/unsupported.

* `MUTT_INVALID_HHEA_NUMBER_OF_HMETRICS`: the value for "numberOfHMetrics" in the hhea table was invalid/unsupported.

* `MUTT_INVALID_HMTX_LENGTH`: the value for the table length of hmtx was invalid.

* `MUTT_HHEA_REQUIRES_MAXP`: the hhea table failed to load becuase maxp is rather not being loaded or failed to load, and hhea relies on maxp.

* `MUTT_HMTX_REQUIRES_MAXP`: the hmtx table failed to load because maxp is rather not being loaded or failed to load, and hmtx relies on mxap.

* `MUTT_HMTX_REQUIRES_HHEA`: the hhea table failed to load because hhea is rather not being loaded 

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

### Group bit values

The following macros are defined for loading groups of tables:

* [0x000003FE] `MUTT_LOAD_REQUIRED_TABLES` - loads all tables required by TrueType standards (cmap, glyf, head, hhea, hmtx, loca, maxp, name, and post).

* [0xFFFFFFFF] `MUTT_LOAD_ALL` - loads everything; sets all flags.

# Reading information from a TrueType font

## The `muttFont` struct

A TrueType font is represented by the struct `muttFont`. Once successfully loaded, the data stored within a `muttFont` struct is entirely separate from the user-allocated TrueType data.

Inside the `muttFont` struct is all of the loaded information from when it was loaded. The actual full list of members is:

* `uint32_m load_flags`: the load flags that were provided to the load function.

* `muttDirectory* directory`: a pointer to a directory listing all of the tables provided by the given font.

* `muttMaxp* maxp`: a pointer to the maxp table.

* `muttResult maxp_res`: the result of loading the member `maxp`.

* `muttHead* head`: a pointer to the head table.

* `muttResult head_res`: the result of loading the member `head`.

* `muttHhea* hhea`: a pointer to the hhea table.

* `muttResult hhea_res`: the result of loading the member `hhea`.

* `muttHmtx* hmtx`: a pointer to the hmtx table.

* `muttResult hmtx_res`: the result of loading the member `hmtx`.

* `muByte* mem`: the inner allocated memory used for holding necessary data.

* `size_m memlen`: the length of the allocated memory, in bytes.

* `size_m memcur`: offset to the latest unused memory in `mem`, in bytes.

Most of the members are in pairs of pointers and result values. If a requested pointer is 0, it could not be loaded, and its corresponding result value will indicate the result enumerator indicating what went wrong.

The contents of a pointer and result pair for information not included in the load flags are undefined.

Note that if the directory fails to load, the entire loading function fails, and what went wrong is returned in the loading function; this is why there is no respective result for the member `directory`.

Note that if an array in a table or directory is of length 0, the value for the pointer within the respective struct is 0.

## Directory information

The struct `muttDirectory` is used to list all of the tables provided by a TrueType font. It is stored in the struct `muttFont` as `muttFont->directory`, and is similar to TrueType's table directory. It is loaded permanently with the flag `MUTT_LOAD_DIRECTORY`.

Its members are:

* `uint16_m num_tables`: equivalent to "numTables" in the table directory.

* `uint16_m search_range`: equivalent to "searchRange" in the table directory.

* `uint16_m entry_selector`: equivalent to "entrySelector" in the table directory.

* `uint16_m range_shift`: equivalent to "rangeShift" in the table directory.

* `muttTableRecord* table_records`: equivalent to "tableRecords" in the table directory.

The struct `muttTableRecord` is similar to TrueType's table record, and has the following members:

* `uint8_m table_tag[4]`: equivalent to "tableTag" in the table record.

* `uint32_m checksum`: equivalent to "checksum" in the table record.

* `uint32_m offset`: equivalent to "offset" in the table record.

* `uint32_m length`: equivalent to "length" in the table record.

## Maxp information

The struct `muttMaxp` is used to represent the maxp table provided by a TrueType font, stored in the struct `muttFont` as `muttFont->maxp`, and loaded with the flag `MUTT_LOAD_MAXP`.

Its members are:

* `uint16_m version_high`: equivalent to the high bytes of "version" in the maxp table.

* `uint16_m version_low`: equivalent to the low bytes "version" in the maxp table.

* `uint16_m num_glyphs`: equivalent to "numGlyphs" in the maxp table.

* `uint16_m max_points`: equivalent to "maxPoints" in the maxp table.

* `uint16_m max_contours`: equivalent to "maxContours" in the maxp table.

* `uint16_m max_composite_points`: equivalent to "maxCompositePoints" in the maxp table.

* `uint16_m max_composite_contours`: equivalent to "maxCompositeContours" in the maxp table.

* `uint16_m max_zones`: equivalent to "maxZones" in the maxp table.

* `uint16_m max_twilight_points`: equivalent to "maxTwilightPoints" in the maxp table.

* `uint16_m max_storage`: equivalent to "maxStorage" in the maxp table.

* `uint16_m max_function_defs`: equivalent to "maxFunctionDefs" in the maxp table.

* `uint16_m max_instruction_defs`: equivalent to "maxInstructionDefs" in the maxp table.

* `uint16_m max_stack_elements`: equivalent to "maxStackElements" in the maxp table.

* `uint16_m max_size_of_instructions`: equivalent to "maxSizeOfInstructions" in the maxp table.

* `uint16_m max_component_elements`: equivalent to "maxComponentElements" in the maxp table.

* `uint16_m max_component_depth`: equivalent to "maxComponentDepth" in the maxp table.

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

* [0x0001] `MUTT_MAC_STYLE_BOLD`: bold.

* [0x0002] `MUTT_MAC_STYLE_ITALIC`: italic.

* [0x0004] `MUTT_MAC_STYLE_UNDERLINE`: underlined.

* [0x0008] `MUTT_MAC_STYLE_OUTLINE`: outlined.

* [0x0010] `MUTT_MAC_STYLE_SHADOW`: shadow.

* [0x0020] `MUTT_MAC_STYLE_CONDENSED`: condensed.

* [0x0040] `MUTT_MAC_STYLE_EXTENDED`: extended.

## Hhea information

The struct `muttHhea` is used to represent the hhea table provided by a TrueType font, stored in the struct `muttFont` as `muttFont->hhea`, and loaded with the flag `MUTT_LOAD_HHEA`.

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

The struct `muttHmtx` is used to represent the hmtx table provided by a TrueType font, stored in the struct `muttFont` as `muttFont->hmtx`, and loaded with the flag `MUTT_LOAD_HMTX`.

Its members are:

* `muttLongHorMetric* hmetrics` - equivalent to "hMetrics" in the hmtx table.

* `int16_m* left_side_bearings` - equivalent to "leftSideBearings" in the hmtx table.

The struct `muttLongHorMetric` is similar to TrueType's LongHorMetric record, and has the following members:

* `uint16_m advance_width` - equivalent to "advanceWidth" in the LongHorMetric record.

* `int16_m lsb` - equivalent to "lsb" in the LongHorMetric record.

# C standard library dependencies

mutt has several C standard library dependencies not provided by its other library dependencies, all of which are overridable by defining them before the inclusion of its header. This is a list of all of those dependencies.

## `stdlib.h` dependencies

* `mu_malloc`: equivalent to `malloc`.

* `mu_free`: equivalent to `free`.

* `mu_realloc`: equivalent to `realloc`.

## `string.h` dependencies

* `mu_memcpy`: equivalent to `memcpy`.

## `math.h` dependencies

* `mu_pow`: equivalent to `pow`.
