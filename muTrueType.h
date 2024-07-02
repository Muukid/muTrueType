/*
muTrueType.h
Public domain single-file C library for reading and rendering TrueType data.
No warranty implied; use at your own risk.

Licensed under MIT License or public domain, whichever you prefer.
More explicit license information at the end of file.

@TODO Define more macros.
@TODO Support for more formats.
@TODO Format conversion functions (?).
@TODO Optimize searches.
@TODO Add information (quite needed) for: post, hmtx (horizontal metrics), OS/2 (metrics required in OpenType), BASE (for when multiple scripts are being used next to each other), GDEF (used for further classification and information about glyphs, like highlighting boxes, ligatures, variations, etc.; GSUB, GPOS, and JSTF may depend on data from this table), GPOS (used for proper position in languages like Urdu, which can make dealing with these things not a nightmare), GSUB (substitution data for positional glyphs, such as in Arabic), avar (used for customizing axis variations for variable fonts), fvar (used to define different font variations under different weights), gvar (used to define specific glyph variations under different weights), HVAR (for glyph-specific variations for hmtx stuff), STAT (for the definition of particular font variations), various other tables not mentioned, and instruction-related tables when that is figured out.
@TODO Add information (not quite needed but still helpful) for: gasp (suggested grid-fitting techniques and such), JSTF (helpful when fitting text to a certain boundary and making it look good), MVAR (glyph-specific metric data, like a lot of the stuff provided in OS/2), VVAR (vertical metric variations for font variations), and MATH (used for the layout of maths formulas).
@TODO Figure out support for instructions.
*/

/* @DOCBEGIN

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

@DOCEND */

