

# muTrueType v1.0.0

muTrueType (acronymized to "mutt") is a public domain single-file C library for retrieving data from the TrueType file format via its tables (the "[low-level API](#low-level-api)"), rendering glyphs to a bitmap (the "[rendering API](#rendering-api)"), and handling the layout/placement/spacing of multiple glyphs in a graphical context (the "[layout API](#layout-api)"). Its header is automatically defined upon inclusion if not already included (`MUTT_H`), and the source code is defined if `MUTT_IMPLEMENTATION` is defined, following the internal structure of:

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

* `muttPost* post` - a pointer to the [post table](#post-table).

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

The union `muttLoca` is used to represent the loca table provided by a TrueType font, stored in the struct `muttFont` as the pointer member "`loca`", and loaded with the flag `MUTT_LOAD_LOCA` (`MUTT_LOAD_MAXP` and `MUTT_LOAD_HEAD` must also be defined). It has the following members:

* `uint16_m* offsets16` - equivalent to the short-format offsets array in the loca table. This member is to be read from if `head->index_to_loc_format` is equal to `MUTT_OFFSET_16`.

* `uint32_m* offsets32` - equivalent to the long-format offsets array in the loca table. This member is to be read from if `head->index_to_loc_format` is equal to `MUTT_OFFSET_32`.

The offsets are verified to be within range of the glyf table, along with all of the other rules within the specification.

## Name table

The struct `muttName` is used to represent the name table provided by a TrueType font, stored in the struct `muttFont` as the pointer mem ber "`name`", and loaded with the flag `MUTT_LOAD_NAME`. It has the following members:

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

### Name result values

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

## `string.h` dependencies

* `mu_memcpy` - equivalent to `memcpy`.

* `mu_memset`- equivalent to `memset`.
