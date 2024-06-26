

# muTrueType v1.0.0

muTrueType (abbreviated to 'mutt') is a public domain header-only single-file C library for retrieving data from the TrueType file format and rendering it to a bitmap via several rendering methods.

***WARNING!*** This library is still under heavy development, has no official releases, and won't be stable until its first public release v1.0.0.

To use it, download the `muTrueType.h` file, add it to your include path, and include it like so:

```c
#define MUTT_IMPLEMENTATION
#include "muTrueType.h"
```

More information about the general structure of a mu library is provided at [the mu library information GitHub repository.](https://github.com/Muukid/mu-library-information)

# Demos

The documentation for this library is rather explicit/hard to read and get a good overview of the library in the process. For this, demos that quickly show the gist of the library and how it works are available in the `demos` folder.

# General overview

mutt is a rather complicated library, so this section gives an overview of the library. More explicit documentation comes after this section.

## Loading a TrueType font

A TrueType font is loaded in as a `muttInfo` struct, and needs to be manually destroyed once successfully loaded. Loading a TrueType font could look something like:

```c
// Load TrueType file

FILE* fptr = fopen("font.ttf", "rb");

// Get size

fseek(fptr, 0L, SEEK_END);
size_t fsize = ftell(fptr);
fseek(fptr, 0L, SEEK_SET);

// Load data into buffer

muByte* data = (muByte*)malloc(fsize);
fread(data, fsize, 1, fptr);

// Close file

fclose(fptr);

// Generate information

muttResult res = MUTT_SUCCESS;
muttInfo info = mu_truetype_get_info(&res, data, fsize);

// ...(do things regarding the font)...

// Free up information

mu_truetype_let_info(&info);
free(data);
```

Note that this means that the data passed to `mu_truetype_get_info` is assumed to exist and stay constant as long as `info` is alive.

## Low-level information querying

mutt has the ability to give low-level information about a TrueType font.

### Tables

mutt has the ability to give information about the tables stored within a TrueType font. All of the tables that are offered by a given TrueType font can be retrieved manually; see the "Table retrieval" section for more.

mutt also internally stores information about required tables within the `muttInfo` struct, such as their position in the TrueType font data and their byte-length. These tables are:

* "cmap" (`muttInfo.req.cmap`)

* "glyf" (`muttInfo.req.glyf`)

* "head" (`muttInfo.req.head`)

* "hhea" (`muttInfo.req.hhea`)

* "hmtx" (`muttInfo.req.hmtx`)

* "loca" (`muttInfo.req.loca`)

* "maxp" (`muttInfo.req.maxp`)

* "name" (`muttInfo.req.name`)

* "post" (`muttInfo.req.post`)

mutt also allows the values implied in several tables to be accessed directly as readable values. These tables are:

* "head" (`muttInfo.head_info`; respective struct `muttHeadInfo`)

* "maxp" (`muttInfo.maxp_info`; respective struct `muttMaxpInfo`)

* "hhea" (`muttInfo.hhea_info`; respective struct `muttHheaInfo`)

For other tables, their contents are extremely variable (such as when they have an array of elements), and are queried via dedicated functions in mutt. These tables are:

* "name" (see "Name table" section)

* "loca" (see "Loca table" section)

* "cmap" (see "Cmap table" section)

* "glyf" (see "Glyf table" section)

## Safety and checks

mutt performs checks on all data that it offers an API to process.\* This means that accessing data in tables that mutt doesn't provide an abstracted API to process is inherently unsafe; this logic also applies to subtables, such as cmap formats without API support.

For the data that mutt does perform checks on, checks are performed on all values to ensure that they're a possibly correct value according to the specification. Checks are also performed to ensure that data that are interpreted as offsets in the data (such as offsets to a subtable) are within allowed ranges to provide safety against attack vectors.

If a check finds something wrong, it will return what was wrong in the form of a `muttResult` enumerator value, usually specifying exactly what value caused an error.

\* - mutt is currently being programmed with the intent of this being the goal by release v1.0.0, and is still in an experimental state that doesn't offer full safety checks.

## Limitations

### Data processing

mutt is primarily built around processing the 9 required tables in TrueType, having an API designed for extracting data from them. Support for other optional tables, and tables that are given via extensions such as TrueType, may be added later, but for now, TrueType supports very little other than that.

See the "Tables" section for more explicit information about what tables are supported and where they're used in the API.

### Format support

mutt, as of right now, only supports conversion of codepoints to glyph ids under format 4, which is usually provided by TrueType fonts. Support for other formats is planned.

### Support for extensions such as OpenType

mutt generally sticks to TrueType specifications, meaning that it won't be able to parse files built purely on extensions such as OpenType, with "purely" in this context meaning that it's built in such a way that it's incompatible with TrueType specifications, such as an OpenType file that doesn't define glyph data under the common "glyf" table and instead uses CFF or CFF2 formats.

However, this doesn't mean that mutt will never be expanded to parse tables not explicitly outlined in the TrueType specification, but outlined in extensions; the file only needs to be in a TrueType wrapper and specify all of the tables required by TrueType.

# Licensing

mutt is licensed under public domain or MIT, whichever you prefer. More information is provided in the accompanying file `license.md` and at the bottom of `muTrueType.h`.

# TrueType documentation

mutt has the ability to work fairly low-level in the details of TrueType, meaning that more deep usage of mutt's API (beyond just fetching glyph IDs and rendering them to a bitmap) necessitates an understanding of the TrueType documentation. Terms from the TrueType documentation will be used with the assumption that the user has read it and understands these terms.

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

* `MUTT_UNEXPECTED_EOF`: the file unexpectedly ended; this means that rather the file is corrupt, or an out-of-range index or length was given by it.

* `MUTT_INVALID_HEAD_TABLE_LENGTH`: the recorded length of the head table was invalid.

* `MUTT_INVALID_MAXP_TABLE_LENGTH`: the recorded length of the maxp table was invalid.

* `MUTT_INVALID_HHEA_TABLE_LENGTH`: the recorded length of the hhea table was invalid.

* `MUTT_INVALID_NAME_TABLE_LENGTH`: the recorded length of the name table was invalid.

* `MUTT_INVALID_CMAP_TABLE_LENGTH`: the recorded length of the cmap table was invalid.

* `MUTT_INVALID_CMAP_F0_TABLE_LENGTH`: the recorded length of a format 0-cmap subtable was invalid.

* `MUTT_INVALID_CMAP_F4_TABLE_LENGTH`: the recorded length of a format 4-cmap subtable was invalid.

* `MUTT_INVALID_CMAP_F12_TABLE_LENGTH`: the recorded length of a format 12-cmap subtable was invalid.

* `MUTT_INVALID_TABLE_DIRECTORY_SFNT_VERSION`: the "sfntVersion" value specified in the table directory was invalid. Because this is the first value read, if this error occurs, it is likely that the data given is not TrueType data (this error gets triggered if the file is OpenType as well).

* `MUTT_INVALID_TABLE_DIRECTORY_NUM_TABLES`: the "numTables" value specified in the table directory was invalid.

* `MUTT_INVALID_TABLE_DIRECTORY_SEARCH_RANGE`: the "searchRange" value specified in the table directory was invalid.

* `MUTT_INVALID_TABLE_DIRECTORY_ENTRY_SELECTOR`: the "entrySelector" value specified in the table directory was invalid.

* `MUTT_INVALID_TABLE_DIRECTORY_RANGE_SHIFT`: the "rangeShift" value specified in the table directory was invalid.

* `MUTT_INVALID_TABLE_RECORD_OFFSET`: the "offset" value specified in a table record was out of range.

* `MUTT_INVALID_TABLE_RECORD_LENGTH`: the "length" value specified in a table record was out of range.

* `MUTT_INVALID_TABLE_RECORD_CHECKSUM`: the "checksum" value specified in a table record was invalid.

* `MUTT_INVALID_HEAD_MAGIC_NUMBER`: the "magicNumber" value specified in the head table was invalid.

* `MUTT_INVALID_HEAD_UNITS_PER_EM`: the "unitsPerEm" value specified in the head table was invalid.

* `MUTT_INVALID_HEAD_X_MIN_MAX`: the "xMin" and "xMax" values specified in the head table were invalid.

* `MUTT_INVALID_HEAD_Y_MIN_MAX`: the "yMin" and "yMax" values specified in the head table were invalid.

* `MUTT_INVALID_HEAD_LOWEST_REC_PPEM`: the "lowestRecPPEM" value specified in the head table was invalid.

* `MUTT_INVALID_HEAD_INDEX_TO_LOC_FORMAT`: the "indexToLocFormat" value specified in the head table was invalid.

* `MUTT_INVALID_HEAD_GLYPH_DATA_FORMAT`: the "glyphDataFormat" value specified in the head table was invalid.

* `MUTT_INVALID_MAXP_MAX_ZONES`: the "maxZones" value specified in the maxp table was invalid.

* `MUTT_INVALID_HHEA_METRIC_DATA_FORMAT`: the "metricDataFormat" value specified in the hhea table was invalid.

* `MUTT_INVALID_CMAP_F0_GLYPH_ID`: a value in the "glyphIdArray" within a format 0-cmap subtable was invalid.

* `MUTT_INVALID_CMAP_F4_SEG_COUNT`: the "segCountX2" value specified in a format 4-cmap subtable was invalid.

* `MUTT_INVALID_CMAP_F4_SEARCH_RANGE`: the "searchRange" value specified in a format 4-cmap subtable was invalid.

* `MUTT_INVALID_CMAP_F4_ENTRY_SELECTOR`: the "entrySelector" value specified in a format 4-cmap subtable was invalid.

* `MUTT_INVALID_CMAP_F4_RANGE_SHIFT`: the "rangeShift" value specified in a format 4-cmap subtable was invalid.

* `MUTT_INVALID_CMAP_F4_END_CODE`: a value in the "endCode" array specified in a format 4-cmap subtable was invalid.

* `MUTT_INVALID_CMAP_F4_START_CODE`: a value in the "startCode" array specified in a format 4-cmap subtable was invalid.

* `MUTT_INVALID_CMAP_F4_ID_RANGE_OFFSET`: a value in the "idRangeOffset" array specified in a format 4-cmap subtable was invalid, AKA gave a value outside of the array "glyphIdArray".

* `MUTT_INVALID_CMAP_F4_ID_DELTA`: a value in the "idDelta" array specified in a format 4-cmap subtable was invalid, AKA resulted in a non-existent glyph id.

* `MUTT_MISSING_REQUIRED_TABLE`: a required table could not be located.

Most of these errors getting triggered imply that rather the data is corrupt (especially in regards to checksum errors), uses some extension or format not supported by this library (such as OpenType), has accidental incorrect values, or is purposely malformed to attempt to get out of the memory region of the file data.

## Result name function

The function `mutt_result_get_name` converts a `muttResult` value into a `const char*` representation, defined below: 

```c
MUDEF const char* mutt_result_get_name(muttResult result);
```


Note that this function is only defined if `MUTT_NAMES` is defined before the inclusion of the header file.

This function returns `"MUTT_UNKNOWN"` if a respective name could not be found.

# Check

mutt has a section of its API dedicated to checking if given TrueType data is not only valid but safe. This API is, on default, used by the loading function '`mu_truetype_get_info`', meaning that all of these functions are called by `mu_truetype_get_info`.

Note that checks are not performed on tables that mutt does not have support for in its API.

## Table directory check

The function `mu_truetype_check_table_directory` checks if the table directory of given TrueType data is valid, defined below: 

```c
MUDEF muttResult mu_truetype_check_table_directory(muByte* data, size_m data_size);
```


## Checksum table check

The function `mu_truetype_check_table_checksum` checks if a given table's checksum value is valid, defined below: 

```c
MUDEF muttResult mu_truetype_check_table_checksum(muByte* table, uint32_m length, uint32_m checksum);
```


## Head table check

The function `mu_truetype_check_head` checks if the head table provided by a TrueType font is valid, defined below: 

```c
MUDEF muttResult mu_truetype_check_head(muByte* table, uint32_m table_length);
```


## Maxp table check

The function `mu_truetype_check_maxp` checks if the maxp table provided by a TrueType font is valid, defined below: 

```c
MUDEF muttResult mu_truetype_check_maxp(muByte* table, uint32_m table_length);
```


## Hhea table check

The function `mu_truetype_check_hhea` checks if the hhea table provided by a TrueType font is valid, defined below: 

```c
MUDEF muttResult mu_truetype_check_hhea(muByte* table, uint32_m table_length);
```


## Name table check

The function `mu_truetype_check_names` checks if the name table provided by a TrueType font is valid, defined below: 

```c
MUDEF muttResult mu_truetype_check_names(muByte* table, uint32_m table_length);
```


## Cmap table check

The function `mu_truetype_check_cmap` checks if the cmap table provided by a TrueType font is valid, defined below: 

```c
MUDEF muttResult mu_truetype_check_cmap(muttInfo* info);
```


This function only checks table formats 0. All other formats go ignored.

## Cmap format table checks

There are several functions defined to check certain format subtables within a cmap table. They are defined below.

Note that each of these functions take in a parameter "`muByte* table`", which is expected to be a pointer to the first value in the subtable that isn't 'format', 'length', or a reserved value, but `length` is expected to include these values.

### Cmap format 0 subtable check

The function `mu_truetype_check_format0` checks if the format 0-cmap subtable provided by a TrueType font is valid, defined below: 

```c
MUDEF muttResult mu_truetype_check_format0(muttInfo* info, muByte* table, uint16_m length);
```


### Cmap format 4 subtable check

The function `mu_truetype_check_format4` checks if the format 4-cmap subtable provided by a TrueType font is valid, defined below: 

```c
MUDEF muttResult mu_truetype_check_format4(muttInfo* info, muByte* table, uint16_m length);
```


# Direct table information

mutt gives an API for querying information about tables stored within a TrueType font.

## Table

The struct `muttTable` is used to define a table in a TrueType font. It has the following members:

* `offset`: the offset to the table, in bytes, starting from the beginning of the TrueType font data, defined below: 

```c
uint32_m offset;
```


* `length`: the length of the table, in bytes, defined below: 

```c
uint32_m length;
```


Note that `offset` is also used to indicate whether or not a given table exists within a TrueType font; if `muttTable.offset == 0`, then the table does not exist.

## Required tables

The struct `muttRequiredTables` is used to define the required tables within a TrueType font. It has the following members:

* `cmap`: the "cmap" table, defined below: 

```c
muttTable cmap;
```


* `glyf`: the "glyf" table, defined below: 

```c
muttTable glyf;
```


* `head`: the "head" table, defined below: 

```c
muttTable head;
```


* `hhea`: the "hhea" table, defined below: 

```c
muttTable hhea;
```


* `hmtx`: the "hmtx" table, defined below: 

```c
muttTable hmtx;
```


* `loca`: the "loca" table, defined below: 

```c
muttTable loca;
```


* `maxp`: the "maxp" table, defined below: 

```c
muttTable maxp;
```


* `name`: the "name" table, defined below: 

```c
muttTable name;
```


* `post`: the "post" table, defined below: 

```c
muttTable post;
```


## Head table information

The struct `muttHeadInfo` is used to define the information provided by the necessary "head" table within a TrueType font. It has the following members:

* `major_version`: equivalent to "majorVersion" in the "head" table, defined below: 

```c
uint16_m major_version;
```


* `minor_version`: equivalent to "minorVersion" in the "head" table, defined below: 

```c
uint16_m minor_version;
```


* `font_revision_high`: equivalent to the high-bytes of "fontRevision" in the "head" table, defined below: 

```c
int16_m font_revision_high;
```


* `font_revision_low`: equivalent to the low-bytes of "fontRevision" in the "head" table, defined below: 

```c
int16_m font_revision_low;
```


* `checksum_adjustment`: equivalent to "checksumAdjustment" in the "head" table, defined below: 

```c
uint32_m checksum_adjustment;
```


* `magic_number`: equivalent to "magicNumber" in the "head" table, defined below: 

```c
uint32_m magic_number;
```


* `flags`: equivalent to "flags" in the "head" table, defined below: 

```c
uint16_m flags;
```


* `units_per_em`: equivalent to "unitsPerEm" in the "head" table, defined below: 

```c
uint16_m units_per_em;
```


* `created`: equivalent to "created" in the "head" table, defined below: 

```c
int64_m created;
```


* `modified`: equivalent to "modified" in the "head" table, defined below: 

```c
int64_m modified;
```


* `x_min`: equivalent to "xMin" in the "head" table, defined below: 

```c
int16_m x_min;
```


* `y_min`: equivalent to "yMin" in the "head" table, defined below: 

```c
int16_m y_min;
```


* `x_max`: equivalent to "xMax" in the "head" table, defined below: 

```c
int16_m x_max;
```


* `y_max`: equivalent to "yMax" in the "head" table, defined below: 

```c
int16_m y_max;
```


* `mac_style`: equivalent to "macStyle" in the "head" table, defined below: 

```c
uint16_m mac_style;
```


* `lowest_rec_ppem`: equivalent to "lowestRecPPEM" in the "head" table, defined below: 

```c
uint16_m lowest_rec_ppem;
```


* `font_direction_hint`: equivalent to "fontDirectionHint" in the "head" table, defined below: 

```c
int16_m font_direction_hint;
```


* `index_to_loc_format`: equivalent to "indexToLocFormat" in the "head" table, defined below: 

```c
int16_m index_to_loc_format;
```


* `glyph_data_format`: equivalent to "glyphDataFormat" in the "head" table, defined below: 

```c
int16_m glyph_data_format;
```


## Maxp table information

The struct `muttMaxpInfo` is used to define the information provided by the necessary "maxp" table within a TrueType font. It has the following members:

* `version_high`: equivalent to the high-bytes of "version" in the "maxp" table, defined below: 

```c
uint16_m version_high;
```


* `version_low`: equivalent to the low-bytes of "version" in the "maxp" table, defined below: 

```c
uint16_m version_low;
```


* `num_glyphs`: equivalent to "numGlyphs" in the "maxp" table, defined below: 

```c
uint16_m num_glyphs;
```


* `max_points`: equivalent to "maxPoints" in the "maxp" table, defined below: 

```c
uint16_m max_points;
```


* `max_contours`: equivalent to "maxContours" in the "maxp" table, defined below: 

```c
uint16_m max_contours;
```


* `max_composite_points`: equivalent to "maxCompositePoints" in the "maxp" table, defined below: 

```c
uint16_m max_composite_points;
```


* `max_composite_contours`: equivalent to "maxCompositeContours" in the "maxp" table, defined below: 

```c
uint16_m max_composite_contours;
```


* `max_zones`: equivalent to "maxZones" in the "maxp" table, defined below: 

```c
uint16_m max_zones;
```


* `max_twilight_points`: equivalent to "maxTwilightPoints" in the "maxp" table, defined below: 

```c
uint16_m max_twilight_points;
```


* `max_storage`: equivalent to "maxStorage" in the "maxp" table, defined below: 

```c
uint16_m max_storage;
```


* `max_function_defs`: equivalent to "maxFunctionDefs" in the "maxp" table, defined below: 

```c
uint16_m max_function_defs;
```


* `max_instruction_defs`: equivalent to "maxInstructionDefs" in the "maxp" table, defined below: 

```c
uint16_m max_instruction_defs;
```


* `max_stack_elements`: equivalent to "maxStackElements" in the "maxp" table, defined below: 

```c
uint16_m max_stack_elements;
```


* `max_size_of_instructions`: equivalent to "maxSizeOfInstructions" in the "maxp" table, defined below: 

```c
uint16_m max_size_of_instructions;
```


* `max_component_elements`: equivalent to "maxComponentElements" in the "maxp" table, defined below: 

```c
uint16_m max_component_elements;
```


* `max_component_depth`: equivalent to "maxComponentDepth" in the "maxp" table, defined below: 

```c
uint16_m max_component_depth;
```


## Hhea table information

The struct `muttHheaInfo` is used to define the information provided by the necessary "hhea" table within a TrueType font. It has the following members:

* `major_version`: equivalent to "majorVersion" in the "hhea" table, defined below: 

```c
uint16_m major_version;
```


* `minor_version`: equivalent to "minorVersion" in the "hhea" table, defined below: 

```c
uint16_m minor_version;
```


* `ascender`: equivalent to "ascender" in the "hhea" table, defined below: 

```c
int16_m ascender;
```


* `descender`: equivalent to "descender" in the "hhea" table, defined below: 

```c
int16_m descender;
```


* `line_gap`: equivalent to "lineGap" in the "hhea" table, defined below: 

```c
int16_m line_gap;
```


* `advance_max_width`: equivalent to "advanceWidthMax" in the "hhea" table, defined below: 

```c
uint16_m advance_max_width;
```


* `min_left_side_bearing`: equivalent to "minLeftSideBearing" in the "hhea" table, defined below: 

```c
int16_m min_left_side_bearing;
```


* `min_right_side_bearing`: equivalent to "minRightSideBearing" in the "hhea" table, defined below: 

```c
int16_m min_right_side_bearing;
```


* `x_max_extent`: equivalent to "xMaxExtent" in the "hhea" table, defined below: 

```c
int16_m x_max_extent;
```


* `caret_slope_rise`: equivalent to "caretSlopeRise" in the "hhea" table, defined below: 

```c
int16_m caret_slope_rise;
```


* `caret_slope_run`: equivalent to "caretSlopeRun" in the "hhea" table, defined below: 

```c
int16_m caret_slope_run;
```


* `caret_offset`: equivalent to "caretOffset" in the "hhea" table, defined below: 

```c
int16_m caret_offset;
```


* `metric_data_format`: equivalent to "metricDataFormat" in the "hhea" table, defined below: 

```c
int16_m metric_data_format;
```


* `number_of_hmetrics`: equivalent to "numberOfHMetrics" in the "hhea" table, defined below: 

```c
uint16_m number_of_hmetrics;
```


# General TrueType information

The struct `muttInfo` is used to refer to the general information about a TrueType font, and is used to refer to a TrueType font across multiple function calls. It has the following documented members:

`data`: the data of the TrueType font, defined below: 

```c
muByte* data;
```


`size`: the size of the TrueType font data, in bytes, defined below: 

```c
size_m size;
```


`table_count`: the amount of tables within the TrueType font data, defined below: 

```c
uint16_m table_count;
```


`req`: the required tables in the TrueType font data, defined below: 

```c
muttRequiredTables req;
```


`head_info`: information retrieved from the required "head" table, defined below: 

```c
muttHeadInfo head_info;
```


`maxp_info`: information retrieved from the required "maxp" table, defined below: 

```c
muttMaxpInfo maxp_info;
```


`hhea_info`: information retrieved from the required "hhea" table, defined below: 

```c
muttHheaInfo hhea_info;
```


All of the members, including those regarding the raw data of the TrueType font, are automatically generated upon a successful call to `mu_truetype_get_info`, and are invalid upon its respective call to `mu_truetype_let_info`. The members are meant to be read, not written.

## Retrieve TrueType information

The function `mu_truetype_get_info` retrieves information about TrueType data and stores it in a `muttInfo` struct, defined below: 

```c
MUDEF muttInfo mu_truetype_get_info(muttResult* result, muByte* data, size_m size);
```


Every successful call to `mu_truetype_get_info` must be matched with a call to `mutt_true_type_let_info`.

The pointer `data` is assumed to be valid and unchanged throughout the returned `muttInfo` struct's lifetime.

## Free TrueType information

The function `mu_truetype_let_info` frees the information retrieved about TrueType data stored in a `muttInfo` struct, defined below: 

```c
MUDEF void mu_truetype_let_info(muttInfo* info);
```


This function must be called on every successfully created `muttInfo` struct.

# Table retrieval

mutt can retrieve information about all of the tables offered in a TrueType ofnt.

## Get table amount

The function `mu_truetype_get_table_count` returns the amount of tables within a given TrueType font, defined below: 

```c
MUDEF uint16_m mu_truetype_get_table_count(muttInfo* info);
```


This function returns `info->table_count`.

## Get table information

The function `mu_truetype_get_table` retrieves information about a requested table witin a given TrueType font, defined below: 

```c
MUDEF void mu_truetype_get_table(muttInfo* info, uint16_m table, muttTable* table_info, char* name);
```


`table` must be a valid index referring to a table, less than `info->table_count`.

`table_info`, if not 0, will be dereferenced and filled in with information about the table.

`name`, if not 0, will be filled with the 4-byte identifier for the table.

# Platform-specific encoding

TrueType has support for multiple character encoding types. For the sake of this library's explicitness, when a character or string of characters is referenced, it is supplemented with a description of its platform encoding, which corresponds to TrueType's documentation.

## Encoding types

The types `muttPlatformID` and `muttEncodingID` are defined (both as `uint16_m`) to represent a platform ID and corresponding encoding ID.

### Platform ID values

* `MUTT_PLATFORM_ID_UNICODE`: Platform ID 0; "Unicode" (UTF-16BE).

* `MUTT_PLATFORM_ID_MACINTOSH`: Platform ID 1; "Macintosh".

* `MUTT_PLATFORM_ID_WINDOWS`: Platform ID 3; "Windows".

* `MUTT_PLATFORM_ID_CUSTOM`: Platform ID 4; "Custom".

### Unicode encoding IDs

* `MUTT_UNICODE_ENCODING_2_0_BMP`: Encoding ID 3; "Unicode 2.0 and onwards semantics, Unicode BMP only".

* `MUTT_UNICODE_ENCODING_2_0`: Encoding ID 4; "Unicode 2.0 and onwards semantics, Unicode full repertoire".

* `MUTT_UNICODE_ENCODING_VAR`: Encoding ID 5; "Unicode variation sequences—for use with subtable format 14".

* `MUTT_UNICODE_ENCODING_FULL`: Encoding ID 6; "Unicode full repertoire—for use with subtable format 13".

### Windows encoding IDs

* `MUTT_WINDOWS_ENCODING_SYMBOL`: Encoding ID 0; "Symbol".

* `MUTT_WINDOWS_ENCODING_UNICODE_BMP`: Encoding ID 1; "Unicode BMP".

* `MUTT_WINDOWS_ENCODING_SHIFTJIS`: Encoding ID 2; "ShiftJIS".

* `MUTT_WINDOWS_ENCODING_PRC`: Encoding ID 3; "PRC".

* `MUTT_WINDOWS_ENCODING_BIG5`: Encoding ID 4; "Big5".

* `MUTT_WINDOWS_ENCODING_WANSUNG`: Encoding ID 5; "Wansung".

* `MUTT_WINDOWS_ENCODING_JOHAB`: Encoding ID 6; "Johab".

* `MUTT_WINDOWS_ENCODING_UNICODE`: Encoding ID 10; "Unicode full repertoire".

## Encoding struct

The struct `muttEncoding` is used to refer to character encoding. It has the following members:

* `platform_id`: the platform ID, defined below: 

```c
muttPlatformID platform_id;
```


* `encoding_id`: the encoding ID, defined below: 

```c
muttEncodingID encoding_id;
```


# Name table

mutt can retrieve information from the "name" table in TrueType based on a requested name ID.

## Name ID

The type `muttNameID` (`uint16_m`) is used to represent a name ID in TrueType. Any value based on the TrueType standard will work, but this type gives a representation for some common name IDs.

### Values

* `MUTT_NAME_COPYRIGHT_NOTICE`: ID 0 based on TrueType standards; "Copyright notice."

* `MUTT_NAME_FONT_FAMILY`: ID 1 based on TrueType standards; "Font Family name."

* `MUTT_NAME_FONT_SUBFAMILY`: ID 2 based on TrueType standards; "Font Subfamily name."

* `MUTT_NAME_FULL_FONT_NAME`: ID 4 based on TrueType standards; "Full font name".

* `MUTT_NAME_VERSION`: ID 5 based on TrueType standards; "Version string."

* `MUTT_NAME_TRADEMARK`: ID 7 based on TrueType standards; "Trademark."

* `MUTT_NAME_MANUFACTURER`: ID 8 based on TrueType standards; "Manufacturer Name."

* `MUTT_NAME_DESIGNER`: ID 9 based on TrueType standards; "Designer."

* `MUTT_NAME_DESCRIPTION`: ID 10 based on TrueType standards; "Description."

* `MUTT_NAME_VENDOR_URL`: ID 11 based on TrueType standards; "URL of Vendor."

* `MUTT_NAME_DESIGNER_URL`: ID 12 based on TrueType standards; "URL of Designer."

* `MUTT_NAME_LICENSE_DESCRIPTION`: ID 13 based on TrueType standards; "License Description."

* `MUTT_NAME_LICENSE_INFO_URL`: ID 14 based on TrueType standards; "License Info URL."

* `MUTT_NAME_TYPOGRAPHIC_FAMILY`: ID 16 based on TrueType standards; "Typographic Family name."

* `MUTT_NAME_SAMPLE_TEXT`: ID 19 based on TrueType standards; "Sample text."

### Name

The function `mutt_name_id_get_name` converts a `muttNameID` value to a `const char*` representation, defined below: 

```c
MUDEF const char* mutt_name_id_get_name(muttNameID nameID);
```


The function `mutt_name_id_get_nice_name` converts a `muttNameID` value to a more readable `const char*` representation, defined below: 

```c
MUDEF const char* mutt_name_id_get_nice_name(muttNameID nameID);
```


Note that these functions are only defined if `MUTT_NAMES` is defined before the inclusion of the header file.

These functions return `"MUTT_UNKNOWN"` if a respective name could not be found.

## Get names

### Get offered name IDs

The function `mu_truetype_get_name_ids` is used to retrieve the name IDs offered by a TrueType font, defined below: 

```c
MUDEF uint16_m mu_truetype_get_name_ids(muttInfo* info, muttNameID* ids);
```


This function returns the amount of name IDs specified by the font. If `ids` is not 0, `ids` is expected to be a pointer to an array of `uint16_m`s at least the length of the amount of name IDs specified by the font, and will be written to as such.

### Get particular name ID

The function `mu_truetype_get_name_id` returns the name ID of a specific name, defined below: 

```c
MUDEF uint16_m mu_truetype_get_name_id(muttInfo* info, uint16_m index);
```


`index` is the index of the name ID being requested.

### Get name

The function `mu_truetype_get_name` retrieves the string for a given name ID, defined below: 

```c
MUDEF char* mu_truetype_get_name(muttInfo* info, uint16_m name_id_index, muttEncoding* encoding, uint16_m* length);
```


This function returns a pointer to an offset in the TrueType font data within `info` which holds the name.

`name_id_index` is the index of the name ID being requested; for example, if the font in question offers 7 name IDs and you wanted the 5th name ID specified, `name_id_index` should be 4. `name_id_index` is *not* the name ID, but instead the index the name in question specified by the font.

If `length` is not 0, `length` is dereferenced and set to the length of the name in bytes.

If `encoding` is not 0, `encoding` is dereferenced and set to the encoding of the string.

Note that the function returned does not necessarily have a null-terminating character.

# Loca table

## Glyph ID referencing

The type `muttGlyphID` (defined as `uint16_m`) is used to refer to a glyph specified by a TrueType font. It matches a glyph's index as specified in the "loca" table. Characters can be converted to these types with the "cmap" section of the mutt API.

Note that a `muttGlyphID` can refer to either a simple or a composite glyph.

Note that the value '0' will always be valid for a `muttGlyphID` in regards to a valid TrueType font, as all TrueType fonts have to specify a glyph for missing characters. This value is used by mutt to refer to a glyph that doesn't exist (for example, if a corresponding `muttGlyphID` is requested for a given character but the font doesn't specify it, 0 is returned, giving the missing character glyph).