#ifndef MUTT_H
	#define MUTT_H

	// @DOCLINE # Other library dependencies
	// @DOCLINE mutt has a dependency on:

	// @DOCLINE * [muUtility v1.1.0](https://github.com/Muukid/muUtility/releases/tag/v1.1.0).
	// @IGNORE
	/* muUtility v1.1.0 header */
	
		#if !defined(MU_CHECK_VERSION_MISMATCHING) && defined(MUU_H) && \
			(MUU_VERSION_MAJOR != 1 || MUU_VERSION_MINOR != 1 || MUU_VERSION_PATCH != 0)

			#pragma message("[MUTT] muUtility's header has already been defined, but version doesn't match the version that this library is built for. This may lead to errors, warnings, or unexpected behavior. Define MU_CHECK_VERSION_MISMATCHING before this to turn off this message.")

		#endif

		#ifndef MUU_H
			#define MUU_H
			
			// @DOCLINE # Secure warnings
			// @DOCLINE mu libraries often use non-secure functions that will trigger warnings on certain compilers. These warnings are, to put it lightly, dumb, so the header section of muUtility defines `_CRT_SECURE_NO_WARNINGS`. However, it is not guaranteed that this definition will actually turn the warnings off, which at that point, they have to be manually turned off by the user.
			#if !defined(MU_SECURE_WARNINGS) && !defined(_CRT_SECURE_NO_WARNINGS)
				#define _CRT_SECURE_NO_WARNINGS
			#endif

			#ifdef __cplusplus
			extern "C" { // }
			#endif

			// @DOCLINE # C standard library dependencies

				// @DOCLINE muUtility has several C standard library dependencies, all of which are overridable by defining them before the inclusion of its header. The following is a list of those dependencies:

				// @DOCLINE ## `stdint.h` dependencies
				#if !defined(int8_m)      || \
					!defined(uint8_m)     || \
					!defined(int16_m)     || \
					!defined(uint16_m)    || \
					!defined(int32_m)     || \
					!defined(uint32_m)    || \
					!defined(int64_m)     || \
					!defined(uint64_m)    || \
					!defined(MU_SIZE_MAX)

					#define __STDC_LIMIT_MACROS
					#define __STDC_CONSTANT_MACROS
					#include <stdint.h>

					// @DOCLINE `int8_m`: equivalent to `int8_t` if `INT8_MAX` is defined, `char` if otherwise
					#ifndef int8_m
						#ifdef INT8_MAX
							#define int8_m int8_t
						#else
							#define int8_m char
						#endif
					#endif

					// @DOCLINE `uint8_m`: equivalent to `uint8_t` if `UINT8_MAX` is defined, `unsigned char` if otherwise
					#ifndef uint8_m
						#ifdef UINT8_MAX
							#define uint8_m uint8_t
						#else
							#define uint8_m unsigned char
						#endif
					#endif

					// @DOCLINE `int16_m`: equivalent to `int16_t` if `INT16_MAX` is defined, `short` if otherwise
					#ifndef int16_m
						#ifdef INT16_MAX
							#define int16_m int16_t
						#else
							#define int16_m short
						#endif
					#endif

					// @DOCLINE `uint16_m`: equivalent to `uint16_t` if `UINT16_MAX` is defined, `unsigned short` if otherwise
					#ifndef uint16_m
						#ifdef UINT16_MAX
							#define uint16_m uint16_t
						#else
							#define uint16_m unsigned short
						#endif
					#endif

					// @DOCLINE `int32_m`: equivalent to `int32_t` if `INT32_MAX` is defined, `long` if otherwise
					#ifndef int32_m
						#ifdef INT32_MAX
							#define int32_m int32_t
						#else
							#define int32_m long
						#endif
					#endif

					// @DOCLINE `uint32_m`: equivalent to `uint32_t` if `UINT32_MAX` is defined, `unsigned long` if otherwise
					#ifndef uint32_m
						#ifdef UINT32_MAX
							#define uint32_m uint32_t
						#else
							#define uint32_m unsigned long
						#endif
					#endif

					// @DOCLINE `int64_m`: equivalent to `int64_t` if `INT64_MAX` is defined, `long long` if otherwise
					#ifndef int64_m
						#ifdef INT64_MAX
							#define int64_m int64_t
						#else
							#define int64_m long long
						#endif
					#endif

					// @DOCLINE `uint64_m`: equivalent to `uint64_t` if `UINT64_MAX` is defined, `unsigned long long` if otherwise
					#ifndef uint64_m
						#ifdef UINT64_MAX
							#define uint64_m uint64_t
						#else
							#define uint64_m unsigned long long
						#endif
					#endif

					// @DOCLINE `MU_SIZE_MAX`: equivalent to `SIZE_MAX`
					#ifndef MU_SIZE_MAX
						#define MU_SIZE_MAX SIZE_MAX
					#endif

				#endif

				// @DOCLINE ## `stddef.h` dependencies
				#if !defined(size_m)

					#include <stddef.h>

					// @DOCLINE `size_m`: equivalent to `size_t`
					#ifndef size_m
						#define size_m size_t
					#endif

				#endif

				// @DOCLINE ## `stdbool.h` dependencies
				#if !defined(muBool)   || \
					!defined(MU_TRUE)  || \
					!defined(MU_FALSE)

					#include <stdbool.h>

					// @DOCLINE `muBool`: equivalent to `bool`
					#ifndef muBool
						#define muBool bool
					#endif

					// @DOCLINE `MU_TRUE`: equivalent to `true`
					#ifndef MU_TRUE
						#define MU_TRUE true
					#endif

					// @DOCLINE `MU_FALSE`: equivalent to `false`
					#ifndef MU_FALSE
						#define MU_FALSE false
					#endif

				#endif

			// @DOCLINE # Macros

				// @DOCLINE ## Version

					// @DOCLINE The macros `MUU_VERSION_MAJOR`, `MUU_VERSION_MINOR`, and `MUU_VERSION_PATCH` are defined to match its respective release version, following the formatting of `MAJOR.MINOR.PATCH`.

					#define MUU_VERSION_MAJOR 1
					#define MUU_VERSION_MINOR 1
					#define MUU_VERSION_PATCH 0

				// @DOCLINE ## `MUDEF`

					// @DOCLINE The `MUDEF` macro is a macro used by virtually all mu libraries, which is generally added before a header-defined variable or function. Its default value is `extern`, but can be changed to `static` by defining `MU_STATIC` before the header section of muUtility is defined. Its value can also be overwritten entirely to anything else by directly defining `MUDEF`.

					#ifndef MUDEF
						#ifdef MU_STATIC
							#define MUDEF static
						#else
							#define MUDEF extern
						#endif
					#endif

				// @DOCLINE ## Zero struct

					// @DOCLINE There are two macros, `MU_ZERO_STRUCT` and `MU_ZERO_STRUCT_CONST`, which are macro functions used to zero-out a struct's contents, with their only parameter being the struct type. The reason this needs to be defined is because the way C and C++ syntax handles an empty struct are different, and need to be adjusted for.

					#ifndef MU_ZERO_STRUCT
						#ifdef __cplusplus
							#define MU_ZERO_STRUCT(s) {}
						#else
							#define MU_ZERO_STRUCT(s) (s){0}
						#endif
					#endif

					#ifndef MU_ZERO_STRUCT_CONST
						#ifdef __cplusplus
							#define MU_ZERO_STRUCT_CONST(s) {}
						#else
							#define MU_ZERO_STRUCT_CONST(s) {0}
						#endif
					#endif

				// @DOCLINE ## Byte

					// @DOCLINE Several macros are defined to interact with the idea of a byte.

					// @DOCLINE The macro `muByte` exists as a macro for `uint8_m`, and represents a byte as a type.

					#ifndef muByte
						#define muByte uint8_m
					#endif

					// @DOCLINE There are also several macro functions used to read and write to bytes. These macros are:

					// @DOCLINE `mu_rle_uint8(b)`: Creates an expression of type `uint8_m` that reads the value stored in the 1-byte-length little-endian byte array `b`.
					#ifndef mu_rle_uint8
						#define mu_rle_uint8(b) ((uint8_m)b[0] << 0)
					#endif
					// @DOCLINE `mu_rbe_uint8(b)`: Creates an expression of type `uint8_m` that reads the value stored in the 1-byte-length big-endian byte array `b`.
					#ifndef mu_rbe_uint8
						#define mu_rbe_uint8(b) ((uint8_m)b[0] << 0)
					#endif

					// @DOCLINE `mu_rle_uint16(b)`: Creates an expression of type `uint16_m` that reads the value stored in the 2-byte-length little-endian byte array `b`.
					#ifndef mu_rle_uint16
						#define mu_rle_uint16(b) ((uint16_m)b[0] << 0 | (uint16_m)b[1] << 8)
					#endif
					// @DOCLINE `mu_rbe_uint16(b)`: Creates an expression of type `uint16_m` that reads the value stored in the 2-byte-length big-endian byte array `b`.
					#ifndef mu_rbe_uint16
						#define mu_rbe_uint16(b) ((uint16_m)b[1] << 0 | (uint16_m)b[0] << 8)
					#endif

					// @DOCLINE `mu_rle_uint32(b)`: Creates an expression of type `uint32_m` that reads the value stored in the 4-byte-length little-endian byte array `b`.
					#ifndef mu_rle_uint32
						#define mu_rle_uint32(b) ((uint32_m)b[0] << 0 | (uint32_m)b[1] << 8 | (uint32_m)b[2] << 16 | (uint32_m)b[3] << 24)
					#endif
					// @DOCLINE `mu_rbe_uint32(b)`: Creates an expression of type `uint32_m` that reads the value stored in the 4-byte-length big-endian byte array `b`.
					#ifndef mu_rbe_uint32
						#define mu_rbe_uint32(b) ((uint32_m)b[3] << 0 | (uint32_m)b[2] << 8 | (uint32_m)b[1] << 16 | (uint32_m)b[0] << 24)
					#endif

					// @DOCLINE `mu_rle_uint64(b)`: Creates an expression of type `uint64_m` that reads the value stored in the 8-byte-length little-endian byte array `b`.
					#ifndef mu_rle_uint64
						#define mu_rle_uint64(b) ((uint64_m)b[0] << 0 | (uint64_m)b[1] << 8 | (uint64_m)b[2] << 16 | (uint64_m)b[3] << 24 | (uint64_m)b[4] << 32 | (uint64_m)b[5] << 40 | (uint64_m)b[6] << 48 | (uint64_m)b[7] << 56)
					#endif
					// @DOCLINE `mu_rbe_uint64(b)`: Creates an expression of type `uint64_m` that reads the value stored in the 8-byte-length big-endian byte array `b`.
					#ifndef mu_rbe_uint64
						#define mu_rbe_uint64(b) ((uint64_m)b[7] << 0 | (uint64_m)b[6] << 8 | (uint64_m)b[5] << 16 | (uint64_m)b[4] << 24 | (uint64_m)b[3] << 32 | (uint64_m)b[2] << 40 | (uint64_m)b[1] << 48 | (uint64_m)b[0] << 56)
					#endif

					// @DOCLINE `mu_wle_uint8(b, i)`: Reads the given contents of the 1-byte-length little-endian byte array `b` into the given `uint8_m` variable `i`.
					#ifndef mu_wle_uint8
						#define mu_wle_uint8(b, i) b[0] = (uint8_m)(i >> 0);
					#endif
					// @DOCLINE `mu_wbe_uint8(b, i)`: Reads the given contents of the 1-byte-length big-endian byte array `b` into the given `uint8_m` variable `i`.
					#ifndef mu_wbe_uint8
						#define mu_wbe_uint8(b, i) b[0] = (uint8_m)(i >> 0);
					#endif

					// @DOCLINE `mu_wle_uint16(b, i)`: Reads the given contents of the 2-byte-length little-endian byte array `b` into the given `uint16_m` variable `i`.
					#ifndef mu_wle_uint16
						#define mu_wle_uint16(b, i) b[0] = (uint8_m)(i >> 0); b[1] = (uint8_m)(i >> 8);
					#endif
					// @DOCLINE `mu_wbe_uint16(b, i)`: Reads the given contents of the 2-byte-length big-endian byte array `b` into the given `uint16_m` variable `i`.
					#ifndef mu_wbe_uint16
						#define mu_wbe_uint16(b, i) b[1] = (uint8_m)(i >> 0); b[0] = (uint8_m)(i >> 8);
					#endif

					// @DOCLINE `mu_wle_uint32(b, i)`: Reads the given contents of the 4-byte-length little-endian byte array `b` into the given `uint32_m` variable `i`.
					#ifndef mu_wle_uint32
						#define mu_wle_uint32(b, i) b[0] = (uint8_m)(i >> 0); b[1] = (uint8_m)(i >> 8); b[2] = (uint8_m)(i >> 16); b[3] = (uint8_m)(i >> 24);
					#endif
					// @DOCLINE `mu_wbe_uint32(b, i)`: Reads the given contents of the 4-byte-length big-endian byte array `b` into the given `uint32_m` variable `i`.
					#ifndef mu_wbe_uint32
						#define mu_wbe_uint32(b, i) b[3] = (uint8_m)(i >> 0); b[2] = (uint8_m)(i >> 8); b[1] = (uint8_m)(i >> 16); b[0] = (uint8_m)(i >> 24);
					#endif

					// @DOCLINE `mu_wle_uint64(b, i)`: Reads the given contents of the 8-byte-length little-endian byte array `b` into the given `uint64_m` variable `i`.
					#ifndef mu_wle_uint64
						#define mu_wle_uint64(b, i) b[0] = (uint8_m)(i >> 0); b[1] = (uint8_m)(i >> 8); b[2] = (uint8_m)(i >> 16); b[3] = (uint8_m)(i >> 24); b[4] = (uint8_m)(i >> 32); b[5] = (uint8_m)(i >> 40); b[6] = (uint8_m)(i >> 48); b[7] = (uint8_m)(i >> 56);
					#endif
					// @DOCLINE `mu_wbe_uint64(b, i)`: Reads the given contents of the 8-byte-length big-endian byte array `b` into the given `uint64_m` variable `i`.
					#ifndef mu_wbe_uint64
						#define mu_wbe_uint64(b, i) b[7] = (uint8_m)(i >> 0); b[6] = (uint8_m)(i >> 8); b[5] = (uint8_m)(i >> 16); b[4] = (uint8_m)(i >> 24); b[3] = (uint8_m)(i >> 32); b[2] = (uint8_m)(i >> 40); b[1] = (uint8_m)(i >> 48); b[0] = (uint8_m)(i >> 56);
					#endif

				// @DOCLINE ## Null values

					// @DOCLINE The `MU_NULL_PTR` macro is a macro value used to represent a null pointer. It is simply 0.
					#ifndef MU_NULL_PTR
						#define MU_NULL_PTR 0
					#endif

					// @DOCLINE The `MU_NULL` macro is a macro value used to represent a null value. It is simply 0.
					#ifndef MU_NULL
						#define MU_NULL 0
					#endif

				// @DOCLINE ## Set result

					/* @DOCBEGIN The `MU_SET_RESULT(res, val)` macro is a macro function that checks if the given parameter `res` is a null pointer. If it is, it does nothing, but if it isn't, it sets its value to the given parameter `val`. This macro saves a lot of code, shrinking down what would be this:

					```c
					if (result != MU_NULL_PTR) {
					    *result = ...;
					}
					```

					into this:
					
					```c
					MU_SET_RESULT(result, ...)
					```

					@DOCEND */

					#ifndef MU_SET_RESULT
						#define MU_SET_RESULT(res, val) if(res!=MU_NULL_PTR){*res=val;}
					#endif

				// @DOCLINE ## Enum

					/* @DOCBEGIN

					The `MU_ENUM(name, ...)` macro is a macro function used to declare an enumerator. The reason why one would prefer this over the traditional way of declaring enumerators is because it actually makes it a `size_m` which can avoid errors on certain compilers (looking at you, Microsoft) in regards to treating enumerators like values. It expands like this:

					```c
					enum _##name{
					    __VA_ARGS__
					};
					typedef enum _##name _##name;
					typedef size_m name;
					```

					@DOCEND */

					#define MU_ENUM(name, ...) enum _##name{__VA_ARGS__};typedef enum _##name _##name; typedef size_m name;

				// @DOCLINE ## Operating system recognition

					/* @DOCBEGIN The macros `MU_WIN32` or `MU_UNIX` are defined (if neither were defined before) in order to allow mu libraries to easily check if they're running on a Windows or Unix system.

					`MU_WIN32` will be defined if `WIN32` or `_WIN32` are defined, one of which is usually pre-defined on Windows systems.

					`MU_UNIX` will be defined if `__unix__` is defined or both `__APPLE__` and `__MACH__` are defined.

					@DOCEND */

					#if !defined(MU_WIN32) && !defined(MU_UNIX)
						#if defined(WIN32) || defined(_WIN32)
							#define MU_WIN32
						#elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
							#define MU_UNIX
						#endif
					#endif

			#ifdef __cplusplus
			}
			#endif

		#endif /* MUU_H */
	// @ATTENTION

	// @DOCLINE Note that mu libraries store their dependencies within their files, so you don't need to import these dependencies yourself.
	// @DOCLINE Note that the libraries listed may also have other dependencies that they also include that aren't listed here.

	#ifdef __cplusplus
	extern "C" { // }
	#endif

	// @DOCLINE # Result enumerator

		// @DOCLINE mutt uses the `muttResult` enumerator to represent how a function went. It has the following possible values:

		MU_ENUM(muttResult,
			// @DOCLINE * `@NLFT`: the task succeeded.
			MUTT_SUCCESS,
			// @DOCLINE * `@NLFT`: the file unexpectedly ended; this means that rather the file is corrupt, or an out-of-range index or length was given by it.
			MUTT_UNEXPECTED_EOF,
			// @DOCLINE * `@NLFT`: the recorded length of the head table was invalid.
			MUTT_INVALID_HEAD_TABLE_LENGTH,
			// @DOCLINE * `@NLFT`: the recorded length of the maxp table was invalid.
			MUTT_INVALID_MAXP_TABLE_LENGTH,
			// @DOCLINE * `@NLFT`: the recorded length of the hhea table was invalid.
			MUTT_INVALID_HHEA_TABLE_LENGTH,
			// @DOCLINE * `@NLFT`: the recorded length of the name table was invalid.
			MUTT_INVALID_NAME_TABLE_LENGTH,
			// @DOCLINE * `@NLFT`: the recorded length of the cmap table was invalid.
			MUTT_INVALID_CMAP_TABLE_LENGTH,
			// @DOCLINE * `@NLFT`: the recorded length of a format 0-cmap subtable was invalid.
			MUTT_INVALID_CMAP_F0_TABLE_LENGTH,
			// @DOCLINE * `@NLFT`: the recorded length of a format 4-cmap subtable was invalid.
			MUTT_INVALID_CMAP_F4_TABLE_LENGTH,
			// @DOCLINE * `@NLFT`: the recorded length of a format 12-cmap subtable was invalid.
			MUTT_INVALID_CMAP_F12_TABLE_LENGTH,
			// @DOCLINE * `@NLFT`: the "sfntVersion" value specified in the table directory was invalid. Because this is the first value read, if this error occurs, it is likely that the data given is not TrueType data (this error gets triggered if the file is OpenType as well).
			MUTT_INVALID_TABLE_DIRECTORY_SFNT_VERSION,
			// @DOCLINE * `@NLFT`: the "numTables" value specified in the table directory was invalid.
			MUTT_INVALID_TABLE_DIRECTORY_NUM_TABLES,
			// @DOCLINE * `@NLFT`: the "searchRange" value specified in the table directory was invalid.
			MUTT_INVALID_TABLE_DIRECTORY_SEARCH_RANGE,
			// @DOCLINE * `@NLFT`: the "entrySelector" value specified in the table directory was invalid.
			MUTT_INVALID_TABLE_DIRECTORY_ENTRY_SELECTOR,
			// @DOCLINE * `@NLFT`: the "rangeShift" value specified in the table directory was invalid.
			MUTT_INVALID_TABLE_DIRECTORY_RANGE_SHIFT,
			// @DOCLINE * `@NLFT`: the "offset" value specified in a table record was out of range.
			MUTT_INVALID_TABLE_RECORD_OFFSET,
			// @DOCLINE * `@NLFT`: the "length" value specified in a table record was out of range.
			MUTT_INVALID_TABLE_RECORD_LENGTH,
			// @DOCLINE * `@NLFT`: the "checksum" value specified in a table record was invalid.
			MUTT_INVALID_TABLE_RECORD_CHECKSUM,
			// @DOCLINE * `@NLFT`: the "magicNumber" value specified in the head table was invalid.
			MUTT_INVALID_HEAD_MAGIC_NUMBER,
			// @DOCLINE * `@NLFT`: the "unitsPerEm" value specified in the head table was invalid.
			MUTT_INVALID_HEAD_UNITS_PER_EM,
			// @DOCLINE * `@NLFT`: the "xMin" and "xMax" values specified in the head table were invalid.
			MUTT_INVALID_HEAD_X_MIN_MAX,
			// @DOCLINE * `@NLFT`: the "yMin" and "yMax" values specified in the head table were invalid.
			MUTT_INVALID_HEAD_Y_MIN_MAX,
			// @DOCLINE * `@NLFT`: the "lowestRecPPEM" value specified in the head table was invalid.
			MUTT_INVALID_HEAD_LOWEST_REC_PPEM,
			// @DOCLINE * `@NLFT`: the "indexToLocFormat" value specified in the head table was invalid.
			MUTT_INVALID_HEAD_INDEX_TO_LOC_FORMAT,
			// @DOCLINE * `@NLFT`: the "glyphDataFormat" value specified in the head table was invalid.
			MUTT_INVALID_HEAD_GLYPH_DATA_FORMAT,
			// @DOCLINE * `@NLFT`: the "maxZones" value specified in the maxp table was invalid.
			MUTT_INVALID_MAXP_MAX_ZONES,
			// @DOCLINE * `@NLFT`: the "metricDataFormat" value specified in the hhea table was invalid.
			MUTT_INVALID_HHEA_METRIC_DATA_FORMAT,
			// @DOCLINE * `@NLFT`: a value in the "glyphIdArray" within a format 0-cmap subtable was invalid.
			MUTT_INVALID_CMAP_F0_GLYPH_ID,
			// @DOCLINE * `@NLFT`: the "segCountX2" value specified in a format 4-cmap subtable was invalid.
			MUTT_INVALID_CMAP_F4_SEG_COUNT,
			// @DOCLINE * `@NLFT`: the "searchRange" value specified in a format 4-cmap subtable was invalid.
			MUTT_INVALID_CMAP_F4_SEARCH_RANGE,
			// @DOCLINE * `@NLFT`: the "entrySelector" value specified in a format 4-cmap subtable was invalid.
			MUTT_INVALID_CMAP_F4_ENTRY_SELECTOR,
			// @DOCLINE * `@NLFT`: the "rangeShift" value specified in a format 4-cmap subtable was invalid.
			MUTT_INVALID_CMAP_F4_RANGE_SHIFT,
			// @DOCLINE * `@NLFT`: a value in the "endCode" array specified in a format 4-cmap subtable was invalid.
			MUTT_INVALID_CMAP_F4_END_CODE,
			// @DOCLINE * `@NLFT`: a value in the "startCode" array specified in a format 4-cmap subtable was invalid.
			MUTT_INVALID_CMAP_F4_START_CODE,
			// @DOCLINE * `@NLFT`: a value in the "idRangeOffset" array specified in a format 4-cmap subtable was invalid, AKA gave a value outside of the array "glyphIdArray".
			MUTT_INVALID_CMAP_F4_ID_RANGE_OFFSET,
			// @DOCLINE * `@NLFT`: a value in the "idDelta" array specified in a format 4-cmap subtable was invalid, AKA resulted in a non-existent glyph id.
			MUTT_INVALID_CMAP_F4_ID_DELTA,
			// @DOCLINE * `@NLFT`: a required table could not be located.
			MUTT_MISSING_REQUIRED_TABLE,
		)

		// @DOCLINE Most of these errors getting triggered imply that rather the data is corrupt (especially in regards to checksum errors), uses some extension or format not supported by this library (such as OpenType), has accidental incorrect values, or is purposely malformed to attempt to get out of the memory region of the file data.

		// @DOCLINE ## Result name function

			#ifdef MUTT_NAMES

			// @DOCLINE The function `mutt_result_get_name` converts a `muttResult` value into a `const char*` representation, defined below: @NLNT
			MUDEF const char* mutt_result_get_name(muttResult result);

			// @DOCLINE Note that this function is only defined if `MUTT_NAMES` is defined before the inclusion of the header file.

			// @DOCLINE This function returns `"MUTT_UNKNOWN"` if a respective name could not be found.

			#endif

	// @DOCLINE # Check

		// @DOCLINE mutt has a section of its API dedicated to checking if given TrueType data is not only valid but safe. This API is, on default, used by the loading function '`mu_truetype_get_info`', meaning that all of these functions are called by `mu_truetype_get_info`.

		// @DOCLINE Note that checks are not performed on tables that mutt does not have support for in its API.

		typedef struct muttInfo muttInfo;

		// @DOCLINE ## Table directory check

			// @DOCLINE The function `mu_truetype_check_table_directory` checks if the table directory of given TrueType data is valid, defined below: @NLNT
			MUDEF muttResult mu_truetype_check_table_directory(muByte* data, size_m data_size);

		// @DOCLINE ## Checksum table check

			// @DOCLINE The function `mu_truetype_check_table_checksum` checks if a given table's checksum value is valid, defined below: @NLNT
			MUDEF muttResult mu_truetype_check_table_checksum(muByte* table, uint32_m length, uint32_m checksum);

		// @DOCLINE ## Head table check

			// @DOCLINE The function `mu_truetype_check_head` checks if the head table provided by a TrueType font is valid, defined below: @NLNT
			MUDEF muttResult mu_truetype_check_head(muByte* table, uint32_m table_length);

		// @DOCLINE ## Maxp table check

			// @DOCLINE The function `mu_truetype_check_maxp` checks if the maxp table provided by a TrueType font is valid, defined below: @NLNT
			MUDEF muttResult mu_truetype_check_maxp(muByte* table, uint32_m table_length);

		// @DOCLINE ## Hhea table check

			// @DOCLINE The function `mu_truetype_check_hhea` checks if the hhea table provided by a TrueType font is valid, defined below: @NLNT
			MUDEF muttResult mu_truetype_check_hhea(muByte* table, uint32_m table_length);

		// @DOCLINE ## Name table check

			// @DOCLINE The function `mu_truetype_check_names` checks if the name table provided by a TrueType font is valid, defined below: @NLNT
			MUDEF muttResult mu_truetype_check_names(muByte* table, uint32_m table_length);

		// @DOCLINE ## Cmap table check

			// @DOCLINE The function `mu_truetype_check_cmap` checks if the cmap table provided by a TrueType font is valid, defined below: @NLNT
			MUDEF muttResult mu_truetype_check_cmap(muttInfo* info);

			// @DOCLINE This function only checks table formats 0. All other formats go ignored.

		// @DOCLINE ## Cmap format table checks

			// @DOCLINE There are several functions defined to check certain format subtables within a cmap table. They are defined below.

			// @DOCLINE Note that each of these functions take in a parameter "`muByte* table`", which is expected to be a pointer to the first value in the subtable that isn't 'format', 'length', or a reserved value, but `length` is expected to include these values.

			// @DOCLINE ### Cmap format 0 subtable check

				// @DOCLINE The function `mu_truetype_check_format0` checks if the format 0-cmap subtable provided by a TrueType font is valid, defined below: @NLNT
				MUDEF muttResult mu_truetype_check_format0(muttInfo* info, muByte* table, uint16_m length);

			// @DOCLINE ### Cmap format 4 subtable check

				// @DOCLINE The function `mu_truetype_check_format4` checks if the format 4-cmap subtable provided by a TrueType font is valid, defined below: @NLNT
				MUDEF muttResult mu_truetype_check_format4(muttInfo* info, muByte* table, uint16_m length);

	// @DOCLINE # Direct table information

		// @DOCLINE mutt gives an API for querying information about tables stored within a TrueType font.

		// @DOCLINE ## Table

			// @DOCLINE The struct `muttTable` is used to define a table in a TrueType font. It has the following members:

			struct muttTable {
				// @DOCLINE * `offset`: the offset to the table, in bytes, starting from the beginning of the TrueType font data, defined below: @NLNT
				uint32_m offset;
				// @DOCLINE * `length`: the length of the table, in bytes, defined below: @NLNT
				uint32_m length;
			}; typedef struct muttTable muttTable;

			// @DOCLINE Note that `offset` is also used to indicate whether or not a given table exists within a TrueType font; if `muttTable.offset == 0`, then the table does not exist.

		// @DOCLINE ## Required tables

			// @DOCLINE The struct `muttRequiredTables` is used to define the required tables within a TrueType font. It has the following members:

			struct muttRequiredTables {
				// @DOCLINE * `cmap`: the "cmap" table, defined below: @NLNT
				muttTable cmap;
				// @DOCLINE * `glyf`: the "glyf" table, defined below: @NLNT
				muttTable glyf;
				// @DOCLINE * `head`: the "head" table, defined below: @NLNT
				muttTable head;
				// @DOCLINE * `hhea`: the "hhea" table, defined below: @NLNT
				muttTable hhea;
				// @DOCLINE * `hmtx`: the "hmtx" table, defined below: @NLNT
				muttTable hmtx;
				// @DOCLINE * `loca`: the "loca" table, defined below: @NLNT
				muttTable loca;
				// @DOCLINE * `maxp`: the "maxp" table, defined below: @NLNT
				muttTable maxp;
				// @DOCLINE * `name`: the "name" table, defined below: @NLNT
				muttTable name;
				// @DOCLINE * `post`: the "post" table, defined below: @NLNT
				muttTable post;
			}; typedef struct muttRequiredTables muttRequiredTables;

		// @DOCLINE ## Head table information

			// @DOCLINE The struct `muttHeadInfo` is used to define the information provided by the necessary "head" table within a TrueType font. It has the following members:

			struct muttHeadInfo {
				// @DOCLINE * `major_version`: equivalent to "majorVersion" in the "head" table, defined below: @NLNT
				uint16_m major_version;
				// @DOCLINE * `minor_version`: equivalent to "minorVersion" in the "head" table, defined below: @NLNT
				uint16_m minor_version;
				// @DOCLINE * `font_revision_high`: equivalent to the high-bytes of "fontRevision" in the "head" table, defined below: @NLNT
				int16_m font_revision_high;
				// @DOCLINE * `font_revision_low`: equivalent to the low-bytes of "fontRevision" in the "head" table, defined below: @NLNT
				int16_m font_revision_low;
				// @DOCLINE * `checksum_adjustment`: equivalent to "checksumAdjustment" in the "head" table, defined below: @NLNT
				uint32_m checksum_adjustment;
				// @DOCLINE * `magic_number`: equivalent to "magicNumber" in the "head" table, defined below: @NLNT
				uint32_m magic_number;
				// @DOCLINE * `flags`: equivalent to "flags" in the "head" table, defined below: @NLNT
				uint16_m flags;
				// @DOCLINE * `units_per_em`: equivalent to "unitsPerEm" in the "head" table, defined below: @NLNT
				uint16_m units_per_em;
				// @DOCLINE * `created`: equivalent to "created" in the "head" table, defined below: @NLNT
				int64_m created;
				// @DOCLINE * `modified`: equivalent to "modified" in the "head" table, defined below: @NLNT
				int64_m modified;
				// @DOCLINE * `x_min`: equivalent to "xMin" in the "head" table, defined below: @NLNT
				int16_m x_min;
				// @DOCLINE * `y_min`: equivalent to "yMin" in the "head" table, defined below: @NLNT
				int16_m y_min;
				// @DOCLINE * `x_max`: equivalent to "xMax" in the "head" table, defined below: @NLNT
				int16_m x_max;
				// @DOCLINE * `y_max`: equivalent to "yMax" in the "head" table, defined below: @NLNT
				int16_m y_max;
				// @DOCLINE * `mac_style`: equivalent to "macStyle" in the "head" table, defined below: @NLNT
				uint16_m mac_style;
				// @DOCLINE * `lowest_rec_ppem`: equivalent to "lowestRecPPEM" in the "head" table, defined below: @NLNT
				uint16_m lowest_rec_ppem;
				// @DOCLINE * `font_direction_hint`: equivalent to "fontDirectionHint" in the "head" table, defined below: @NLNT
				int16_m font_direction_hint;
				// @DOCLINE * `index_to_loc_format`: equivalent to "indexToLocFormat" in the "head" table, defined below: @NLNT
				int16_m index_to_loc_format;
				// @DOCLINE * `glyph_data_format`: equivalent to "glyphDataFormat" in the "head" table, defined below: @NLNT
				int16_m glyph_data_format;
			};
			typedef struct muttHeadInfo muttHeadInfo;

		// @DOCLINE ## Maxp table information

			// @DOCLINE The struct `muttMaxpInfo` is used to define the information provided by the necessary "maxp" table within a TrueType font. It has the following members:

			struct muttMaxpInfo {
				// @DOCLINE * `version_high`: equivalent to the high-bytes of "version" in the "maxp" table, defined below: @NLNT
				uint16_m version_high;
				// @DOCLINE * `version_low`: equivalent to the low-bytes of "version" in the "maxp" table, defined below: @NLNT
				uint16_m version_low;
				// @DOCLINE * `num_glyphs`: equivalent to "numGlyphs" in the "maxp" table, defined below: @NLNT
				uint16_m num_glyphs;
				// @DOCLINE * `max_points`: equivalent to "maxPoints" in the "maxp" table, defined below: @NLNT
				uint16_m max_points;
				// @DOCLINE * `max_contours`: equivalent to "maxContours" in the "maxp" table, defined below: @NLNT
				uint16_m max_contours;
				// @DOCLINE * `max_composite_points`: equivalent to "maxCompositePoints" in the "maxp" table, defined below: @NLNT
				uint16_m max_composite_points;
				// @DOCLINE * `max_composite_contours`: equivalent to "maxCompositeContours" in the "maxp" table, defined below: @NLNT
				uint16_m max_composite_contours;
				// @DOCLINE * `max_zones`: equivalent to "maxZones" in the "maxp" table, defined below: @NLNT
				uint16_m max_zones;
				// @DOCLINE * `max_twilight_points`: equivalent to "maxTwilightPoints" in the "maxp" table, defined below: @NLNT
				uint16_m max_twilight_points;
				// @DOCLINE * `max_storage`: equivalent to "maxStorage" in the "maxp" table, defined below: @NLNT
				uint16_m max_storage;
				// @DOCLINE * `max_function_defs`: equivalent to "maxFunctionDefs" in the "maxp" table, defined below: @NLNT
				uint16_m max_function_defs;
				// @DOCLINE * `max_instruction_defs`: equivalent to "maxInstructionDefs" in the "maxp" table, defined below: @NLNT
				uint16_m max_instruction_defs;
				// @DOCLINE * `max_stack_elements`: equivalent to "maxStackElements" in the "maxp" table, defined below: @NLNT
				uint16_m max_stack_elements;
				// @DOCLINE * `max_size_of_instructions`: equivalent to "maxSizeOfInstructions" in the "maxp" table, defined below: @NLNT
				uint16_m max_size_of_instructions;
				// @DOCLINE * `max_component_elements`: equivalent to "maxComponentElements" in the "maxp" table, defined below: @NLNT
				uint16_m max_component_elements;
				// @DOCLINE * `max_component_depth`: equivalent to "maxComponentDepth" in the "maxp" table, defined below: @NLNT
				uint16_m max_component_depth;
			};
			typedef struct muttMaxpInfo muttMaxpInfo;

		// @DOCLINE ## Hhea table information

			// @DOCLINE The struct `muttHheaInfo` is used to define the information provided by the necessary "hhea" table within a TrueType font. It has the following members:

			struct muttHheaInfo {
				// @DOCLINE * `major_version`: equivalent to "majorVersion" in the "hhea" table, defined below: @NLNT
				uint16_m major_version;
				// @DOCLINE * `minor_version`: equivalent to "minorVersion" in the "hhea" table, defined below: @NLNT
				uint16_m minor_version;
				// @DOCLINE * `ascender`: equivalent to "ascender" in the "hhea" table, defined below: @NLNT
				int16_m ascender;
				// @DOCLINE * `descender`: equivalent to "descender" in the "hhea" table, defined below: @NLNT
				int16_m descender;
				// @DOCLINE * `line_gap`: equivalent to "lineGap" in the "hhea" table, defined below: @NLNT
				int16_m line_gap;
				// @DOCLINE * `advance_max_width`: equivalent to "advanceWidthMax" in the "hhea" table, defined below: @NLNT
				uint16_m advance_max_width;
				// @DOCLINE * `min_left_side_bearing`: equivalent to "minLeftSideBearing" in the "hhea" table, defined below: @NLNT
				int16_m min_left_side_bearing;
				// @DOCLINE * `min_right_side_bearing`: equivalent to "minRightSideBearing" in the "hhea" table, defined below: @NLNT
				int16_m min_right_side_bearing;
				// @DOCLINE * `x_max_extent`: equivalent to "xMaxExtent" in the "hhea" table, defined below: @NLNT
				int16_m x_max_extent;
				// @DOCLINE * `caret_slope_rise`: equivalent to "caretSlopeRise" in the "hhea" table, defined below: @NLNT
				int16_m caret_slope_rise;
				// @DOCLINE * `caret_slope_run`: equivalent to "caretSlopeRun" in the "hhea" table, defined below: @NLNT
				int16_m caret_slope_run;
				// @DOCLINE * `caret_offset`: equivalent to "caretOffset" in the "hhea" table, defined below: @NLNT
				int16_m caret_offset;
				// @DOCLINE * `metric_data_format`: equivalent to "metricDataFormat" in the "hhea" table, defined below: @NLNT
				int16_m metric_data_format;
				// @DOCLINE * `number_of_hmetrics`: equivalent to "numberOfHMetrics" in the "hhea" table, defined below: @NLNT
				uint16_m number_of_hmetrics;
			};
			typedef struct muttHheaInfo muttHheaInfo;

	// @DOCLINE # General TrueType information

		// @DOCLINE The struct `muttInfo` is used to refer to the general information about a TrueType font, and is used to refer to a TrueType font across multiple function calls. It has the following documented members:

		struct muttInfo {
			// @DOCLINE `data`: the data of the TrueType font, defined below: @NLNT
			muByte* data;
			// @DOCLINE `size`: the size of the TrueType font data, in bytes, defined below: @NLNT
			size_m size;

			// @DOCLINE `table_count`: the amount of tables within the TrueType font data, defined below: @NLNT
			uint16_m table_count;

			// @DOCLINE `req`: the required tables in the TrueType font data, defined below: @NLNT
			muttRequiredTables req;

			// @DOCLINE `head_info`: information retrieved from the required "head" table, defined below: @NLNT
			muttHeadInfo head_info;
			// @DOCLINE `maxp_info`: information retrieved from the required "maxp" table, defined below: @NLNT
			muttMaxpInfo maxp_info;
			// @DOCLINE `hhea_info`: information retrieved from the required "hhea" table, defined below: @NLNT
			muttHheaInfo hhea_info;
		};

		// @DOCLINE All of the members, including those regarding the raw data of the TrueType font, are automatically generated upon a successful call to `mu_truetype_get_info`, and are invalid upon its respective call to `mu_truetype_let_info`. The members are meant to be read, not written.

		// @DOCLINE ## Retrieve TrueType information

			// @DOCLINE The function `mu_truetype_get_info` retrieves information about TrueType data and stores it in a `muttInfo` struct, defined below: @NLNT
			MUDEF muttInfo mu_truetype_get_info(muttResult* result, muByte* data, size_m size);

			// @DOCLINE Every successful call to `mu_truetype_get_info` must be matched with a call to `mutt_true_type_let_info`.

			// @DOCLINE The pointer `data` is assumed to be valid and unchanged throughout the returned `muttInfo` struct's lifetime.

		// @DOCLINE ## Free TrueType information

			// @DOCLINE The function `mu_truetype_let_info` frees the information retrieved about TrueType data stored in a `muttInfo` struct, defined below: @NLNT
			MUDEF void mu_truetype_let_info(muttInfo* info);

			// @DOCLINE This function must be called on every successfully created `muttInfo` struct.

	// @DOCLINE # Table retrieval

		// @DOCLINE mutt can retrieve information about all of the tables offered in a TrueType ofnt.

		// @DOCLINE ## Get table amount

			// @DOCLINE The function `mu_truetype_get_table_count` returns the amount of tables within a given TrueType font, defined below: @NLNT
			MUDEF uint16_m mu_truetype_get_table_count(muttInfo* info);

			// @DOCLINE This function returns `info->table_count`.

		// @DOCLINE ## Get table information

			// @DOCLINE The function `mu_truetype_get_table` retrieves information about a requested table witin a given TrueType font, defined below: @NLNT
			MUDEF void mu_truetype_get_table(muttInfo* info, uint16_m table, muttTable* table_info, char* name);

			// @DOCLINE `table` must be a valid index referring to a table, less than `info->table_count`.

			// @DOCLINE `table_info`, if not 0, will be dereferenced and filled in with information about the table.

			// @DOCLINE `name`, if not 0, will be filled with the 4-byte identifier for the table.

	// @DOCLINE # Platform-specific encoding

		// @DOCLINE TrueType has support for multiple character encoding types. For the sake of this library's explicitness, when a character or string of characters is referenced, it is supplemented with a description of its platform encoding, which corresponds to TrueType's documentation.

		// @DOCLINE ## Encoding types

			// @DOCLINE The types `muttPlatformID` and `muttEncodingID` are defined (both as `uint16_m`) to represent a platform ID and corresponding encoding ID.

			#define muttPlatformID uint16_m
			#define muttEncodingID uint16_m

			// @DOCLINE ### Platform ID values

				// @DOCLINE * `MUTT_PLATFORM_ID_UNICODE`: Platform ID 0; "Unicode" (UTF-16BE).
				#define MUTT_PLATFORM_ID_UNICODE 0
				// @DOCLINE * `MUTT_PLATFORM_ID_MACINTOSH`: Platform ID 1; "Macintosh".
				#define MUTT_PLATFORM_ID_MACINTOSH 1
				// @DOCLINE * `MUTT_PLATFORM_ID_WINDOWS`: Platform ID 3; "Windows".
				#define MUTT_PLATFORM_ID_WINDOWS 3
				// @DOCLINE * `MUTT_PLATFORM_ID_CUSTOM`: Platform ID 4; "Custom".
				#define MUTT_PLATFORM_ID_CUSTOM 4

			// @DOCLINE ### Unicode encoding IDs

				// @DOCLINE * `MUTT_UNICODE_ENCODING_2_0_BMP`: Encoding ID 3; "Unicode 2.0 and onwards semantics, Unicode BMP only".
				#define MUTT_UNICODE_ENCODING_2_0_BMP 3
				// @DOCLINE * `MUTT_UNICODE_ENCODING_2_0`: Encoding ID 4; "Unicode 2.0 and onwards semantics, Unicode full repertoire".
				#define MUTT_UNICODE_ENCODING_2_0 4
				// @DOCLINE * `MUTT_UNICODE_ENCODING_VAR`: Encoding ID 5; "Unicode variation sequences—for use with subtable format 14".
				#define MUTT_UNICODE_ENCODING_VAR 5
				// @DOCLINE * `MUTT_UNICODE_ENCODING_FULL`: Encoding ID 6; "Unicode full repertoire—for use with subtable format 13".
				#define MUTT_UNICODE_ENCODING_FULL 6

			// @DOCLINE ### Windows encoding IDs

				// @DOCLINE * `MUTT_WINDOWS_ENCODING_SYMBOL`: Encoding ID 0; "Symbol".
				#define MUTT_WINDOWS_ENCODING_SYMBOL 0
				// @DOCLINE * `MUTT_WINDOWS_ENCODING_UNICODE_BMP`: Encoding ID 1; "Unicode BMP".
				#define MUTT_WINDOWS_ENCODING_UNICODE_BMP 1
				// @DOCLINE * `MUTT_WINDOWS_ENCODING_SHIFTJIS`: Encoding ID 2; "ShiftJIS".
				#define MUTT_WINDOWS_ENCODING_SHIFTJIS 2
				// @DOCLINE * `MUTT_WINDOWS_ENCODING_PRC`: Encoding ID 3; "PRC".
				#define MUTT_WINDOWS_ENCODING_PRC 3
				// @DOCLINE * `MUTT_WINDOWS_ENCODING_BIG5`: Encoding ID 4; "Big5".
				#define MUTT_WINDOWS_ENCODING_BIG5 4
				// @DOCLINE * `MUTT_WINDOWS_ENCODING_WANSUNG`: Encoding ID 5; "Wansung".
				#define MUTT_WINDOWS_ENCODING_WANSUNG 5
				// @DOCLINE * `MUTT_WINDOWS_ENCODING_JOHAB`: Encoding ID 6; "Johab".
				#define MUTT_WINDOWS_ENCODING_JOHAB 6
				// @DOCLINE * `MUTT_WINDOWS_ENCODING_UNICODE`: Encoding ID 10; "Unicode full repertoire".
				#define MUTT_WINDOWS_ENCODING_UNICODE 10

		// @DOCLINE ## Encoding struct

			// @DOCLINE The struct `muttEncoding` is used to refer to character encoding. It has the following members:

			struct muttEncoding {
				// @DOCLINE * `platform_id`: the platform ID, defined below: @NLNT
				muttPlatformID platform_id;
				// @DOCLINE * `encoding_id`: the encoding ID, defined below: @NLNT
				muttEncodingID encoding_id;
			}; typedef struct muttEncoding muttEncoding;

	// @DOCLINE # Name table

		// @DOCLINE mutt can retrieve information from the "name" table in TrueType based on a requested name ID.

		// @DOCLINE ## Name ID

			// @DOCLINE The type `muttNameID` (`uint16_m`) is used to represent a name ID in TrueType. Any value based on the TrueType standard will work, but this type gives a representation for some common name IDs.

			#define muttNameID uint16_m

			// @DOCLINE ### Values

			// @DOCLINE * `MUTT_NAME_COPYRIGHT_NOTICE`: ID 0 based on TrueType standards; "Copyright notice."
			#define MUTT_NAME_COPYRIGHT_NOTICE 0
			// @DOCLINE * `MUTT_NAME_FONT_FAMILY`: ID 1 based on TrueType standards; "Font Family name."
			#define MUTT_NAME_FONT_FAMILY 1
			// @DOCLINE * `MUTT_NAME_FONT_SUBFAMILY`: ID 2 based on TrueType standards; "Font Subfamily name."
			#define MUTT_NAME_FONT_SUBFAMILY 2
			// @DOCLINE * `MUTT_NAME_FULL_FONT_NAME`: ID 4 based on TrueType standards; "Full font name".
			#define MUTT_NAME_FULL_FONT_NAME 4
			// @DOCLINE * `MUTT_NAME_VERSION`: ID 5 based on TrueType standards; "Version string."
			#define MUTT_NAME_VERSION 5
			// @DOCLINE * `MUTT_NAME_TRADEMARK`: ID 7 based on TrueType standards; "Trademark."
			#define MUTT_NAME_TRADEMARK 7
			// @DOCLINE * `MUTT_NAME_MANUFACTURER`: ID 8 based on TrueType standards; "Manufacturer Name."
			#define MUTT_NAME_MANUFACTURER 8
			// @DOCLINE * `MUTT_NAME_DESIGNER`: ID 9 based on TrueType standards; "Designer."
			#define MUTT_NAME_DESIGNER 9
			// @DOCLINE * `MUTT_NAME_DESCRIPTION`: ID 10 based on TrueType standards; "Description."
			#define MUTT_NAME_DESCRIPTION 10
			// @DOCLINE * `MUTT_NAME_VENDOR_URL`: ID 11 based on TrueType standards; "URL of Vendor."
			#define MUTT_NAME_VENDOR_URL 11
			// @DOCLINE * `MUTT_NAME_DESIGNER_URL`: ID 12 based on TrueType standards; "URL of Designer."
			#define MUTT_NAME_DESIGNER_URL 12
			// @DOCLINE * `MUTT_NAME_LICENSE_DESCRIPTION`: ID 13 based on TrueType standards; "License Description."
			#define MUTT_NAME_LICENSE_DESCRIPTION 13
			// @DOCLINE * `MUTT_NAME_LICENSE_INFO_URL`: ID 14 based on TrueType standards; "License Info URL."
			#define MUTT_NAME_LICENSE_INFO_URL 14
			// @DOCLINE * `MUTT_NAME_TYPOGRAPHIC_FAMILY`: ID 16 based on TrueType standards; "Typographic Family name."
			#define MUTT_NAME_TYPOGRAPHIC_FAMILY 16
			// @DOCLINE * `MUTT_NAME_SAMPLE_TEXT`: ID 19 based on TrueType standards; "Sample text."
			#define MUTT_NAME_SAMPLE_TEXT 19

			// @DOCLINE ### Name

			#ifdef MUTT_NAMES

			// @DOCLINE The function `mutt_name_id_get_name` converts a `muttNameID` value to a `const char*` representation, defined below: @NLNT
			MUDEF const char* mutt_name_id_get_name(muttNameID nameID);

			// @DOCLINE The function `mutt_name_id_get_nice_name` converts a `muttNameID` value to a more readable `const char*` representation, defined below: @NLNT
			MUDEF const char* mutt_name_id_get_nice_name(muttNameID nameID);

			// @DOCLINE Note that these functions are only defined if `MUTT_NAMES` is defined before the inclusion of the header file.

			// @DOCLINE These functions return `"MUTT_UNKNOWN"` if a respective name could not be found.

			#endif

		// @DOCLINE ## Get names

			// @DOCLINE ### Get offered name IDs

				// @DOCLINE The function `mu_truetype_get_name_ids` is used to retrieve the name IDs offered by a TrueType font, defined below: @NLNT
				MUDEF uint16_m mu_truetype_get_name_ids(muttInfo* info, muttNameID* ids);

				// @DOCLINE This function returns the amount of name IDs specified by the font. If `ids` is not 0, `ids` is expected to be a pointer to an array of `uint16_m`s at least the length of the amount of name IDs specified by the font, and will be written to as such.

			// @DOCLINE ### Get particular name ID

				// @DOCLINE The function `mu_truetype_get_name_id` returns the name ID of a specific name, defined below: @NLNT
				MUDEF uint16_m mu_truetype_get_name_id(muttInfo* info, uint16_m index);

				// @DOCLINE `index` is the index of the name ID being requested.

			// @DOCLINE ### Get name

				// @DOCLINE The function `mu_truetype_get_name` retrieves the string for a given name ID, defined below: @NLNT
				MUDEF char* mu_truetype_get_name(muttInfo* info, uint16_m name_id_index, muttEncoding* encoding, uint16_m* length);

				// @DOCLINE This function returns a pointer to an offset in the TrueType font data within `info` which holds the name.

				// @DOCLINE `name_id_index` is the index of the name ID being requested; for example, if the font in question offers 7 name IDs and you wanted the 5th name ID specified, `name_id_index` should be 4. `name_id_index` is *not* the name ID, but instead the index the name in question specified by the font.

				// @DOCLINE If `length` is not 0, `length` is dereferenced and set to the length of the name in bytes.

				// @DOCLINE If `encoding` is not 0, `encoding` is dereferenced and set to the encoding of the string.

				// @DOCLINE Note that the function returned does not necessarily have a null-terminating character.

	// @DOCLINE # Loca table

		// @DOCLINE ## Glyph ID referencing

			// @DOCLINE The type `muttGlyphID` (defined as `uint16_m`) is used to refer to a glyph specified by a TrueType font. It matches a glyph's index as specified in the "loca" table. Characters can be converted to these types with the "cmap" section of the mutt API.

			#define muttGlyphID uint16_m

			// @DOCLINE Note that a `muttGlyphID` can refer to either a simple or a composite glyph.

			// @DOCLINE Note that the value '0' will always be valid for a `muttGlyphID` in regards to a valid TrueType font, as all TrueType fonts have to specify a glyph for missing characters. This value is used by mutt to refer to a glyph that doesn't exist (for example, if a corresponding `muttGlyphID` is requested for a given character but the font doesn't specify it, 0 is returned, giving the missing character glyph).

		// @DOCLINE ## Get glyf table via glyph ID

			// @DOCLINE The function `mu_truetype_get_glyf_table` returns a pointer to a glyf table data from a glyph ID based off of the "loca" table data, defined below: @NLNT
			MUDEF muByte* mu_truetype_get_glyf_table(muttInfo* info, muttGlyphID id, uint32_m* length);

			// @DOCLINE If `length` is not 0, `length` is dereferenced and set to the length of the glyf table data being pointed to in bytes.

			// @DOCLINE `id` must be a valid glyph ID value and less than `info.maxp_info.num_glyphs`.

	// @DOCLINE # Cmap table

		// @DOCLINE ## Encoding record definition

			// @DOCLINE The struct `muttEncodingRecord` is used to define a TrueType cmap encoding record. It has the following members:

			struct muttEncodingRecord {
				// @DOCLINE * `encoding`: the encoding, defined below: @NLNT
				muttEncoding encoding;
				// @DOCLINE * `subtable`: a pointer to the subtable in the TrueType data, defined below: @NLNT
				muByte* subtable;
				// @DOCLINE * `format`: the format of the subtable, defined below: @NLNT
				uint16_m format;
				// @DOCLINE * `language`: the Macintosh-specific language of the subtable, defined below: @NLNT
				uint32_m language;
			};
			typedef struct muttEncodingRecord muttEncodingRecord;

			// @DOCLINE The member "`language`" is 0 if `format` does not have a language field.

		// @DOCLINE ## Get encoding record

			// @DOCLINE The function `mu_truetype_get_encoding_record` retrieves a requested encoding record given in the cmap table of a given TrueType font, defined below: @NLNT
			MUDEF void mu_truetype_get_encoding_record(muttInfo* info, muttEncodingRecord* record, uint16_m id);

			// @DOCLINE `id` is the index of the encoding record; the amount of encoding records (and optionally all of the encoding records) can be retrieved via the function `mu_truetype_get_encoding_records`.

		// @DOCLINE ## Get all encoding records

			// @DOCLINE The function `mu_truetype_get_encoding_records` retrieves the encoding records listed in the cmap table of a given TrueType font, defined below: @NLNT
			MUDEF uint16_m mu_truetype_get_encoding_records(muttInfo* info, muttEncodingRecord* records);

			// @DOCLINE This function returns the amount of encoding records stored within the given TrueType font.

			// @DOCLINE If `records` is not 0, `records` should be a `muttEncodingRecord` array of a length equivalent to how many encoding records are stored within the given TrueType font.

		// @DOCLINE ## Cmap formats

			// @DOCLINE Since TrueType stores its cmap data in multiple formats, mutt has a struct and corresponding lookup function for each supported format type.

			// @DOCLINE ### Format 4

				// @DOCLINE #### Struct

				// @DOCLINE The struct for format 4 is `muttFormat4`, and its members are:

				struct muttFormat4 {
					// @DOCLINE `table`: a pointer to the table data, defined below: @NLNT
					muByte* table;
					// @DOCLINE `length`: equivalent to "length" in the format 4 cmap subtable, defined below: @NLNT
					uint16_m length;
					// @DOCLINE `language`: equivalent to "language" in the format 4 cmap subtable, defined below: @NLNT
					uint16_m language;
					// @DOCLINE `seg_count_x2`: equivalent to "segCountX2" in the format 4 cmap subtable, defined below: @NLNT
					uint16_m seg_count_x2;
					// @DOCLINE `seg_count`: `seg_count_x2` divided by two, defined below: @NLNT
					uint16_m seg_count;
					// @DOCLINE `search_range`: equivalent to "searchRange" in the format 4 cmap subtable, defined below: @NLNT
					uint16_m search_range;
					// @DOCLINE `entry_selector`: equivalent to "entrySelector" in the format 4 cmap subtable, defined below: @NLNT
					uint16_m entry_selector;
					// @DOCLINE `range_shift`: equivalent to "rangeShift" in the format 4 cmap subtable, defined below: @NLNT
					uint16_m range_shift;
					// @DOCLINE `end_code`: a pointer to the "endCode" byte data in the format 4 cmap subtable, defined below: @NLNT
					muByte* end_code;
					// @DOCLINE `start_code`: a pointer to the "startCode" byte data in the format 4 cmap subtable, defined below: @NLNT
					muByte* start_code;
					// @DOCLINE `id_delta`: a pointer to the "idDelta" byte data in the format 4 cmap subtable, defined below: @NLNT
					muByte* id_delta;
					// @DOCLINE `id_range_offset`: a pointer to the "idRangeOffset" byte data in the format 4 cmap subtable, defined below: @NLNT
					muByte* id_range_offset;
					// @DOCLINE `glyph_id_array`: a pointer to the "glyphIdArray" byte data in the format 4 cmap subtable, defined below: @NLNT
					muByte* glyph_id_array;
					// @DOCLINE `glyph_id_length`: the length of `glyph_id_array`, in bytes, defined below: @NLNT
					uint16_m glyph_id_length;
				};
				typedef struct muttFormat4 muttFormat4;

				// @DOCLINE Note that `id_delta` is incorrectly defined in Apple's TrueType reference manual as being an unsigned 16-bit array; it is a signed 16-bit array.

				// @DOCLINE #### Get format 4 information

				// @DOCLINE The function `mu_truetype_get_format_4` is used to get information about a format 4 cmap subtable, defined below: @NLNT
				MUDEF void mu_truetype_get_format_4(muByte* subtable, muttFormat4* format);

				// @DOCLINE `subtable` is equivalent to `muttEncodingRecord.subtable` if `muttEncodingRecord.format` equals 4.

				// @DOCLINE #### Get glyph from format 4

				// @DOCLINE The function `mu_truetype_get_glyph_format_4` returns a glyph ID from a format 4 cmap subtable, defined below: @NLNT
				MUDEF muttGlyphID mu_truetype_get_glyph_format_4(muttFormat4* format, uint16_m character_code);

				// @DOCLINE `character_code` is a valid character code in regards to the respective encoding record's platform & encoding ID.

				// @DOCLINE This function returns 0 if the given character code is not mapped.

	// @DOCLINE # Glyf table

		// @DOCLINE ## Header information

			// @DOCLINE The header information about a glyph is the information provided by the "Glyph Header". It is represented as the struct `muttGlyphHeader` in mutt.

			// @DOCLINE ### Struct

				// @DOCLINE The struct `muttGlyphHeader` has the following members:

				struct muttGlyphHeader {
					// @DOCLINE * `data`: a pointer to the glyph header data, defined below: @NLNT
					muByte* data;
					// @DOCLINE * `number_of_contours`: equivalent to "numberOfContours" in the "Glyph Header" of the "glyf" table, defined below: @NLNT
					int16_m number_of_contours;
					// @DOCLINE * `x_min`: equivalent to "xMin" in the "Glyph Header" of the "glyf" table, defined below: @NLNT
					int16_m x_min;
					// @DOCLINE * `y_min`: equivalent to "yMin" in the "Glyph Header" of the "glyf" table, defined below: @NLNT
					int16_m y_min;
					// @DOCLINE * `x_max`: equivalent to "xMax" in the "Glyph Header" of the "glyf" table, defined below: @NLNT
					int16_m x_max;
					// @DOCLINE * `y_max`: equivalent to "yMax" in the "Glyph Header" of the "glyf" table, defined below: @NLNT
					int16_m y_max;
				};
				typedef struct muttGlyphHeader muttGlyphHeader;

			// @DOCLINE ### Retrieve glyph header

				// @DOCLINE The function `mu_truetype_get_glyph_header` retrieves the glyph header of a given glyph, defined below: @NLNT
				MUDEF void mu_truetype_get_glyph_header(muByte* table, muttGlyphHeader* header);

				// @DOCLINE `table` must be a pointer to a valid glyph table; a pointer to a specific glyph table based on a glyph ID can be accessed via the "loca" table.

				// @DOCLINE Note that when calling `mu_truetype_get_glyf_table`, if `length` is set to 0, then this function should not be called on it, as it is a glyph with no description, existing as an empty glyph, and calling this function on it will return the description of some other glyph.

		// @DOCLINE ## Simple glyph data

			// @DOCLINE mutt can be used to extract simple glyph data, represented by the struct `muttSimpleGlyph`.

			// @DOCLINE ### Struct

				// @DOCLINE The struct `muttSimpleGlyph` represents the data of a simple glyph. It has the following members:

				struct muttSimpleGlyph {
					// @DOCLINE * `end_pts_of_contours`: a pointer to memory from `muttGlyphHeader->data`; equivalent to "endPtsOfContours" in the "Simple Glyph table", defined below: @NLNT
					muByte* end_pts_of_contours;
					// @DOCLINE * `instruction_length`: equivalent to "instructionLength" in the "Simple Glyph table", defined below: @NLNT
					uint16_m instruction_length;
					// @DOCLINE * `instructions`: a pointer to memory from `muttGlyphHeader->data`; equivalent to "instructions" in the "Simple Glyph table", defined below: @NLNT
					uint8_m* instructions;
					// @DOCLINE * `flags`: 0 or a pointer to user-allocated memory; an array of flags for each point in the glyph, defined below: @NLNT
					uint8_m* flags;
					// @DOCLINE * `x_coordinates`: 0 or a pointer to user-allocated memory; an array of X-coordinates for each point in the glyph, defined below: @NLNT
					int16_m* x_coordinates;
					// @DOCLINE * `y_coordinates`: 0 or a pointer to user-allocated memory; an array of Y-coordinates for each point in the glyph, defined below: @NLNT
					int16_m* y_coordinates;
				};
				typedef struct muttSimpleGlyph muttSimpleGlyph;

				// @DOCLINE `end_pts_of_contours` is a pointer to where "endPtsOfContours" lies in `muttGlyphHeader->data`, and is filled in automatically by `mu_truetype_get_simple_glyph`. The data type of "endPtsOfContours" is "uint16", meaning that endianness conversions may need to be performed to retrieve the correct value, hence why the data type of `end_pts_of_contours` is `muByte*`; to retrieve an element from this list, use the function `mu_truetype_get_contour_end_pt`.

				// @DOCLINE `instruction_length` is automatically filled in by `mu_truetype_get_simple_glyph`.

				// @DOCLINE `instructions` is a pointer to where "instructions" lies in `muttGlyphHeader->data`, and is filled in automatically by `mu_truetype_get_simple_glyph`. Because the data type of "instructions" is "uint8", it can be read directly, hence why the data type of `instructions` is `uint8_m*`.

				// @DOCLINE If `flags`, `x_coordinates`, or `y_coordinates` are not set to 0 upon being called to `mu_truetype_get_simple_glyph`, `flags` is expected to be a pointer to user-allocated memory whose size is at least `sizeof(uint8_m) * mu_truetype_get_contour_end_pt(muttSimpleGlyph, muttGlyphHeader->number_of_contours-1)`.

				// @DOCLINE The interpretation for `flags` is different than how it is defined in the "Simple Glyph table" in TrueType's documentation: whereas "flags" in the "Simple Glyph table" can represent multiple flags with one flag element, `flags` in `muttGlyphHeader` has an element for every single point; `flags[n]` is the flag for point `n`.

				// @DOCLINE If `flags`, `x_coordinates`, or `y_coordinates` are not set to 0 upon being called to `mu_truetype_get_simple_glyph`, `x_coordinates` is expected to be a pointer to user-allocated memory whose size is at least `sizeof(int32_m) * mu_truetype_get_contour_end_pt(muttSimpleGlyph, muttGlyphHeader->number_of_contours-1)`. The same logic applies to `y_coordinates`.

				// @DOCLINE The interpretation for `x_coordinates` and `y_coordinates` are different than how it is defined in the "Simple Glyph table" in TrueType's documentation: whereas "xCoordinates" and "yCoordinates" in the "Simple Glyph table" can represent multiple coordinates with one element, `x_coordinates` and `y_coordinates` in `muttGlyphHeader` have an element for every single point; `x_coordinates[n]` and `y_coordinates[n]` are the coordinates for point `n`. This also means that the values stored in the coordinate arrays within `muttGlyphHeader` store the raw coordinates in FUnits, whereas in the "Simple Glyph table", they're stored as offsets from the previous point.

			// @DOCLINE ### Flag macros

				// @DOCLINE The following macros are defined to make bit-masking the flag values of a glyph easier:

				// @DOCLINE * [0x01] `MUTT_ON_CURVE_POINT`: equivalent to "ON_CURVE_POINT".
				#define MUTT_ON_CURVE_POINT 0x01
				// @DOCLINE * [0x02] `MUTT_X_SHORT_VECTOR`: equivalent to "X_SHORT_VECTOR".
				#define MUTT_X_SHORT_VECTOR 0x02
				// @DOCLINE * [0x04] `MUTT_Y_SHORT_VECTOR`: equivalent to "Y_SHORT_VECTOR".
				#define MUTT_Y_SHORT_VECTOR 0x04
				// @DOCLINE * [0x08] `MUTT_REPEAT_FLAG`: equivalent to "REPEAT_FLAG".
				#define MUTT_REPEAT_FLAG 0x08
				// @DOCLINE * [0x10] `MUTT_X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR`: equivalent to "X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR".
				#define MUTT_X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR 0x10
				// @DOCLINE * [0x20] `MUTT_Y_IS_SAME_OR_POSITIVE_Y_SHORT_VECTOR`: equivalent to "Y_IS_SAME_OR_POSITIVE_Y_SHORT_VECTOR".
				#define MUTT_Y_IS_SAME_OR_POSITIVE_Y_SHORT_VECTOR 0x20
				// @DOCLINE * [0x40] `MUTT_OVERLAP_SIMPLE`: equivalent to "OVERLAP_SIMPLE".
				#define MUTT_OVERLAP_SIMPLE 0x40

			// @DOCLINE ### Get simple glyph data

				// @DOCLINE The function `mu_truetype_get_simple_glyph` is used to retrieve a simple glyph's data, defined below: @NLNT
				MUDEF uint16_m mu_truetype_get_simple_glyph(muttGlyphHeader* header, muttSimpleGlyph* glyph);

				// @DOCLINE If `glyph` is 0, the amount of points within the glyph is returned. This is useful for allocating the memory necessary for the data first and then retrieving all of the data without additional unnecessary operations.

				// @DOCLINE If `glyph` is not 0, `flags`, `x_coordinates`, and `y_coordinates` must all rather be 0 (to which they will be ignored) or all valid pointers to arrays of their respective type with a length of at least the nmuber of points in the glyph.

				// @DOCLINE See the "Struct" section for `muttSimpleGlyph` for specific memory requirements and how to interpret the values that this function fills in.

			// @DOCLINE ### Get contour end point

				// @DOCLINE The function `mu_truetype_get_contour_end_pt` retrieves a value from the "endPtsOfContours" array within a "Simple Glyph table", defined below: @NLNT
				MUDEF uint16_m mu_truetype_get_contour_end_pt(muttSimpleGlyph* glyph, uint16_m contour);

				// @DOCLINE `contour` must be below the number of contours for the respective glyph.

		// @DOCLINE ## Composite glyph data

			// @DOCLINE mutt can be used to extract composite glyph data.

			// @DOCLINE ### Composite record struct

				// @DOCLINE The struct `muttCompositeRecord` represents a record in a composite glyph. It has the following members:

				struct muttCompositeRecord {
					// @DOCLINE * `flags`: equivalent to "flags" in the "Component Glyph record", defined below: @NLNT
					uint16_m flags;
					// @DOCLINE * `glyph_index`: equivalent to "glyphIndex" in the "Component Glyph record", defined below: @NLNT
					uint16_m glyph_index;
					// @DOCLINE * `argument1`: the x-offset (see clarifications below), defined below: @NLNT
					muByte argument1[2];
					// @DOCLINE * `argument2`: the y-offset (see clarifications below), defined below: @NLNT
					muByte argument2[2];
					// @DOCLINE * `scales`: the possible scales of the record (see clarifications below), defined below: @NLNT
					float scales[4];
					// @DOCLINE * `next`: address to the next byte after the last byte of the composite record; used for retrieving the next composite record, defined below: @NLNT
					muByte* next;
				};
				typedef struct muttCompositeRecord muttCompositeRecord;

				// @DOCLINE The contents of `argument1` and `argument2` are dependent on the value of `flags` (see TrueType documentation), and, in order to be read, the contents of `argument1` and `argument2` are to be casted to a pointer of the indicated type and then dereferenced.

				/* @DOCBEGIN

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

				@DOCEND */

			// @DOCLINE #### Composite record flag macros

				// @DOCLINE The following macros are defined for bit-masking `muttCompositeRecord->flags`:

				// @DOCLINE * [0x0001] `MUTT_ARG_1_AND_2_ARE_WORDS`: equivalent to "ARG_1_AND_2_ARE_WORDS".
				#define MUTT_ARG_1_AND_2_ARE_WORDS 0x0001
				// @DOCLINE * [0x0002] `MUTT_ARGS_ARE_XY_VALUES`: equivalent to "ARGS_ARE_XY_VALUES".
				#define MUTT_ARGS_ARE_XY_VALUES 0x0002
				// @DOCLINE * [0x0004] `MUTT_ROUND_XY_TO_GRID`: equivalent to "ROUND_XY_TO_GRID".
				#define MUTT_ROUND_XY_TO_GRID 0x0004
				// @DOCLINE * [0x0008] `MUTT_WE_HAVE_A_SCALE`: equivalent to "WE_HAVE_A_SCALE".
				#define MUTT_WE_HAVE_A_SCALE 0x0008
				// @DOCLINE * [0x0020] `MUTT_MORE_COMPONENTS`: equivalent to "MORE_COMPONENTS".
				#define MUTT_MORE_COMPONENTS 0x0020
				// @DOCLINE * [0x0040] `MUTT_WE_HAVE_AN_X_AND_Y_SCALE`: equivalent to "WE_HAVE_AN_X_AND_Y_SCALE".
				#define MUTT_WE_HAVE_AN_X_AND_Y_SCALE 0x0040
				// @DOCLINE * [0x0080] `MUTT_WE_HAVE_A_TWO_BY_TWO`: equivalent to "WE_HAVE_A_TWO_BY_TWO".
				#define MUTT_WE_HAVE_A_TWO_BY_TWO 0x0080
				// @DOCLINE * [0x0100] `MUTT_WE_HAVE_INSTRUCTIONS`: equivalent to "WE_HAVE_INSTRUCTIONS".
				#define MUTT_WE_HAVE_INSTRUCTIONS 0x0100
				// @DOCLINE * [0x0200] `MUTT_USE_MY_METRICS`: equivalent to "USE_MY_METRICS".
				#define MUTT_USE_MY_METRICS 0x0200
				// @DOCLINE * [0x0400] `MUTT_OVERLAP_COMPOUND`: equivalent to "OVERLAP_COMPOUND".
				#define MUTT_OVERLAP_COMPOUND 0x0400
				// @DOCLINE * [0x0800] `MUTT_SCALED_COMPONENT_OFFSET`: equivalent to "SCALED_COMPONENT_OFFSET".
				#define MUTT_SCALED_COMPONENT_OFFSET 0x0800
				// @DOCLINE * [0x1000] `MUTT_UNSCALED_COMPONENT_OFFSET`: equivalent to "UNSCALED_COMPONENT_OFFSET".
				#define MUTT_UNSCALED_COMPONENT_OFFSET 0x1000

			// @DOCLINE #### Composite instructions struct

				// @DOCLINE The struct `muttCompositeInstructions` represents the optional instructions that may come after the last composite record in a composite glyph. It has the following members:

				struct muttCompositeInstructions {
					// @DOCLINE * `num_instr`: the number of instructions, defined below: @NLNT
					uint16_m num_instr;
					// @DOCLINE * `instr`: a pointer to data within the TrueType font for the instructions, defined below: @NLNT
					uint8_m* instr;
				};
				typedef struct muttCompositeInstructions muttCompositeInstructions;

			// @DOCLINE ### Get composite record

				// @DOCLINE The function `mu_truetype_get_composite_record` retrieves a composite record from a glyph, defined below: @NLNT
				MUDEF void mu_truetype_get_composite_record(muttGlyphHeader* header, muttCompositeRecord* record, muttCompositeRecord* prev_record);

				// @DOCLINE `record` is the record struct to be filled in.

				// @DOCLINE `prev_record` is the previous composite record; if this function is getting called on the first composite record, this should be set to 0 to indicate as such.

			// @DOCLINE ### Get composite instructions

				// @DOCLINE The function `mu_truetype_get_composite_instructions` retrieves the instructions of a composite glyph, defined below: @NLNT
				MUDEF void mu_truetype_get_composite_instructions(muttCompositeRecord* last_record, muttCompositeInstructions* instructions);

				// @DOCLINE `last_record` is the last composite record specified in the entire glyph. It must be the last record because the data for the instructions is only stored after the last composite record.

	// @DOCLINE # Miscellaneous inner utility functions

		// @DOCLINE The following functions are miscellaneous functions used for reading values from a TrueType file.

		// @DOCLINE ## F2DOT14 reading

			// @DOCLINE The macro function "MUTT_F2DOT14" creates an expression for a float equivalent of a given array that stores 2 bytes representing a big-endian F2DOT14, defined below: @NLNT
			#define MUTT_F2DOT14(b) (float)((*(int8_m*)&b[0]) & 0xC0) + ((float)(mu_rbe_uint16(b) & 0xFFFF) / 16384.f)
			//                                            ^ 0 or 1?

		// @DOCLINE ## idDelta logic

			// @DOCLINE The logic behind adding an idDelta value to a glyph id retrieved in certain cmap formats can be confusing; the function `mu_truetype_id_delta` calculates this, defined below: @NLNT
			MUDEF uint16_m mu_truetype_id_delta(uint16_m character_code, int16_m delta);

	// @DOCLINE # Version macro

		// @DOCLINE mutt defines three macros to define the version of mutt: `MUTT_VERSION_MAJOR`, `MUTT_VERSION_MINOR`, and `MUTT_VERSION_PATCH`, following the format of `vMAJOR.MINOR.PATCH`.

		#define MUTT_VERSION_MAJOR 1
		#define MUTT_VERSION_MINOR 0
		#define MUTT_VERSION_PATCH 0

	// @DOCLINE # C standard library dependencies

		// @DOCLINE mutt has several C standard library dependencies not provided by its other library dependencies, all of which are overridable by defining them before the inclusion of its header. This is a list of all of those dependencies.

		#if !defined(mu_memcpy)

			// @DOCLINE ## `string.h` dependencies
			#include <string.h>

			// @DOCLINE `mu_memcpy`: equivalent to `memcpy`.
			#ifndef mu_memcpy
				#define mu_memcpy memcpy
			#endif

		#endif

		#if !defined(mu_pow)

			// @DOCLINE ## `math.h` dependencies
			#include <math.h>

			// @DOCLINE `mu_pow`: equivalent to `pow`.
			#ifndef mu_pow
				#define mu_pow pow
			#endif

		#endif

	#ifdef __cplusplus
	}
	#endif

