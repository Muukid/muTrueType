/*
muTrueType.h
Public domain single-file C library for reading and rendering TrueType data.
No warranty implied; use at your own risk.

Licensed under MIT License or public domain, whichever you prefer.
More explicit license information at the end of file.

@TODO Allow access to table information.
*/

/* @DOCBEGIN

# muTrueType v1.0.0

muTrueType (abbreviated to 'mutt') is a public domain header-only single-file C library for retrieving data from the TrueType file format and rendering it to a bitmap via several rendering methods. To use it, download the `muTrueType.h` file, add it to your include path, and include it like so:

```c
#define MUTT_IMPLEMENTATION
#include "muTrueType.h"
```

More information about the general structure of a mu library is provided at [the mu library information GitHub repository.](https://github.com/Muukid/mu-library-information)

# Demos

Demos that quickly show the gist of the library and how it works are available in the `demos` folder.

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
		)

		// @DOCLINE ## Result name function

			#ifdef MUTT_NAMES

			// @DOCLINE The function `mutt_result_get_name` converts a `muttResult` value into a `const char*` representation, defined below: @NLNT
			MUDEF const char* mutt_result_get_name(muttResult result);

			// @DOCLINE Note that this function is only defined if `MUTT_NAMES` is defined before the inclusion of the header file.

			// @DOCLINE This function returns `"MUTT_UNKNOWN"` if a respective name could not be found.

			#endif

	// @DOCLINE # TrueType information

		// @DOCLINE There are several structs used to give an overview of a TrueType font. This section covers those structs.

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
				// @DOCLINE * `gylf`: the "glyf" table, defined below: @NLNT
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

		// @DOCLINE ## General gylph information

			// @DOCLINE The struct `muttGeneralGylphInfo` represents general information about the glyphs that are defined by a TrueType font. It has the following members:

			struct muttGeneralGlyphInfo {
				// @DOCLINE * `glyph_count`: the "number of glyphs in the font", defined below: @NLNT
				uint16_m glyph_count;
				// @DOCLINE * `max_points`: the "maximum [number of] points in a non-composite glyph", defined below: @NLNT
				uint16_m max_points;
				// @DOCLINE * `max_contours`: the "maximum [number of] contours in a non-composite glyph", defined below: @NLNT
				uint16_m max_contours;
				// @DOCLINE * `max_composite_points`: the "maximum [number of] points in a composite glyph", defined below: @NLNT
				uint16_m max_composite_points;
				// @DOCLINE * `max_composite_contours`: the "maximum [number of] contours in a composite glyph", defined below: @NLNT
				uint16_m max_composite_contours;
				// @DOCLINE * `head_flags`: flags for the head table: @NLNT
				uint16_m head_flags;
				// @DOCLINE * `units_per_em`: the units per em, defined below: @NLNT
				uint16_m units_per_em;
				// @DOCLINE * `x_min`: minimum x-coordinate for all defined glyph bounding boxes, defined below: @NLNT
				int16_m x_min;
				// @DOCLINE * `y_min`: minimum y-coordinate for all defined glyph bounding boxes, defined below: @NLNT
				int16_m y_min;
				// @DOCLINE * `x_max`: maximum x-coordinate for all defined glyph bounding boxes, defined below: @NLNT
				int16_m x_max;
				// @DOCLINE * `y_max`: maximum y-coordinate for all defined glyph bounding boxes, defined below: @NLNT
				int16_m y_max;
				// @DOCLINE * `mac_style`: corresponds to macStyle in the "head" table, defined below: @NLNT
				uint16_m mac_style;
				// @DOCLINE * `lowest_rec_ppem`: "smallest readable size in pixels", defined below: @NLNT
				uint16_m lowest_rec_ppem;
			};
			typedef struct muttGeneralGlyphInfo muttGeneralGlyphInfo;

			// @DOCLINE Most of this information is based off of the "head" table and the "maxp" table.

			// @DOCLINE ### Mac style macros

				// @DOCLINE To make interacting with `muttGeneralGlyphInfo.mac_style` easier, mutt defines several macros to check for the bits within its given value. Said values are defined below:

				// @DOCLINE * `MUTT_MAC_STYLE_BOLD`: bit 0; "Bold (if set to 1)".
				#define MUTT_MAC_STYLE_BOLD 0
				// @DOCLINE * `MUTT_MAC_STYLE_ITALIC`: bit 1; "Italic (if set to 1)".
				#define MUTT_MAC_STYLE_ITALIC 1
				// @DOCLINE * `MUTT_MAC_STYLE_UNDERLINE`: bit 2; "Underline (if set to 1)".
				#define MUTT_MAC_STYLE_UNDERLINE 2
				// @DOCLINE * `MUTT_MAC_STYLE_OUTLINE`: bit 3; "Outline (if set to 1)".
				#define MUTT_MAC_STYLE_OUTLINE 3
				// @DOCLINE * `MUTT_MAC_STYLE_SHADOW`: bit 4; "Shadow (if set to 1)".
				#define MUTT_MAC_STYLE_SHADOW 4
				// @DOCLINE * `MUTT_MAC_STYLE_CONDENSED`: bit 5; "Condensed (if set to 1)".
				#define MUTT_MAC_STYLE_CONDENSED 5
				// @DOCLINE * `MUTT_MAC_STYLE_EXTENDED`: bit 6; "Extended (if set to 1)".
				#define MUTT_MAC_STYLE_EXTENDED 6

		// @DOCLINE ## Advanced font information

			// @DOCLINE The struct `muttAdvancedInfo` is used to refer to more specific general information about a TrueType font. It has the following members:

			struct muttAdvancedInfo {
				// @DOCLINE * `created`: the time this font was initially created; in number of seconds since 12:00 midnight on 1 January 1904 in GMT/UTC time, defined below: @NLNT
				int64_m created;
				// @DOCLINE * `modified`: the time this font was last modified; in number of seconds since 12:00 midnight on 1 January 1904 in GMT/UTC time, defined below: @NLNT
				int64_m modified;
			};
			typedef struct muttAdvancedInfo muttAdvancedInfo;

		// @DOCLINE ## General TrueType information struct

			// @DOCLINE The struct `muttInfo` is used to refer to the general information about a TrueType font, and is used to refer to a TrueType font across multiple function calls. It has the following documented members:

			struct muttInfo {
				// @DOCLINE `data`: the data of the TrueType font, defined below: @NLNT
				muByte* data;
				// @DOCLINE `size`: the size of the TrueType font data, in bytes, defined below: @NLNT
				size_m size;

				// @DOCLINE `req`: the required tables in the TrueType font data, defined below: @NLNT
				muttRequiredTables req;

				// @DOCLINE `glyph_info`: general information about the glyphs specified in the TrueType font data, defined below: @NLNT
				muttGeneralGlyphInfo glyph_info;
				// @DOCLINE `adv_info`: advanced information about the font, defined below: @NLNT
				muttAdvancedInfo adv_info;
			}; typedef struct muttInfo muttInfo;

			// @DOCLINE All of the members, including those regarding the raw data of the TrueType font, are automatically generated upon a successful call to `mutt_truetype_get_info`, and are invalid upon its respective call to `mutt_truetype_let_info`. The members are meant to be read, not written.

		// @DOCLINE ## Retrieve TrueType information

			// @DOCLINE The function `mutt_truetype_get_info` retrieves information about TrueType data and stores it in a `muttInfo` struct, defined below: @NLNT
			MUDEF muttInfo mutt_truetype_get_info(muttResult* result, muByte* data, size_m size);

			// @DOCLINE Its non-result-checking equivalent macro is defined below: @NLNT
			#define mu_truetype_get_info(...) mutt_truetype_get_info(0, __VA_ARGS__)

			// @DOCLINE Every successful call to `mutt_true_type_get_info` must be matched with a call to `mutt_true_type_let_info`.

			// @DOCLINE The pointer `data` is assumed to be valid and unchanged throughout the returned `muttInfo` struct's lifetime.

		// @DOCLINE ## Free TrueType information

			// @DOCLINE The function `mutt_truetype_let_info` frees the information retrieved about TrueType data stored in a `muttInfo` struct, defined below: @NLNT
			MUDEF void mutt_truetype_let_info(muttInfo* info);

			// @DOCLINE Its non-result-checking equivalent macro is defined below: @NLNT
			#define mu_truetype_let_info(...) mutt_truetype_let_info(__VA_ARGS__)

			// @DOCLINE This function must be called on every successfully created `muttInfo` struct.

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

	// @DOCLINE # General font information

		// @DOCLINE There are several attributes about a font that can be stored within a TrueType file, to which mutt gives an API to retrieve this information. This section does not include glyph data; that is covered in a later section.

		// @DOCLINE ## Names

			// @DOCLINE mutt can retrieve information from the "name" table in TrueType based on a requested name ID.

			// @DOCLINE ### Name ID

				// @DOCLINE The type `muttNameID` (`uint16_m`) is used to represent a name ID in TrueType. Any value based on the TrueType standard will work, but this type gives a representation for some common name IDs.

				#define muttNameID uint16_m

				// @DOCLINE #### Values

				// @DOCLINE * `MUTT_NAME_COPYRIGHT_NOTICE`: ID 0 based on TrueType standards; "Copyright notice."
				#define MUTT_NAME_COPYRIGHT_NOTICE 0
				// @DOCLINE * `MUTT_NAME_FONT_FAMILY`: ID 1 based on TrueType standards; "Font Family name."
				#define MUTT_NAME_FONT_FAMILY 1
				// @DOCLINE * `MUTT_NAME_FONT_SUBFAMILY`: ID 2 based on TrueType standards; "Font Subfamily name."
				#define MUTT_NAME_FONT_SUBFAMILY 2
				// @DOCLINE * `MUTT_NAME_FULL_FONT_NAME`: ID 4 based on TrueType standards; "Full font name".
				#define MUTT_NAME_FULL_FONT_NAME 4
				// @DOCLINE * `MUTT_NAME_VERSION_STRING`: ID 5 based on TrueType standards; "Version string."
				#define MUTT_NAME_VERSION_STRING 5
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

				// @DOCLINE #### Name

				#ifdef MUTT_NAMES

				// @DOCLINE The function `mutt_name_id_get_name` converts a `muttNameID` value to a `const char*` representation, defined below: @NLNT
				MUDEF const char* mutt_name_id_get_name(muttNameID nameID);

				// @DOCLINE The function `mutt_name_id_get_nice_name` converts a `muttNameID` value to a more readable `const char*` representation, defined below: @NLNT
				MUDEF const char* mutt_name_id_get_nice_name(muttNameID nameID);

				// @DOCLINE Note that these functions are only defined if `MUTT_NAMES` is defined before the inclusion of the header file.

				// @DOCLINE These functions return `"MUTT_UNKNOWN"` if a respective name could not be found.

				#endif

			// @DOCLINE ### Get names

				// @DOCLINE #### Get offered name IDs

					// @DOCLINE The function `mutt_truetype_get_name_ids` is used to retrieve the name IDs offered by a TrueType font, defined below: @NLNT
					MUDEF uint16_m mutt_truetype_get_name_ids(muttInfo* info, muttNameID* ids);

					// @DOCLINE Its non-result-checking equivalent macro is defined below: @NLNT
					#define mu_truetype_get_name_ids(...) mutt_truetype_get_name_ids(__VA_ARGS__)

					// @DOCLINE This function returns the amount of name IDs specified by the font. If `ids` is not 0, `ids` is expected to be a pointer to an array of `uint16_m`s at least the length of the amount of name IDs specified by the font, and will be written to as such.

				// @DOCLINE #### Get name

					// @DOCLINE The function `mutt_truetype_get_name` retrieves the string for a given name ID, defined below: @NLNT
					MUDEF char* mutt_truetype_get_name(muttInfo* info, uint16_m name_id_index, muttEncoding* encoding, uint16_m* length);

					// @DOCLINE Its non-result-checking equivalent macro is defined below: @NLNT
					#define mu_truetype_get_name(...) mutt_truetype_get_name(__VA_ARGS__)

					// @DOCLINE This function returns a pointer to an offset in the TrueType font data within `info` which holds the name.

					// @DOCLINE `name_id_index` is the index of the name ID being requested; for example, if the font in question offers 7 name IDs and you wanted the 5th name ID specified, `name_id_index` should be 4. `name_id_index` is *not* the name ID, but instead the index the name in question specified by the font.

					// @DOCLINE If `length` is not 0, `length` is dereferenced and set to the length of the name in bytes.

					// @DOCLINE If `encoding` is not 0, `encoding` is dereferenced and set to the encoding of the string.

					// @DOCLINE Note that the function returned does not necessarily have a null-terminating character.

	// @DOCLINE # Glyph information

		// @DOCLINE mutt can be used to extract information about particular glyphs stored within a TrueType font, including glyph data.

		// @DOCLINE ## Glyph reference

			// @DOCLINE The type `muttGlyphID` (defined as `uint16_m`) is used to refer to a glyph specified by a TrueType font. Values under this type are *not* codepoints or anything relating to actual character references, they only act as a reference to a glyph. Characters can be converted to these types with other parts of mutt.

			#define muttGylphID uint16_m

			// @DOCLINE Note that the value '0' will always be valid for a `muttGlyphID` in regards to a valid TrueType font, as all TrueType fonts have to specify a glyph for missing characters. This value is used by mutt to refer to a glyph that doesn't exist (for example, if a corresponding `muttGlyphID` is requested for a given character but the font doesn't specify it, 0 is returned, giving the missing character glyph).

	// @DOCLINE # Version macro

		// @DOCLINE mutt defines three macros to define the version of mutt: `MUTT_VERSION_MAJOR`, `MUTT_VERSION_MINOR`, and `MUTT_VERSION_PATCH`, following the format of `vMAJOR.MINOR.PATCH`.

		#define MUTT_VERSION_MAJOR 1
		#define MUTT_VERSION_MINOR 0
		#define MUTT_VERSION_PATCH 0

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
			}
		}

		MUDEF const char* mutt_name_id_get_name(muttNameID nameID) {
			switch (nameID) {
				default: return "MUTT_UNKNOWN"; break;
				case MUTT_NAME_COPYRIGHT_NOTICE: return "MUTT_NAME_COPYRIGHT_NOTICE"; break;
				case MUTT_NAME_FONT_FAMILY: return "MUTT_NAME_FONT_FAMILY"; break;
				case MUTT_NAME_FONT_SUBFAMILY: return "MUTT_NAME_FONT_SUBFAMILY"; break;
				case MUTT_NAME_FULL_FONT_NAME: return "MUTT_NAME_FULL_FONT_NAME"; break;
				case MUTT_NAME_VERSION_STRING: return "MUTT_NAME_VERSION_STRING"; break;
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
				case MUTT_NAME_VERSION_STRING: return "Version string"; break;
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

	/* Get/Let info */

		MUDEF muttInfo mutt_truetype_get_info(muttResult* result, muByte* data, size_m size) {
			muttInfo info = MU_ZERO_STRUCT(muttInfo);

			info.data = data;
			info.size = size;

			size_m p = 0;
			uint16_m tables;

			/* Offset subtable */
			{
				p += 4;
				tables = mu_rbe_uint16((&data[p]));
				p += 8;
			}

			/* Table directories */
			{
				for (uint16_m i = 0; i < tables; i++) {
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

			muByte* maxp = &data[info.req.maxp.offset];
			muByte* head = &data[info.req.head.offset];

			/* General glyph info */
			{
				uint16_m u16;

				info.glyph_info.glyph_count = mu_rbe_uint16((&maxp[4]));
				info.glyph_info.max_points = mu_rbe_uint16((&maxp[6]));
				info.glyph_info.max_contours = mu_rbe_uint16((&maxp[8]));
				info.glyph_info.max_composite_points = mu_rbe_uint16((&maxp[10]));
				info.glyph_info.max_composite_contours = mu_rbe_uint16((&maxp[12]));
				info.glyph_info.head_flags = mu_rbe_uint16((&head[16]));
				info.glyph_info.units_per_em = mu_rbe_uint16((&head[18]));
				u16 = mu_rbe_uint16((&head[36]));
				info.glyph_info.x_min = *(int16_m*)&u16;
				u16 = mu_rbe_uint16((&head[38]));
				info.glyph_info.y_min = *(int16_m*)&u16;
				u16 = mu_rbe_uint16((&head[40]));
				info.glyph_info.x_max = *(int16_m*)&u16;
				u16 = mu_rbe_uint16((&head[42]));
				info.glyph_info.y_max = *(int16_m*)&u16;
				info.glyph_info.mac_style = mu_rbe_uint16((&head[44]));
				info.glyph_info.lowest_rec_ppem = mu_rbe_uint16((&head[46]));
			}

			/* Advanced info */
			{
				uint64_m u64;

				// ?
				u64 = mu_rbe_uint64((&head[18]));
				info.adv_info.modified = *(int64_m*)&u64;
				u64 = mu_rbe_uint64((&head[26]));
				info.adv_info.modified = *(int64_m*)&u64;
			}

			return info; if (result) {}
		}

		MUDEF void mutt_truetype_let_info(muttInfo* info) {
			return; if (info) {}
		}

	/* Names */

		MUDEF uint16_m mutt_truetype_get_name_ids(muttInfo* info, uint16_m* ids) {
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

		MUDEF char* mutt_truetype_get_name(muttInfo* info, uint16_m name_id_index, muttEncoding* encoding, uint16_m* length) {
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