## Get glyf table via glyph ID

The function `mu_truetype_get_glyf_table` returns a pointer to a glyf table data from a glyph ID based off of the "loca" table data, defined below: 

```c
MUDEF muByte* mu_truetype_get_glyf_table(muttInfo* info, muttGlyphID id, uint32_m* length);
```


If `length` is not 0, `length` is dereferenced and set to the length of the glyf table data being pointed to in bytes.

`id` must be a valid glyph ID value and less than `info.maxp_info.num_glyphs`.

# Cmap table

## Encoding record definition

The struct `muttEncodingRecord` is used to define a TrueType cmap encoding record. It has the following members:

* `encoding`: the encoding, defined below: 

```c
muttEncoding encoding;
```


* `subtable`: a pointer to the subtable in the TrueType data, defined below: 

```c
muByte* subtable;
```


* `format`: the format of the subtable, defined below: 

```c
uint16_m format;
```


* `language`: the Macintosh-specific language of the subtable, defined below: 

```c
uint32_m language;
```


The member "`language`" is 0 if `format` does not have a language field.

## Get encoding record

The function `mu_truetype_get_encoding_record` retrieves a requested encoding record given in the cmap table of a given TrueType font, defined below: 

```c
MUDEF void mu_truetype_get_encoding_record(muttInfo* info, muttEncodingRecord* record, uint16_m id);
```