#endif /* MUTT_H */


#ifdef MUTT_IMPLEMENTATION
	#ifdef __cplusplus
	extern "C" { // }
	#endif

	/* Name functions */

		#ifdef MUTT_NAMES

		MUDEF const char* mutt_result_get_name(muttResult result) {
			switch (result) {
				default: return "MUTT_UNKNOWN"; break;
				case MUTT_SUCCESS: return "MUTT_SUCCESS"; break;
				case MUTT_UNEXPECTED_EOF: return "MUTT_UNEXPECTED_EOF"; break;
				case MUTT_INVALID_HEAD_TABLE_LENGTH: return "MUTT_INVALID_HEAD_TABLE_LENGTH"; break;
				case MUTT_INVALID_MAXP_TABLE_LENGTH: return "MUTT_INVALID_MAXP_TABLE_LENGTH"; break;
				case MUTT_INVALID_HHEA_TABLE_LENGTH: return "MUTT_INVALID_HHEA_TABLE_LENGTH"; break;
				case MUTT_INVALID_NAME_TABLE_LENGTH: return "MUTT_INVALID_NAME_TABLE_LENGTH"; break;
				case MUTT_INVALID_CMAP_TABLE_LENGTH: return "MUTT_INVALID_CMAP_TABLE_LENGTH"; break;
				case MUTT_INVALID_CMAP_F0_TABLE_LENGTH: return "MUTT_INVALID_CMAP_F0_TABLE_LENGTH"; break;
				case MUTT_INVALID_CMAP_F4_TABLE_LENGTH: return "MUTT_INVALID_CMAP_F4_TABLE_LENGTH"; break;
				case MUTT_INVALID_CMAP_F12_TABLE_LENGTH: return "MUTT_INVALID_CMAP_F12_TABLE_LENGTH"; break;
				case MUTT_INVALID_TABLE_DIRECTORY_SFNT_VERSION: return "MUTT_INVALID_TABLE_DIRECTORY_SFNT_VERSION"; break;
				case MUTT_INVALID_TABLE_DIRECTORY_NUM_TABLES: return "MUTT_INVALID_TABLE_DIRECTORY_NUM_TABLES"; break;
				case MUTT_INVALID_TABLE_DIRECTORY_SEARCH_RANGE: return "MUTT_INVALID_TABLE_DIRECTORY_SEARCH_RANGE"; break;
				case MUTT_INVALID_TABLE_DIRECTORY_ENTRY_SELECTOR: return "MUTT_INVALID_TABLE_DIRECTORY_ENTRY_SELECTOR"; break;
				case MUTT_INVALID_TABLE_DIRECTORY_RANGE_SHIFT: return "MUTT_INVALID_TABLE_DIRECTORY_RANGE_SHIFT"; break;
				case MUTT_INVALID_TABLE_RECORD_OFFSET: return "MUTT_INVALID_TABLE_RECORD_OFFSET"; break;
				case MUTT_INVALID_TABLE_RECORD_LENGTH: return "MUTT_INVALID_TABLE_RECORD_LENGTH"; break;
				case MUTT_INVALID_TABLE_RECORD_CHECKSUM: return "MUTT_INVALID_TABLE_RECORD_CHECKSUM"; break;
				case MUTT_INVALID_HEAD_MAGIC_NUMBER: return "MUTT_INVALID_HEAD_MAGIC_NUMBER"; break;
				case MUTT_INVALID_HEAD_UNITS_PER_EM: return "MUTT_INVALID_HEAD_UNITS_PER_EM"; break;
				case MUTT_INVALID_HEAD_X_MIN_MAX: return "MUTT_INVALID_HEAD_X_MIN_MAX"; break;
				case MUTT_INVALID_HEAD_Y_MIN_MAX: return "MUTT_INVALID_HEAD_Y_MIN_MAX"; break;
				case MUTT_INVALID_HEAD_LOWEST_REC_PPEM: return "MUTT_INVALID_HEAD_LOWEST_REC_PPEM"; break;
				case MUTT_INVALID_HEAD_INDEX_TO_LOC_FORMAT: return "MUTT_INVALID_HEAD_INDEX_TO_LOC_FORMAT"; break;
				case MUTT_INVALID_HEAD_GLYPH_DATA_FORMAT: return "MUTT_INVALID_HEAD_GLYPH_DATA_FORMAT"; break;
				case MUTT_INVALID_MAXP_MAX_ZONES: return "MUTT_INVALID_MAXP_MAX_ZONES"; break;
				case MUTT_INVALID_HHEA_METRIC_DATA_FORMAT: return "MUTT_INVALID_HHEA_METRIC_DATA_FORMAT"; break;
				case MUTT_INVALID_CMAP_F0_GLYPH_ID: return "MUTT_INVALID_CMAP_F0_GLYPH_ID"; break;
				case MUTT_INVALID_CMAP_F4_SEG_COUNT: return "MUTT_INVALID_CMAP_F4_SEG_COUNT"; break;
				case MUTT_INVALID_CMAP_F4_SEARCH_RANGE: return "MUTT_INVALID_CMAP_F4_SEARCH_RANGE"; break;
				case MUTT_INVALID_CMAP_F4_ENTRY_SELECTOR: return "MUTT_INVALID_CMAP_F4_ENTRY_SELECTOR"; break;
				case MUTT_INVALID_CMAP_F4_RANGE_SHIFT: return "MUTT_INVALID_CMAP_F4_RANGE_SHIFT"; break;
				case MUTT_INVALID_CMAP_F4_END_CODE: return "MUTT_INVALID_CMAP_F4_END_CODE"; break;
				case MUTT_INVALID_CMAP_F4_START_CODE: return "MUTT_INVALID_CMAP_F4_START_CODE"; break;
				case MUTT_INVALID_CMAP_F4_ID_RANGE_OFFSET: return "MUTT_INVALID_CMAP_F4_ID_RANGE_OFFSET"; break;
				case MUTT_INVALID_CMAP_F4_ID_DELTA: return "MUTT_INVALID_CMAP_F4_ID_DELTA"; break;
				case MUTT_MISSING_REQUIRED_TABLE: return "MUTT_MISSING_REQUIRED_TABLE"; break;
			}
		}

		MUDEF const char* mutt_name_id_get_name(muttNameID nameID) {
			switch (nameID) {
				default: return "MUTT_UNKNOWN"; break;
				case MUTT_NAME_COPYRIGHT_NOTICE: return "MUTT_NAME_COPYRIGHT_NOTICE"; break;
				case MUTT_NAME_FONT_FAMILY: return "MUTT_NAME_FONT_FAMILY"; break;
				case MUTT_NAME_FONT_SUBFAMILY: return "MUTT_NAME_FONT_SUBFAMILY"; break;
				case MUTT_NAME_FULL_FONT_NAME: return "MUTT_NAME_FULL_FONT_NAME"; break;
				case MUTT_NAME_VERSION: return "MUTT_NAME_VERSION"; break;
				case MUTT_NAME_TRADEMARK: return "MUTT_NAME_TRADEMARK"; break;
				case MUTT_NAME_MANUFACTURER: return "MUTT_NAME_MANUFACTURER"; break;
				case MUTT_NAME_DESIGNER: return "MUTT_NAME_DESIGNER"; break;
				case MUTT_NAME_DESCRIPTION: return "MUTT_NAME_DESCRIPTION"; break;
				case MUTT_NAME_VENDOR_URL: return "MUTT_NAME_VENDOR_URL"; break;
				case MUTT_NAME_DESIGNER_URL: return "MUTT_NAME_DESIGNER_URL"; break;
				case MUTT_NAME_LICENSE_DESCRIPTION: return "MUTT_NAME_LICENSE_DESCRIPTION"; break;
				case MUTT_NAME_LICENSE_INFO_URL: return "MUTT_NAME_LICENSE_INFO_URL"; break;
				case MUTT_NAME_TYPOGRAPHIC_FAMILY: return "MUTT_NAME_TYPOGRAPHIC_FAMILY"; break;
				case MUTT_NAME_SAMPLE_TEXT: return "MUTT_NAME_SAMPLE_TEXT"; break;
			}
		}

		MUDEF const char* mutt_name_id_get_nice_name(muttNameID nameID) {
			switch (nameID) {
				default: return "MUTT_UNKNOWN"; break;
				case MUTT_NAME_COPYRIGHT_NOTICE: return "Copyright notice"; break;
				case MUTT_NAME_FONT_FAMILY: return "Font family name"; break;
				case MUTT_NAME_FONT_SUBFAMILY: return "Font subfamily name"; break;
				case MUTT_NAME_FULL_FONT_NAME: return "Full font name"; break;
				case MUTT_NAME_VERSION: return "Version"; break;
				case MUTT_NAME_TRADEMARK: return "Trademark"; break;
				case MUTT_NAME_MANUFACTURER: return "Manufacturer name"; break;
				case MUTT_NAME_DESIGNER: return "Designer"; break;
				case MUTT_NAME_DESCRIPTION: return "Description"; break;
				case MUTT_NAME_VENDOR_URL: return "URL of vendor"; break;
				case MUTT_NAME_DESIGNER_URL: return "URL of designer"; break;
				case MUTT_NAME_LICENSE_DESCRIPTION: return "License description"; break;
				case MUTT_NAME_LICENSE_INFO_URL: return "License info URL"; break;
				case MUTT_NAME_TYPOGRAPHIC_FAMILY: return "Typographic family name"; break;
				case MUTT_NAME_SAMPLE_TEXT: return "Sample text"; break;
			}
		}

		#endif

	/* Checks */

		// https://stackoverflow.com/a/600306
		#define MUTT_IS_POW_OF_2(v) ((v != 0) && ((v & (v-1)) == 0))

		MUDEF muttResult mu_truetype_check_table_directory(muByte* data, size_m data_size) {
			uint16_m num_tables;
			muByte* orig_data = data;

			/* Table directory */
			{
				uint32_m u32;
				uint16_m u16;

				if (data_size < 12) {
					return MUTT_UNEXPECTED_EOF;
				}

				// sfntVersion
				u32 = mu_rbe_uint32(data);
				data += 4;
				if (u32 != 0x00010000/* && u32 != 0x4F54544F*/) {
					return MUTT_INVALID_TABLE_DIRECTORY_SFNT_VERSION;
				}

				// numTables
				num_tables = mu_rbe_uint16(data);
				data += 2;
				if (num_tables < 9) {
					return MUTT_INVALID_TABLE_DIRECTORY_NUM_TABLES;
				}

				// searchRange
				uint16_m search_range = mu_rbe_uint16(data);
				u16 = search_range;
				data += 2;
				
				if (u16%16 != 0) {
					return MUTT_INVALID_TABLE_DIRECTORY_SEARCH_RANGE;
				}
				u16 /= 16;

				if (!MUTT_IS_POW_OF_2(u16)) {
					return MUTT_INVALID_TABLE_DIRECTORY_SEARCH_RANGE;
				}

				if (u16 > num_tables || u16*2 < num_tables) {
					return MUTT_INVALID_TABLE_DIRECTORY_SEARCH_RANGE;
				}

				// entrySelector
				u16 = mu_rbe_uint16(data);
				data += 2;

				// (undo log2)
				u16 = mu_pow(2, u16);

				if (!((u16 != 0) && ((u16 & (u16-1)) == 0))) {
					return MUTT_INVALID_TABLE_DIRECTORY_ENTRY_SELECTOR;
				}
				if (u16 > num_tables || u16*2 < num_tables) {
					return MUTT_INVALID_TABLE_DIRECTORY_ENTRY_SELECTOR;
				}

				// rangeShift
				u16 = mu_rbe_uint16(data);
				data += 2;

				u16 += search_range;

				if (u16%16 != 0) {
					return MUTT_INVALID_TABLE_DIRECTORY_RANGE_SHIFT;
				}
				if (u16/16 != num_tables) {
					return MUTT_INVALID_TABLE_DIRECTORY_RANGE_SHIFT;
				}

				// tableRecords
				if (data_size < (12 + ((uint32_m)num_tables*16))) {
					return MUTT_UNEXPECTED_EOF;
				}
			}

			/* Table records */
			{
				while (num_tables != 0) {
					num_tables -= 1;

					// tableTag
					uint32_m table_tag = mu_rbe_uint32(data);
					data += 4;

					// checksum
					uint32_m checksum = mu_rbe_uint32(data);
					data += 4;

					// offset
					uint32_m offset = mu_rbe_uint32(data);
					data += 4;
					if (offset >= data_size) {
						return MUTT_INVALID_TABLE_RECORD_OFFSET;
					}

					// length
					uint32_m length = mu_rbe_uint32(data);
					data += 4;
					if (((size_m)offset + (size_m)length) >= data_size) {
						return MUTT_INVALID_TABLE_RECORD_LENGTH;
					}

					// (Don't check head because its checksum is weird)
					if (table_tag != 0x68656164) {
						if (mu_truetype_check_table_checksum(&orig_data[offset], length, checksum) != MUTT_SUCCESS) {
							return MUTT_INVALID_TABLE_RECORD_CHECKSUM;
						}
					}
				}
			}

			return MUTT_SUCCESS;
		}

		#ifndef mu_rle_uint24
			#define mu_rle_uint24(b) ((uint32_m)b[0] << 0 | (uint32_m)b[1] << 8 | (uint32_m)b[2] << 16)
		#endif

		#ifndef mu_rbe_uint24
			#define mu_rbe_uint24(b) ((uint32_m)b[2] << 0 | (uint32_m)b[1] << 8 | (uint32_m)b[0] << 16)
		#endif

		MUDEF muttResult mu_truetype_check_table_checksum(muByte* table, uint32_m length, uint32_m checksum) {
			muByte* end = &table[length];
			uint32_m current_checksum = 0;

			uint32_m off = length % 4;

			while (table < end) {
				if ((table+4) >= end && off != 0) {
					if (off == 2) {
						current_checksum += mu_rbe_uint16(table) << 16;
					} else if (off == 3) {
						current_checksum += mu_rbe_uint24(table) << 8;
					} else {
						current_checksum += table[0] << 24;
					}
				} else {
					current_checksum += mu_rbe_uint32(table);
				}
				table += 4;
			}

			if (checksum != current_checksum) {
				return MUTT_INVALID_TABLE_RECORD_CHECKSUM;
			}

			return MUTT_SUCCESS;
		}

		MUDEF muttResult mu_truetype_check_head(muByte* table, uint32_m table_length) {
			if (table_length != 54) {
				return MUTT_INVALID_HEAD_TABLE_LENGTH;
			}

			// We're not checking the checksum adjustment because of font collection files.
			uint16_m u16;

			// magicNumber
			table += 12;
			if (mu_rbe_uint32(table) != 0x5F0F3CF5) {
				return MUTT_INVALID_HEAD_MAGIC_NUMBER;
			}

			// unitsPerEm
			table += 6;
			u16 = mu_rbe_uint16(table);
			if (u16 < 16 || u16 > 16384) {
				return MUTT_INVALID_HEAD_UNITS_PER_EM;
			}

			// xMin / xMax
			int16_m min, max;

			// xMin
			table += 18;
			u16 = mu_rbe_uint16(table);
			min = *(int16_m*)&u16;

			// xMax
			table += 4;
			u16 = mu_rbe_uint16(table);
			max = *(int16_m*)&u16;

			// Comparison
			if (max < min) { 
				return MUTT_INVALID_HEAD_X_MIN_MAX;
			}

			// yMin / yMax
			
			// yMin
			table -= 2;
			u16 = mu_rbe_uint16(table);
			min = *(int16_m*)&u16;

			// yMax
			table += 4;
			u16 = mu_rbe_uint16(table);
			max = *(int16_m*)&u16;

			// Comparison
			if (max < min) { 
				return MUTT_INVALID_HEAD_Y_MIN_MAX;
			}

			// lowestRecPPEM
			table += 4;
			if (mu_rbe_uint16(table) == 0) {
				return MUTT_INVALID_HEAD_LOWEST_REC_PPEM;
			}

			// indexToLocFormat
			table += 4;
			// (This check also rules out negative values :P)
			if (mu_rbe_uint16(table) > 1) {
				return MUTT_INVALID_HEAD_INDEX_TO_LOC_FORMAT;
			}

			// glyphDataFormat
			table += 2;
			if (mu_rbe_uint16(table) != 0) {
				return MUTT_INVALID_HEAD_GLYPH_DATA_FORMAT;
			}

			return MUTT_SUCCESS;
		}

		MUDEF muttResult mu_truetype_check_maxp(muByte* table, uint32_m table_length) {
			if (table_length != 32) {
				return MUTT_INVALID_MAXP_TABLE_LENGTH;
			}
			uint16_m u16;

			// maxZones
			table += 14;
			u16 = mu_rbe_uint16(table);
			if (u16 != 1 && u16 != 2) {
				return MUTT_INVALID_MAXP_MAX_ZONES;
			}

			return MUTT_SUCCESS;
		}

		MUDEF muttResult mu_truetype_check_hhea(muByte* table, uint32_m table_length) {
			if (table_length != 36) {
				return MUTT_INVALID_HHEA_TABLE_LENGTH;
			}

			// metricDataFormat
			table += 32;
			if (mu_rbe_uint16(table) != 0) {
				return MUTT_INVALID_HHEA_METRIC_DATA_FORMAT;
			}

			return MUTT_SUCCESS;
		}

		MUDEF muttResult mu_truetype_check_names(muByte* table, uint32_m table_length) {
			if (table_length < 6) {
				return MUTT_INVALID_NAME_TABLE_LENGTH;
			}

			muByte* orig_name = table;

			// Version
			uint16_m version = mu_rbe_uint16(table); if (version) {}	
			table += 2;

			// Count
			uint16_m count = mu_rbe_uint16(table);
			table += 2;

			// Storage offset
			uint16_m storage_offset = mu_rbe_uint16(table);
			table += 2;
			if (storage_offset >= table_length) {
				return MUTT_INVALID_NAME_TABLE_LENGTH;
			}

			/* Name records */

			// Check table record length
			if ((table+(12*(uint32_m)count))-orig_name > table_length) {
				return MUTT_INVALID_NAME_TABLE_LENGTH;
			}

			// Loop through each record
			for (uint16_m i = 0; i < count; i++)
			{
				// Length
				table += 8;
				uint16_m length = mu_rbe_uint16(table);
				table += 2;

				// String offset
				uint16_m string_offset = mu_rbe_uint16(table);
				table += 2;

				// Check if the values given are in range
				if ((uint32_m)storage_offset+(uint32_m)string_offset+(uint32_m)length > table_length) {
					return MUTT_INVALID_NAME_TABLE_LENGTH;
				}
			}

			if (version == 0) {
				return MUTT_SUCCESS;
			}

			if ((table-orig_name)+2 > table_length) {
				return MUTT_INVALID_NAME_TABLE_LENGTH;
			}

			// Lang tag count
			uint16_m lang_tag_count = mu_rbe_uint16(table);
			table += 2;

			// Make sure lang tags are in range
			if ((table+(4*(uint32_m)lang_tag_count))-orig_name > table_length) {
				return MUTT_INVALID_NAME_TABLE_LENGTH;
			}

			// Loop through each lang tag
			for (uint16_m i = 0; i < lang_tag_count; i++) {
				// Length
				uint16_m length = mu_rbe_uint16(table);
				table += 2;

				// Lang tag offset
				uint16_m lang_tag_offset = mu_rbe_uint16(table);
				table += 2;

				// Check if the values given are in range
				if ((uint32_m)storage_offset+(uint32_m)lang_tag_offset+(uint32_m)length > table_length) {
					return MUTT_INVALID_NAME_TABLE_LENGTH;
				}
			}

			return MUTT_SUCCESS;
		}

		MUDEF muttResult mu_truetype_check_cmap(muttInfo* info) {
			muByte* table = &info->data[info->req.cmap.offset];
			muByte* orig_table = table;
			uint32_m table_length = info->req.cmap.length;

			// numTables
			if (table_length < 4) {
				return MUTT_INVALID_CMAP_TABLE_LENGTH;
			}

			table += 2;
			uint16_m num_tables = mu_rbe_uint16(table);
			if (table_length < 4 + ((uint32_m)num_tables * 8)) {
				return MUTT_INVALID_CMAP_TABLE_LENGTH;
			}

			// Loop through each encoding record
			table += 2;
			for (uint16_m t = 0; t < num_tables; t++) {
				// subtableOffset
				table += 4;
				uint32_m subtable_offset = mu_rbe_uint32(table);
				if (subtable_offset+8 >= table_length) {
					return MUTT_INVALID_CMAP_TABLE_LENGTH;
				}

				// Get to the subtable
				muByte* subtable = orig_table + subtable_offset;

				// format
				uint16_m format = mu_rbe_uint16(subtable);

				// Skip unspecified formats
				switch (format) {
					default: table += 4; continue; break;
					case 0: case 2: case 4: case 6: case 8:
					case 10: case 12: case 13: case 14: break;
				}

				// Length

				uint32_m length;
				if (format < 8) {
					subtable += 2;
					length = (uint32_m)mu_rbe_uint16(subtable);
					subtable += 2;
				} else if (format < 14) {
					subtable += 4;
					length = mu_rbe_uint32(subtable);
					subtable += 4;
				} else {
					subtable += 2;
					length = mu_rbe_uint32(subtable);
					subtable += 4;
				}

				if (subtable_offset + length > table_length) {
					return MUTT_INVALID_CMAP_TABLE_LENGTH;
				}

				// Verify table

				muttResult res = MUTT_SUCCESS;
				switch (format) {
					case 0: res = mu_truetype_check_format0(info, subtable, length); break;
					case 4: res = mu_truetype_check_format4(info, subtable, length); break;
				}

				if (res != MUTT_SUCCESS) {
					return res;
				}

				table += 4;
			}

			return MUTT_SUCCESS;
		}

		MUDEF muttResult mu_truetype_check_format0(muttInfo* info, muByte* table, uint16_m length) {
			if (length != 262) {
				return MUTT_INVALID_CMAP_F0_TABLE_LENGTH;
			}

			// Loop through each element in glyphIdArray
			table += 2;
			for (uint16_m i = 0; i < 256; i++) {
				// Make sure it's within the valid glyph value range
				if (table[0] >= info->maxp_info.num_glyphs) {
					return MUTT_INVALID_CMAP_F0_GLYPH_ID;
				}

				table += 1;
			}

			return MUTT_SUCCESS;
		}

		MUDEF muttResult mu_truetype_check_format4(muttInfo* info, muByte* table, uint16_m length) {
			if (length < 16) {
				return MUTT_INVALID_CMAP_F4_TABLE_LENGTH;
			}
			uint16_m u16;

			// segCountX2
			table += 2;
			uint16_m seg_count_x2 = mu_rbe_uint16(table);
			if (seg_count_x2 % 2 != 0) {
				return MUTT_INVALID_CMAP_F4_SEG_COUNT;
			}
			uint16_m seg_count = seg_count_x2 / 2;

			// glyphIdArray length calculations

			uint16_m non_id_length = 16 + ((uint32_m)seg_count * 8);
			if (length < non_id_length) {
				return MUTT_INVALID_CMAP_F4_TABLE_LENGTH;
			}

			uint16_m glyph_id_array_len = length - non_id_length;
			if (glyph_id_array_len % 2 != 0) {
				return MUTT_INVALID_CMAP_F4_TABLE_LENGTH;
			}
			glyph_id_array_len /= 2;

			// searchRange
			table += 2;
			uint16_m search_range = mu_rbe_uint16(table);
			if (search_range % 2 != 0) {
				return MUTT_INVALID_CMAP_F4_SEARCH_RANGE;
			}
			search_range /= 2;

			if (!MUTT_IS_POW_OF_2(search_range) || search_range > seg_count || search_range*2 < seg_count) {
				return MUTT_INVALID_CMAP_F4_SEARCH_RANGE;
			}

			// entrySelector
			table += 2;
			u16 = mu_rbe_uint16(table);

			if (mu_pow(2, u16) != search_range) {
				return MUTT_INVALID_CMAP_F4_ENTRY_SELECTOR;
			}

			// rangeShift
			table += 2;
			u16 = mu_rbe_uint16(table);

			if (u16 != seg_count_x2-(search_range*2)) {
				return MUTT_INVALID_CMAP_F4_RANGE_SHIFT;
			}

			// Segments
			table += 2;

			uint16_m prev_end_code;
			for (uint16_m i = 0; i < seg_count; i++) {

				// End code
				uint16_m end_code = mu_rbe_uint16(table);

				// Check for incremental end code
				if (i != 0) {
					if (end_code <= prev_end_code) {
						return MUTT_INVALID_CMAP_F4_END_CODE;
					}
				}

				// Start code
				uint16_m start_code = mu_rbe_uint16((table+seg_count_x2+2));

				// Check for valid startCode and endCode range
				if (end_code < start_code) {
					return MUTT_INVALID_CMAP_F4_START_CODE;
				}

				// Check for last 0xFFFF startCode and endCode value
				if (i+1 >= seg_count && (start_code != 0xFFFF || end_code != 0xFFFF)) {
					return MUTT_INVALID_CMAP_F4_END_CODE;
				}

				// idDelta (tested later)
				u16 = mu_rbe_uint16((table+(seg_count*4)+2));
				int16_m id_delta = *(int16_m*)&u16;

				// idRangeOffset
				muByte* pid_range_offset = table+(seg_count*6)+2;
				uint16_m id_range_offset = mu_rbe_uint16(pid_range_offset);
				if (id_range_offset%2 != 0) {
					// This technically isn't INVALID, but like... ???
					return MUTT_INVALID_CMAP_F4_END_CODE;
				}
				id_range_offset /= 2;

				if (id_range_offset != 0 && i+1 < seg_count) {
					// Make sure the offset will result in an index within glyphIdArray
					if (id_range_offset < (seg_count-i)) {
						return MUTT_INVALID_CMAP_F4_ID_RANGE_OFFSET;
					}
					if (id_range_offset - (seg_count-i) + (end_code-start_code) >= glyph_id_array_len) {
						return MUTT_INVALID_CMAP_F4_ID_RANGE_OFFSET;
					}

					// Verify possible glyph IDs

					muByte* pstart_c = pid_range_offset + (id_range_offset*2);
					uint16_m code_count = (end_code-start_code)+1;
					
					for (uint16_m c = 0; c < code_count; c++) {
						uint16_m glyph_id = mu_rbe_uint16(pstart_c);

						if (glyph_id != 0) {
							glyph_id = mu_truetype_id_delta(glyph_id, id_delta);
							if (glyph_id >= info->maxp_info.num_glyphs) {
								return MUTT_INVALID_CMAP_F4_ID_DELTA;
							}
						}

						pstart_c += 2;
					}
				} else {
					// Verify max possible glyph ID

					uint16_m max_glyph_id = mu_truetype_id_delta(end_code, id_delta);
					if (max_glyph_id >= info->maxp_info.num_glyphs) {
						return MUTT_INVALID_CMAP_F4_ID_DELTA;
					}
				}

				table += 2;
				prev_end_code = end_code;
			}

			return MUTT_SUCCESS;
		}

		/*MUDEF muttResult mu_truetype_check_format12(muttInfo* info, muByte* table, uint16_m length) {
			if (length < 16) {
				return MUTT_INVALID_CMAP_F12_TABLE_LENGTH;
			}

			return MUTT_SUCCESS;
		}*/

	/* Get/Let info */

		MUDEF muttInfo mu_truetype_get_info(muttResult* result, muByte* data, size_m size) {
			muttResult res;

			res = mu_truetype_check_table_directory(data, size);
			if (res != MUTT_SUCCESS) {
				MU_SET_RESULT(result, res)
				return MU_ZERO_STRUCT(muttInfo);
			}

			muttInfo info = MU_ZERO_STRUCT(muttInfo);

			info.data = data;
			info.size = size;

			size_m p = 0;

			/* Offset subtable */
			{
				p += 4;
				info.table_count = mu_rbe_uint16((&data[p]));
				p += 8;
			}

			/* Table directories */
			{
				for (uint16_m i = 0; i < info.table_count; i++) {
					// tag
					uint32_m u32 = mu_rbe_uint32((&data[p]));

					switch (u32) {
						default: break;
						case 0x636D6170: {
							info.req.cmap.offset = mu_rbe_uint32((&data[p+8]));
							info.req.cmap.length = mu_rbe_uint32((&data[p+12]));
						} break;
						case 0x676C7966: {
							info.req.glyf.offset = mu_rbe_uint32((&data[p+8]));
							info.req.glyf.length = mu_rbe_uint32((&data[p+12]));
						} break;
						case 0x68656164: {
							info.req.head.offset = mu_rbe_uint32((&data[p+8]));
							info.req.head.length = mu_rbe_uint32((&data[p+12]));
						} break;
						case 0x68686561: {
							info.req.hhea.offset = mu_rbe_uint32((&data[p+8]));
							info.req.hhea.length = mu_rbe_uint32((&data[p+12]));
						} break;
						case 0x686D7478: {
							info.req.hmtx.offset = mu_rbe_uint32((&data[p+8]));
							info.req.hmtx.length = mu_rbe_uint32((&data[p+12]));
						} break;
						case 0x6C6F6361: {
							info.req.loca.offset = mu_rbe_uint32((&data[p+8]));
							info.req.loca.length = mu_rbe_uint32((&data[p+12]));
						} break;
						case 0x6D617870: {
							info.req.maxp.offset = mu_rbe_uint32((&data[p+8]));
							info.req.maxp.length = mu_rbe_uint32((&data[p+12]));
						} break;
						case 0x6E616D65: {
							info.req.name.offset = mu_rbe_uint32((&data[p+8]));
							info.req.name.length = mu_rbe_uint32((&data[p+12]));
						} break;
						case 0x706F7374: {
							info.req.post.offset = mu_rbe_uint32((&data[p+8]));
							info.req.post.length = mu_rbe_uint32((&data[p+12]));
						} break;
					}

					p += 16;
				}
			}

			// (Check if all required tables have been found)
			if (info.req.cmap.offset == 0 ||
				info.req.glyf.offset == 0 ||
				info.req.head.offset == 0 ||
				info.req.hhea.offset == 0 ||
				info.req.hmtx.offset == 0 ||
				info.req.loca.offset == 0 ||
				info.req.maxp.offset == 0 ||
				info.req.name.offset == 0 ||
				info.req.post.offset == 0
			) {
				MU_SET_RESULT(result, MUTT_MISSING_REQUIRED_TABLE)
				mu_truetype_let_info(&info);
				return MU_ZERO_STRUCT(muttInfo);
			}

			/* Checks */

			res = mu_truetype_check_head(&data[info.req.head.offset], info.req.head.length);
			if (res != MUTT_SUCCESS) {
				MU_SET_RESULT(result, res)
				mu_truetype_let_info(&info);
				return MU_ZERO_STRUCT(muttInfo);
			}

			/* Head info */
			{
				muByte* head = &data[info.req.head.offset];
				uint16_m u16;
				uint64_m u64;

				/*
				0:  uint16 majorVersion
				2:  uint16 minorVersion
				4:  int16  fontRevision.high
				6:  int16  fontRevision.low
				8:  uint32 checksumAdjustment
				12: uint32 magicNumber
				16: uint16 flags
				18: uint16 unitsPerEm
				20: int64  created
				28: int64  modified
				36: int16  xMin
				38: int16  yMin
				40: int16  xMax
				42: int16  yMax
				44: uint16 macStyle
				46: uint16 lowestRecPPEM
				48: int16  fontDirectionHint
				50: int16  indexToLocFormat
				52: int16  glyphDataFormat*/

				info.head_info.major_version = mu_rbe_uint16((&head[0]));
				info.head_info.minor_version = mu_rbe_uint16((&head[2]));
				u16 = mu_rbe_uint16((&head[4])); info.head_info.font_revision_high = *(int16_m*)&u16;
				u16 = mu_rbe_uint16((&head[6])); info.head_info.font_revision_low = *(int16_m*)&u16;
				info.head_info.checksum_adjustment = mu_rbe_uint32((&head[8]));
				info.head_info.magic_number = mu_rbe_uint32((&head[12]));
				info.head_info.flags = mu_rbe_uint16((&head[16]));
				info.head_info.units_per_em = mu_rbe_uint16((&head[18]));
				u64 = mu_rbe_uint64((&head[20])); info.head_info.created = *(int64_m*)&u64;
				u64 = mu_rbe_uint64((&head[28])); info.head_info.modified = *(int64_m*)&u64;
				u16 = mu_rbe_uint16((&head[36])); info.head_info.x_min = *(int16_m*)&u16;
				u16 = mu_rbe_uint16((&head[38])); info.head_info.y_min = *(int16_m*)&u16;
				u16 = mu_rbe_uint16((&head[40])); info.head_info.x_max = *(int16_m*)&u16;
				u16 = mu_rbe_uint16((&head[42])); info.head_info.y_max = *(int16_m*)&u16;
				info.head_info.mac_style = mu_rbe_uint16((&head[44]));
				info.head_info.lowest_rec_ppem = mu_rbe_uint16((&head[46]));
				u16 = mu_rbe_uint16((&head[48])); info.head_info.font_direction_hint = *(int16_m*)&u16;
				u16 = mu_rbe_uint16((&head[50])); info.head_info.index_to_loc_format = *(int16_m*)&u16;
				u16 = mu_rbe_uint16((&head[52])); info.head_info.glyph_data_format = *(int16_m*)&u16;
			}

			res = mu_truetype_check_maxp(&data[info.req.maxp.offset], info.req.maxp.length);
			if (res != MUTT_SUCCESS) {
				MU_SET_RESULT(result, res)
				mu_truetype_let_info(&info);
				return MU_ZERO_STRUCT(muttInfo);
			}

			/* Maxp info */
			{
				muByte* maxp = &data[info.req.maxp.offset];

				/*
				0:  uint32 version
				4:  uint16 numGlyphs
				6:  uint16 maxPoints
				8:  uint16 maxContours
				10: uint16 maxCompositePoints
				12: uint16 maxCompositeContours
				14: uint16 maxZones
				16: uint16 maxTwilightPoints
				18: uint16 maxStorage
				20: uint16 maxFunctionDefs
				22: uint16 maxInstructionDefs
				24: uint16 maxStackElements
				26: uint16 maxSizeOfInstructions
				28: uint16 maxComponentElements
				30: uint16 maxComponentDepth*/

				info.maxp_info.version_high = mu_rbe_uint16((&maxp[0]));
				info.maxp_info.version_low = mu_rbe_uint16((&maxp[2]));
				info.maxp_info.num_glyphs = mu_rbe_uint16((&maxp[4]));
				info.maxp_info.max_points = mu_rbe_uint16((&maxp[6]));
				info.maxp_info.max_contours = mu_rbe_uint16((&maxp[8]));
				info.maxp_info.max_composite_points = mu_rbe_uint16((&maxp[10]));
				info.maxp_info.max_composite_contours = mu_rbe_uint16((&maxp[12]));
				info.maxp_info.max_zones = mu_rbe_uint16((&maxp[14]));
				info.maxp_info.max_twilight_points = mu_rbe_uint16((&maxp[16]));
				info.maxp_info.max_storage = mu_rbe_uint16((&maxp[18]));
				info.maxp_info.max_function_defs = mu_rbe_uint16((&maxp[20]));
				info.maxp_info.max_instruction_defs = mu_rbe_uint16((&maxp[22]));
				info.maxp_info.max_stack_elements = mu_rbe_uint16((&maxp[24]));
				info.maxp_info.max_size_of_instructions = mu_rbe_uint16((&maxp[26]));
				info.maxp_info.max_component_elements = mu_rbe_uint16((&maxp[28]));
				info.maxp_info.max_component_depth = mu_rbe_uint16((&maxp[30]));
			}

			res = mu_truetype_check_hhea(&data[info.req.hhea.offset], info.req.hhea.length);
			if (res != MUTT_SUCCESS) {
				MU_SET_RESULT(result, res)
				mu_truetype_let_info(&info);
				return MU_ZERO_STRUCT(muttInfo);
			}

			/* Hhea info */
			{
				muByte* hhea = &data[info.req.hhea.offset];
				uint16_m u16;

				/*
				0:  uint16 major_version;
				2:  uint16 minor_version;
				4:  int16  ascender;
				6:  int16  descender;
				8:  int16  line_gap;
				10: uint16 advance_max_width;
				12: int16  min_left_side_bearing;
				14: int16  min_right_side_bearing;
				16: int16  x_max_extent;
				18: int16  caret_slope_rise;
				20: int16  caret_slope_run;
				22: int16  caret_offset;
				32: int16  metric_data_format;
				34: uint16 number_of_hmetrics;*/

				info.hhea_info.major_version = mu_rbe_uint16((&hhea[0]));
				info.hhea_info.minor_version = mu_rbe_uint16((&hhea[2]));
				u16 = mu_rbe_uint16((&hhea[4])); info.hhea_info.ascender = *(int16_m*)&u16;
				u16 = mu_rbe_uint16((&hhea[6])); info.hhea_info.descender = *(int16_m*)&u16;
				u16 = mu_rbe_uint16((&hhea[8])); info.hhea_info.line_gap = *(int16_m*)&u16;
				info.hhea_info.advance_max_width = mu_rbe_uint16((&hhea[10]));
				u16 = mu_rbe_uint16((&hhea[12])); info.hhea_info.min_left_side_bearing = *(int16_m*)&u16;
				u16 = mu_rbe_uint16((&hhea[14])); info.hhea_info.min_right_side_bearing = *(int16_m*)&u16;
				u16 = mu_rbe_uint16((&hhea[16])); info.hhea_info.x_max_extent = *(int16_m*)&u16;
				u16 = mu_rbe_uint16((&hhea[18])); info.hhea_info.caret_slope_rise = *(int16_m*)&u16;
				u16 = mu_rbe_uint16((&hhea[20])); info.hhea_info.caret_slope_run = *(int16_m*)&u16;
				u16 = mu_rbe_uint16((&hhea[22])); info.hhea_info.caret_offset = *(int16_m*)&u16;
				u16 = mu_rbe_uint16((&hhea[32])); info.hhea_info.metric_data_format = *(int16_m*)&u16;
				info.hhea_info.number_of_hmetrics = mu_rbe_uint16((&hhea[34]));
			}

			res = mu_truetype_check_names(&data[info.req.name.offset], info.req.name.length);
			if (res != MUTT_SUCCESS) {
				MU_SET_RESULT(result, res)
				mu_truetype_let_info(&info);
				return MU_ZERO_STRUCT(muttInfo);
			}

			res = mu_truetype_check_cmap(&info);
			if (res != MUTT_SUCCESS) {
				MU_SET_RESULT(result, res)
				mu_truetype_let_info(&info);
				return MU_ZERO_STRUCT(muttInfo);
			}

			return info;
		}

		MUDEF void mu_truetype_let_info(muttInfo* info) {
			return; if (info) {}
		}

	/* Tables */

		MUDEF uint16_m mu_truetype_get_table_count(muttInfo* info) {
			return info->table_count;
		}

		MUDEF void mu_truetype_get_table(muttInfo* info, uint16_m table, muttTable* table_info, char* name) {
			muByte* table_data = &info->data[12+(16*(uint32_m)table)];

			if (table_info) {
				table_info->offset = mu_rbe_uint32((&table_data[8]));
				table_info->length = mu_rbe_uint32((&table_data[12]));
			}

			if (name) {
				mu_memcpy(name, table_data, 4);
			}
		}

	/* Name table */

		MUDEF uint16_m mu_truetype_get_name_ids(muttInfo* info, uint16_m* ids) {
			muByte* name = &info->data[info->req.name.offset];

			// Give count if that's all we need
			uint16_m count = mu_rbe_uint16((&name[2]));
			if (!ids) {
				return count;
			}

			// Go through all NameRecords and get nameID.
			muByte* offsets = name + 6;
			for (uint16_m i = 0; i < count; i++) {
				ids[i] = mu_rbe_uint16((&offsets[6]));
				offsets += 12;
			}

			return count;
		}

		MUDEF uint16_m mu_truetype_get_name_id(muttInfo* info, uint16_m index) {
			/*muByte* name = &info->data[info->req.name.offset];
			muByte* offsets = name + 6;
			muByte* index_offset = offsets + (12*(uint32_m)index);
			return mu_rbe_uint16((&index_offset[6]));*/
			// Into: (lol)
			return mu_rbe_uint16((&(&info->data[info->req.name.offset] + 6 + (12*(uint32_m)index))[6]));
		}

		MUDEF char* mu_truetype_get_name(muttInfo* info, uint16_m name_id_index, muttEncoding* encoding, uint16_m* length) {
			muByte* name = &info->data[info->req.name.offset];
			// Get to the respective NameRecord
			muByte* name_record = name + 6 + (name_id_index*12);

			// Record encoding
			if (encoding != 0) {
				muttEncoding enc = {
					(uint16_m)mu_rbe_uint16(name_record),
					(uint16_m)mu_rbe_uint16((&name_record[2]))
				};
				*encoding = enc;
			}

			// Record length
			if (length != 0) {
				*length = mu_rbe_uint16((&name_record[8]));
			}

			// Return pointer to name
			uint16_m storage_offset = mu_rbe_uint16((name+4));
			uint16_m string_offset = mu_rbe_uint16((name_record+10));
			return (char*)(name+storage_offset+string_offset);
		}

	/* Loca table */

		MUDEF muByte* mu_truetype_get_glyf_table(muttInfo* info, muttGlyphID id, uint32_m* length) {
			muByte* loca = &info->data[info->req.loca.offset];
			muByte* glyf = &info->data[info->req.glyf.offset];

			// Offset16 handling
			if (info->head_info.index_to_loc_format == 0) {
				uint16_m offset = mu_rbe_uint16((&loca[id*sizeof(uint16_m)]));
				muByte* ptr = &glyf[((uint32_m)offset)*2];
				if (length) {
					*length = (uint32_m)(mu_truetype_get_glyf_table(info, id+1, 0) - ptr);
				}
				return ptr;
			}
			// Offset32 handling
			else {
				uint32_m offset = mu_rbe_uint32((&loca[id*sizeof(uint32_m)]));
				muByte* ptr = &glyf[offset];
				if (length) {
					*length = (uint32_m)(mu_truetype_get_glyf_table(info, id+1, 0) - ptr);
				}
				return ptr;
			}
		}

	/* Cmap table */

		MUDEF void mu_truetype_get_encoding_record(muttInfo* info, muttEncodingRecord* record, uint16_m id) {
			// Calculate pointer to the encoding record
			muByte* ptr = &info->data[info->req.cmap.offset+4+((uint32_m)(id)*8)];

			// Read data
			record->encoding.platform_id = mu_rbe_uint16((ptr));
			record->encoding.encoding_id = mu_rbe_uint16((&ptr[2]));
			record->subtable = &info->data[info->req.cmap.offset+mu_rbe_uint32((&ptr[4]))];
			record->format = mu_rbe_uint16((record->subtable));

			// Get language
			if (record->format >= 8 && record->format < 14) {
				record->language = mu_rbe_uint32((&record->subtable[8]));
			} else if (record->format < 8) {
				record->language = (uint32_m)mu_rbe_uint16((&record->subtable[4]));
			} else {
				record->language = 0;
			}
		}

		MUDEF uint16_m mu_truetype_get_encoding_records(muttInfo* info, muttEncodingRecord* records) {
			uint16_m rec = mu_rbe_uint16((&info->data[info->req.cmap.offset+2]));
			if (!records) {
				return rec;
			}

			for (uint16_m id = 0; id < rec; id += 1) {
				mu_truetype_get_encoding_record(info, &records[id], id);
			}
			return rec;
		}

		/* Format 4 */

			MUDEF void mu_truetype_get_format_4(muByte* subtable, muttFormat4* format) {
				/*
				0: uint16 format
				2: uint16 length
				4: uint16 language
				6: uint16 seg_count_x2 // uint16 seg_count=seg_count_x2/2
				8: uint16 search_range
				10: uint16 entry_selector
				12: uint16 range_shift
				14: uint16 end_code[seg_count]
				// 14+seg_count_x2: uint16 reserved_pad
				16+seg_count_x2: uint16 start_code[seg_count]
				16+(2*seg_count_x2): int16 id_delta[seg_count]
				16+(3*seg_count_x2): uint16 id_range_offset[seg_count]
				16+(4*seg_count_x2): uint16 glyph_id_array[]*/
				format->table = subtable;
				format->length = mu_rbe_uint16((&subtable[2]));
				format->language = mu_rbe_uint16((&subtable[4]));
				format->seg_count_x2 = mu_rbe_uint16((&subtable[6]));
				format->seg_count = format->seg_count_x2 / 2;
				format->search_range = mu_rbe_uint16((&subtable[8]));
				format->entry_selector = mu_rbe_uint16((&subtable[10]));
				format->range_shift = mu_rbe_uint16((&subtable[12]));
				format->end_code = &subtable[14];
				format->start_code = &subtable[16+format->seg_count_x2];
				format->id_delta = &subtable[16+(2*format->seg_count_x2)];
				format->id_range_offset = &subtable[16+(3*format->seg_count_x2)];
				format->glyph_id_array = &subtable[16+(4*format->seg_count_x2)];
				format->glyph_id_length = format->length - (16+(4*format->seg_count_x2));
			}

			MUDEF muttGlyphID mu_truetype_get_glyph_format_4(muttFormat4* format, uint16_m character_code) {
				// Go through each segment:
				for (uint16_m seg = 0; seg < format->seg_count; seg++) {
					// Calculate start and end values
					uint16_m seg_offset = (uint16_m)(seg*2);
					uint16_m start = mu_rbe_uint16((format->start_code+seg_offset));
					uint16_m end = mu_rbe_uint16((format->end_code+seg_offset));

					// I thought that we could exit early, but although endCode is defined as being
					// incremental, startCode isn't. :L

					// See if the code lies within the range
					if (character_code >= start && character_code <= end) {
						// Get idRangeOffset and delta
						uint16_m range_offset = mu_rbe_uint16((format->id_range_offset+seg_offset));
						uint16_m u16 = mu_rbe_uint16((format->id_delta+seg_offset));
						int16_m delta = *(int16_m*)&u16;

						// If range offset is 0, delta logic and we're done
						if (range_offset == 0) {
							return mu_truetype_id_delta(character_code, delta);
						}

						// Offset into glyphIdArray, in bytes (in effect by the *2 at the end;
						// inside parentheses is calculated like glyphIdArray[...])
						uint16_m id = mu_rbe_uint16((&format->glyph_id_array[(uint16_m)(
							// Range offset (divided by 2 to compensate for glyphIdArray[...])
							(range_offset/2)
							// Compensate for range offset being relative to &rangeOffset[i]:
							- (format->seg_count-seg)
							// Offset character index relative to range
							+ (character_code - start)
						)*2]));

						// Return 0 if it's 0; no delta logic
						if (id == 0) {
							return 0;
						}

						// Return delta logic
						return mu_truetype_id_delta(id, delta);
					}
				}

				return 0;
			}

	/* Glyf table */

		MUDEF void mu_truetype_get_glyph_header(muByte* table, muttGlyphHeader* header) {
			uint16_m u16;
			header->data = table;
			u16 = mu_rbe_uint16((&table[0])); header->number_of_contours = *(int16_m*)&u16;
			u16 = mu_rbe_uint16((&table[2])); header->x_min = *(int16_m*)&u16;
			u16 = mu_rbe_uint16((&table[4])); header->y_min = *(int16_m*)&u16;
			u16 = mu_rbe_uint16((&table[6])); header->x_max = *(int16_m*)&u16;
			u16 = mu_rbe_uint16((&table[8])); header->y_max = *(int16_m*)&u16;
		}

		MUDEF uint16_m mu_truetype_get_simple_glyph(muttGlyphHeader* header, muttSimpleGlyph* glyph) {
			muByte* data;
			uint16_m p; // (point count tracker)

			// Calculate amount of points
			data = &header->data[10]; // (to endPtsOfContours)
			uint16_m num_of_points = mu_rbe_uint16((&data[(uint32_m)(header->number_of_contours-1) * (uint32_m)(2)])); // (endPtsOfContours[numberOfContours-1]+1)
			num_of_points += 1;

			// Return points if needed
			if (!glyph) {
				return num_of_points;
			}

			/* end_pts_of_contours */

			glyph->end_pts_of_contours = data;
			data += (uint32_m)(header->number_of_contours)*(uint32_m)(2);

			/* instruction_length */

			glyph->instruction_length = mu_rbe_uint16(data);
			data += 2;

			/* instructions */

			glyph->instructions = (uint8_m*)data;
			data += glyph->instruction_length;

			/* Flags */

			if (glyph->flags == 0) {
				return 0;
			}

			for (p = 0; p < num_of_points; p++) {
				glyph->flags[p] = (uint8_m)data[0];

				if (glyph->flags[p] & MUTT_REPEAT_FLAG) {
					uint8_m rep_count = (uint8_m)data[1];

					for (uint8_m i = 0; i < rep_count; i++) {
						glyph->flags[p+i+1] = glyph->flags[p];
						data += 1;
					}

					p += rep_count;
				}

				data += 1;
			}

			/* X-coordinates */

			uint8_m flag;
			uint8_m size;
			uint16_m u16;

			for (p = 0; p < num_of_points; p++) {
				flag = glyph->flags[p];

				// Get coordinate value alone

				if (flag & MUTT_X_SHORT_VECTOR) {
					// 1 byte long
					size = 1;
					if (flag & MUTT_X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR) {
						// Positive
						glyph->x_coordinates[p] = (int16_m)(mu_rbe_uint8(data));
					} else {
						// Negative
						glyph->x_coordinates[p] = -((int16_m)(mu_rbe_uint8(data)));
					}
				} else {
					if (flag & MUTT_X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR) {
						// Duplicate from before
						if (p > 0) {
							glyph->x_coordinates[p] = glyph->x_coordinates[p-1];
						}
						continue; // (no further interpretation or moving needed)
					} else {
						// 2 bytes long, signed
						size = 2;
						u16 = mu_rbe_uint16(data);
						glyph->x_coordinates[p] = *(int16_m*)&u16;
					}
				}

				// Fix coordinate value
				if (p != 0) {
					glyph->x_coordinates[p] += glyph->x_coordinates[p-1];
				}

				data += size;
			}

			/* Y-coordinates */

			for (p = 0; p < num_of_points; p++) {
				flag = glyph->flags[p];

				// Get coordinate value alone

				if (flag & MUTT_Y_SHORT_VECTOR) {
					// 1 byte long
					size = 1;
					if (flag & MUTT_Y_IS_SAME_OR_POSITIVE_Y_SHORT_VECTOR) {
						// Positive
						glyph->y_coordinates[p] = (int16_m)(mu_rbe_uint8(data));
					} else {
						// Negative
						glyph->y_coordinates[p] = -((int16_m)(mu_rbe_uint8(data)));
					}
				} else {
					if (flag & MUTT_Y_IS_SAME_OR_POSITIVE_Y_SHORT_VECTOR) {
						// Duplicate from before
						if (p > 0) {
							glyph->y_coordinates[p] = glyph->y_coordinates[p-1];
						}
						continue; // (no further interpretation or moving needed)
					} else {
						// 2 bytes long, signed
						size = 2;
						u16 = mu_rbe_uint16(data);
						glyph->y_coordinates[p] = *(int16_m*)&u16;
					}
				}

				// Fix coordinate value
				if (p != 0) {
					glyph->y_coordinates[p] += glyph->y_coordinates[p-1];
				}

				data += size;
			}

			return 0;
		}

		MUDEF uint16_m mu_truetype_get_contour_end_pt(muttSimpleGlyph* glyph, uint16_m contour) {
			return mu_rbe_uint16((&glyph->end_pts_of_contours[(uint32_m)(contour)*(uint32_m)(2)]));
		}

		MUDEF void mu_truetype_get_composite_record(muttGlyphHeader* header, muttCompositeRecord* record, muttCompositeRecord* prev_record) {
			// Get byte position
			muByte* b;
			if (prev_record) {
				b = prev_record->next;
			} else {
				b = &header->data[10];
			}

			// Flags
			uint16_m flags;
			record->flags = mu_rbe_uint16(b);
			flags = record->flags;
			b += 2;

			// glyphIndex
			record->glyph_index = mu_rbe_uint16(b);
			b += 2;

			// argument1 and argument2
			if (flags & MUTT_ARG_1_AND_2_ARE_WORDS) {
				// 16-bit
				uint16_m u16;
				if (flags & MUTT_ARGS_ARE_XY_VALUES) {
					// signed
					u16 = mu_rbe_uint16(b);
					(*(int16_m*)record->argument1) = *(int16_m*)&u16;
					b += 2;

					u16 = mu_rbe_uint16(b);
					(*(int16_m*)record->argument2) = *(int16_m*)&u16;
					b += 2;
				} else {
					// unsigned
					(*(uint16_m*)record->argument1) = mu_rbe_uint16(b);
					b += 2;

					(*(uint16_m*)record->argument2) = mu_rbe_uint16(b);
					b += 2;
				}
			} else {
				// 8-bit
				if (flags & MUTT_ARGS_ARE_XY_VALUES) {
					// signed
					record->argument1[0] = *(int8_m*)b;
					b += 1;

					record->argument2[0] = *(int8_m*)b;
					b += 1;
				} else {
					// unsigned
					record->argument1[0] = *(uint8_m*)b;
					b += 1;
					
					record->argument2[0] = *(uint8_m*)b;
					b += 1;
				}
			}

			// Scales
			if (flags & MUTT_WE_HAVE_A_SCALE) {
				// Single scale
				record->scales[0] = MUTT_F2DOT14(b);
				b += 2;
			} else if (flags & MUTT_WE_HAVE_AN_X_AND_Y_SCALE) {
				// X and Y scale
				record->scales[0] = MUTT_F2DOT14(b);
				b += 2;
				record->scales[1] = MUTT_F2DOT14(b);
				b += 2;
			} else if (flags & MUTT_WE_HAVE_A_TWO_BY_TWO) {
				// 2x2 affine
				uint8_m i = 0;
				do {
					record->scales[i] = MUTT_F2DOT14(b);
					b += 2;
				} while (i <= 4);
			}

			// (we have to set this even if its the last, for instructions)
			record->next = b;
		}

		MUDEF void mu_truetype_get_composite_instructions(muttCompositeRecord* last_record, muttCompositeInstructions* instructions) {
			instructions->num_instr = mu_rbe_uint16(last_record->next);
			instructions->instr = &last_record->next[2];
		}

	/* Misc. */

		MUDEF uint16_m mu_truetype_id_delta(uint16_m character_code, int16_m delta) {
			if (delta == 0) {
				return character_code;
			}

			int32_m big_id = (int32_m)(character_code + delta) % 65536;
			if (big_id < 0) {
				big_id += 65536;
			}
			return (uint16_m)big_id;
		}

	#ifdef __cplusplus
	}
	#endif
#endif /* MUTT_IMPLEMENTATION */

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

