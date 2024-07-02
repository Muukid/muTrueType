

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

* `MUTT_INVALID_TABLE_DIRECTORY_LENGTH`: the length of the given TrueType data is not enough for the table directory. Likely the length is incorrect or the data given is not TrueType data.

* `MUTT_INVALID_TABLE_DIRECTORY_SFNT_VERSION`: the value for "sfntVersion" in the table directory was invalid. Since this is the first value read when loading TrueType data, this most likely means that rather the data given is corrupt, not TrueType data, or is under another incompatible wrapper (such as fonts that use CFF data).

* `MUTT_INVALID_TABLE_DIRECTORY_SEARCH_RANGE`: the value for "searchRange" in the table directory was invalid.

* `MUTT_INVALID_TABLE_DIRECTORY_ENTRY_SELECTOR`: the value for "entrySelector" in the table directory was invalid.

* `MUTT_INVALID_TABLE_DIRECTORY_RANGE_SHIFT`: the value for "rangeShift" in the table directory was invalid.

* `MUTT_INVALID_TABLE_RECORD_OFFSET`: the value for "offset" in a table record was out of range.

* `MUTT_INVALID_TABLE_RECORD_LENGTH`: the value for "length" in a table record was out of range.

* `MUTT_INVALID_TABLE_RECORD_CHECKSUM`: the value for "checksum" in a table record was invalid.

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

### Group bit values

The following macros are defined for loading groups of tables:

* [0x000003FE] `MUTT_LOAD_REQUIRED_TABLES` - loads all tables required by TrueType standards (cmap, glyf, head, hhea, hmtx, loca, maxp, name, and post).

* [0xFFFFFFFF] `MUTT_LOAD_ALL` - loads everything; sets all flags.

# Reading information from a TrueType font

## The `muttFont` struct

A TrueType font is represented by the struct `muttFont`. Once successfully loaded, the data stored within a `muttFont` struct is entirely separate from the user-allocated TrueType data.

Inside the `muttFont` struct is all of the loaded information from when it was loaded. The actual full list of members is:

* `directory`: a pointer to a directory listing all of the tables provided by the given font, defined below: 

```c
muttDirectory* directory;
```


* `directory_res`: the result of loading the member `directory`, defined below: 

```c
muttResult directory_res;
```


* `mem`: the inner allocated memory used for holding necessary data, defined below: 

```c
muByte* mem;
```


* `memlen`: the length of the allocated memory, in bytes, defined below: 

```c
size_m memlen;
```


* `memcur`: offset to the latest unused memory in `mem`, in bytes, defined below: 

```c
size_m memcur;
```


Most of the members are in pairs of pointers and result values. If a requested pointer is 0, it could not be loaded, and its corresponding result value will indicate the result enumerator indicating what went wrong.

The contents of a pointer and result pair for information not included in the load flags are undefined.

## The `muttDirectory` struct

The struct `muttDirectory` is used to list all of the tables provided by a TrueType font. It is stored in the struct `muttFont`, and is similar to TrueType's table directory.

Its members are:

* `num_tables`: equivalent to "numTables" in the table directory, defined below: 

```c
uint16_m num_tables;
```


* `search_range`: equivalent to "searchRange" in the table directory, defined below: 

```c
uint16_m search_range;
```


* `entry_selector`: equivalent to "entrySelector" in the table directory, defined below: 

```c
uint16_m entry_selector;
```


* `range_shift`: equivalent to "rangeShift" in the table directory, defined below: 

```c
uint16_m range_shift;
```


* `table_records`: equivalent to "tableRecords" in the table directory, defined below: 

```c
muttTableRecord* table_records;
```


The struct `muttTableRecord` is similar to TrueType's table record, and has the following members:

* `table_tag`: equivalent to "tableTag" in the table record, defined below: 

```c
uint8_m table_tag[4];
```


* `checksum`: equivalent to "checksum" in the table record, defined below: 

```c
uint32_m checksum;
```


* `offset`: equivalent to "offset" in the table record, defined below: 

```c
uint32_m offset;
```


* `length`: equivalent to "length" in the table record, defined below: 

```c
uint32_m length;
```


## C standard library dependencies

mutt has several C standard library dependencies not provided by its other library dependencies, all of which are overridable by defining them before the inclusion of its header. This is a list of all of those dependencies.

## `stdlib.h` dependencies

* `mu_malloc`: equivalent to `malloc`.

* `mu_free`: equivalent to `free`.

* `mu_realloc`: equivalent to `realloc`.

## `string.h` dependencies

* `mu_memcpy`: equivalent to `memcpy`.

## `math.h` dependencies

* `mu_pow`: equivalent to `pow`.