`id` is the index of the encoding record; the amount of encoding records (and optionally all of the encoding records) can be retrieved via the function `mu_truetype_get_encoding_records`.

## Get all encoding records

The function `mu_truetype_get_encoding_records` retrieves the encoding records listed in the cmap table of a given TrueType font, defined below: 

```c
MUDEF uint16_m mu_truetype_get_encoding_records(muttInfo* info, muttEncodingRecord* records);
```


This function returns the amount of encoding records stored within the given TrueType font.

If `records` is not 0, `records` should be a `muttEncodingRecord` array of a length equivalent to how many encoding records are stored within the given TrueType font.

## Cmap formats

Since TrueType stores its cmap data in multiple formats, mutt has a struct and corresponding lookup function for each supported format type.

### Format 4

#### Struct

The struct for format 4 is `muttFormat4`, and its members are:

`table`: a pointer to the table data, defined below: 

```c
muByte* table;
```


`length`: equivalent to "length" in the format 4 cmap subtable, defined below: 

```c
uint16_m length;
```


`language`: equivalent to "language" in the format 4 cmap subtable, defined below: 

```c
uint16_m language;
```


`seg_count_x2`: equivalent to "segCountX2" in the format 4 cmap subtable, defined below: 

```c
uint16_m seg_count_x2;
```


