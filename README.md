

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

 `[0xFFFFFFFF] `MUTT_LOAD_ALL` - loads all tables that could be supported by mutt.

# Low-level API

The low-level API of mutt is designed to support reading information from the tables provided by TrueType. It is used internally by all other parts of the mutt API. All values provided by the low-level API have been checked to be valid, and are guaranteed to be valid once given to the user, unless explicitly stated otherwise.

## Font struct

The font struct, `muttFont`, is the primary way of reading information from TrueType tables, holding pointers to each table's defined data, and is automatically filled using the function [`mutt_load`](#loading-a-font). It has the following members:

* `muttLoadFlags load_flags` - flags indicating which tables successfully loaded.

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

# Result

The type `muttResult` (typedef for `uint32_m`) is defined to represent how a task went. Result values can be "fatal" (meaning that the task completely failed to execute, and the program will continue as if the task had never been attempted), "non-fatal" (meaning that the task partially failed, but was still able to complete the task), and "successful" (meaning that the task fully succeeded). The following values are defined for `muttResult` (all values not explicitly stated as being fatal, non-fatal, or successful are assumed to be fatal):

* `MUTT_SUCCESS` - the task was successfully completed; real value 0.

* `MUTT_INVALID_DIRECTORY_LENGTH` - the length of the table directory was invalid. This is the first check performed on the length of the font file data, so if this result is given, it is likely that the data given is not font file data.

* `MUTT_INVALID_DIRECTORY_SFNT_VERSION` - the value of "sfntVersion" in the table directory was invalid/unsupported.

* `MUTT_INVALID_DIRECTORY_NUM_TABLES` - the value of "numTables" in the table directory was invalid; the number of tables must be at least 9 to store all tables required in TrueType.

* `MUTT_INVALID_DIRECTORY_RECORD_OFFSET` - the value of "offset" in a table record within the table directory was out of range.

* `MUTT_INVALID_DIRECTORY_RECORD_LENGTH` - the value of "length" in a table record within the table directory was out of range.

* `MUTT_INVALID_DIRECTORY_RECORD_CHECKSUM` - the value of "checksum" in a table record within the table directory was invalid, implying that the table data was incorrect.

* `MUTT_INVALID_DIRECTORY_RECORD_TABLE_TAG` - the table tag of a table within the table directory was a duplicate of a previous one.

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