`seg_count`: `seg_count_x2` divided by two, defined below: 

```c
uint16_m seg_count;
```


`search_range`: equivalent to "searchRange" in the format 4 cmap subtable, defined below: 

```c
uint16_m search_range;
```


`entry_selector`: equivalent to "entrySelector" in the format 4 cmap subtable, defined below: 

```c
uint16_m entry_selector;
```


`range_shift`: equivalent to "rangeShift" in the format 4 cmap subtable, defined below: 

```c
uint16_m range_shift;
```


`end_code`: a pointer to the "endCode" byte data in the format 4 cmap subtable, defined below: 

```c
muByte* end_code;
```


`start_code`: a pointer to the "startCode" byte data in the format 4 cmap subtable, defined below: 

```c
muByte* start_code;
```


`id_delta`: a pointer to the "idDelta" byte data in the format 4 cmap subtable, defined below: 

```c
muByte* id_delta;
```


`id_range_offset`: a pointer to the "idRangeOffset" byte data in the format 4 cmap subtable, defined below: 

```c
muByte* id_range_offset;
```


`glyph_id_array`: a pointer to the "glyphIdArray" byte data in the format 4 cmap subtable, defined below: 

```c
muByte* glyph_id_array;
```


`glyph_id_length`: the length of `glyph_id_array`, in bytes, defined below: 

```c
uint16_m glyph_id_length;
```


Note that `id_delta` is incorrectly defined in Apple's TrueType reference manual as being an unsigned 16-bit array; it is a signed 16-bit array.

#### Get format 4 information

The function `mu_truetype_get_format_4` is used to get information about a format 4 cmap subtable, defined below: 

```c
MUDEF void mu_truetype_get_format_4(muByte* subtable, muttFormat4* format);
```


`subtable` is equivalent to `muttEncodingRecord.subtable` if `muttEncodingRecord.format` equals 4.

#### Get glyph from format 4

The function `mu_truetype_get_glyph_format_4` returns a glyph ID from a format 4 cmap subtable, defined below: 

```c
MUDEF muttGlyphID mu_truetype_get_glyph_format_4(muttFormat4* format, uint16_m character_code);
```


`character_code` is a valid character code in regards to the respective encoding record's platform & encoding ID.

This function returns 0 if the given character code is not mapped.

# Glyf table

## Header information

The header information about a glyph is the information provided by the "Glyph Header". It is represented as the struct `muttGlyphHeader` in mutt.

### Struct

The struct `muttGlyphHeader` has the following members:

* `data`: a pointer to the glyph header data, defined below: 

```c
muByte* data;
```


* `number_of_contours`: equivalent to "numberOfContours" in the "Glyph Header" of the "glyf" table, defined below: 

```c
int16_m number_of_contours;
```


* `x_min`: equivalent to "xMin" in the "Glyph Header" of the "glyf" table, defined below: 

```c
int16_m x_min;
```


* `y_min`: equivalent to "yMin" in the "Glyph Header" of the "glyf" table, defined below: 

```c
int16_m y_min;
```


* `x_max`: equivalent to "xMax" in the "Glyph Header" of the "glyf" table, defined below: 

```c
int16_m x_max;
```


* `y_max`: equivalent to "yMax" in the "Glyph Header" of the "glyf" table, defined below: 

```c
int16_m y_max;
```


### Retrieve glyph header

The function `mu_truetype_get_glyph_header` retrieves the glyph header of a given glyph, defined below: 

```c
MUDEF void mu_truetype_get_glyph_header(muByte* table, muttGlyphHeader* header);
```


`table` must be a pointer to a valid glyph table; a pointer to a specific glyph table based on a glyph ID can be accessed via the "loca" table.

Note that when calling `mu_truetype_get_glyf_table`, if `length` is set to 0, then this function should not be called on it, as it is a glyph with no description, existing as an empty glyph, and calling this function on it will return the description of some other glyph.

## Simple glyph data

mutt can be used to extract simple glyph data, represented by the struct `muttSimpleGlyph`.

### Struct

The struct `muttSimpleGlyph` represents the data of a simple glyph. It has the following members:

* `end_pts_of_contours`: a pointer to memory from `muttGlyphHeader->data`; equivalent to "endPtsOfContours" in the "Simple Glyph table", defined below: 

```c
muByte* end_pts_of_contours;
```


* `instruction_length`: equivalent to "instructionLength" in the "Simple Glyph table", defined below: 

```c
uint16_m instruction_length;
```


* `instructions`: a pointer to memory from `muttGlyphHeader->data`; equivalent to "instructions" in the "Simple Glyph table", defined below: 

```c
uint8_m* instructions;
```


* `flags`: 0 or a pointer to user-allocated memory; an array of flags for each point in the glyph, defined below: 

```c
uint8_m* flags;
```


* `x_coordinates`: 0 or a pointer to user-allocated memory; an array of X-coordinates for each point in the glyph, defined below: 

```c
int16_m* x_coordinates;
```


* `y_coordinates`: 0 or a pointer to user-allocated memory; an array of Y-coordinates for each point in the glyph, defined below: 

```c
int16_m* y_coordinates;
```


`end_pts_of_contours` is a pointer to where "endPtsOfContours" lies in `muttGlyphHeader->data`, and is filled in automatically by `mu_truetype_get_simple_glyph`. The data type of "endPtsOfContours" is "uint16", meaning that endianness conversions may need to be performed to retrieve the correct value, hence why the data type of `end_pts_of_contours` is `muByte*`; to retrieve an element from this list, use the function `mu_truetype_get_contour_end_pt`.

`instruction_length` is automatically filled in by `mu_truetype_get_simple_glyph`.

`instructions` is a pointer to where "instructions" lies in `muttGlyphHeader->data`, and is filled in automatically by `mu_truetype_get_simple_glyph`. Because the data type of "instructions" is "uint8", it can be read directly, hence why the data type of `instructions` is `uint8_m*`.

If `flags`, `x_coordinates`, or `y_coordinates` are not set to 0 upon being called to `mu_truetype_get_simple_glyph`, `flags` is expected to be a pointer to user-allocated memory whose size is at least `sizeof(uint8_m) * mu_truetype_get_contour_end_pt(muttSimpleGlyph, muttGlyphHeader->number_of_contours-1)`.

The interpretation for `flags` is different than how it is defined in the "Simple Glyph table" in TrueType's documentation: whereas "flags" in the "Simple Glyph table" can represent multiple flags with one flag element, `flags` in `muttGlyphHeader` has an element for every single point; `flags[n]` is the flag for point `n`.

If `flags`, `x_coordinates`, or `y_coordinates` are not set to 0 upon being called to `mu_truetype_get_simple_glyph`, `x_coordinates` is expected to be a pointer to user-allocated memory whose size is at least `sizeof(int32_m) * mu_truetype_get_contour_end_pt(muttSimpleGlyph, muttGlyphHeader->number_of_contours-1)`. The same logic applies to `y_coordinates`.

The interpretation for `x_coordinates` and `y_coordinates` are different than how it is defined in the "Simple Glyph table" in TrueType's documentation: whereas "xCoordinates" and "yCoordinates" in the "Simple Glyph table" can represent multiple coordinates with one element, `x_coordinates` and `y_coordinates` in `muttGlyphHeader` have an element for every single point; `x_coordinates[n]` and `y_coordinates[n]` are the coordinates for point `n`. This also means that the values stored in the coordinate arrays within `muttGlyphHeader` store the raw coordinates in FUnits, whereas in the "Simple Glyph table", they're stored as offsets from the previous point.

### Flag macros

The following macros are defined to make bit-masking the flag values of a glyph easier:

* [0x01] `MUTT_ON_CURVE_POINT`: equivalent to "ON_CURVE_POINT".

* [0x02] `MUTT_X_SHORT_VECTOR`: equivalent to "X_SHORT_VECTOR".

* [0x04] `MUTT_Y_SHORT_VECTOR`: equivalent to "Y_SHORT_VECTOR".

* [0x08] `MUTT_REPEAT_FLAG`: equivalent to "REPEAT_FLAG".

* [0x10] `MUTT_X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR`: equivalent to "X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR".

* [0x20] `MUTT_Y_IS_SAME_OR_POSITIVE_Y_SHORT_VECTOR`: equivalent to "Y_IS_SAME_OR_POSITIVE_Y_SHORT_VECTOR".

* [0x40] `MUTT_OVERLAP_SIMPLE`: equivalent to "OVERLAP_SIMPLE".

### Get simple glyph data

The function `mu_truetype_get_simple_glyph` is used to retrieve a simple glyph's data, defined below: 

```c
MUDEF uint16_m mu_truetype_get_simple_glyph(muttGlyphHeader* header, muttSimpleGlyph* glyph);
```


If `glyph` is 0, the amount of points within the glyph is returned. This is useful for allocating the memory necessary for the data first and then retrieving all of the data without additional unnecessary operations.

If `glyph` is not 0, `flags`, `x_coordinates`, and `y_coordinates` must all rather be 0 (to which they will be ignored) or all valid pointers to arrays of their respective type with a length of at least the nmuber of points in the glyph.

See the "Struct" section for `muttSimpleGlyph` for specific memory requirements and how to interpret the values that this function fills in.

### Get contour end point

The function `mu_truetype_get_contour_end_pt` retrieves a value from the "endPtsOfContours" array within a "Simple Glyph table", defined below: 

```c
MUDEF uint16_m mu_truetype_get_contour_end_pt(muttSimpleGlyph* glyph, uint16_m contour);
```


`contour` must be below the number of contours for the respective glyph.

## Composite glyph data

mutt can be used to extract composite glyph data.

### Composite record struct

The struct `muttCompositeRecord` represents a record in a composite glyph. It has the following members:

* `flags`: equivalent to "flags" in the "Component Glyph record", defined below: 

```c
uint16_m flags;
```


* `glyph_index`: equivalent to "glyphIndex" in the "Component Glyph record", defined below: 

```c
uint16_m glyph_index;
```


* `argument1`: the x-offset (see clarifications below), defined below: 

```c
muByte argument1[2];
```


* `argument2`: the y-offset (see clarifications below), defined below: 

```c
muByte argument2[2];
```


* `scales`: the possible scales of the record (see clarifications below), defined below: 

```c
float scales[4];
```


* `next`: address to the next byte after the last byte of the composite record; used for retrieving the next composite record, defined below: 

```c
muByte* next;
```


The contents of `argument1` and `argument2` are dependent on the value of `flags` (see TrueType documentation), and, in order to be read, the contents of `argument1` and `argument2` are to be casted to a pointer of the indicated type and then dereferenced.


For example, if bits ARG_1_AND_2_ARE_WORDS and ARGS_ARE_XY_VALUES are set, then the contents of `argument1` and `argument2` are read as:

```c
int16_m arg1 = *(int16_m*)record.argument1;
int16_m arg2 = *(int16_m*)record.argument2;
```

`scales` can also depend on the flags given the following circumstances:

* If WE_HAVE_A_SCALE is set, `scales[0]` is the scale for the component, and the contents of `scales[1]`, `scales[2]`, and `scales[3]` are undefined.

* If WE_HAVE_AN_X_AND_Y_SCALE is set, `scales[0]` is the x-scale for the component, `scales[1]` is the y-scale for the component, and the contents of `scales[2]` and `scales[3]` are undefined.

* If WE_HAVE_A_TWO_BY_TWO is set, all four values of the scales are used, with the following example C pseudocode determining the position transformations based on the values:

```c
x_new = scales[0]*x + scales[2]*y;
y_new = scales[1]*x + scales[3]*y;
```


#### Composite record flag macros

The following macros are defined for bit-masking `muttCompositeRecord->flags`:

* [0x0001] `MUTT_ARG_1_AND_2_ARE_WORDS`: equivalent to "ARG_1_AND_2_ARE_WORDS".

* [0x0002] `MUTT_ARGS_ARE_XY_VALUES`: equivalent to "ARGS_ARE_XY_VALUES".

* [0x0004] `MUTT_ROUND_XY_TO_GRID`: equivalent to "ROUND_XY_TO_GRID".

* [0x0008] `MUTT_WE_HAVE_A_SCALE`: equivalent to "WE_HAVE_A_SCALE".

* [0x0020] `MUTT_MORE_COMPONENTS`: equivalent to "MORE_COMPONENTS".

* [0x0040] `MUTT_WE_HAVE_AN_X_AND_Y_SCALE`: equivalent to "WE_HAVE_AN_X_AND_Y_SCALE".

* [0x0080] `MUTT_WE_HAVE_A_TWO_BY_TWO`: equivalent to "WE_HAVE_A_TWO_BY_TWO".

* [0x0100] `MUTT_WE_HAVE_INSTRUCTIONS`: equivalent to "WE_HAVE_INSTRUCTIONS".

* [0x0200] `MUTT_USE_MY_METRICS`: equivalent to "USE_MY_METRICS".

* [0x0400] `MUTT_OVERLAP_COMPOUND`: equivalent to "OVERLAP_COMPOUND".

* [0x0800] `MUTT_SCALED_COMPONENT_OFFSET`: equivalent to "SCALED_COMPONENT_OFFSET".

* [0x1000] `MUTT_UNSCALED_COMPONENT_OFFSET`: equivalent to "UNSCALED_COMPONENT_OFFSET".

#### Composite instructions struct

The struct `muttCompositeInstructions` represents the optional instructions that may come after the last composite record in a composite glyph. It has the following members:

* `num_instr`: the number of instructions, defined below: 

```c
uint16_m num_instr;
```


* `instr`: a pointer to data within the TrueType font for the instructions, defined below: 

```c
uint8_m* instr;
```


### Get composite record

The function `mu_truetype_get_composite_record` retrieves a composite record from a glyph, defined below: 

```c
MUDEF void mu_truetype_get_composite_record(muttGlyphHeader* header, muttCompositeRecord* record, muttCompositeRecord* prev_record);
```


`record` is the record struct to be filled in.

`prev_record` is the previous composite record; if this function is getting called on the first composite record, this should be set to 0 to indicate as such.

### Get composite instructions

The function `mu_truetype_get_composite_instructions` retrieves the instructions of a composite glyph, defined below: 

```c
MUDEF void mu_truetype_get_composite_instructions(muttCompositeRecord* last_record, muttCompositeInstructions* instructions);
```


`last_record` is the last composite record specified in the entire glyph. It must be the last record because the data for the instructions is only stored after the last composite record.

# Miscellaneous inner utility functions

The following functions are miscellaneous functions used for reading values from a TrueType file.

## F2DOT14 reading

The macro function "MUTT_F2DOT14" creates an expression for a float equivalent of a given array that stores 2 bytes representing a big-endian F2DOT14, defined below: 

```c
#define MUTT_F2DOT14(b) (float)((*(int8_m*)&b[0]) & 0xC0) + ((float)(mu_rbe_uint16(b) & 0xFFFF) / 16384.f)
```


## idDelta logic

The logic behind adding an idDelta value to a glyph id retrieved in certain cmap formats can be confusing; the function `mu_truetype_id_delta` calculates this, defined below: 

```c
MUDEF uint16_m mu_truetype_id_delta(uint16_m character_code, int16_m delta);
```


# Version macro

mutt defines three macros to define the version of mutt: `MUTT_VERSION_MAJOR`, `MUTT_VERSION_MINOR`, and `MUTT_VERSION_PATCH`, following the format of `vMAJOR.MINOR.PATCH`.

# C standard library dependencies

mutt has several C standard library dependencies not provided by its other library dependencies, all of which are overridable by defining them before the inclusion of its header. This is a list of all of those dependencies.

## `string.h` dependencies

`mu_memcpy`: equivalent to `memcpy`.

## `math.h` dependencies

`mu_pow`: equivalent to `pow`.
