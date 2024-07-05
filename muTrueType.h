/*
muTrueType.h
Public domain single-file C library for reading and rendering TrueType data.
No warranty implied; use at your own risk.

Licensed under MIT License or public domain, whichever you prefer.
More explicit license information at the end of file.

@TODO Throw errors for duplicate tags.

@DOCBEGIN

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

@DOCEND
*/

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
			// @DOCLINE * `@NLFT` - the task succeeded.
			MUTT_SUCCESS,
			// @DOCLINE * `@NLFT` - a call to malloc failed; memory was insufficient to perform the operation
			MUTT_FAILED_MALLOC,
			// @DOCLINE * `@NLFT` - a call to realloc failled; memory was insufficient to perform the operation.
			MUTT_FAILED_REALLOC,
			// @DOCLINE * `@NLFT` - the table could not be located within the data.
			MUTT_UNFOUND_TABLE,
			// @DOCLINE * `@NLFT` - another table with the same tag was found.
			MUTT_DUPLICATE_TABLE,
			// @DOCLINE * `@NLFT` - the length of the given TrueType data is not enough for the table directory. Likely the length is incorrect or the data given is not TrueType data.
			MUTT_INVALID_TABLE_DIRECTORY_LENGTH,
			// @DOCLINE * `@NLFT` - the value for "sfntVersion" in the table directory was invalid. Since this is the first value read when loading TrueType data, this most likely means that rather the data given is corrupt, not TrueType data, or is under another incompatible wrapper (such as fonts that use CFF data).
			MUTT_INVALID_TABLE_DIRECTORY_SFNT_VERSION,
			// @DOCLINE * `@NLFT` - the value for "searchRange" in the table directory was invalid.
			MUTT_INVALID_TABLE_DIRECTORY_SEARCH_RANGE,
			// @DOCLINE * `@NLFT` - the value for "entrySelector" in the table directory was invalid.
			MUTT_INVALID_TABLE_DIRECTORY_ENTRY_SELECTOR,
			// @DOCLINE * `@NLFT` - the value for "rangeShift" in the table directory was invalid.
			MUTT_INVALID_TABLE_DIRECTORY_RANGE_SHIFT,
			// @DOCLINE * `@NLFT` - the value for "offset" in a table record was out of range.
			MUTT_INVALID_TABLE_RECORD_OFFSET,
			// @DOCLINE * `@NLFT` - the value for "length" in a table record was out of range.
			MUTT_INVALID_TABLE_RECORD_LENGTH,
			// @DOCLINE * `@NLFT` - the value for "checksum" in a table record was invalid.
			MUTT_INVALID_TABLE_RECORD_CHECKSUM,
			// @DOCLINE * `@NLFT` - the value for the table length of the maxp table was invalid. This could mean that an unsupported version of the table is being used.
			MUTT_INVALID_MAXP_LENGTH,
			// @DOCLINE * `@NLFT` - the version value in the maxp table was invalid/unsupported.
			MUTT_INVALID_MAXP_VERSION,
			// @DOCLINE * `@NLFT` - the value for "maxZones" in the maxp table was invalid.
			MUTT_INVALID_MAXP_MAX_ZONES,
			// @DOCLINE * `@NLFT` - the value for the table length of the head table was invalid. This could mean that an unsupported version of the table is being used.
			MUTT_INVALID_HEAD_LENGTH,
			// @DOCLINE * `@NLFT` - the version value in the head table was invalid/unsupported.
			MUTT_INVALID_HEAD_VERSION,
			// @DOCLINE * `@NLFT` - the value for "magicNumber" in the head table was invalid.
			MUTT_INVALID_HEAD_MAGIC_NUMBER,
			// @DOCLINE * `@NLFT` - the value for "unitsPerEm" in the head table was invalid.
			MUTT_INVALID_HEAD_UNITS_PER_EM,
			// @DOCLINE * `@NLFT` - the values for "xMin" and "xMax" in the head table were invalid.
			MUTT_INVALID_HEAD_X_MIN_MAX,
			// @DOCLINE * `@NLFT` - the values for "yMin" and "yMax" in the head table were invalid.
			MUTT_INVALID_HEAD_Y_MIN_MAX,
			// @DOCLINE * `@NLFT` - the value for "indexToLocFormat" in the head table was invalid.
			MUTT_INVALID_HEAD_INDEX_TO_LOC_FORMAT,
			// @DOCLINE * `@NLFT` - the value for "glyphDataFormat" in the head table was invalid/unsupported.
			MUTT_INVALID_HEAD_GLYPH_DATA_FORMAT,
			// @DOCLINE * `@NLFT` - the value for the table length of the hhea table was invalid. This could mean that an unsupported version of the table is being used.
			MUTT_INVALID_HHEA_LENGTH,
			// @DOCLINE * `@NLFT` - the version value in the hhea table was invalid/unsupported.
			MUTT_INVALID_HHEA_VERSION,
			// @DOCLINE * `@NLFT` - the value for "metricDataFormat" in the hhea table was invalid/unsupported.
			MUTT_INVALID_HHEA_METRIC_DATA_FORMAT,
			// @DOCLINE * `@NLFT` - the value for "numberOfHMetrics" in the hhea table was invalid/unsupported.
			MUTT_INVALID_HHEA_NUMBER_OF_HMETRICS,
			// @DOCLINE * `@NLFT` - the value for the table length of the hmtx table was invalid.
			MUTT_INVALID_HMTX_LENGTH,
			// @DOCLINE * `@NLFT` - the value for the table length of the loca table was invalid.
			MUTT_INVALID_LOCA_LENGTH,
			// @DOCLINE * `@NLFT` - the value for the table length of the post table was invalid.
			MUTT_INVALID_POST_LENGTH,
			// @DOCLINE * `@NLFT` - the value "version" in the post table was invalid/unsupported.
			MUTT_INVALID_POST_VERSION,
			// @DOCLINE * `@NLFT` - an index in the array "glyphNameIndex" in the version 2.0 post subtable was out of range.
			MUTT_INVALID_POST_GLYPH_NAME_INDEX,
			// @DOCLINE * `@NLFT` - the value for the table length of the name table was invalid. This could mean that an unsupported version of the table is being used.
			MUTT_INVALID_NAME_LENGTH,
			// @DOCLINE * `@NLFT` - the value "version" in the name table was invalid/unsupported.
			MUTT_INVALID_NAME_VERSION,
			// @DOCLINE * `@NLFT` - the value "storageOffset" in the name table was out of range.
			MUTT_INVALID_NAME_STORAGE_OFFSET,
			// @DOCLINE * `@NLFT` - the value "platformID" in a NameRecord within the "nameRecord" array in the name table was invalid/unsupported.
			MUTT_INVALID_NAME_PLATFORM_ID,
			// @DOCLINE * `@NLFT` - the value "encodingID" in a NameRecord within the "nameRecord" array in the name table was invalid/unsupported.
			MUTT_INVALID_NAME_ENCODING_ID,
			// @DOCLINE * `@NLFT` - the value "languageID" in a NameRecord within the "nameRecord" array in the name table was invalid/unsupported.
			MUTT_INVALID_NAME_LANGUAGE_ID,
			// @DOCLINE * `@NLFT` - the value "stringOffset" and "length" in a NameRecord within the "nameRecord" array in the name table were out of range.
			MUTT_INVALID_NAME_STRING_OFFSET,
			// @DOCLINE * `@NLFT` - the value "langTagOffset" and "length" in a LangTagRecord within the "langTagRecord" array in the name table were out of range.
			MUTT_INVALID_NAME_LANG_TAG_OFFSET,
			// @DOCLINE * `@NLFT` - the hhea table failed to load becuase maxp is rather not being loaded or failed to load, and hhea relies on maxp.
			MUTT_HHEA_REQUIRES_MAXP,
			// @DOCLINE * `@NLFT` - the hmtx table failed to load because maxp is rather not being loaded or failed to load, and hmtx relies on mxap.
			MUTT_HMTX_REQUIRES_MAXP,
			// @DOCLINE * `@NLFT` - the hmtx table failed to load because hhea is rather not being loaded or failed to load, and hmtx relies on hhea.
			MUTT_HMTX_REQUIRES_HHEA,
			// @DOCLINE * `@NLFT` - the loca table failed to load because head is rather not being loaded or failed to load, and loca relies on head.
			MUTT_LOCA_REQUIRES_HEAD,
			// @DOCLINE * `@NLFT` - the loca table failed to load because maxp is rather not being loaded or failed to load, and loca relies on maxp.
			MUTT_LOCA_REQUIRES_MAXP,
		)

		// @DOCLINE Most of these errors getting triggered imply that rather the data is corrupt (especially in regards to checksum errors), uses some extension or format not supported by this library (such as OpenType), has accidental incorrect values, or is purposely malformed to attempt to get out of the memory region of the file data.

		// @DOCLINE ## Result name function

			#ifdef MUTT_NAMES

			// @DOCLINE The function `mutt_result_get_name` converts a `muttResult` value into a `const char*` representation, defined below: @NLNT
			MUDEF const char* mutt_result_get_name(muttResult result);

			// @DOCLINE Note that this function is only defined if `MUTT_NAMES` is defined before the inclusion of the header file.

			// @DOCLINE This function returns `"MUTT_UNKNOWN"` if a respective name could not be found.

			#endif

	// @DOCLINE # Loading a TrueType font

		typedef struct muttFont muttFont;

		typedef struct muttDirectory muttDirectory;

		typedef struct muttMaxp muttMaxp;
		typedef struct muttHead muttHead;
		typedef struct muttHhea muttHhea;
		typedef struct muttHmtx muttHmtx;
		typedef union muttLoca muttLoca;
		typedef struct muttPost muttPost;
		typedef struct muttName muttName;

		// @DOCLINE ## Loading and deloading functions

			// @DOCLINE The function `mutt_load` loads information about TrueType data into a `muttFont` struct, defined below: @NLNT
			MUDEF muttResult mutt_load(muByte* data, size_m datalen, muttFont* font, uint32_m load_flags);

			// @DOCLINE The function `mutt_deload` deloads information retrieved from TrueType data within a `muttFont` struct, defined below: @NLNT
			MUDEF void mutt_deload(muttFont* font);

			// @DOCLINE `mutt_deload` must be called at *some* point on every successfully loaded font. Once `mutt_deload` is called on a `muttFont` struct, its contents are undefined.

			// @DOCLINE If `mutt_load` doesn't return `MUTT_SUCCESS`, the font failed to load entirely, the contents of `font` are undefined, and `mutt_deload` should not be called on it.

			// @DOCLINE `mutt_load` checks if all of the values within each requested table are correct according to the specification, and doesn't load a given table if one is found, which helps prevent bad values and attack vectors.

		// @DOCLINE ## Loading flags

			// @DOCLINE The function `mutt_load` takes in a parameter, `load_flags`, which determines what information about the TrueType font does and doesn't get loaded. The value `0` loads no information, and a bit equaling `1` means to load it.

			// @DOCLINE ### Exact bit values

				// @DOCLINE The following macros are defined for certain bits indicating what information to load:

				// @DOCLINE * [0x00000001] `MUTT_LOAD_DIRECTORY` - load the directory and permanently store the results. The directory is loaded no matter what, but this bit ensures that the directory data isn't wiped after loading.
				#define MUTT_LOAD_DIRECTORY 0x00000001

				// @DOCLINE * [0x00000002] `MUTT_LOAD_MAXP` - load the maxp table.
				#define MUTT_LOAD_MAXP 0x00000002

				// @DOCLINE * [0x00000004] `MUTT_LOAD_HEAD` - load the head table.
				#define MUTT_LOAD_HEAD 0x00000004

				// @DOCLINE * [0x00000008] `MUTT_LOAD_HHEA` - load the hhea table.
				#define MUTT_LOAD_HHEA 0x00000008

				// @DOCLINE * [0x00000010] `MUTT_LOAD_HMTX` - load the hmtx table.
				#define MUTT_LOAD_HMTX 0x00000010

				// @DOCLINE * [0x00000020] `MUTT_LOAD_LOCA` - load the loca table.
				#define MUTT_LOAD_LOCA 0x00000020

				// @DOCLINE * [0x00000040] `MUTT_LOAD_POST` - load the post table.
				#define MUTT_LOAD_POST 0x00000040

				// @DOCLINE * [0x00000080] `MUTT_LOAD_NAME` - load the name table.
				#define MUTT_LOAD_NAME 0x00000080

			// @DOCLINE ### Group bit values

				// @DOCLINE The following macros are defined for loading groups of tables:

				// @DOCLINE * [0x000003FE] `MUTT_LOAD_REQUIRED_TABLES` - loads all tables required by TrueType standards (cmap, glyf, head, hhea, hmtx, loca, maxp, name, and post).
				#define MUTT_LOAD_REQUIRED_TABLES 0x000003FE

				// @DOCLINE * [0xFFFFFFFF] `MUTT_LOAD_ALL` - loads everything; sets all flags.
				#define MUTT_LOAD_ALL 0xFFFFFFFF

	// @DOCLINE # Reading information from a TrueType font

		// @DOCLINE ## The `muttFont` struct

			// @DOCLINE A TrueType font is represented by the struct `muttFont`. Once successfully loaded, the data stored within a `muttFont` struct is entirely separate from the user-allocated TrueType data.

			// @DOCLINE Inside the `muttFont` struct is all of the loaded information from when it was loaded. The actual full list of members is:

			struct muttFont {
				// @DOCLINE * `@NLFT load_flags` - the load flags that were provided to the load function.
				uint32_m load_flags;

				// @DOCLINE * `@NLFT* directory` - a pointer to a directory listing all of the tables provided by the given font.
				muttDirectory* directory;

				// @DOCLINE * `@NLFT* maxp` - a pointer to the maxp table.
				muttMaxp* maxp;
				// @DOCLINE * `@NLFT maxp_res` - the result of loading the member `maxp`.
				muttResult maxp_res;

				// @DOCLINE * `@NLFT* head` - a pointer to the head table.
				muttHead* head;
				// @DOCLINE * `@NLFT head_res` - the result of loading the member `head`.
				muttResult head_res;

				// @DOCLINE * `@NLFT* hhea` - a pointer to the hhea table.
				muttHhea* hhea;
				// @DOCLINE * `@NLFT hhea_res` - the result of loading the member `hhea`.
				muttResult hhea_res;

				// @DOCLINE * `@NLFT* hmtx` - a pointer to the hmtx table.
				muttHmtx* hmtx;
				// @DOCLINE * `@NLFT hmtx_res` - the result of loading the member `hmtx`.
				muttResult hmtx_res;

				// @DOCLINE * `@NLFT* loca` - a pointer to the loca table.
				muttLoca* loca;
				// @DOCLINE * `@NLFT loca_res` - the result of loading the member `loca`.
				muttResult loca_res;

				// @DOCLINE * `@NLFT* post` - a pointer to the post table.
				muttPost* post;
				// @DOCLINE * `@NLFT post_res` - the result of loading the member `post`.
				muttResult post_res;

				// @DOCLINE * `@NLFT* name` - a pointer to the name table.
				muttName* name;
				// @DOCLINE * `@NLFT name_res` - the result of loading the member `name`.
				muttResult name_res;

				// @DOCLINE * `@NLFT* mem` - the inner allocated memory used for holding necessary data.
				muByte* mem;
				// @DOCLINE * `@NLFT memlen` - the length of the allocated memory, in bytes.
				size_m memlen;
				// @DOCLINE * `@NLFT memcur` - offset to the latest unused memory in `mem`, in bytes.
				size_m memcur;
			};

			// @DOCLINE Most of the members are in pairs of pointers and result values. If a requested pointer is 0, it could not be loaded, and its corresponding result value will indicate the result enumerator indicating what went wrong.

			// @DOCLINE The contents of a pointer and result pair for information not included in the load flags are undefined.

			// @DOCLINE Note that if the directory fails to load, the entire loading function fails, and what went wrong is returned in the loading function; this is why there is no respective result for the member `directory`.

			// @DOCLINE Note that if an array in a table or directory is of length 0, the value for the pointer within the respective struct is 0 unless stated otherwise.

		// @DOCLINE ## Directory information

			// @DOCLINE The struct `muttDirectory` is used to list all of the tables provided by a TrueType font. It is stored in the struct `muttFont` as `muttFont->directory`, and is similar to TrueType's table directory. It is loaded permanently with the flag `MUTT_LOAD_DIRECTORY`.

			typedef struct muttTableRecord muttTableRecord;

			// @DOCLINE Its members are:
			struct muttDirectory {
				// @DOCLINE * `@NLFT num_tables` - equivalent to "numTables" in the table directory.
				uint16_m num_tables;
				// @DOCLINE * `@NLFT search_range` - equivalent to "searchRange" in the table directory.
				uint16_m search_range;
				// @DOCLINE * `@NLFT entry_selector` - equivalent to "entrySelector" in the table directory.
				uint16_m entry_selector;
				// @DOCLINE * `@NLFT range_shift` - equivalent to "rangeShift" in the table directory.
				uint16_m range_shift;
				// @DOCLINE * `@NLFT* table_records` - equivalent to "tableRecords" in the table directory.
				muttTableRecord* table_records;
			};

			// @DOCLINE The struct `muttTableRecord` is similar to TrueType's table record, and has the following members:

			struct muttTableRecord {
				// @DOCLINE * `@NLFT table_tag[4]` - equivalent to "tableTag" in the table record.
				uint8_m table_tag[4];
				// @DOCLINE * `@NLFT checksum` - equivalent to "checksum" in the table record.
				uint32_m checksum;
				// @DOCLINE * `@NLFT offset` - equivalent to "offset" in the table record.
				uint32_m offset;
				// @DOCLINE * `@NLFT length` - equivalent to "length" in the table record.
				uint32_m length;
			};

		// @DOCLINE ## Maxp information

			// @DOCLINE The struct `muttMaxp` is used to represent the maxp table provided by a TrueType font, stored in the struct `muttFont` as `muttFont->maxp`, and loaded with the flag `MUTT_LOAD_MAXP`.

			// @DOCLINE Its members are:
			struct muttMaxp {
				// @DOCLINE * `@NLFT version_high` - equivalent to the high bytes of "version" in the maxp table.
				uint16_m version_high;
				// @DOCLINE * `@NLFT version_low` - equivalent to the low bytes "version" in the maxp table.
				uint16_m version_low;
				// @DOCLINE * `@NLFT num_glyphs` - equivalent to "numGlyphs" in the maxp table.
				uint16_m num_glyphs;
				// @DOCLINE * `@NLFT max_points` - equivalent to "maxPoints" in the maxp table.
				uint16_m max_points;
				// @DOCLINE * `@NLFT max_contours` - equivalent to "maxContours" in the maxp table.
				uint16_m max_contours;
				// @DOCLINE * `@NLFT max_composite_points` - equivalent to "maxCompositePoints" in the maxp table.
				uint16_m max_composite_points;
				// @DOCLINE * `@NLFT max_composite_contours` - equivalent to "maxCompositeContours" in the maxp table.
				uint16_m max_composite_contours;
				// @DOCLINE * `@NLFT max_zones` - equivalent to "maxZones" in the maxp table.
				uint16_m max_zones;
				// @DOCLINE * `@NLFT max_twilight_points` - equivalent to "maxTwilightPoints" in the maxp table.
				uint16_m max_twilight_points;
				// @DOCLINE * `@NLFT max_storage` - equivalent to "maxStorage" in the maxp table.
				uint16_m max_storage;
				// @DOCLINE * `@NLFT max_function_defs` - equivalent to "maxFunctionDefs" in the maxp table.
				uint16_m max_function_defs;
				// @DOCLINE * `@NLFT max_instruction_defs` - equivalent to "maxInstructionDefs" in the maxp table.
				uint16_m max_instruction_defs;
				// @DOCLINE * `@NLFT max_stack_elements` - equivalent to "maxStackElements" in the maxp table.
				uint16_m max_stack_elements;
				// @DOCLINE * `@NLFT max_size_of_instructions` - equivalent to "maxSizeOfInstructions" in the maxp table.
				uint16_m max_size_of_instructions;
				// @DOCLINE * `@NLFT max_component_elements` - equivalent to "maxComponentElements" in the maxp table.
				uint16_m max_component_elements;
				// @DOCLINE * `@NLFT max_component_depth` - equivalent to "maxComponentDepth" in the maxp table.
				uint16_m max_component_depth;
			};

		// @DOCLINE ## Head information

			// @DOCLINE The struct `muttHead` is used to represent the head table provided by a TrueType font, stored in the struct `muttFont` as `muttFont->head`, and loaded with the flag `MUTT_LOAD_HEAD`.

			// @DOCLINE Its members are:
			struct muttHead {
				// @DOCLINE * `@NLFT font_revision_high` - equivalent to the high bytes of "fontRevision" in the head table.
				uint16_m font_revision_high;
				// @DOCLINE * `@NLFT font_revision_low` - equivalent to the low bytes of "fontRevision" in the head table.
				uint16_m font_revision_low;
				// @DOCLINE * `@NLFT checksum_adjustment` - equivalent to "checksumAdjustment" in the head table.
				uint32_m checksum_adjustment;
				// @DOCLINE * `@NLFT flags` - equivalent to "flags" in the head table.
				uint16_m flags;
				// @DOCLINE * `@NLFT units_per_em` - equivalent to "unitsPerEm" in the head table.
				uint16_m units_per_em;
				// @DOCLINE * `@NLFT created` - equivalent to "created" in the head table.
				int64_m created;
				// @DOCLINE * `@NLFT modified` - equivalent to "modified" in the head table.
				int64_m modified;
				// @DOCLINE * `@NLFT x_min` - equivalent to "xMin" in the head table.
				int16_m x_min;
				// @DOCLINE * `@NLFT y_min` - equivalent to "yMin" in the head table.
				int16_m y_min;
				// @DOCLINE * `@NLFT x_max` - equivalent to "xMax" in the head table.
				int16_m x_max;
				// @DOCLINE * `@NLFT y_max` - equivalent to "yMax" in the head table.
				int16_m y_max;
				// @DOCLINE * `@NLFT mac_style` - equivalent to "macStyle" in the head table.
				uint16_m mac_style;
				// @DOCLINE * `@NLFT lowest_rec_ppem` - equivalent to "lowestRecPPEM" in the head table.
				uint16_m lowest_rec_ppem;
				// @DOCLINE * `@NLFT font_direction_hint` - equivalent to "fontDirectionHint" in the head table.
				int16_m font_direction_hint;
				// @DOCLINE * `@NLFT index_to_loc_format` - equivalent to "indexToLocFormat" in the head table.
				int16_m index_to_loc_format;
				// @DOCLINE * `@NLFT glyph_data_format` - equivalent to "glyphDataFormat" in the head table.
				int16_m glyph_data_format;
			};

			// @DOCLINE ### Head mac style macros

				// @DOCLINE The following macros are defined to make bit-masking the `mac_style` member of the `muttHead` struct easier:

				// @DOCLINE * [0x0001] `MUTT_MAC_STYLE_BOLD` - bold.
				#define MUTT_MAC_STYLE_BOLD 0x0001
				// @DOCLINE * [0x0002] `MUTT_MAC_STYLE_ITALIC` - italic.
				#define MUTT_MAC_STYLE_ITALIC 0x0002
				// @DOCLINE * [0x0004] `MUTT_MAC_STYLE_UNDERLINE` - underlined.
				#define MUTT_MAC_STYLE_UNDERLINE 0x0004
				// @DOCLINE * [0x0008] `MUTT_MAC_STYLE_OUTLINE` - outlined.
				#define MUTT_MAC_STYLE_OUTLINE 0x0008
				// @DOCLINE * [0x0010] `MUTT_MAC_STYLE_SHADOW` - shadow.
				#define MUTT_MAC_STYLE_SHADOW 0x0010
				// @DOCLINE * [0x0020] `MUTT_MAC_STYLE_CONDENSED` - condensed.
				#define MUTT_MAC_STYLE_CONDENSED 0x0020
				// @DOCLINE * [0x0040] `MUTT_MAC_STYLE_EXTENDED` - extended.
				#define MUTT_MAC_STYLE_EXTENDED 0x0040

			// @DOCLINE ### Head index to loc format macros

				// @DOCLINE The following macros are defined to make the value of the `index_to_loc_format` member of the `muttHead` struct easier to interpret:

				// @DOCLINE * [0x0000] `MUTT_LOCA_FORMAT_OFFSET16` - short offsets (offset16).
				#define MUTT_LOCA_FORMAT_OFFSET16 0x0000
				// @DOCLINE * [0x0001] `MUTT_LOCA_FORMAT_OFFSET32` - long offsets (offset32).
				#define MUTT_LOCA_FORMAT_OFFSET32 0x0001

		// @DOCLINE ## Hhea information

			// @DOCLINE The struct `muttHhea` is used to represent the hhea table provided by a TrueType font, stored in the struct `muttFont` as `muttFont->hhea`, and loaded with the flag `MUTT_LOAD_HHEA` (flag `MUTT_LOAD_MAXP` also needs to be set for loca to load successfully).

			// @DOCLINE Its members are:
			struct muttHhea {
				// @DOCLINE * `@NLFT ascender` - equivalent to "ascender" in the hhea table.
				int16_m ascender;
				// @DOCLINE * `@NLFT descender` - equivalent to "descender" in the hhea table.
				int16_m descender;
				// @DOCLINE * `@NLFT line_gap` - equivalent to "lineGap" in the hhea table.
				int16_m line_gap;
				// @DOCLINE * `@NLFT advance_width_max` - equivalent to "advanceWidthMax" in the hhea table.
				uint16_m advance_width_max;
				// @DOCLINE * `@NLFT min_left_side_bearing` - equivalent to "minLeftSideBearing" in the hhea table.
				int16_m min_left_side_bearing;
				// @DOCLINE * `@NLFT min_right_side_bearing` - equivalent to "minRightSideBearing" in the hhea table.
				int16_m min_right_side_bearing;
				// @DOCLINE * `@NLFT x_max_extent` - equivalent to "xMaxExtent" in the hhea table.
				int16_m x_max_extent;
				// @DOCLINE * `@NLFT caret_slope_rise` - equivalent to "caretSlopeRise" in the hhea table.
				int16_m caret_slope_rise;
				// @DOCLINE * `@NLFT caret_slope_run` - equivalent to "caretSlopeRun" in the hhea table.
				int16_m caret_slope_run;
				// @DOCLINE * `@NLFT caret_offset` - equivalent to "caretOffset" in the hhea table.
				int16_m caret_offset;
				// @DOCLINE * `@NLFT metric_data_format` - equivalent to "metricDataFormat" in the hhea table.
				int16_m metric_data_format;
				// @DOCLINE * `@NLFT number_of_hmetrics` - equivalent to "numberOfHMetrics" in the hhea table.
				uint16_m number_of_hmetrics;
			};

		// @DOCLINE ## Hmtx information

			// @DOCLINE The struct `muttHmtx` is used to represent the hmtx table provided by a TrueType font, stored in the struct `muttFont` as `muttFont->hmtx`, and loaded with the flag `MUTT_LOAD_HMTX` (flags `MUTT_LOAD_MAXP` and `MUTT_LOAD_HHEA` also need to be set for loca to load successfully).

			// @DOCLINE Its members are:
			typedef struct muttLongHorMetric muttLongHorMetric;
			struct muttHmtx {
				// @DOCLINE * `@NLFT* hmetrics` - equivalent to "hMetrics" in the hmtx table.
				muttLongHorMetric* hmetrics;
				// @DOCLINE * `@NLFT* left_side_bearings` - equivalent to "leftSideBearings" in the hmtx table.
				int16_m* left_side_bearings;
			};

			// @DOCLINE The struct `muttLongHorMetric` is similar to TrueType's LongHorMetric record, and has the following members:

			struct muttLongHorMetric {
				// @DOCLINE * `@NLFT advance_width` - equivalent to "advanceWidth" in the LongHorMetric record.
				uint16_m advance_width;
				// @DOCLINE * `@NLFT lsb` - equivalent to "lsb" in the LongHorMetric record.
				int16_m lsb;
			};

		// @DOCLINE ## Loca information

			// @DOCLINE The union `muttLoca` is used to represent the loca table provided by a TrueType font, stored in the struct `muttFont` as `muttFont->loca`, and loaded with the flag `MUTT_LOAD_LOCA` (flags `MUTT_LOAD_HEAD` and `MUTT_LOAD_MAXP` also need to be set for loca to load successfully).

			// @DOCLINE Its members are:
			union muttLoca {
				// @DOCLINE * `@NLFT* offsets16` - equivalent to "offsets" in the short format of the loca table.
				uint16_m* offsets16;
				// @DOCLINE * `@NLFT* offsets32` - equivalent to "offsets" in the long format of the loca table.
				uint32_m* offsets32;
			};

		// @DOCLINE ## Post information

			// @DOCLINE ### Version 2.0 post subtable
			// @DOCLINE The struct `muttPost20` represents a version 2.0 post subtable. It has the following members:
			struct muttPost20 {
				// @DOCLINE * `@NLFT num_glyphs` - equivalent to "numGlyphs" in version 2.0 of the post subtable. If this value is equal to 0, the contents of all other members are undefined.
				uint16_m num_glyphs;
				// @DOCLINE * `@NLFT* glyph_name_index` - equivalent to "glyphNameIndex" in version 2.0 of the post subtable.
				uint16_m* glyph_name_index;
				// @DOCLINE * `@NLFT* string_data` - equivalent to "stringData" in version 2.0 of the post subtable.
				uint8_m* string_data;
			};
			typedef struct muttPost20 muttPost20;

			// @DOCLINE ### Version 2.5 post subtable
			// @DOCLINE The struct `muttPost25` represents a version 2.5 post subtable. It has the following members:

			struct muttPost25 {
				// @DOCLINE * `@NLFT num_glyphs` - equivalent to "numGlyphs" in version 2.5 of the post subtable. If this value is equal to 0, the contents of all other members are undefined.
				uint16_m num_glyphs;
				// @DOCLINE * `@NLFT* offset` - equivalent to "offset" in version 2.5 of the post subtable.
				int8_m* offset;
			};
			typedef struct muttPost25 muttPost25;

			// @DOCLINE ### Post subtable
			// @DOCLINE The union `muttPostSubtable` represents the possible subtables offered by different versions of the post table. It has the following members:

			union muttPostSubtable {
				// @DOCLINE * `@NLFT v20` - version 2.0 post subtable.
				muttPost20 v20;
				// @DOCLINE * `@NLFT v25` - version 2.5 post subtable.
				muttPost25 v25;
			};
			typedef union muttPostSubtable muttPostSubtable;

			// @DOCLINE ### Post struct

			// @DOCLINE The struct `muttPost` is used to represent the post table provided by a TrueType font, stored in the struct `muttFont` as `muttFont->post`, and loaded with the flag `MUTT_LOAD_POST`.

			// @DOCLINE Its members are:
			struct muttPost {
				// @DOCLINE * `@NLFT version_high` - equivalent to the high bytes of "version" in the post table.
				uint16_m version_high;
				// @DOCLINE * `@NLFT version_low` - equivalent to the low bytes of "version" in the post table.
				uint16_m version_low;
				// @DOCLINE * `@NLFT italic_angle` - equivalent to "italicAngle" in the post table.
				int32_m italic_angle;
				// @DOCLINE * `@NLFT underline_position` - equivalent to "underlinePosition" in the post table.
				int16_m underline_position;
				// @DOCLINE * `@NLFT underline_thickness` - equivalent to "underlineThickness" in the post table.
				int16_m underline_thickness;
				// @DOCLINE * `@NLFT is_fixed_pitch` - equivalent to "isFixedPitch" in the post table.
				uint32_m is_fixed_pitch;
				// @DOCLINE * `@NLFT min_mem_type42` - equivalent to "minMemType42" in the post table.
				uint32_m min_mem_type42;
				// @DOCLINE * `@NLFT max_mem_type42` - equivalent to "maxMemType42" in the post table.
				uint32_m max_mem_type42;
				// @DOCLINE * `@NLFT min_mem_type1` - equivalent to "minMemType1" in the post table.
				uint32_m min_mem_type1;
				// @DOCLINE * `@NLFT max_mem_type1` - equivalent to "maxMemType1" in the post table.
				uint32_m max_mem_type1;
				// @DOCLINE * `@NLFT subtable` - the subtable offered by the version of the post table; the contents of this member are undefined if the version is not 2.0 or 2.5.
				muttPostSubtable subtable;
			};

		// @DOCLINE ## Name information

			// @DOCLINE The struct `muttName` is used to represent the name table provided by a TrueType font, stored in the struct `muttFont` as `muttFont->name`, and loaded with the flag `MUTT_LOAD_NAME`.

			// @DOCLINE Its members are:
			typedef struct muttNameRecord muttNameRecord;
			typedef struct muttLangTagRecord muttLangTagRecord;
			struct muttName {
				// @DOCLINE * `@NLFT version` - equivalent to "version" in the name table.
				uint16_m version;
				// @DOCLINE * `@NLFT count` - equivalent to "count" in the name table. If this value is 0, the contents of all members listed after this are undefined.
				uint16_m count;
				// @DOCLINE * `@NLFT storage_offset` - equivalent to "storageOffset" in the name table.
				uint16_m storage_offset;
				// @DOCLINE * `@NLFT* name_record` - equivalent to "nameRecord" in the name table.
				muttNameRecord* name_record;
				// @DOCLINE * `@NLFT lang_tag_count` - equivalent to "langTagCount" in the name table (version 1). The value of this member is undefined if `version` is 0.
				uint16_m lang_tag_count;
				// @DOCLINE * `@NLFT* lang_tag_record` - equivalent to "langTagRecord" in the name table (version 1). The value of this member is undefined if `version` is 0.
				muttLangTagRecord* lang_tag_record;
				// @DOCLINE * `@NLFT* storage` - the raw storage data.
				muByte* storage;
			};

			// @DOCLINE ### Name record
			// @DOCLINE The struct `muttNameRecord` represents a NameRecord in the name table. It has the following members:
			struct muttNameRecord {
				// @DOCLINE * `@NLFT platform_id` - equivalent to "platformID" in the NameRecord subtable of the name table.
				uint16_m platform_id;
				// @DOCLINE * `@NLFT encoding_id` - equivalent to "encodingID" in the NameRecord subtable of the name table.
				uint16_m encoding_id;
				// @DOCLINE * `@NLFT language_id` - equivalent to "languageID" in the NameRecord subtable of the name table.
				uint16_m language_id;
				// @DOCLINE * `@NLFT name_id` - equivalent to "nameID" in the NameRecord subtable of the name table.
				uint16_m name_id;
				// @DOCLINE * `@NLFT length` - equivalent to "length" in the NameRecord subtable of the name table.
				uint16_m length;
				// @DOCLINE * `@NLFT string_offset` - equivalent to "stringOffset" in the NameRecord subtable of the name table.
				uint16_m string_offset;
			};

			// @DOCLINE The raw data for the string can be accessed via `&muttName.storage[muttNameRecord.string_offset]`.

			// @DOCLINE ### Lang tag record
			// @DOCLINE The struct `muttLangTagRecord` represents a LangTagRecord in the name table. It has the following members:
			struct muttLangTagRecord {
				// @DOCLINE * `@NLFT length` - equivalent to "length" in the LangTagRecord subtable of the name table.
				uint16_m length;
				// @DOCLINE * `@NLFT lang_tag_offset` - equivalent to "langTagOffset" in the LangTagRecord subtable of the name table.
				uint16_m lang_tag_offset;
			};

			// @DOCLINE The raw data for the language tag string can be accessed via `&muttName.storage[muttLangTagRecord.lang_tag_offset]`.

			// @DOCLINE ### Name ID macros

			// @DOCLINE The following macros are defined for getting the meaning of some name IDs:

			// @DOCLINE * [00] `MUTT_NAME_ID_COPYRIGHT`
			#define MUTT_NAME_ID_COPYRIGHT 0
			// @DOCLINE * [01] `MUTT_NAME_ID_FONT_FAMILY`
			#define MUTT_NAME_ID_FONT_FAMILY 1
			// @DOCLINE * [02] `MUTT_NAME_ID_FONT_SUBFAMILY`
			#define MUTT_NAME_ID_FONT_SUBFAMILY 2
			// @DOCLINE * [03] `MUTT_NAME_ID_UNIQUE`
			#define MUTT_NAME_ID_UNIQUE 3
			// @DOCLINE * [04] `MUTT_NAME_ID_FONT_NAME`
			#define MUTT_NAME_ID_FONT_NAME 4
			// @DOCLINE * [05] `MUTT_NAME_ID_VERSION`
			#define MUTT_NAME_ID_VERSION 5
			// @DOCLINE * [06] `MUTT_NAME_ID_POSTSCRIPT_NAME`
			#define MUTT_NAME_ID_POSTSCRIPT_NAME 6
			// @DOCLINE * [07] `MUTT_NAME_ID_TRADEMARK`
			#define MUTT_NAME_ID_TRADEMARK 7
			// @DOCLINE * [08] `MUTT_NAME_ID_MANUFACTURER`
			#define MUTT_NAME_ID_MANUFACTURER 8
			// @DOCLINE * [09] `MUTT_NAME_ID_DESIGNER`
			#define MUTT_NAME_ID_DESIGNER 9
			// @DOCLINE * [10] `MUTT_NAME_ID_DESCRIPTION`
			#define MUTT_NAME_ID_DESCRIPTION 10
			// @DOCLINE * [11] `MUTT_NAME_ID_VENDOR_URL`
			#define MUTT_NAME_ID_VENDOR_URL 11
			// @DOCLINE * [12] `MUTT_NAME_ID_DESIGNER_URL`
			#define MUTT_NAME_ID_DESIGNER_URL 12
			// @DOCLINE * [13] `MUTT_NAME_ID_LICENSE_DESCRIPTION`
			#define MUTT_NAME_ID_LICENSE_DESCRIPTION 13
			// @DOCLINE * [14] `MUTT_NAME_ID_LICENSE_URL`
			#define MUTT_NAME_ID_LICENSE_URL 14
			// @DOCLINE * [16] `MUTT_NAME_ID_TYPOGRAPHIC_FAMILY`
			#define MUTT_NAME_ID_TYPOGRAPHIC_FAMILY 16
			// @DOCLINE * [17] `MUTT_NAME_ID_TYPOGRAPHIC_SUBFAMILY`
			#define MUTT_NAME_ID_TYPOGRAPHIC_SUBFAMILY 17
			// @DOCLINE * [18] `MUTT_NAME_ID_COMPATIBLE`
			#define MUTT_NAME_ID_COMPATIBLE 18
			// @DOCLINE * [19] `MUTT_NAME_ID_SAMPLE_TEXT`
			#define MUTT_NAME_ID_SAMPLE_TEXT 19
			// @DOCLINE * [20] `MUTT_NAME_ID_FINDFONT`
			#define MUTT_NAME_ID_FINDFONT 20
			// @DOCLINE * [21] `MUTT_NAME_ID_WWS_FAMILY`
			#define MUTT_NAME_ID_WWS_FAMILY 21
			// @DOCLINE * [22] `MUTT_NAME_ID_WWS_SUBFAMILY`
			#define MUTT_NAME_ID_WWS_SUBFAMILY 22
			// @DOCLINE * [23] `MUTT_NAME_ID_LIGHT_BACKGROUND`
			#define MUTT_NAME_ID_LIGHT_BACKGROUND 23
			// @DOCLINE * [24] `MUTT_NAME_ID_DARK_BACKGROUND`
			#define MUTT_NAME_ID_DARK_BACKGROUND 24
			// @DOCLINE * [25] `MUTT_NAME_ID_VARIATIONS_PREFIX`
			#define MUTT_NAME_ID_VARIATIONS_PREFIX 25

			// @DOCLINE The exact meaning of these values can be found in the TrueType and OpenType documentation. Note that name IDs are not limited to just the values defined above.

			#ifdef MUTT_NAMES
			// @DOCLINE The function `mutt_name_id_get_name` returns a stringified version of a given name ID value, directly converting its macro equivalent to a string version (for example, `MUTT_NAME_ID_COPYRIGHT` turns into `"MUTT_NAME_ID_COPYRIGHT"`), defined below: @NLNT
			MUDEF const char* mutt_name_id_get_name(uint16_m name_id);

			// @DOCLINE The function `mutt_name_id_get_nice_name` returns a readable string version of a given name ID value, directly converting its macro equivalent to a readable string version (for example, `MUTT_NAME_ID_COPYRIGHT` turns into `"Copyright notice"`), defined below: @NLNT
			MUDEF const char* mutt_name_id_get_nice_name(uint16_m name_id);
			#endif

			// @DOCLINE Both above-listed functions returns `MUTT_UNKNOWN` if there is no defined macro equivalent for the given name ID value. Both functions are only defined if `MUTT_NAMES` is defined.

			// @DOCLINE ### Platform ID macros

			// @DOCLINE The following macros are defined for getting the meaning of some name IDs:

			// @DOCLINE * [0] `MUTT_PLATFORM_UNICODE`
			#define MUTT_PLATFORM_UNICODE 0
			// @DOCLINE * [1] `MUTT_PLATFORM_MACINTOSH`
			#define MUTT_PLATFORM_MACINTOSH 1
			// @DOCLINE * [2] `MUTT_PLATFORM_ISO`
			#define MUTT_PLATFORM_ISO 2
			// @DOCLINE * [3] `MUTT_PLATFORM_WINDOWS`
			#define MUTT_PLATFORM_WINDOWS 3
			// @DOCLINE * [4] `MUTT_PLATFORM_CUSTOM`
			#define MUTT_PLATFORM_CUSTOM 4

			// @DOCLINE All of the platform ID values defined above are the ones defined by TrueType and OpenType as of the writing of this, and are the only platform ID values compatible with mutt, and an error will be thrown for a table if a platform ID value is given that is not one of the values defined above.

			// @DOCLINE Note that `MUTT_PLATFORM_ISO` and `MUTT_PLATFORM_CUSTOM` are invalid values for a platform ID in a name table; they will not be a given value in the name table if it successfully loaded.

			// @DOCLINE More information about these values is available in the TrueType standard.

			#ifdef MUTT_NAMES
			// @DOCLINE The function `mutt_platform_id_get_name` returns a stringified version of a given platform ID value, directly converting its macro equivalent to a string version (for example, `MUTT_PLATFORM_UNICODE` turns into `"MUTT_PLATFORM_UNICODE"`), defined below: @NLNT
			MUDEF const char* mutt_platform_id_get_name(uint16_m platform_id);

			// @DOCLINE The function `mutt_platform_id_get_nice_name` returns a readable string version of a given platform ID value, directly converting its macro equivalent to a readable string version (for example, `MUTT_PLATFORM_UNICODE` turns into `"Unicode"`), defined below: @NLNT
			MUDEF const char* mutt_platform_id_get_nice_name(uint16_m platform_id);
			#endif

			// @DOCLINE Both above-listed functions returns `MUTT_UNKNOWN` if there is no defined macro equivalent for the given platform ID value. Both functions are only defined if `MUTT_NAMES` is defined.

			// @DOCLINE ### Unicode encoding ID macros

			// @DOCLINE The following macros are defined for interpreting Unicode encoding IDs:

			// @DOCLINE * [0] `MUTT_UNICODE_ENCODING_1_0`
			#define MUTT_UNICODE_ENCODING_1_0 0
			// @DOCLINE * [1] `MUTT_UNICODE_ENCODING_1_1`
			#define MUTT_UNICODE_ENCODING_1_1 1
			// @DOCLINE * [2] `MUTT_UNICODE_ENCODING_ISO_IEC_10646`
			#define MUTT_UNICODE_ENCODING_ISO_IEC_10646 2
			// @DOCLINE * [3] `MUTT_UNICODE_ENCODING_2_0_BMP`
			#define MUTT_UNICODE_ENCODING_2_0_BMP 3
			// @DOCLINE * [4] `MUTT_UNICODE_ENCODING_2_0_FULL`
			#define MUTT_UNICODE_ENCODING_2_0_FULL 4
			// @DOCLINE * [5] `MUTT_UNICODE_ENCODING_VARIATION`
			#define MUTT_UNICODE_ENCODING_VARIATION 5
			// @DOCLINE * [6] `MUTT_UNICODE_ENCODING_FULL`
			#define MUTT_UNICODE_ENCODING_FULL 6

			// @DOCLINE All of the Unicode encoding ID values defined above are the ones defined by TrueType and OpenType as of writing this, and are the only Unicode encoding ID values compatible with mutt, and an error will be thrown for a table if a Unicode encoding ID value is given that is not one of the values defined above.

			// @DOCLINE Note that `MUTT_UNICODE_ENCODING_VARIATION` and `MUTT_UNICODE_ENCODING_FULL` are invalid values for a Unicode encoding ID in a name table; they will not be a given value in the name table if it successfully loaded.

			// @DOCLINE More information about these values is available in the TrueType standard.

			#ifdef MUTT_NAMES
			// @DOCLINE The function `mutt_unicode_encoding_id_get_name` returns a stringified version of a given Unicode encoding ID value, directly converting its macro equivalent to a string version (for example, `MUTT_UNICODE_ENCODING_1_0` turns into `"MUTT_UNICODE_ENCODING_1_0"`), defined below: @NLNT
			MUDEF const char* mutt_unicode_encoding_id_get_name(uint16_m encoding_id);

			// @DOCLINE The function `mutt_unicode_encoding_id_get_nice_name` returns a readable string version of a given Unicode encoding ID value, directly converting its macro equivalent to a readable string version (for example, `MUTT_UNICODE_ENCODING_1_0` turns into `"Unicode 1.0"`), defined below: @NLNT
			MUDEF const char* mutt_unicode_encoding_id_get_nice_name(uint16_m encoding_id);
			#endif

			// @DOCLINE Both above-listed functions returns `MUTT_UNKNOWN` if there is no defined macro equivalent for the given Unicode encoding ID value. Both functions are only defined if `MUTT_NAMES` is defined.

			// @DOCLINE ### Macintosh encoding ID macros

			// @DOCLINE The following macros are defined for interpreting Macintosh encoding IDs:

			// @DOCLINE * [00] `MUTT_MACINTOSH_ENCODING_ROMAN`
			#define MUTT_MACINTOSH_ENCODING_ROMAN 0
			// @DOCLINE * [01] `MUTT_MACINTOSH_ENCODING_JAPANESE`
			#define MUTT_MACINTOSH_ENCODING_JAPANESE 1
			// @DOCLINE * [02] `MUTT_MACINTOSH_ENCODING_CHINESE_TRADITIONAL`
			#define MUTT_MACINTOSH_ENCODING_CHINESE_TRADITIONAL 2
			// @DOCLINE * [03] `MUTT_MACINTOSH_ENCODING_KOREAN`
			#define MUTT_MACINTOSH_ENCODING_KOREAN 3
			// @DOCLINE * [04] `MUTT_MACINTOSH_ENCODING_ARABIC`
			#define MUTT_MACINTOSH_ENCODING_ARABIC 4
			// @DOCLINE * [05] `MUTT_MACINTOSH_ENCODING_HEBREW`
			#define MUTT_MACINTOSH_ENCODING_HEBREW 5
			// @DOCLINE * [06] `MUTT_MACINTOSH_ENCODING_GREEK`
			#define MUTT_MACINTOSH_ENCODING_GREEK 6
			// @DOCLINE * [07] `MUTT_MACINTOSH_ENCODING_RUSSIAN`
			#define MUTT_MACINTOSH_ENCODING_RUSSIAN 7
			// @DOCLINE * [08] `MUTT_MACINTOSH_ENCODING_RSYMBOL`
			#define MUTT_MACINTOSH_ENCODING_RSYMBOL 8
			// @DOCLINE * [09] `MUTT_MACINTOSH_ENCODING_DEVANAGARI`
			#define MUTT_MACINTOSH_ENCODING_DEVANAGARI 9
			// @DOCLINE * [10] `MUTT_MACINTOSH_ENCODING_GURMUKHI`
			#define MUTT_MACINTOSH_ENCODING_GURMUKHI 10
			// @DOCLINE * [11] `MUTT_MACINTOSH_ENCODING_GUJARATI`
			#define MUTT_MACINTOSH_ENCODING_GUJARATI 11
			// @DOCLINE * [12] `MUTT_MACINTOSH_ENCODING_ODIA`
			#define MUTT_MACINTOSH_ENCODING_ODIA 12
			// @DOCLINE * [13] `MUTT_MACINTOSH_ENCODING_BANGLA`
			#define MUTT_MACINTOSH_ENCODING_BANGLA 13
			// @DOCLINE * [14] `MUTT_MACINTOSH_ENCODING_TAMIL`
			#define MUTT_MACINTOSH_ENCODING_TAMIL 14
			// @DOCLINE * [15] `MUTT_MACINTOSH_ENCODING_TELUGU`
			#define MUTT_MACINTOSH_ENCODING_TELUGU 15
			// @DOCLINE * [16] `MUTT_MACINTOSH_ENCODING_KANNADA`
			#define MUTT_MACINTOSH_ENCODING_KANNADA 16
			// @DOCLINE * [17] `MUTT_MACINTOSH_ENCODING_MALAYALAM`
			#define MUTT_MACINTOSH_ENCODING_MALAYALAM 17
			// @DOCLINE * [18] `MUTT_MACINTOSH_ENCODING_SINHALESE`
			#define MUTT_MACINTOSH_ENCODING_SINHALESE 18
			// @DOCLINE * [19] `MUTT_MACINTOSH_ENCODING_BURMESE`
			#define MUTT_MACINTOSH_ENCODING_BURMESE 19
			// @DOCLINE * [20] `MUTT_MACINTOSH_ENCODING_KHMER`
			#define MUTT_MACINTOSH_ENCODING_KHMER 20
			// @DOCLINE * [21] `MUTT_MACINTOSH_ENCODING_THAI`
			#define MUTT_MACINTOSH_ENCODING_THAI 21
			// @DOCLINE * [22] `MUTT_MACINTOSH_ENCODING_LAOTIAN`
			#define MUTT_MACINTOSH_ENCODING_LAOTIAN 22
			// @DOCLINE * [23] `MUTT_MACINTOSH_ENCODING_GEORGIAN`
			#define MUTT_MACINTOSH_ENCODING_GEORGIAN 23
			// @DOCLINE * [24] `MUTT_MACINTOSH_ENCODING_ARMENIAN`
			#define MUTT_MACINTOSH_ENCODING_ARMENIAN 24
			// @DOCLINE * [25] `MUTT_MACINTOSH_ENCODING_CHINESE_SIMPLIFIED`
			#define MUTT_MACINTOSH_ENCODING_CHINESE_SIMPLIFIED 25
			// @DOCLINE * [26] `MUTT_MACINTOSH_ENCODING_TIBETAN`
			#define MUTT_MACINTOSH_ENCODING_TIBETAN 26
			// @DOCLINE * [27] `MUTT_MACINTOSH_ENCODING_MONGOLIAN`
			#define MUTT_MACINTOSH_ENCODING_MONGOLIAN 27
			// @DOCLINE * [28] `MUTT_MACINTOSH_ENCODING_GEEZ`
			#define MUTT_MACINTOSH_ENCODING_GEEZ 28
			// @DOCLINE * [29] `MUTT_MACINTOSH_ENCODING_SLAVIC`
			#define MUTT_MACINTOSH_ENCODING_SLAVIC 29
			// @DOCLINE * [30] `MUTT_MACINTOSH_ENCODING_VIETNAMESE`
			#define MUTT_MACINTOSH_ENCODING_VIETNAMESE 30
			// @DOCLINE * [31] `MUTT_MACINTOSH_ENCODING_SINDHI`
			#define MUTT_MACINTOSH_ENCODING_SINDHI 31
			// @DOCLINE * [32] `MUTT_MACINTOSH_ENCODING_UNINTERPRETED`
			#define MUTT_MACINTOSH_ENCODING_UNINTERPRETED 32

			// @DOCLINE All of the Macintosh encoding ID values defined above are the ones defined by TrueType and OpenType as of writing this, and are the only Macintosh encoding ID values compatible with mutt, and an error will be thrown for a table if a Macintosh encoding ID value is given that is not one of the values defined above.

			// @DOCLINE More information about these values is available in the TrueType standard.

			#ifdef MUTT_NAMES
			// @DOCLINE The function `mutt_macintosh_encoding_id_get_name` returns a stringified version of a given Macintosh encoding ID value, directly converting its macro equivalent to a string version (for example, `MUTT_MACINTOSH_ENCODING_ROMAN` turns into `"MUTT_MACINTOSH_ENCODING_ROMAN"`), defined below: @NLNT
			MUDEF const char* mutt_macintosh_encoding_id_get_name(uint16_m encoding_id);

			// @DOCLINE The function `mutt_macintosh_encoding_id_get_nice_name` returns a readable string version of a given Macintosh encoding ID value, directly converting its macro equivalent to a readable string version (for example, `MUTT_MACINTOSH_ENCODING_ROMAN` turns into `"Roman"`), defined below: @NLNT
			MUDEF const char* mutt_macintosh_encoding_id_get_nice_name(uint16_m encoding_id);
			#endif

			// @DOCLINE Both above-listed functions returns `MUTT_UNKNOWN` if there is no defined macro equivalent for the given Macintosh encoding ID value. Both functions are only defined if `MUTT_NAMES` is defined.

			// @DOCLINE ### Macintosh language ID macros

			// @DOCLINE The following macros are defined for interpreting Macintosh language IDs:

			// @DOCLINE * [000] `MUTT_MACINTOSH_LANGUAGE_ENGLISH`
			#define MUTT_MACINTOSH_LANGUAGE_ENGLISH 0
			// @DOCLINE * [001] `MUTT_MACINTOSH_LANGUAGE_FRENCH`
			#define MUTT_MACINTOSH_LANGUAGE_FRENCH 1
			// @DOCLINE * [002] `MUTT_MACINTOSH_LANGUAGE_GERMAN`
			#define MUTT_MACINTOSH_LANGUAGE_GERMAN 2
			// @DOCLINE * [003] `MUTT_MACINTOSH_LANGUAGE_ITALIAN`
			#define MUTT_MACINTOSH_LANGUAGE_ITALIAN 3
			// @DOCLINE * [004] `MUTT_MACINTOSH_LANGUAGE_DUTCH`
			#define MUTT_MACINTOSH_LANGUAGE_DUTCH 4
			// @DOCLINE * [005] `MUTT_MACINTOSH_LANGUAGE_SWEDISH`
			#define MUTT_MACINTOSH_LANGUAGE_SWEDISH 5
			// @DOCLINE * [006] `MUTT_MACINTOSH_LANGUAGE_SPANISH`
			#define MUTT_MACINTOSH_LANGUAGE_SPANISH 6
			// @DOCLINE * [007] `MUTT_MACINTOSH_LANGUAGE_DANISH`
			#define MUTT_MACINTOSH_LANGUAGE_DANISH 7
			// @DOCLINE * [008] `MUTT_MACINTOSH_LANGUAGE_PORTUGUESE`
			#define MUTT_MACINTOSH_LANGUAGE_PORTUGUESE 8
			// @DOCLINE * [009] `MUTT_MACINTOSH_LANGUAGE_NORWEGIAN`
			#define MUTT_MACINTOSH_LANGUAGE_NORWEGIAN 9
			// @DOCLINE * [010] `MUTT_MACINTOSH_LANGUAGE_HEBREW`
			#define MUTT_MACINTOSH_LANGUAGE_HEBREW 10
			// @DOCLINE * [011] `MUTT_MACINTOSH_LANGUAGE_JAPANESE`
			#define MUTT_MACINTOSH_LANGUAGE_JAPANESE 11
			// @DOCLINE * [012] `MUTT_MACINTOSH_LANGUAGE_ARABIC`
			#define MUTT_MACINTOSH_LANGUAGE_ARABIC 12
			// @DOCLINE * [013] `MUTT_MACINTOSH_LANGUAGE_FINNISH`
			#define MUTT_MACINTOSH_LANGUAGE_FINNISH 13
			// @DOCLINE * [014] `MUTT_MACINTOSH_LANGUAGE_GREEK`
			#define MUTT_MACINTOSH_LANGUAGE_GREEK 14
			// @DOCLINE * [015] `MUTT_MACINTOSH_LANGUAGE_ICELANDIC`
			#define MUTT_MACINTOSH_LANGUAGE_ICELANDIC 15
			// @DOCLINE * [016] `MUTT_MACINTOSH_LANGUAGE_MALTESE`
			#define MUTT_MACINTOSH_LANGUAGE_MALTESE 16
			// @DOCLINE * [017] `MUTT_MACINTOSH_LANGUAGE_TURKISH`
			#define MUTT_MACINTOSH_LANGUAGE_TURKISH 17
			// @DOCLINE * [018] `MUTT_MACINTOSH_LANGUAGE_CROATIAN`
			#define MUTT_MACINTOSH_LANGUAGE_CROATIAN 18
			// @DOCLINE * [019] `MUTT_MACINTOSH_LANGUAGE_CHINESE_TRADITIONAL`
			#define MUTT_MACINTOSH_LANGUAGE_CHINESE_TRADITIONAL 19
			// @DOCLINE * [020] `MUTT_MACINTOSH_LANGUAGE_URDU`
			#define MUTT_MACINTOSH_LANGUAGE_URDU 20
			// @DOCLINE * [021] `MUTT_MACINTOSH_LANGUAGE_HINDI`
			#define MUTT_MACINTOSH_LANGUAGE_HINDI 21
			// @DOCLINE * [022] `MUTT_MACINTOSH_LANGUAGE_THAI`
			#define MUTT_MACINTOSH_LANGUAGE_THAI 22
			// @DOCLINE * [023] `MUTT_MACINTOSH_LANGUAGE_KOREAN`
			#define MUTT_MACINTOSH_LANGUAGE_KOREAN 23
			// @DOCLINE * [024] `MUTT_MACINTOSH_LANGUAGE_LITHUANIAN`
			#define MUTT_MACINTOSH_LANGUAGE_LITHUANIAN 24
			// @DOCLINE * [025] `MUTT_MACINTOSH_LANGUAGE_POLISH`
			#define MUTT_MACINTOSH_LANGUAGE_POLISH 25
			// @DOCLINE * [026] `MUTT_MACINTOSH_LANGUAGE_HUNGARIAN`
			#define MUTT_MACINTOSH_LANGUAGE_HUNGARIAN 26
			// @DOCLINE * [027] `MUTT_MACINTOSH_LANGUAGE_ESTONIAN`
			#define MUTT_MACINTOSH_LANGUAGE_ESTONIAN 27
			// @DOCLINE * [028] `MUTT_MACINTOSH_LANGUAGE_LATVIAN`
			#define MUTT_MACINTOSH_LANGUAGE_LATVIAN 28
			// @DOCLINE * [029] `MUTT_MACINTOSH_LANGUAGE_SAMI`
			#define MUTT_MACINTOSH_LANGUAGE_SAMI 29
			// @DOCLINE * [030] `MUTT_MACINTOSH_LANGUAGE_FAROESE`
			#define MUTT_MACINTOSH_LANGUAGE_FAROESE 30
			// @DOCLINE * [031] `MUTT_MACINTOSH_LANGUAGE_FARSI_PERSIAN`
			#define MUTT_MACINTOSH_LANGUAGE_FARSI_PERSIAN 31
			// @DOCLINE * [032] `MUTT_MACINTOSH_LANGUAGE_RUSSIAN`
			#define MUTT_MACINTOSH_LANGUAGE_RUSSIAN 32
			// @DOCLINE * [033] `MUTT_MACINTOSH_LANGUAGE_CHINESE_SIMPLIFIED`
			#define MUTT_MACINTOSH_LANGUAGE_CHINESE_SIMPLIFIED 33
			// @DOCLINE * [034] `MUTT_MACINTOSH_LANGUAGE_FLEMISH`
			#define MUTT_MACINTOSH_LANGUAGE_FLEMISH 34
			// @DOCLINE * [035] `MUTT_MACINTOSH_LANGUAGE_IRISH_GAELIC`
			#define MUTT_MACINTOSH_LANGUAGE_IRISH_GAELIC 35
			// @DOCLINE * [036] `MUTT_MACINTOSH_LANGUAGE_ALBANIAN`
			#define MUTT_MACINTOSH_LANGUAGE_ALBANIAN 36
			// @DOCLINE * [037] `MUTT_MACINTOSH_LANGUAGE_ROMANIAN`
			#define MUTT_MACINTOSH_LANGUAGE_ROMANIAN 37
			// @DOCLINE * [038] `MUTT_MACINTOSH_LANGUAGE_CZECH`
			#define MUTT_MACINTOSH_LANGUAGE_CZECH 38
			// @DOCLINE * [039] `MUTT_MACINTOSH_LANGUAGE_SLOVAK`
			#define MUTT_MACINTOSH_LANGUAGE_SLOVAK 39
			// @DOCLINE * [040] `MUTT_MACINTOSH_LANGUAGE_SLOVENIAN`
			#define MUTT_MACINTOSH_LANGUAGE_SLOVENIAN 40
			// @DOCLINE * [041] `MUTT_MACINTOSH_LANGUAGE_YIDDISH`
			#define MUTT_MACINTOSH_LANGUAGE_YIDDISH 41
			// @DOCLINE * [042] `MUTT_MACINTOSH_LANGUAGE_SERBIAN`
			#define MUTT_MACINTOSH_LANGUAGE_SERBIAN 42
			// @DOCLINE * [043] `MUTT_MACINTOSH_LANGUAGE_MACEDONIAN`
			#define MUTT_MACINTOSH_LANGUAGE_MACEDONIAN 43
			// @DOCLINE * [044] `MUTT_MACINTOSH_LANGUAGE_BULGARIAN`
			#define MUTT_MACINTOSH_LANGUAGE_BULGARIAN 44
			// @DOCLINE * [045] `MUTT_MACINTOSH_LANGUAGE_UKRAINIAN`
			#define MUTT_MACINTOSH_LANGUAGE_UKRAINIAN 45
			// @DOCLINE * [046] `MUTT_MACINTOSH_LANGUAGE_BYELORUSSIAN`
			#define MUTT_MACINTOSH_LANGUAGE_BYELORUSSIAN 46
			// @DOCLINE * [047] `MUTT_MACINTOSH_LANGUAGE_UZBEK`
			#define MUTT_MACINTOSH_LANGUAGE_UZBEK 47
			// @DOCLINE * [048] `MUTT_MACINTOSH_LANGUAGE_KAZAKH`
			#define MUTT_MACINTOSH_LANGUAGE_KAZAKH 48
			// @DOCLINE * [049] `MUTT_MACINTOSH_LANGUAGE_AZERBAIJANI_CYRILLIC`
			#define MUTT_MACINTOSH_LANGUAGE_AZERBAIJANI_CYRILLIC 49
			// @DOCLINE * [050] `MUTT_MACINTOSH_LANGUAGE_AZERBAIJANI_ARABIC`
			#define MUTT_MACINTOSH_LANGUAGE_AZERBAIJANI_ARABIC 50
			// @DOCLINE * [051] `MUTT_MACINTOSH_LANGUAGE_ARMENIAN`
			#define MUTT_MACINTOSH_LANGUAGE_ARMENIAN 51
			// @DOCLINE * [052] `MUTT_MACINTOSH_LANGUAGE_GEORGIAN`
			#define MUTT_MACINTOSH_LANGUAGE_GEORGIAN 52
			// @DOCLINE * [053] `MUTT_MACINTOSH_LANGUAGE_MOLDAVIAN`
			#define MUTT_MACINTOSH_LANGUAGE_MOLDAVIAN 53
			// @DOCLINE * [054] `MUTT_MACINTOSH_LANGUAGE_KIRGHIZ`
			#define MUTT_MACINTOSH_LANGUAGE_KIRGHIZ 54
			// @DOCLINE * [055] `MUTT_MACINTOSH_LANGUAGE_TAJIKI`
			#define MUTT_MACINTOSH_LANGUAGE_TAJIKI 55
			// @DOCLINE * [056] `MUTT_MACINTOSH_LANGUAGE_TURKMEN`
			#define MUTT_MACINTOSH_LANGUAGE_TURKMEN 56
			// @DOCLINE * [057] `MUTT_MACINTOSH_LANGUAGE_MONGOLIAN`
			#define MUTT_MACINTOSH_LANGUAGE_MONGOLIAN 57
			// @DOCLINE * [058] `MUTT_MACINTOSH_LANGUAGE_MONGOLIAN_CYRILLIC`
			#define MUTT_MACINTOSH_LANGUAGE_MONGOLIAN_CYRILLIC 58
			// @DOCLINE * [059] `MUTT_MACINTOSH_LANGUAGE_PASHTO`
			#define MUTT_MACINTOSH_LANGUAGE_PASHTO 59
			// @DOCLINE * [060] `MUTT_MACINTOSH_LANGUAGE_KURDISH`
			#define MUTT_MACINTOSH_LANGUAGE_KURDISH 60
			// @DOCLINE * [061] `MUTT_MACINTOSH_LANGUAGE_KASHMIRI`
			#define MUTT_MACINTOSH_LANGUAGE_KASHMIRI 61
			// @DOCLINE * [062] `MUTT_MACINTOSH_LANGUAGE_SINDHI`
			#define MUTT_MACINTOSH_LANGUAGE_SINDHI 62
			// @DOCLINE * [063] `MUTT_MACINTOSH_LANGUAGE_TIBETAN`
			#define MUTT_MACINTOSH_LANGUAGE_TIBETAN 63
			// @DOCLINE * [064] `MUTT_MACINTOSH_LANGUAGE_NEPALI`
			#define MUTT_MACINTOSH_LANGUAGE_NEPALI 64
			// @DOCLINE * [065] `MUTT_MACINTOSH_LANGUAGE_SANSKRIT`
			#define MUTT_MACINTOSH_LANGUAGE_SANSKRIT 65
			// @DOCLINE * [066] `MUTT_MACINTOSH_LANGUAGE_MARATHI`
			#define MUTT_MACINTOSH_LANGUAGE_MARATHI 66
			// @DOCLINE * [067] `MUTT_MACINTOSH_LANGUAGE_BENGALI`
			#define MUTT_MACINTOSH_LANGUAGE_BENGALI 67
			// @DOCLINE * [068] `MUTT_MACINTOSH_LANGUAGE_ASSAMESE`
			#define MUTT_MACINTOSH_LANGUAGE_ASSAMESE 68
			// @DOCLINE * [069] `MUTT_MACINTOSH_LANGUAGE_GUJARATI`
			#define MUTT_MACINTOSH_LANGUAGE_GUJARATI 69
			// @DOCLINE * [070] `MUTT_MACINTOSH_LANGUAGE_PUNJABI`
			#define MUTT_MACINTOSH_LANGUAGE_PUNJABI 70
			// @DOCLINE * [071] `MUTT_MACINTOSH_LANGUAGE_ORIYA`
			#define MUTT_MACINTOSH_LANGUAGE_ORIYA 71
			// @DOCLINE * [072] `MUTT_MACINTOSH_LANGUAGE_MALAYALAM`
			#define MUTT_MACINTOSH_LANGUAGE_MALAYALAM 72
			// @DOCLINE * [073] `MUTT_MACINTOSH_LANGUAGE_KANNADA`
			#define MUTT_MACINTOSH_LANGUAGE_KANNADA 73
			// @DOCLINE * [074] `MUTT_MACINTOSH_LANGUAGE_TAMIL`
			#define MUTT_MACINTOSH_LANGUAGE_TAMIL 74
			// @DOCLINE * [075] `MUTT_MACINTOSH_LANGUAGE_TELUGU`
			#define MUTT_MACINTOSH_LANGUAGE_TELUGU 75
			// @DOCLINE * [076] `MUTT_MACINTOSH_LANGUAGE_SINHALESE`
			#define MUTT_MACINTOSH_LANGUAGE_SINHALESE 76
			// @DOCLINE * [077] `MUTT_MACINTOSH_LANGUAGE_BURMESE`
			#define MUTT_MACINTOSH_LANGUAGE_BURMESE 77
			// @DOCLINE * [078] `MUTT_MACINTOSH_LANGUAGE_KHMER`
			#define MUTT_MACINTOSH_LANGUAGE_KHMER 78
			// @DOCLINE * [079] `MUTT_MACINTOSH_LANGUAGE_LAO`
			#define MUTT_MACINTOSH_LANGUAGE_LAO 79
			// @DOCLINE * [080] `MUTT_MACINTOSH_LANGUAGE_VIETNAMESE`
			#define MUTT_MACINTOSH_LANGUAGE_VIETNAMESE 80
			// @DOCLINE * [081] `MUTT_MACINTOSH_LANGUAGE_INDONESIAN`
			#define MUTT_MACINTOSH_LANGUAGE_INDONESIAN 81
			// @DOCLINE * [082] `MUTT_MACINTOSH_LANGUAGE_TAGALOG`
			#define MUTT_MACINTOSH_LANGUAGE_TAGALOG 82
			// @DOCLINE * [083] `MUTT_MACINTOSH_LANGUAGE_MALAY_ROMAN`
			#define MUTT_MACINTOSH_LANGUAGE_MALAY_ROMAN 83
			// @DOCLINE * [084] `MUTT_MACINTOSH_LANGUAGE_MALAY_ARABIC`
			#define MUTT_MACINTOSH_LANGUAGE_MALAY_ARABIC 84
			// @DOCLINE * [085] `MUTT_MACINTOSH_LANGUAGE_AMHARIC`
			#define MUTT_MACINTOSH_LANGUAGE_AMHARIC 85
			// @DOCLINE * [086] `MUTT_MACINTOSH_LANGUAGE_TIGRINYA`
			#define MUTT_MACINTOSH_LANGUAGE_TIGRINYA 86
			// @DOCLINE * [087] `MUTT_MACINTOSH_LANGUAGE_GALLA`
			#define MUTT_MACINTOSH_LANGUAGE_GALLA 87
			// @DOCLINE * [088] `MUTT_MACINTOSH_LANGUAGE_SOMALI`
			#define MUTT_MACINTOSH_LANGUAGE_SOMALI 88
			// @DOCLINE * [089] `MUTT_MACINTOSH_LANGUAGE_SWAHILI`
			#define MUTT_MACINTOSH_LANGUAGE_SWAHILI 89
			// @DOCLINE * [090] `MUTT_MACINTOSH_LANGUAGE_KINYARWANDA_RUANDA`
			#define MUTT_MACINTOSH_LANGUAGE_KINYARWANDA_RUANDA 90
			// @DOCLINE * [091] `MUTT_MACINTOSH_LANGUAGE_RUNDI`
			#define MUTT_MACINTOSH_LANGUAGE_RUNDI 91
			// @DOCLINE * [092] `MUTT_MACINTOSH_LANGUAGE_NYANJA_CHEWA`
			#define MUTT_MACINTOSH_LANGUAGE_NYANJA_CHEWA 92
			// @DOCLINE * [093] `MUTT_MACINTOSH_LANGUAGE_MALAGASY`
			#define MUTT_MACINTOSH_LANGUAGE_MALAGASY 93
			// @DOCLINE * [094] `MUTT_MACINTOSH_LANGUAGE_ESPERANTO`
			#define MUTT_MACINTOSH_LANGUAGE_ESPERANTO 94
			// @DOCLINE * [128] `MUTT_MACINTOSH_LANGUAGE_WELSH`
			#define MUTT_MACINTOSH_LANGUAGE_WELSH 128
			// @DOCLINE * [129] `MUTT_MACINTOSH_LANGUAGE_BASQUE`
			#define MUTT_MACINTOSH_LANGUAGE_BASQUE 129
			// @DOCLINE * [130] `MUTT_MACINTOSH_LANGUAGE_CATALAN`
			#define MUTT_MACINTOSH_LANGUAGE_CATALAN 130
			// @DOCLINE * [131] `MUTT_MACINTOSH_LANGUAGE_LATIN`
			#define MUTT_MACINTOSH_LANGUAGE_LATIN 131
			// @DOCLINE * [132] `MUTT_MACINTOSH_LANGUAGE_QUECHUA`
			#define MUTT_MACINTOSH_LANGUAGE_QUECHUA 132
			// @DOCLINE * [133] `MUTT_MACINTOSH_LANGUAGE_GUARANI`
			#define MUTT_MACINTOSH_LANGUAGE_GUARANI 133
			// @DOCLINE * [134] `MUTT_MACINTOSH_LANGUAGE_AYMARA`
			#define MUTT_MACINTOSH_LANGUAGE_AYMARA 134
			// @DOCLINE * [135] `MUTT_MACINTOSH_LANGUAGE_TATAR`
			#define MUTT_MACINTOSH_LANGUAGE_TATAR 135
			// @DOCLINE * [136] `MUTT_MACINTOSH_LANGUAGE_UIGHUR`
			#define MUTT_MACINTOSH_LANGUAGE_UIGHUR 136
			// @DOCLINE * [137] `MUTT_MACINTOSH_LANGUAGE_DZONGKHA`
			#define MUTT_MACINTOSH_LANGUAGE_DZONGKHA 137
			// @DOCLINE * [138] `MUTT_MACINTOSH_LANGUAGE_JAVANESE`
			#define MUTT_MACINTOSH_LANGUAGE_JAVANESE 138
			// @DOCLINE * [139] `MUTT_MACINTOSH_LANGUAGE_SUNDANESE`
			#define MUTT_MACINTOSH_LANGUAGE_SUNDANESE 139
			// @DOCLINE * [140] `MUTT_MACINTOSH_LANGUAGE_GALICIAN`
			#define MUTT_MACINTOSH_LANGUAGE_GALICIAN 140
			// @DOCLINE * [141] `MUTT_MACINTOSH_LANGUAGE_AFRIKAANS`
			#define MUTT_MACINTOSH_LANGUAGE_AFRIKAANS 141
			// @DOCLINE * [142] `MUTT_MACINTOSH_LANGUAGE_BRETON`
			#define MUTT_MACINTOSH_LANGUAGE_BRETON 142
			// @DOCLINE * [143] `MUTT_MACINTOSH_LANGUAGE_INUKTITUT`
			#define MUTT_MACINTOSH_LANGUAGE_INUKTITUT 143
			// @DOCLINE * [144] `MUTT_MACINTOSH_LANGUAGE_SCOTTISH_GAELIC`
			#define MUTT_MACINTOSH_LANGUAGE_SCOTTISH_GAELIC 144
			// @DOCLINE * [145] `MUTT_MACINTOSH_LANGUAGE_MANX_GAELIC`
			#define MUTT_MACINTOSH_LANGUAGE_MANX_GAELIC 145
			// @DOCLINE * [146] `MUTT_MACINTOSH_LANGUAGE_IRISH_GAELIC_DOT`
			#define MUTT_MACINTOSH_LANGUAGE_IRISH_GAELIC_DOT 146
			// @DOCLINE * [147] `MUTT_MACINTOSH_LANGUAGE_TONGAN`
			#define MUTT_MACINTOSH_LANGUAGE_TONGAN 147
			// @DOCLINE * [148] `MUTT_MACINTOSH_LANGUAGE_GREEK_POLYTONIC`
			#define MUTT_MACINTOSH_LANGUAGE_GREEK_POLYTONIC 148
			// @DOCLINE * [149] `MUTT_MACINTOSH_LANGUAGE_GREENLANDIC`
			#define MUTT_MACINTOSH_LANGUAGE_GREENLANDIC 149
			// @DOCLINE * [150] `MUTT_MACINTOSH_LANGUAGE_AZERBAIJANI`
			#define MUTT_MACINTOSH_LANGUAGE_AZERBAIJANI 150

			// @DOCLINE All of the Macintosh language ID values defined above are the ones defined by TrueType and OpenType as of writing this, and are the only Macintosh language ID values compatible with mutt, and an error will be thrown for a table if a Macintosh language ID value is given that is not one of the values defined above.

			// @DOCLINE More information about these values is available in the TrueType standard.

			#ifdef MUTT_NAMES
			// @DOCLINE The function `mutt_macintosh_language_id_get_name` returns a stringified version of a given Macintosh language ID value, directly converting its macro equivalent to a string version (for example, `MUTT_MACINTOSH_LANGUAGE_ENGLISH` turns into `"MUTT_MACINTOSH_LANGUAGE_ENGLISH"`), defined below: @NLNT
			MUDEF const char* mutt_macintosh_language_id_get_name(uint16_m language_id);

			// @DOCLINE The function `mutt_macintosh_language_id_get_nice_name` returns a readable string version of a given Macintosh language ID value, directly converting its macro equivalent to a readable string version (for example, `MUTT_MACINTOSH_LANGUAGE_ENGLISH` turns into `"English"`), defined below: @NLNT
			MUDEF const char* mutt_macintosh_language_id_get_nice_name(uint16_m language_id);
			#endif

			// @DOCLINE Both above-listed functions returns `MUTT_UNKNOWN` if there is no defined macro equivalent for the given Macintosh language ID value. Both functions are only defined if `MUTT_NAMES` is defined.

			// @DOCLINE ### ISO encoding ID macros

			// @DOCLINE The following macros are defined for interpreting ISO encoding IDs:

			// @DOCLINE * [0] `MUTT_ISO_ENCODING_7_BIT_ASCII`
			#define MUTT_ISO_ENCODING_7_BIT_ASCII 0
			// @DOCLINE * [1] `MUTT_ISO_ENCODING_10646`
			#define MUTT_ISO_ENCODING_10646 1
			// @DOCLINE * [2] `MUTT_ISO_ENCODING_8859_1`
			#define MUTT_ISO_ENCODING_8859_1 2

			// @DOCLINE All of the ISO encoding ID values defined above are the ones defined by TrueType and OpenType as of writing this, and are the only ISO encoding ID values compatible with mutt, and an error will be thrown for a table if an ISO encoding ID value is given that is not one of the values defined above.

			// @DOCLINE More information about these values is available in the TrueType standard.

			#ifdef MUTT_NAMES
			// @DOCLINE The function `mutt_iso_encoding_id_get_name` returns a stringified version of a given ISO encoding ID value, directly converting its macro equivalent to a string version (for example, `MUTT_ISO_ENCODING_7_BIT_ASCII` turns into `"MUTT_ISO_ENCODING_7_BIT_ASCII"`), defined below: @NLNT
			MUDEF const char* mutt_iso_encoding_id_get_name(uint16_m encoding_id);

			// @DOCLINE The function `mutt_iso_encoding_id_get_nice_name` returns a readable string version of a given ISO encoding ID value, directly converting its macro equivalent to a readable string version (for example, `MUTT_ISO_ENCODING_7_BIT_ASCII` turns into `"7-bit ASCII"`), defined below: @NLNT
			MUDEF const char* mutt_iso_encoding_id_get_nice_name(uint16_m encoding_id);
			#endif

			// @DOCLINE Both above-listed functions returns `MUTT_UNKNOWN` if there is no defined macro equivalent for the given ISO encoding ID value. Both functions are only defined if `MUTT_NAMES` is defined.

			// @DOCLINE ### Windows encoding ID macros

			// @DOCLINE The following macros are defined for interpreting Windows encoding IDs:

			// @DOCLINE * [00] `MUTT_WINDOWS_ENCODING_SYMBOL`
			#define MUTT_WINDOWS_ENCODING_SYMBOL 0
			// @DOCLINE * [01] `MUTT_WINDOWS_ENCODING_UNICODE_BMP`
			#define MUTT_WINDOWS_ENCODING_UNICODE_BMP 1
			// @DOCLINE * [02] `MUTT_WINDOWS_ENCODING_SHIFT_JIS`
			#define MUTT_WINDOWS_ENCODING_SHIFT_JIS 2
			// @DOCLINE * [03] `MUTT_WINDOWS_ENCODING_PRC`
			#define MUTT_WINDOWS_ENCODING_PRC 3
			// @DOCLINE * [04] `MUTT_WINDOWS_ENCODING_BIG5`
			#define MUTT_WINDOWS_ENCODING_BIG5 4
			// @DOCLINE * [05] `MUTT_WINDOWS_ENCODING_WANSUNG`
			#define MUTT_WINDOWS_ENCODING_WANSUNG 5
			// @DOCLINE * [06] `MUTT_WINDOWS_ENCODING_JOHAB`
			#define MUTT_WINDOWS_ENCODING_JOHAB 6
			// @DOCLINE * [10] `MUTT_WINDOWS_ENCODING_UNICODE_FULL`
			#define MUTT_WINDOWS_ENCODING_UNICODE_FULL 10

			// @DOCLINE All of the Windows encoding ID values defined above are the ones defined by TrueType and OpenType as of writing this, and are the only Windows encoding ID values compatible with mutt, and an error will be thrown for a table if a Windows encoding ID value is given that is not one of the values defined above.

			// @DOCLINE More information about these values is available in the OpenType standard.

			#ifdef MUTT_NAMES
			// @DOCLINE The function `mutt_windows_encoding_id_get_name` returns a stringified version of a given Windows encoding ID value, directly converting its macro equivalent to a string version (for example, `MUTT_WINDOWS_ENCODING_SYMBOL` turns into `"MUTT_WINDOWS_ENCODING_SYMBOL"`), defined below: @NLNT
			MUDEF const char* mutt_windows_encoding_id_get_name(uint16_m encoding_id);

			// @DOCLINE The function `mutt_windows_encoding_id_get_nice_name` returns a readable string version of a given Windows encoding ID value, directly converting its macro equivalent to a readable string version (for example, `MUTT_WINDOWS_ENCODING_SYMBOL` turns into `"Symbol"`), defined below: @NLNT
			MUDEF const char* mutt_windows_encoding_id_get_nice_name(uint16_m encoding_id);
			#endif

			// @DOCLINE Both above-listed functions returns `MUTT_UNKNOWN` if there is no defined macro equivalent for the given Windows encoding ID value. Both functions are only defined if `MUTT_NAMES` is defined.

	// @DOCLINE # Version macros

		// @DOCLINE mutt defines three macros to define the version of mutt: `MUTT_VERSION_MAJOR`, `MUTT_VERSION_MINOR`, and `MUTT_VERSION_PATCH`, following the format of `vMAJOR.MINOR.PATCH`.

		#define MUTT_VERSION_MAJOR 1
		#define MUTT_VERSION_MINOR 0
		#define MUTT_VERSION_PATCH 0

	// @DOCLINE # C standard library dependencies

		// @DOCLINE mutt has several C standard library dependencies not provided by its other library dependencies, all of which are overridable by defining them before the inclusion of its header. This is a list of all of those dependencies.

		#if !defined(mu_malloc) || \
			!defined(mu_free) || \
			!defined(mu_realloc)

			// @DOCLINE ## `stdlib.h` dependencies
			#include <stdlib.h>

			// @DOCLINE * `mu_malloc` - equivalent to `malloc`.
			#ifndef mu_malloc
				#define mu_malloc malloc
			#endif

			// @DOCLINE * `mu_free` - equivalent to `free`.
			#ifndef mu_free
				#define mu_free free
			#endif

			// @DOCLINE * `mu_realloc` - equivalent to `realloc`.
			#ifndef mu_realloc
				#define mu_realloc realloc
			#endif

		#endif

		#if !defined(mu_memcpy)

			// @DOCLINE ## `string.h` dependencies
			#include <string.h>

			// @DOCLINE * `mu_memcpy` - equivalent to `memcpy`.
			#ifndef mu_memcpy
				#define mu_memcpy memcpy
			#endif

		#endif

		#if !defined(mu_pow)

			// @DOCLINE ## `math.h` dependencies
			#include <math.h>

			// @DOCLINE * `mu_pow` - equivalent to `pow`.
			#ifndef mu_pow
				#define mu_pow pow
			#endif

		#endif

	#ifdef __cplusplus
	}
	#endif

#endif /* MUTT_H */

/* Names */

	#ifdef MUTT_NAMES

	MUDEF const char* mutt_result_get_name(muttResult result) {
		switch (result) {
			default: return "MUTT_UNKNOWN"; break;
			case MUTT_SUCCESS: return "MUTT_SUCCESS"; break;
			case MUTT_FAILED_MALLOC: return "MUTT_FAILED_MALLOC"; break;
			case MUTT_FAILED_REALLOC: return "MUTT_FAILED_REALLOC"; break;
			case MUTT_UNFOUND_TABLE: return "MUTT_UNFOUND_TABLE"; break;
			case MUTT_DUPLICATE_TABLE: return "MUTT_DUPLICATE_TABLE"; break;
			case MUTT_INVALID_TABLE_DIRECTORY_LENGTH: return "MUTT_INVALID_TABLE_DIRECTORY_LENGTH"; break;
			case MUTT_INVALID_TABLE_DIRECTORY_SFNT_VERSION: return "MUTT_INVALID_TABLE_DIRECTORY_SFNT_VERSION"; break;
			case MUTT_INVALID_TABLE_DIRECTORY_SEARCH_RANGE: return "MUTT_INVALID_TABLE_DIRECTORY_SEARCH_RANGE"; break;
			case MUTT_INVALID_TABLE_DIRECTORY_ENTRY_SELECTOR: return "MUTT_INVALID_TABLE_DIRECTORY_ENTRY_SELECTOR"; break;
			case MUTT_INVALID_TABLE_DIRECTORY_RANGE_SHIFT: return "MUTT_INVALID_TABLE_DIRECTORY_RANGE_SHIFT"; break;
			case MUTT_INVALID_TABLE_RECORD_OFFSET: return "MUTT_INVALID_TABLE_RECORD_OFFSET"; break;
			case MUTT_INVALID_TABLE_RECORD_LENGTH: return "MUTT_INVALID_TABLE_RECORD_LENGTH"; break;
			case MUTT_INVALID_TABLE_RECORD_CHECKSUM: return "MUTT_INVALID_TABLE_RECORD_CHECKSUM"; break;
			case MUTT_INVALID_MAXP_LENGTH: return "MUTT_INVALID_MAXP_LENGTH"; break;
			case MUTT_INVALID_MAXP_VERSION: return "MUTT_INVALID_MAXP_VERSION"; break;
			case MUTT_INVALID_MAXP_MAX_ZONES: return "MUTT_INVALID_MAXP_MAX_ZONES"; break;
			case MUTT_INVALID_HEAD_LENGTH: return "MUTT_INVALID_HEAD_LENGTH"; break;
			case MUTT_INVALID_HEAD_VERSION: return "MUTT_INVALID_HEAD_VERSION"; break;
			case MUTT_INVALID_HEAD_MAGIC_NUMBER: return "MUTT_INVALID_HEAD_MAGIC_NUMBER"; break;
			case MUTT_INVALID_HEAD_UNITS_PER_EM: return "MUTT_INVALID_HEAD_UNITS_PER_EM"; break;
			case MUTT_INVALID_HEAD_X_MIN_MAX: return "MUTT_INVALID_HEAD_X_MIN_MAX"; break;
			case MUTT_INVALID_HEAD_Y_MIN_MAX: return "MUTT_INVALID_HEAD_Y_MIN_MAX"; break;
			case MUTT_INVALID_HEAD_INDEX_TO_LOC_FORMAT: return "MUTT_INVALID_HEAD_INDEX_TO_LOC_FORMAT"; break;
			case MUTT_INVALID_HEAD_GLYPH_DATA_FORMAT: return "MUTT_INVALID_HEAD_GLYPH_DATA_FORMAT"; break;
			case MUTT_INVALID_HHEA_LENGTH: return "MUTT_INVALID_HHEA_LENGTH"; break;
			case MUTT_INVALID_HHEA_VERSION: return "MUTT_INVALID_HHEA_VERSION"; break;
			case MUTT_INVALID_HHEA_METRIC_DATA_FORMAT: return "MUTT_INVALID_HHEA_METRIC_DATA_FORMAT"; break;
			case MUTT_INVALID_HHEA_NUMBER_OF_HMETRICS: return "MUTT_INVALID_HHEA_NUMBER_OF_HMETRICS"; break;
			case MUTT_INVALID_HMTX_LENGTH: return "MUTT_INVALID_HMTX_LENGTH"; break;
			case MUTT_INVALID_LOCA_LENGTH: return "MUTT_INVALID_LOCA_LENGTH"; break;
			case MUTT_INVALID_POST_LENGTH: return "MUTT_INVALID_POST_LENGTH"; break;
			case MUTT_INVALID_POST_VERSION: return "MUTT_INVALID_POST_VERSION"; break;
			case MUTT_INVALID_POST_GLYPH_NAME_INDEX: return "MUTT_INVALID_POST_GLYPH_NAME_INDEX"; break;
			case MUTT_INVALID_NAME_LENGTH: return "MUTT_INVALID_NAME_LENGTH"; break;
			case MUTT_INVALID_NAME_VERSION: return "MUTT_INVALID_NAME_VERSION"; break;
			case MUTT_INVALID_NAME_STORAGE_OFFSET: return "MUTT_INVALID_NAME_STORAGE_OFFSET"; break;
			case MUTT_INVALID_NAME_PLATFORM_ID: return "MUTT_INVALID_NAME_PLATFORM_ID"; break;
			case MUTT_INVALID_NAME_ENCODING_ID: return "MUTT_INVALID_NAME_ENCODING_ID"; break;
			case MUTT_INVALID_NAME_LANGUAGE_ID: return "MUTT_INVALID_NAME_LANGUAGE_ID"; break;
			case MUTT_INVALID_NAME_STRING_OFFSET: return "MUTT_INVALID_NAME_STRING_OFFSET"; break;
			case MUTT_INVALID_NAME_LANG_TAG_OFFSET: return "MUTT_INVALID_NAME_LANG_TAG_OFFSET"; break;
			case MUTT_HHEA_REQUIRES_MAXP: return "MUTT_HHEA_REQUIRES_MAXP"; break;
			case MUTT_HMTX_REQUIRES_MAXP: return "MUTT_HMTX_REQUIRES_MAXP"; break;
			case MUTT_HMTX_REQUIRES_HHEA: return "MUTT_HMTX_REQUIRES_HHEA"; break;
			case MUTT_LOCA_REQUIRES_HEAD: return "MUTT_LOCA_REQUIRES_HEAD"; break;
			case MUTT_LOCA_REQUIRES_MAXP: return "MUTT_LOCA_REQUIRES_MAXP"; break;
		}
	}

	MUDEF const char* mutt_name_id_get_name(uint16_m name_id) {
		switch (name_id) {
			default: return "MUTT_UNKNOWN"; break;
			case MUTT_NAME_ID_COPYRIGHT: return "MUTT_NAME_ID_COPYRIGHT"; break;
			case MUTT_NAME_ID_FONT_FAMILY: return "MUTT_NAME_ID_FONT_FAMILY"; break;
			case MUTT_NAME_ID_FONT_SUBFAMILY: return "MUTT_NAME_ID_FONT_SUBFAMILY"; break;
			case MUTT_NAME_ID_UNIQUE: return "MUTT_NAME_ID_UNIQUE"; break;
			case MUTT_NAME_ID_FONT_NAME: return "MUTT_NAME_ID_FONT_NAME"; break;
			case MUTT_NAME_ID_VERSION: return "MUTT_NAME_ID_VERSION"; break;
			case MUTT_NAME_ID_POSTSCRIPT_NAME: return "MUTT_NAME_ID_POSTSCRIPT_NAME"; break;
			case MUTT_NAME_ID_TRADEMARK: return "MUTT_NAME_ID_TRADEMARK"; break;
			case MUTT_NAME_ID_MANUFACTURER: return "MUTT_NAME_ID_MANUFACTURER"; break;
			case MUTT_NAME_ID_DESIGNER: return "MUTT_NAME_ID_DESIGNER"; break;
			case MUTT_NAME_ID_DESCRIPTION: return "MUTT_NAME_ID_DESCRIPTION"; break;
			case MUTT_NAME_ID_VENDOR_URL: return "MUTT_NAME_ID_VENDOR_URL"; break;
			case MUTT_NAME_ID_DESIGNER_URL: return "MUTT_NAME_ID_DESIGNER_URL"; break;
			case MUTT_NAME_ID_LICENSE_DESCRIPTION: return "MUTT_NAME_ID_LICENSE_DESCRIPTION"; break;
			case MUTT_NAME_ID_LICENSE_URL: return "MUTT_NAME_ID_LICENSE_URL"; break;
			case MUTT_NAME_ID_TYPOGRAPHIC_FAMILY: return "MUTT_NAME_ID_TYPOGRAPHIC_FAMILY"; break;
			case MUTT_NAME_ID_TYPOGRAPHIC_SUBFAMILY: return "MUTT_NAME_ID_TYPOGRAPHIC_SUBFAMILY"; break;
			case MUTT_NAME_ID_COMPATIBLE: return "MUTT_NAME_ID_COMPATIBLE"; break;
			case MUTT_NAME_ID_SAMPLE_TEXT: return "MUTT_NAME_ID_SAMPLE_TEXT"; break;
			case MUTT_NAME_ID_FINDFONT: return "MUTT_NAME_ID_FINDFONT"; break;
			case MUTT_NAME_ID_WWS_FAMILY: return "MUTT_NAME_ID_WWS_FAMILY"; break;
			case MUTT_NAME_ID_WWS_SUBFAMILY: return "MUTT_NAME_ID_WWS_SUBFAMILY"; break;
			case MUTT_NAME_ID_LIGHT_BACKGROUND: return "MUTT_NAME_ID_LIGHT_BACKGROUND"; break;
			case MUTT_NAME_ID_DARK_BACKGROUND: return "MUTT_NAME_ID_DARK_BACKGROUND"; break;
			case MUTT_NAME_ID_VARIATIONS_PREFIX: return "MUTT_NAME_ID_VARIATIONS_PREFIX"; break;
		}
	}

	MUDEF const char* mutt_name_id_get_nice_name(uint16_m name_id) {
		switch (name_id) {
			default: return "MUTT_UNKNOWN"; break;
			case MUTT_NAME_ID_COPYRIGHT: return "Copyright notice"; break;
			case MUTT_NAME_ID_FONT_FAMILY: return "Font family name"; break;
			case MUTT_NAME_ID_FONT_SUBFAMILY: return "Font subfamily name"; break;
			case MUTT_NAME_ID_UNIQUE: return "Unique font identifier"; break;
			case MUTT_NAME_ID_FONT_NAME: return "Full font name"; break;
			case MUTT_NAME_ID_VERSION: return "Version"; break;
			case MUTT_NAME_ID_POSTSCRIPT_NAME: return "PostScript name"; break;
			case MUTT_NAME_ID_TRADEMARK: return "Trademark"; break;
			case MUTT_NAME_ID_MANUFACTURER: return "Manufacturer name"; break;
			case MUTT_NAME_ID_DESIGNER: return "Designer name"; break;
			case MUTT_NAME_ID_DESCRIPTION: return "Description"; break;
			case MUTT_NAME_ID_VENDOR_URL: return "Vendor URL"; break;
			case MUTT_NAME_ID_DESIGNER_URL: return "Designer URL"; break;
			case MUTT_NAME_ID_LICENSE_DESCRIPTION: return "License description"; break;
			case MUTT_NAME_ID_LICENSE_URL: return "License URL"; break;
			case MUTT_NAME_ID_TYPOGRAPHIC_FAMILY: return "Typographic family"; break;
			case MUTT_NAME_ID_TYPOGRAPHIC_SUBFAMILY: return "Typographic subfamily"; break;
			case MUTT_NAME_ID_COMPATIBLE: return "Compatible full"; break;
			case MUTT_NAME_ID_SAMPLE_TEXT: return "Sample text"; break;
			case MUTT_NAME_ID_FINDFONT: return "PostScript CID findfont"; break;
			case MUTT_NAME_ID_WWS_FAMILY: return "WWS family"; break;
			case MUTT_NAME_ID_WWS_SUBFAMILY: return "WWS subfamily"; break;
			case MUTT_NAME_ID_LIGHT_BACKGROUND: return "Light background"; break;
			case MUTT_NAME_ID_DARK_BACKGROUND: return "Dark background"; break;
			case MUTT_NAME_ID_VARIATIONS_PREFIX: return "PostScript variation prefix"; break;
		}
	}

	MUDEF const char* mutt_platform_id_get_name(uint16_m platform_id) {
		switch (platform_id) {
			default: return "MUTT_UNKNOWN"; break;
			case MUTT_PLATFORM_UNICODE: return "MUTT_PLATFORM_UNICODE"; break;
			case MUTT_PLATFORM_MACINTOSH: return "MUTT_PLATFORM_MACINTOSH"; break;
			case MUTT_PLATFORM_ISO: return "MUTT_PLATFORM_ISO"; break;
			case MUTT_PLATFORM_WINDOWS: return "MUTT_PLATFORM_WINDOWS"; break;
			case MUTT_PLATFORM_CUSTOM: return "MUTT_PLATFORM_CUSTOM"; break;
		}
	}

	MUDEF const char* mutt_platform_id_get_nice_name(uint16_m platform_id) {
		switch (platform_id) {
			default: return "MUTT_UNKNOWN"; break;
			case MUTT_PLATFORM_UNICODE: return "Unicode"; break;
			case MUTT_PLATFORM_MACINTOSH: return "Macintosh"; break;
			case MUTT_PLATFORM_ISO: return "ISO"; break;
			case MUTT_PLATFORM_WINDOWS: return "Windows"; break;
			case MUTT_PLATFORM_CUSTOM: return "Custom"; break;
		}
	}

	MUDEF const char* mutt_unicode_encoding_id_get_name(uint16_m encoding_id) {
		switch (encoding_id) {
			default: return "MUTT_UNKNOWN"; break;
			case MUTT_UNICODE_ENCODING_1_0: return "MUTT_UNICODE_ENCODING_1_0"; break;
			case MUTT_UNICODE_ENCODING_1_1: return "MUTT_UNICODE_ENCODING_1_1"; break;
			case MUTT_UNICODE_ENCODING_ISO_IEC_10646: return "MUTT_UNICODE_ENCODING_ISO_IEC_10646"; break;
			case MUTT_UNICODE_ENCODING_2_0_BMP: return "MUTT_UNICODE_ENCODING_2_0_BMP"; break;
			case MUTT_UNICODE_ENCODING_2_0_FULL: return "MUTT_UNICODE_ENCODING_2_0_FULL"; break;
			case MUTT_UNICODE_ENCODING_VARIATION: return "MUTT_UNICODE_ENCODING_VARIATION"; break;
			case MUTT_UNICODE_ENCODING_FULL: return "MUTT_UNICODE_ENCODING_FULL"; break;
		}
	}

	MUDEF const char* mutt_unicode_encoding_id_get_nice_name(uint16_m encoding_id) {
		switch (encoding_id) {
			default: return "MUTT_UNKNOWN"; break;
			case MUTT_UNICODE_ENCODING_1_0: return "Unicode 1.0"; break;
			case MUTT_UNICODE_ENCODING_1_1: return "Unicode 1.1"; break;
			case MUTT_UNICODE_ENCODING_ISO_IEC_10646: return "ISO/IEC 10646"; break;
			case MUTT_UNICODE_ENCODING_2_0_BMP: return "Unicode 2.0+ BMP"; break;
			case MUTT_UNICODE_ENCODING_2_0_FULL: return "Unicode 2.0+ full"; break;
			case MUTT_UNICODE_ENCODING_VARIATION: return "Unicode variation sequences"; break;
			case MUTT_UNICODE_ENCODING_FULL: return "Unicode full"; break;
		}
	}

	MUDEF const char* mutt_macintosh_encoding_id_get_name(uint16_m encoding_id) {
		switch (encoding_id) {
			default: return "MUTT_UNKNOWN"; break;
			case MUTT_MACINTOSH_ENCODING_ROMAN: return "MUTT_MACINTOSH_ENCODING_ROMAN"; break;
			case MUTT_MACINTOSH_ENCODING_JAPANESE: return "MUTT_MACINTOSH_ENCODING_JAPANESE"; break;
			case MUTT_MACINTOSH_ENCODING_CHINESE_TRADITIONAL: return "MUTT_MACINTOSH_ENCODING_CHINESE_TRADITIONAL"; break;
			case MUTT_MACINTOSH_ENCODING_KOREAN: return "MUTT_MACINTOSH_ENCODING_KOREAN"; break;
			case MUTT_MACINTOSH_ENCODING_ARABIC: return "MUTT_MACINTOSH_ENCODING_ARABIC"; break;
			case MUTT_MACINTOSH_ENCODING_HEBREW: return "MUTT_MACINTOSH_ENCODING_HEBREW"; break;
			case MUTT_MACINTOSH_ENCODING_GREEK: return "MUTT_MACINTOSH_ENCODING_GREEK"; break;
			case MUTT_MACINTOSH_ENCODING_RUSSIAN: return "MUTT_MACINTOSH_ENCODING_RUSSIAN"; break;
			case MUTT_MACINTOSH_ENCODING_RSYMBOL: return "MUTT_MACINTOSH_ENCODING_RSYMBOL"; break;
			case MUTT_MACINTOSH_ENCODING_DEVANAGARI: return "MUTT_MACINTOSH_ENCODING_DEVANAGARI"; break;
			case MUTT_MACINTOSH_ENCODING_GURMUKHI: return "MUTT_MACINTOSH_ENCODING_GURMUKHI"; break;
			case MUTT_MACINTOSH_ENCODING_GUJARATI: return "MUTT_MACINTOSH_ENCODING_GUJARATI"; break;
			case MUTT_MACINTOSH_ENCODING_ODIA: return "MUTT_MACINTOSH_ENCODING_ODIA"; break;
			case MUTT_MACINTOSH_ENCODING_BANGLA: return "MUTT_MACINTOSH_ENCODING_BANGLA"; break;
			case MUTT_MACINTOSH_ENCODING_TAMIL: return "MUTT_MACINTOSH_ENCODING_TAMIL"; break;
			case MUTT_MACINTOSH_ENCODING_TELUGU: return "MUTT_MACINTOSH_ENCODING_TELUGU"; break;
			case MUTT_MACINTOSH_ENCODING_KANNADA: return "MUTT_MACINTOSH_ENCODING_KANNADA"; break;
			case MUTT_MACINTOSH_ENCODING_MALAYALAM: return "MUTT_MACINTOSH_ENCODING_MALAYALAM"; break;
			case MUTT_MACINTOSH_ENCODING_SINHALESE: return "MUTT_MACINTOSH_ENCODING_SINHALESE"; break;
			case MUTT_MACINTOSH_ENCODING_BURMESE: return "MUTT_MACINTOSH_ENCODING_BURMESE"; break;
			case MUTT_MACINTOSH_ENCODING_KHMER: return "MUTT_MACINTOSH_ENCODING_KHMER"; break;
			case MUTT_MACINTOSH_ENCODING_THAI: return "MUTT_MACINTOSH_ENCODING_THAI"; break;
			case MUTT_MACINTOSH_ENCODING_LAOTIAN: return "MUTT_MACINTOSH_ENCODING_LAOTIAN"; break;
			case MUTT_MACINTOSH_ENCODING_GEORGIAN: return "MUTT_MACINTOSH_ENCODING_GEORGIAN"; break;
			case MUTT_MACINTOSH_ENCODING_ARMENIAN: return "MUTT_MACINTOSH_ENCODING_ARMENIAN"; break;
			case MUTT_MACINTOSH_ENCODING_CHINESE_SIMPLIFIED: return "MUTT_MACINTOSH_ENCODING_CHINESE_SIMPLIFIED"; break;
			case MUTT_MACINTOSH_ENCODING_TIBETAN: return "MUTT_MACINTOSH_ENCODING_TIBETAN"; break;
			case MUTT_MACINTOSH_ENCODING_MONGOLIAN: return "MUTT_MACINTOSH_ENCODING_MONGOLIAN"; break;
			case MUTT_MACINTOSH_ENCODING_GEEZ: return "MUTT_MACINTOSH_ENCODING_GEEZ"; break;
			case MUTT_MACINTOSH_ENCODING_SLAVIC: return "MUTT_MACINTOSH_ENCODING_SLAVIC"; break;
			case MUTT_MACINTOSH_ENCODING_VIETNAMESE: return "MUTT_MACINTOSH_ENCODING_VIETNAMESE"; break;
			case MUTT_MACINTOSH_ENCODING_SINDHI: return "MUTT_MACINTOSH_ENCODING_SINDHI"; break;
			case MUTT_MACINTOSH_ENCODING_UNINTERPRETED: return "MUTT_MACINTOSH_ENCODING_UNINTERPRETED"; break;
		}
	}

	MUDEF const char* mutt_macintosh_encoding_id_get_nice_name(uint16_m encoding_id) {
		switch (encoding_id) {
			default: return "MUTT_UNKNOWN"; break;
			case MUTT_MACINTOSH_ENCODING_ROMAN: return "Roman"; break;
			case MUTT_MACINTOSH_ENCODING_JAPANESE: return "Japanese"; break;
			case MUTT_MACINTOSH_ENCODING_CHINESE_TRADITIONAL: return "Chinese (Traditional)"; break;
			case MUTT_MACINTOSH_ENCODING_KOREAN: return "Korean"; break;
			case MUTT_MACINTOSH_ENCODING_ARABIC: return "Arabic"; break;
			case MUTT_MACINTOSH_ENCODING_HEBREW: return "Hebrew"; break;
			case MUTT_MACINTOSH_ENCODING_GREEK: return "Greek"; break;
			case MUTT_MACINTOSH_ENCODING_RUSSIAN: return "Russian"; break;
			case MUTT_MACINTOSH_ENCODING_RSYMBOL: return "RSymbol"; break;
			case MUTT_MACINTOSH_ENCODING_DEVANAGARI: return "Devanagari"; break;
			case MUTT_MACINTOSH_ENCODING_GURMUKHI: return "Gurmukhi"; break;
			case MUTT_MACINTOSH_ENCODING_GUJARATI: return "Gujarati"; break;
			case MUTT_MACINTOSH_ENCODING_ODIA: return "Odia"; break;
			case MUTT_MACINTOSH_ENCODING_BANGLA: return "Bangla"; break;
			case MUTT_MACINTOSH_ENCODING_TAMIL: return "Tamil"; break;
			case MUTT_MACINTOSH_ENCODING_TELUGU: return "Telugu"; break;
			case MUTT_MACINTOSH_ENCODING_KANNADA: return "Kannada"; break;
			case MUTT_MACINTOSH_ENCODING_MALAYALAM: return "Malayalam"; break;
			case MUTT_MACINTOSH_ENCODING_SINHALESE: return "Sinhalese"; break;
			case MUTT_MACINTOSH_ENCODING_BURMESE: return "Burmese"; break;
			case MUTT_MACINTOSH_ENCODING_KHMER: return "Khmer"; break;
			case MUTT_MACINTOSH_ENCODING_THAI: return "Thai"; break;
			case MUTT_MACINTOSH_ENCODING_LAOTIAN: return "Laotian"; break;
			case MUTT_MACINTOSH_ENCODING_GEORGIAN: return "Georgian"; break;
			case MUTT_MACINTOSH_ENCODING_ARMENIAN: return "Armenian"; break;
			case MUTT_MACINTOSH_ENCODING_CHINESE_SIMPLIFIED: return "Chinese (Simplified)"; break;
			case MUTT_MACINTOSH_ENCODING_TIBETAN: return "Tibetan"; break;
			case MUTT_MACINTOSH_ENCODING_MONGOLIAN: return "Mongolian"; break;
			case MUTT_MACINTOSH_ENCODING_GEEZ: return "Geez"; break;
			case MUTT_MACINTOSH_ENCODING_SLAVIC: return "Slavic"; break;
			case MUTT_MACINTOSH_ENCODING_VIETNAMESE: return "Vietnamese"; break;
			case MUTT_MACINTOSH_ENCODING_SINDHI: return "Sindhi"; break;
			case MUTT_MACINTOSH_ENCODING_UNINTERPRETED: return "Uninterpreted"; break;
		}
	}

	MUDEF const char* mutt_macintosh_language_id_get_name(uint16_m language_id) {
		switch (language_id) {
			default: return "MUTT_UNKNOWN"; break;
			case MUTT_MACINTOSH_LANGUAGE_ENGLISH: return "MUTT_MACINTOSH_LANGUAGE_ENGLISH"; break;
			case MUTT_MACINTOSH_LANGUAGE_FRENCH: return "MUTT_MACINTOSH_LANGUAGE_FRENCH"; break;
			case MUTT_MACINTOSH_LANGUAGE_GERMAN: return "MUTT_MACINTOSH_LANGUAGE_GERMAN"; break;
			case MUTT_MACINTOSH_LANGUAGE_ITALIAN: return "MUTT_MACINTOSH_LANGUAGE_ITALIAN"; break;
			case MUTT_MACINTOSH_LANGUAGE_DUTCH: return "MUTT_MACINTOSH_LANGUAGE_DUTCH"; break;
			case MUTT_MACINTOSH_LANGUAGE_SWEDISH: return "MUTT_MACINTOSH_LANGUAGE_SWEDISH"; break;
			case MUTT_MACINTOSH_LANGUAGE_SPANISH: return "MUTT_MACINTOSH_LANGUAGE_SPANISH"; break;
			case MUTT_MACINTOSH_LANGUAGE_DANISH: return "MUTT_MACINTOSH_LANGUAGE_DANISH"; break;
			case MUTT_MACINTOSH_LANGUAGE_PORTUGUESE: return "MUTT_MACINTOSH_LANGUAGE_PORTUGUESE"; break;
			case MUTT_MACINTOSH_LANGUAGE_NORWEGIAN: return "MUTT_MACINTOSH_LANGUAGE_NORWEGIAN"; break;
			case MUTT_MACINTOSH_LANGUAGE_HEBREW: return "MUTT_MACINTOSH_LANGUAGE_HEBREW"; break;
			case MUTT_MACINTOSH_LANGUAGE_JAPANESE: return "MUTT_MACINTOSH_LANGUAGE_JAPANESE"; break;
			case MUTT_MACINTOSH_LANGUAGE_ARABIC: return "MUTT_MACINTOSH_LANGUAGE_ARABIC"; break;
			case MUTT_MACINTOSH_LANGUAGE_FINNISH: return "MUTT_MACINTOSH_LANGUAGE_FINNISH"; break;
			case MUTT_MACINTOSH_LANGUAGE_GREEK: return "MUTT_MACINTOSH_LANGUAGE_GREEK"; break;
			case MUTT_MACINTOSH_LANGUAGE_ICELANDIC: return "MUTT_MACINTOSH_LANGUAGE_ICELANDIC"; break;
			case MUTT_MACINTOSH_LANGUAGE_MALTESE: return "MUTT_MACINTOSH_LANGUAGE_MALTESE"; break;
			case MUTT_MACINTOSH_LANGUAGE_TURKISH: return "MUTT_MACINTOSH_LANGUAGE_TURKISH"; break;
			case MUTT_MACINTOSH_LANGUAGE_CROATIAN: return "MUTT_MACINTOSH_LANGUAGE_CROATIAN"; break;
			case MUTT_MACINTOSH_LANGUAGE_CHINESE_TRADITIONAL: return "MUTT_MACINTOSH_LANGUAGE_CHINESE_TRADITIONAL"; break;
			case MUTT_MACINTOSH_LANGUAGE_URDU: return "MUTT_MACINTOSH_LANGUAGE_URDU"; break;
			case MUTT_MACINTOSH_LANGUAGE_HINDI: return "MUTT_MACINTOSH_LANGUAGE_HINDI"; break;
			case MUTT_MACINTOSH_LANGUAGE_THAI: return "MUTT_MACINTOSH_LANGUAGE_THAI"; break;
			case MUTT_MACINTOSH_LANGUAGE_KOREAN: return "MUTT_MACINTOSH_LANGUAGE_KOREAN"; break;
			case MUTT_MACINTOSH_LANGUAGE_LITHUANIAN: return "MUTT_MACINTOSH_LANGUAGE_LITHUANIAN"; break;
			case MUTT_MACINTOSH_LANGUAGE_POLISH: return "MUTT_MACINTOSH_LANGUAGE_POLISH"; break;
			case MUTT_MACINTOSH_LANGUAGE_HUNGARIAN: return "MUTT_MACINTOSH_LANGUAGE_HUNGARIAN"; break;
			case MUTT_MACINTOSH_LANGUAGE_ESTONIAN: return "MUTT_MACINTOSH_LANGUAGE_ESTONIAN"; break;
			case MUTT_MACINTOSH_LANGUAGE_LATVIAN: return "MUTT_MACINTOSH_LANGUAGE_LATVIAN"; break;
			case MUTT_MACINTOSH_LANGUAGE_SAMI: return "MUTT_MACINTOSH_LANGUAGE_SAMI"; break;
			case MUTT_MACINTOSH_LANGUAGE_FAROESE: return "MUTT_MACINTOSH_LANGUAGE_FAROESE"; break;
			case MUTT_MACINTOSH_LANGUAGE_FARSI_PERSIAN: return "MUTT_MACINTOSH_LANGUAGE_FARSI_PERSIAN"; break;
			case MUTT_MACINTOSH_LANGUAGE_RUSSIAN: return "MUTT_MACINTOSH_LANGUAGE_RUSSIAN"; break;
			case MUTT_MACINTOSH_LANGUAGE_CHINESE_SIMPLIFIED: return "MUTT_MACINTOSH_LANGUAGE_CHINESE_SIMPLIFIED"; break;
			case MUTT_MACINTOSH_LANGUAGE_FLEMISH: return "MUTT_MACINTOSH_LANGUAGE_FLEMISH"; break;
			case MUTT_MACINTOSH_LANGUAGE_IRISH_GAELIC: return "MUTT_MACINTOSH_LANGUAGE_IRISH_GAELIC"; break;
			case MUTT_MACINTOSH_LANGUAGE_ALBANIAN: return "MUTT_MACINTOSH_LANGUAGE_ALBANIAN"; break;
			case MUTT_MACINTOSH_LANGUAGE_ROMANIAN: return "MUTT_MACINTOSH_LANGUAGE_ROMANIAN"; break;
			case MUTT_MACINTOSH_LANGUAGE_CZECH: return "MUTT_MACINTOSH_LANGUAGE_CZECH"; break;
			case MUTT_MACINTOSH_LANGUAGE_SLOVAK: return "MUTT_MACINTOSH_LANGUAGE_SLOVAK"; break;
			case MUTT_MACINTOSH_LANGUAGE_SLOVENIAN: return "MUTT_MACINTOSH_LANGUAGE_SLOVENIAN"; break;
			case MUTT_MACINTOSH_LANGUAGE_YIDDISH: return "MUTT_MACINTOSH_LANGUAGE_YIDDISH"; break;
			case MUTT_MACINTOSH_LANGUAGE_SERBIAN: return "MUTT_MACINTOSH_LANGUAGE_SERBIAN"; break;
			case MUTT_MACINTOSH_LANGUAGE_MACEDONIAN: return "MUTT_MACINTOSH_LANGUAGE_MACEDONIAN"; break;
			case MUTT_MACINTOSH_LANGUAGE_BULGARIAN: return "MUTT_MACINTOSH_LANGUAGE_BULGARIAN"; break;
			case MUTT_MACINTOSH_LANGUAGE_UKRAINIAN: return "MUTT_MACINTOSH_LANGUAGE_UKRAINIAN"; break;
			case MUTT_MACINTOSH_LANGUAGE_BYELORUSSIAN: return "MUTT_MACINTOSH_LANGUAGE_BYELORUSSIAN"; break;
			case MUTT_MACINTOSH_LANGUAGE_UZBEK: return "MUTT_MACINTOSH_LANGUAGE_UZBEK"; break;
			case MUTT_MACINTOSH_LANGUAGE_KAZAKH: return "MUTT_MACINTOSH_LANGUAGE_KAZAKH"; break;
			case MUTT_MACINTOSH_LANGUAGE_AZERBAIJANI_CYRILLIC: return "MUTT_MACINTOSH_LANGUAGE_AZERBAIJANI_CYRILLIC"; break;
			case MUTT_MACINTOSH_LANGUAGE_AZERBAIJANI_ARABIC: return "MUTT_MACINTOSH_LANGUAGE_AZERBAIJANI_ARABIC"; break;
			case MUTT_MACINTOSH_LANGUAGE_ARMENIAN: return "MUTT_MACINTOSH_LANGUAGE_ARMENIAN"; break;
			case MUTT_MACINTOSH_LANGUAGE_GEORGIAN: return "MUTT_MACINTOSH_LANGUAGE_GEORGIAN"; break;
			case MUTT_MACINTOSH_LANGUAGE_MOLDAVIAN: return "MUTT_MACINTOSH_LANGUAGE_MOLDAVIAN"; break;
			case MUTT_MACINTOSH_LANGUAGE_KIRGHIZ: return "MUTT_MACINTOSH_LANGUAGE_KIRGHIZ"; break;
			case MUTT_MACINTOSH_LANGUAGE_TAJIKI: return "MUTT_MACINTOSH_LANGUAGE_TAJIKI"; break;
			case MUTT_MACINTOSH_LANGUAGE_TURKMEN: return "MUTT_MACINTOSH_LANGUAGE_TURKMEN"; break;
			case MUTT_MACINTOSH_LANGUAGE_MONGOLIAN: return "MUTT_MACINTOSH_LANGUAGE_MONGOLIAN"; break;
			case MUTT_MACINTOSH_LANGUAGE_MONGOLIAN_CYRILLIC: return "MUTT_MACINTOSH_LANGUAGE_MONGOLIAN_CYRILLIC"; break;
			case MUTT_MACINTOSH_LANGUAGE_PASHTO: return "MUTT_MACINTOSH_LANGUAGE_PASHTO"; break;
			case MUTT_MACINTOSH_LANGUAGE_KURDISH: return "MUTT_MACINTOSH_LANGUAGE_KURDISH"; break;
			case MUTT_MACINTOSH_LANGUAGE_KASHMIRI: return "MUTT_MACINTOSH_LANGUAGE_KASHMIRI"; break;
			case MUTT_MACINTOSH_LANGUAGE_SINDHI: return "MUTT_MACINTOSH_LANGUAGE_SINDHI"; break;
			case MUTT_MACINTOSH_LANGUAGE_TIBETAN: return "MUTT_MACINTOSH_LANGUAGE_TIBETAN"; break;
			case MUTT_MACINTOSH_LANGUAGE_NEPALI: return "MUTT_MACINTOSH_LANGUAGE_NEPALI"; break;
			case MUTT_MACINTOSH_LANGUAGE_SANSKRIT: return "MUTT_MACINTOSH_LANGUAGE_SANSKRIT"; break;
			case MUTT_MACINTOSH_LANGUAGE_MARATHI: return "MUTT_MACINTOSH_LANGUAGE_MARATHI"; break;
			case MUTT_MACINTOSH_LANGUAGE_BENGALI: return "MUTT_MACINTOSH_LANGUAGE_BENGALI"; break;
			case MUTT_MACINTOSH_LANGUAGE_ASSAMESE: return "MUTT_MACINTOSH_LANGUAGE_ASSAMESE"; break;
			case MUTT_MACINTOSH_LANGUAGE_GUJARATI: return "MUTT_MACINTOSH_LANGUAGE_GUJARATI"; break;
			case MUTT_MACINTOSH_LANGUAGE_PUNJABI: return "MUTT_MACINTOSH_LANGUAGE_PUNJABI"; break;
			case MUTT_MACINTOSH_LANGUAGE_ORIYA: return "MUTT_MACINTOSH_LANGUAGE_ORIYA"; break;
			case MUTT_MACINTOSH_LANGUAGE_MALAYALAM: return "MUTT_MACINTOSH_LANGUAGE_MALAYALAM"; break;
			case MUTT_MACINTOSH_LANGUAGE_KANNADA: return "MUTT_MACINTOSH_LANGUAGE_KANNADA"; break;
			case MUTT_MACINTOSH_LANGUAGE_TAMIL: return "MUTT_MACINTOSH_LANGUAGE_TAMIL"; break;
			case MUTT_MACINTOSH_LANGUAGE_TELUGU: return "MUTT_MACINTOSH_LANGUAGE_TELUGU"; break;
			case MUTT_MACINTOSH_LANGUAGE_SINHALESE: return "MUTT_MACINTOSH_LANGUAGE_SINHALESE"; break;
			case MUTT_MACINTOSH_LANGUAGE_BURMESE: return "MUTT_MACINTOSH_LANGUAGE_BURMESE"; break;
			case MUTT_MACINTOSH_LANGUAGE_KHMER: return "MUTT_MACINTOSH_LANGUAGE_KHMER"; break;
			case MUTT_MACINTOSH_LANGUAGE_LAO: return "MUTT_MACINTOSH_LANGUAGE_LAO"; break;
			case MUTT_MACINTOSH_LANGUAGE_VIETNAMESE: return "MUTT_MACINTOSH_LANGUAGE_VIETNAMESE"; break;
			case MUTT_MACINTOSH_LANGUAGE_INDONESIAN: return "MUTT_MACINTOSH_LANGUAGE_INDONESIAN"; break;
			case MUTT_MACINTOSH_LANGUAGE_TAGALOG: return "MUTT_MACINTOSH_LANGUAGE_TAGALOG"; break;
			case MUTT_MACINTOSH_LANGUAGE_MALAY_ROMAN: return "MUTT_MACINTOSH_LANGUAGE_MALAY_ROMAN"; break;
			case MUTT_MACINTOSH_LANGUAGE_MALAY_ARABIC: return "MUTT_MACINTOSH_LANGUAGE_MALAY_ARABIC"; break;
			case MUTT_MACINTOSH_LANGUAGE_AMHARIC: return "MUTT_MACINTOSH_LANGUAGE_AMHARIC"; break;
			case MUTT_MACINTOSH_LANGUAGE_TIGRINYA: return "MUTT_MACINTOSH_LANGUAGE_TIGRINYA"; break;
			case MUTT_MACINTOSH_LANGUAGE_GALLA: return "MUTT_MACINTOSH_LANGUAGE_GALLA"; break;
			case MUTT_MACINTOSH_LANGUAGE_SOMALI: return "MUTT_MACINTOSH_LANGUAGE_SOMALI"; break;
			case MUTT_MACINTOSH_LANGUAGE_SWAHILI: return "MUTT_MACINTOSH_LANGUAGE_SWAHILI"; break;
			case MUTT_MACINTOSH_LANGUAGE_KINYARWANDA_RUANDA: return "MUTT_MACINTOSH_LANGUAGE_KINYARWANDA_RUANDA"; break;
			case MUTT_MACINTOSH_LANGUAGE_RUNDI: return "MUTT_MACINTOSH_LANGUAGE_RUNDI"; break;
			case MUTT_MACINTOSH_LANGUAGE_NYANJA_CHEWA: return "MUTT_MACINTOSH_LANGUAGE_NYANJA_CHEWA"; break;
			case MUTT_MACINTOSH_LANGUAGE_MALAGASY: return "MUTT_MACINTOSH_LANGUAGE_MALAGASY"; break;
			case MUTT_MACINTOSH_LANGUAGE_ESPERANTO: return "MUTT_MACINTOSH_LANGUAGE_ESPERANTO"; break;
			case MUTT_MACINTOSH_LANGUAGE_WELSH: return "MUTT_MACINTOSH_LANGUAGE_WELSH"; break;
			case MUTT_MACINTOSH_LANGUAGE_BASQUE: return "MUTT_MACINTOSH_LANGUAGE_BASQUE"; break;
			case MUTT_MACINTOSH_LANGUAGE_CATALAN: return "MUTT_MACINTOSH_LANGUAGE_CATALAN"; break;
			case MUTT_MACINTOSH_LANGUAGE_LATIN: return "MUTT_MACINTOSH_LANGUAGE_LATIN"; break;
			case MUTT_MACINTOSH_LANGUAGE_QUECHUA: return "MUTT_MACINTOSH_LANGUAGE_QUECHUA"; break;
			case MUTT_MACINTOSH_LANGUAGE_GUARANI: return "MUTT_MACINTOSH_LANGUAGE_GUARANI"; break;
			case MUTT_MACINTOSH_LANGUAGE_AYMARA: return "MUTT_MACINTOSH_LANGUAGE_AYMARA"; break;
			case MUTT_MACINTOSH_LANGUAGE_TATAR: return "MUTT_MACINTOSH_LANGUAGE_TATAR"; break;
			case MUTT_MACINTOSH_LANGUAGE_UIGHUR: return "MUTT_MACINTOSH_LANGUAGE_UIGHUR"; break;
			case MUTT_MACINTOSH_LANGUAGE_DZONGKHA: return "MUTT_MACINTOSH_LANGUAGE_DZONGKHA"; break;
			case MUTT_MACINTOSH_LANGUAGE_JAVANESE: return "MUTT_MACINTOSH_LANGUAGE_JAVANESE"; break;
			case MUTT_MACINTOSH_LANGUAGE_SUNDANESE: return "MUTT_MACINTOSH_LANGUAGE_SUNDANESE"; break;
			case MUTT_MACINTOSH_LANGUAGE_GALICIAN: return "MUTT_MACINTOSH_LANGUAGE_GALICIAN"; break;
			case MUTT_MACINTOSH_LANGUAGE_AFRIKAANS: return "MUTT_MACINTOSH_LANGUAGE_AFRIKAANS"; break;
			case MUTT_MACINTOSH_LANGUAGE_BRETON: return "MUTT_MACINTOSH_LANGUAGE_BRETON"; break;
			case MUTT_MACINTOSH_LANGUAGE_INUKTITUT: return "MUTT_MACINTOSH_LANGUAGE_INUKTITUT"; break;
			case MUTT_MACINTOSH_LANGUAGE_SCOTTISH_GAELIC: return "MUTT_MACINTOSH_LANGUAGE_SCOTTISH_GAELIC"; break;
			case MUTT_MACINTOSH_LANGUAGE_MANX_GAELIC: return "MUTT_MACINTOSH_LANGUAGE_MANX_GAELIC"; break;
			case MUTT_MACINTOSH_LANGUAGE_IRISH_GAELIC_DOT: return "MUTT_MACINTOSH_LANGUAGE_IRISH_GAELIC_DOT"; break;
			case MUTT_MACINTOSH_LANGUAGE_TONGAN: return "MUTT_MACINTOSH_LANGUAGE_TONGAN"; break;
			case MUTT_MACINTOSH_LANGUAGE_GREEK_POLYTONIC: return "MUTT_MACINTOSH_LANGUAGE_GREEK_POLYTONIC"; break;
			case MUTT_MACINTOSH_LANGUAGE_GREENLANDIC: return "MUTT_MACINTOSH_LANGUAGE_GREENLANDIC"; break;
			case MUTT_MACINTOSH_LANGUAGE_AZERBAIJANI: return "MUTT_MACINTOSH_LANGUAGE_AZERBAIJANI"; break;
		}
	}

	MUDEF const char* mutt_macintosh_language_id_get_nice_name(uint16_m language_id) {
		switch (language_id) {
			default: return "MUTT_UNKNOWN"; break;
			case MUTT_MACINTOSH_LANGUAGE_ENGLISH: return "English"; break;
			case MUTT_MACINTOSH_LANGUAGE_FRENCH: return "French"; break;
			case MUTT_MACINTOSH_LANGUAGE_GERMAN: return "German"; break;
			case MUTT_MACINTOSH_LANGUAGE_ITALIAN: return "Italian"; break;
			case MUTT_MACINTOSH_LANGUAGE_DUTCH: return "Dutch"; break;
			case MUTT_MACINTOSH_LANGUAGE_SWEDISH: return "Swedish"; break;
			case MUTT_MACINTOSH_LANGUAGE_SPANISH: return "Spanish"; break;
			case MUTT_MACINTOSH_LANGUAGE_DANISH: return "Danish"; break;
			case MUTT_MACINTOSH_LANGUAGE_PORTUGUESE: return "Portuguese"; break;
			case MUTT_MACINTOSH_LANGUAGE_NORWEGIAN: return "Norwegian"; break;
			case MUTT_MACINTOSH_LANGUAGE_HEBREW: return "Hebrew"; break;
			case MUTT_MACINTOSH_LANGUAGE_JAPANESE: return "Japanese"; break;
			case MUTT_MACINTOSH_LANGUAGE_ARABIC: return "Arabic"; break;
			case MUTT_MACINTOSH_LANGUAGE_FINNISH: return "Finnish"; break;
			case MUTT_MACINTOSH_LANGUAGE_GREEK: return "Greek"; break;
			case MUTT_MACINTOSH_LANGUAGE_ICELANDIC: return "Icelandic"; break;
			case MUTT_MACINTOSH_LANGUAGE_MALTESE: return "Maltese"; break;
			case MUTT_MACINTOSH_LANGUAGE_TURKISH: return "Turkish"; break;
			case MUTT_MACINTOSH_LANGUAGE_CROATIAN: return "Croatian"; break;
			case MUTT_MACINTOSH_LANGUAGE_CHINESE_TRADITIONAL: return "Chinese (Traditional)"; break;
			case MUTT_MACINTOSH_LANGUAGE_URDU: return "Urdu"; break;
			case MUTT_MACINTOSH_LANGUAGE_HINDI: return "Hindi"; break;
			case MUTT_MACINTOSH_LANGUAGE_THAI: return "Thai"; break;
			case MUTT_MACINTOSH_LANGUAGE_KOREAN: return "Korean"; break;
			case MUTT_MACINTOSH_LANGUAGE_LITHUANIAN: return "Lithuanian"; break;
			case MUTT_MACINTOSH_LANGUAGE_POLISH: return "Polish"; break;
			case MUTT_MACINTOSH_LANGUAGE_HUNGARIAN: return "Hungarian"; break;
			case MUTT_MACINTOSH_LANGUAGE_ESTONIAN: return "Estonian"; break;
			case MUTT_MACINTOSH_LANGUAGE_LATVIAN: return "Latvian"; break;
			case MUTT_MACINTOSH_LANGUAGE_SAMI: return "Sami"; break;
			case MUTT_MACINTOSH_LANGUAGE_FAROESE: return "Faroese"; break;
			case MUTT_MACINTOSH_LANGUAGE_FARSI_PERSIAN: return "Farsi/Persian"; break;
			case MUTT_MACINTOSH_LANGUAGE_RUSSIAN: return "Russian"; break;
			case MUTT_MACINTOSH_LANGUAGE_CHINESE_SIMPLIFIED: return "Chinese (Simplified)"; break;
			case MUTT_MACINTOSH_LANGUAGE_FLEMISH: return "Flemish"; break;
			case MUTT_MACINTOSH_LANGUAGE_IRISH_GAELIC: return "Irish Gaelic"; break;
			case MUTT_MACINTOSH_LANGUAGE_ALBANIAN: return "Albanian"; break;
			case MUTT_MACINTOSH_LANGUAGE_ROMANIAN: return "Romanian"; break;
			case MUTT_MACINTOSH_LANGUAGE_CZECH: return "Czech"; break;
			case MUTT_MACINTOSH_LANGUAGE_SLOVAK: return "Slovak"; break;
			case MUTT_MACINTOSH_LANGUAGE_SLOVENIAN: return "Slovenian"; break;
			case MUTT_MACINTOSH_LANGUAGE_YIDDISH: return "Yiddish"; break;
			case MUTT_MACINTOSH_LANGUAGE_SERBIAN: return "Serbian"; break;
			case MUTT_MACINTOSH_LANGUAGE_MACEDONIAN: return "Macedonian"; break;
			case MUTT_MACINTOSH_LANGUAGE_BULGARIAN: return "Bulgarian"; break;
			case MUTT_MACINTOSH_LANGUAGE_UKRAINIAN: return "Ukrainian"; break;
			case MUTT_MACINTOSH_LANGUAGE_BYELORUSSIAN: return "Byelorussian"; break;
			case MUTT_MACINTOSH_LANGUAGE_UZBEK: return "Uzbek"; break;
			case MUTT_MACINTOSH_LANGUAGE_KAZAKH: return "Kazakh"; break;
			case MUTT_MACINTOSH_LANGUAGE_AZERBAIJANI_CYRILLIC: return "Azerbaijani (Cyrillic script)"; break;
			case MUTT_MACINTOSH_LANGUAGE_AZERBAIJANI_ARABIC: return "Azerbaijani (Arabic script)"; break;
			case MUTT_MACINTOSH_LANGUAGE_ARMENIAN: return "Armenian"; break;
			case MUTT_MACINTOSH_LANGUAGE_GEORGIAN: return "Georgian"; break;
			case MUTT_MACINTOSH_LANGUAGE_MOLDAVIAN: return "Moldavian"; break;
			case MUTT_MACINTOSH_LANGUAGE_KIRGHIZ: return "Kirghiz"; break;
			case MUTT_MACINTOSH_LANGUAGE_TAJIKI: return "Tajiki"; break;
			case MUTT_MACINTOSH_LANGUAGE_TURKMEN: return "Turkmen"; break;
			case MUTT_MACINTOSH_LANGUAGE_MONGOLIAN: return "Mongolian (Mongolian script)"; break;
			case MUTT_MACINTOSH_LANGUAGE_MONGOLIAN_CYRILLIC: return "Mongolian (Cyrillic script)"; break;
			case MUTT_MACINTOSH_LANGUAGE_PASHTO: return "Pashto"; break;
			case MUTT_MACINTOSH_LANGUAGE_KURDISH: return "Kurdish"; break;
			case MUTT_MACINTOSH_LANGUAGE_KASHMIRI: return "Kashmiri"; break;
			case MUTT_MACINTOSH_LANGUAGE_SINDHI: return "Sindhi"; break;
			case MUTT_MACINTOSH_LANGUAGE_TIBETAN: return "Tibetan"; break;
			case MUTT_MACINTOSH_LANGUAGE_NEPALI: return "Nepali"; break;
			case MUTT_MACINTOSH_LANGUAGE_SANSKRIT: return "Sanskrit"; break;
			case MUTT_MACINTOSH_LANGUAGE_MARATHI: return "Marathi"; break;
			case MUTT_MACINTOSH_LANGUAGE_BENGALI: return "Bengali"; break;
			case MUTT_MACINTOSH_LANGUAGE_ASSAMESE: return "Assamese"; break;
			case MUTT_MACINTOSH_LANGUAGE_GUJARATI: return "Gujarati"; break;
			case MUTT_MACINTOSH_LANGUAGE_PUNJABI: return "Punjabi"; break;
			case MUTT_MACINTOSH_LANGUAGE_ORIYA: return "Oriya"; break;
			case MUTT_MACINTOSH_LANGUAGE_MALAYALAM: return "Malayalam"; break;
			case MUTT_MACINTOSH_LANGUAGE_KANNADA: return "Kannada"; break;
			case MUTT_MACINTOSH_LANGUAGE_TAMIL: return "Tamil"; break;
			case MUTT_MACINTOSH_LANGUAGE_TELUGU: return "Telugu"; break;
			case MUTT_MACINTOSH_LANGUAGE_SINHALESE: return "Sinhalese"; break;
			case MUTT_MACINTOSH_LANGUAGE_BURMESE: return "Burmese"; break;
			case MUTT_MACINTOSH_LANGUAGE_KHMER: return "Khmer"; break;
			case MUTT_MACINTOSH_LANGUAGE_LAO: return "Lao"; break;
			case MUTT_MACINTOSH_LANGUAGE_VIETNAMESE: return "Vietnamese"; break;
			case MUTT_MACINTOSH_LANGUAGE_INDONESIAN: return "Indonesian"; break;
			case MUTT_MACINTOSH_LANGUAGE_TAGALOG: return "Tagalog"; break;
			case MUTT_MACINTOSH_LANGUAGE_MALAY_ROMAN: return "Malay (Roman script)"; break;
			case MUTT_MACINTOSH_LANGUAGE_MALAY_ARABIC: return "Malay (Arabic script)"; break;
			case MUTT_MACINTOSH_LANGUAGE_AMHARIC: return "Amharic"; break;
			case MUTT_MACINTOSH_LANGUAGE_TIGRINYA: return "Tigrinya"; break;
			case MUTT_MACINTOSH_LANGUAGE_GALLA: return "Galla"; break;
			case MUTT_MACINTOSH_LANGUAGE_SOMALI: return "Somali"; break;
			case MUTT_MACINTOSH_LANGUAGE_SWAHILI: return "Swahili"; break;
			case MUTT_MACINTOSH_LANGUAGE_KINYARWANDA_RUANDA: return "Kinyarwanda/Ruanda"; break;
			case MUTT_MACINTOSH_LANGUAGE_RUNDI: return "Rundi"; break;
			case MUTT_MACINTOSH_LANGUAGE_NYANJA_CHEWA: return "Nyanja/Chewa"; break;
			case MUTT_MACINTOSH_LANGUAGE_MALAGASY: return "Malagasy"; break;
			case MUTT_MACINTOSH_LANGUAGE_ESPERANTO: return "Esperanto"; break;
			case MUTT_MACINTOSH_LANGUAGE_WELSH: return "Welsh"; break;
			case MUTT_MACINTOSH_LANGUAGE_BASQUE: return "Basque"; break;
			case MUTT_MACINTOSH_LANGUAGE_CATALAN: return "Catalan"; break;
			case MUTT_MACINTOSH_LANGUAGE_LATIN: return "Latin"; break;
			case MUTT_MACINTOSH_LANGUAGE_QUECHUA: return "Quechua"; break;
			case MUTT_MACINTOSH_LANGUAGE_GUARANI: return "Guarani"; break;
			case MUTT_MACINTOSH_LANGUAGE_AYMARA: return "Aymara"; break;
			case MUTT_MACINTOSH_LANGUAGE_TATAR: return "Tatar"; break;
			case MUTT_MACINTOSH_LANGUAGE_UIGHUR: return "Uighur"; break;
			case MUTT_MACINTOSH_LANGUAGE_DZONGKHA: return "Dzongkha"; break;
			case MUTT_MACINTOSH_LANGUAGE_JAVANESE: return "Javanese (Roman script)"; break;
			case MUTT_MACINTOSH_LANGUAGE_SUNDANESE: return "Sundanese (Roman script)"; break;
			case MUTT_MACINTOSH_LANGUAGE_GALICIAN: return "Galician"; break;
			case MUTT_MACINTOSH_LANGUAGE_AFRIKAANS: return "Afrikaans"; break;
			case MUTT_MACINTOSH_LANGUAGE_BRETON: return "Breton"; break;
			case MUTT_MACINTOSH_LANGUAGE_INUKTITUT: return "Inuktitut"; break;
			case MUTT_MACINTOSH_LANGUAGE_SCOTTISH_GAELIC: return "Scottish Gaelic"; break;
			case MUTT_MACINTOSH_LANGUAGE_MANX_GAELIC: return "Manx Gaelic"; break;
			case MUTT_MACINTOSH_LANGUAGE_IRISH_GAELIC_DOT: return "Irish Gaelic (With dot above)"; break;
			case MUTT_MACINTOSH_LANGUAGE_TONGAN: return "Tongan"; break;
			case MUTT_MACINTOSH_LANGUAGE_GREEK_POLYTONIC: return "Greek (Polytonic)"; break;
			case MUTT_MACINTOSH_LANGUAGE_GREENLANDIC: return "Greenlandic"; break;
			case MUTT_MACINTOSH_LANGUAGE_AZERBAIJANI: return "Azerbaijani (Roman script)"; break;
		}
	}

	MUDEF const char* mutt_iso_encoding_id_get_name(uint16_m encoding_id) {
		switch (encoding_id) {
			default: return "MUTT_UNKNOWN"; break;
			case MUTT_ISO_ENCODING_7_BIT_ASCII: return "MUTT_ISO_ENCODING_7_BIT_ASCII"; break;
			case MUTT_ISO_ENCODING_10646: return "MUTT_ISO_ENCODING_10646"; break;
			case MUTT_ISO_ENCODING_8859_1: return "MUTT_ISO_ENCODING_8859_1"; break;
		}
	}

	MUDEF const char* mutt_iso_encoding_id_get_nice_name(uint16_m encoding_id) {
		switch (encoding_id) {
			default: return "MUTT_UNKNOWN"; break;
			case MUTT_ISO_ENCODING_7_BIT_ASCII: return "7-bit ASCII"; break;
			case MUTT_ISO_ENCODING_10646: return "ISO 10646"; break;
			case MUTT_ISO_ENCODING_8859_1: return "ISO 8859-1"; break;
		}
	}

	MUDEF const char* mutt_windows_encoding_id_get_name(uint16_m encoding_id) {
		switch (encoding_id) {
			default: return "MUTT_UNKNOWN"; break;
			case MUTT_WINDOWS_ENCODING_SYMBOL: return "MUTT_WINDOWS_ENCODING_SYMBOL"; break;
			case MUTT_WINDOWS_ENCODING_UNICODE_BMP: return "MUTT_WINDOWS_ENCODING_UNICODE_BMP"; break;
			case MUTT_WINDOWS_ENCODING_SHIFT_JIS: return "MUTT_WINDOWS_ENCODING_SHIFT_JIS"; break;
			case MUTT_WINDOWS_ENCODING_PRC: return "MUTT_WINDOWS_ENCODING_PRC"; break;
			case MUTT_WINDOWS_ENCODING_BIG5: return "MUTT_WINDOWS_ENCODING_BIG5"; break;
			case MUTT_WINDOWS_ENCODING_WANSUNG: return "MUTT_WINDOWS_ENCODING_WANSUNG"; break;
			case MUTT_WINDOWS_ENCODING_JOHAB: return "MUTT_WINDOWS_ENCODING_JOHAB"; break;
			case MUTT_WINDOWS_ENCODING_UNICODE_FULL: return "MUTT_WINDOWS_ENCODING_UNICODE_FULL"; break;
		}
	}

	MUDEF const char* mutt_windows_encoding_id_get_nice_name(uint16_m encoding_id) {
		switch (encoding_id) {
			default: return "MUTT_UNKNOWN"; break;
			case MUTT_WINDOWS_ENCODING_SYMBOL: return "Symbol"; break;
			case MUTT_WINDOWS_ENCODING_UNICODE_BMP: return "Unicode BMP"; break;
			case MUTT_WINDOWS_ENCODING_SHIFT_JIS: return "ShiftJIS"; break;
			case MUTT_WINDOWS_ENCODING_PRC: return "PRC"; break;
			case MUTT_WINDOWS_ENCODING_BIG5: return "Big5"; break;
			case MUTT_WINDOWS_ENCODING_WANSUNG: return "Wansung"; break;
			case MUTT_WINDOWS_ENCODING_JOHAB: return "Johab"; break;
			case MUTT_WINDOWS_ENCODING_UNICODE_FULL: return "Unicode full"; break;
		}
	}

	#endif

/* Allocation */

	// Moves the cursor forward if needed for allocation of memory;
	// the actual pointer to memory is &font->mem[font->memcur]
	// BEFORE calling this function.
	muttResult mutt_get_mem(muttFont* font, size_m len) {
		if (font->memlen-font->memcur >= len) {
			font->memcur += len;
			return MUTT_SUCCESS;
		}

		font->memlen *= 2;
		while (font->memlen-font->memcur < len) {
			font->memlen *= 2;
		}

		muByte* new_mem = (muByte*)mu_realloc(font->mem, font->memlen);
		if (new_mem == 0) {
			return MUTT_FAILED_REALLOC;
		}

		font->memcur += len;
		return MUTT_SUCCESS;
	}

/* Loading */

	// https://stackoverflow.com/a/600306
	#define MUTT_IS_POW_OF_2(v) ((v != 0) && ((v & (v-1)) == 0))

	/* Checksum */

		#ifndef mu_rle_uint24
			#define mu_rle_uint24(b) ((uint32_m)b[0] << 0 | (uint32_m)b[1] << 8 | (uint32_m)b[2] << 16)
		#endif

		#ifndef mu_rbe_uint24
			#define mu_rbe_uint24(b) ((uint32_m)b[2] << 0 | (uint32_m)b[1] << 8 | (uint32_m)b[0] << 16)
		#endif

		muBool mu_truetype_check_table_checksum(muByte* table, uint32_m length, uint32_m checksum) {
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
				return MU_FALSE;
			}

			return MU_TRUE;
		}

	/* Directory */

		muttResult mutt_load_directory(muttFont* font, muByte* data, uint32_m datalen, muttDirectory* directory) {
			uint16_m u16;
			muttResult res;
			muByte* orig_data = data;

			/* All first constant members */

				// : Verify length
				if (datalen < 12) {
					return MUTT_INVALID_TABLE_DIRECTORY_LENGTH;
				}

				// sfntVersion
				// : Verify it's the correct version
				if (mu_rbe_uint32(data) != 0x00010000) {
					return MUTT_INVALID_TABLE_DIRECTORY_SFNT_VERSION;
				}

				// numTables
				data += 4;
				directory->num_tables = mu_rbe_uint16(data);

				// searchRange
				data += 2;
				directory->search_range = mu_rbe_uint16(data);

				// : Verify a multiple of 16
				if (directory->search_range%16 != 0) {
					return MUTT_INVALID_TABLE_DIRECTORY_SEARCH_RANGE;
				}
				u16 = directory->search_range / 16;

				// : Verify a power of 2
				if (!MUTT_IS_POW_OF_2(u16)) {
					return MUTT_INVALID_TABLE_DIRECTORY_SEARCH_RANGE;
				}

				// : Verify it's lowest power of 2
				if (u16 > directory->num_tables || u16*2 < directory->num_tables) {
					return MUTT_INVALID_TABLE_DIRECTORY_SEARCH_RANGE;
				}

				// entrySelector
				data += 2;
				directory->entry_selector = mu_rbe_uint16(data);

				// : Undo log2 for calculations
				u16 = mu_pow(2, directory->entry_selector);

				// : Verify a power of 2
				if (!MUTT_IS_POW_OF_2(u16)) {
					return MUTT_INVALID_TABLE_DIRECTORY_ENTRY_SELECTOR;
				}

				// : Verify it's lowest power of 2
				if (u16 > directory->num_tables || u16*2 < directory->num_tables) {
					return MUTT_INVALID_TABLE_DIRECTORY_ENTRY_SELECTOR;
				}

				// rangeShift
				data += 2;
				directory->range_shift = mu_rbe_uint16(data);

				// : Undo subtraction of searchRange
				u16 = directory->range_shift + directory->search_range;

				// : Verify divisible by 16
				if (u16%16 != 0) {
					return MUTT_INVALID_TABLE_DIRECTORY_RANGE_SHIFT;
				}

				// : Undo 16 multiplication and verify equals numTables
				if (u16/16 != directory->num_tables) {
					return MUTT_INVALID_TABLE_DIRECTORY_RANGE_SHIFT;
				}

			/* Table records */

				// : Verify length
				if (datalen < (size_m)(12 + (directory->num_tables*16))) {
					return MUTT_INVALID_TABLE_DIRECTORY_LENGTH;
				}

				// : Allocate table records
				if (directory->num_tables == 0) {
					directory->table_records = 0;
					return MUTT_SUCCESS;
				}
				directory->table_records = (muttTableRecord*)&font->mem[font->memcur];

				res = mutt_get_mem(font, directory->num_tables*sizeof(muttTableRecord));
				if (res != MUTT_SUCCESS) {
					return res;
				}

				// : Loop through each record
				data += 2;

				for (uint16_m i = 0; i < directory->num_tables; i++) {
					muttTableRecord* record = &directory->table_records[i];
					// tableTag
					mu_memcpy(&record->table_tag, data, 4);
					data += 4;

					// checksum
					record->checksum = mu_rbe_uint32(data);
					data += 4;

					// offset
					record->offset = mu_rbe_uint32(data);
					data += 4;

					// : Verify offset
					if (record->offset >= datalen) {
						return MUTT_INVALID_TABLE_RECORD_OFFSET;
					}

					// length
					record->length = mu_rbe_uint32(data);
					data += 4;

					// : Verify length
					if (record->offset+record->length > datalen) {
						return MUTT_INVALID_TABLE_RECORD_LENGTH;
					}

					// : Verify checksum (don't check head cuz it's weird)
					if (record->table_tag[0] != 'h' || record->table_tag[1] != 'e' ||
						record->table_tag[2] != 'a' || record->table_tag[3] != 'd' ) {
						if (!mu_truetype_check_table_checksum(&orig_data[record->offset], record->length, record->checksum)) {
							return MUTT_INVALID_TABLE_RECORD_CHECKSUM;
						}
					}
				}

			return MUTT_SUCCESS;
		}

	/* Maxp */

		muttResult mutt_load_maxp(muttFont* font, muttMaxp* maxp, muByte* data, uint32_m length) {
			// Ensure length
			if (length < 32) {
				return MUTT_INVALID_MAXP_LENGTH;
			}

			// Version high
			maxp->version_high = mu_rbe_uint16(data);
			if (maxp->version_high != 0x0001) {
				return MUTT_INVALID_MAXP_VERSION;
			}

			// Version low
			data += 2;
			maxp->version_low = mu_rbe_uint16(data);
			if (maxp->version_low != 0x0000) {
				return MUTT_INVALID_MAXP_VERSION;
			}

			// numGlyphs
			data += 2;
			maxp->num_glyphs = mu_rbe_uint16(data);

			// maxPoints
			data += 2;
			maxp->max_points = mu_rbe_uint16(data);

			// maxContours
			data += 2;
			maxp->max_contours = mu_rbe_uint16(data);

			// maxCompositePoints
			data += 2;
			maxp->max_composite_points = mu_rbe_uint16(data);

			// maxCompositeContours
			data += 2;
			maxp->max_composite_contours = mu_rbe_uint16(data);

			// maxZones
			data += 2;
			maxp->max_zones = mu_rbe_uint16(data);
			if (maxp->max_zones != 1 && maxp->max_zones != 2) {
				return MUTT_INVALID_MAXP_MAX_ZONES;
			}

			// maxTwilightPoints
			data += 2;
			maxp->max_twilight_points = mu_rbe_uint16(data);

			// maxStorage
			maxp->max_storage = mu_rbe_uint16(data);
			data += 2;

			// maxFunctionDefs
			data += 2;
			maxp->max_function_defs = mu_rbe_uint16(data);

			// maxInstructionDefs
			data += 2;
			maxp->max_instruction_defs = mu_rbe_uint16(data);

			// maxStackElements
			data += 2;
			maxp->max_stack_elements = mu_rbe_uint16(data);

			// maxSizeOfInstructions
			data += 2;
			maxp->max_size_of_instructions = mu_rbe_uint16(data);

			// maxComponentElements
			data += 2;
			maxp->max_component_elements = mu_rbe_uint16(data);

			// maxComponentDepth
			data += 2;
			maxp->max_component_depth = mu_rbe_uint16(data);

			return MUTT_SUCCESS; if (font) {}
		}

	/* Head */

		muttResult mutt_load_head(muttFont* font, muttHead* head, muByte* data, uint32_m length) {
			uint64_m u64;
			uint16_m u16;

			// Ensure length
			if (length < 54) {
				return MUTT_INVALID_HEAD_LENGTH;
			}

			// majorVersion
			if (mu_rbe_uint16(data) != 1) {
				return MUTT_INVALID_HEAD_VERSION;
			}
			data += 2;

			// minorVersion
			if (mu_rbe_uint16(data) != 0) {
				return MUTT_INVALID_HEAD_VERSION;
			}
			data += 2;

			// fontRevision
			head->font_revision_high = mu_rbe_uint16(data);
			data += 2;
			head->font_revision_low = mu_rbe_uint16(data);
			data += 2;

			// checksumAdjustment
			head->checksum_adjustment = mu_rbe_uint32(data);
			data += 4;

			// magicNumber
			if (mu_rbe_uint32(data) != 0x5F0F3CF5) {
				return MUTT_INVALID_HEAD_MAGIC_NUMBER;
			}
			data += 4;

			// flags
			head->flags = mu_rbe_uint16(data);
			data += 2;

			// unitsPerEm
			head->units_per_em = mu_rbe_uint16(data);
			if (head->units_per_em < 16 || head->units_per_em > 16384) {
				return MUTT_INVALID_HEAD_UNITS_PER_EM;
			}
			data += 2;

			// created
			u64 = mu_rbe_uint64(data);
			head->created = *(int64_m*)&u64;
			data += 8;

			// modified
			u64 = mu_rbe_uint64(data);
			head->modified = *(int64_m*)&u64;
			data += 8;

			// xMin
			u16 = mu_rbe_uint16(data);
			head->x_min = *(int16_m*)&u16;
			data += 2;

			// yMin
			u16 = mu_rbe_uint16(data);
			head->y_min = *(int16_m*)&u16;
			data += 2;

			// xMax
			u16 = mu_rbe_uint16(data);
			head->x_max = *(int16_m*)&u16;
			// : Check if x-min/max values make sense
			if (head->x_max < head->x_min) {
				return MUTT_INVALID_HEAD_X_MIN_MAX;
			}
			data += 2;

			// yMax
			u16 = mu_rbe_uint16(data);
			head->y_max = *(int16_m*)&u16;
			// : Check if x-min/max values make sense
			if (head->y_max < head->y_min) {
				return MUTT_INVALID_HEAD_Y_MIN_MAX;
			}
			data += 2;

			// macStyle
			head->mac_style = mu_rbe_uint16(data);
			data += 2;

			// lowestRecPPEM
			head->lowest_rec_ppem = mu_rbe_uint16(data);
			data += 2;

			// fontDirectionHint
			u16 = mu_rbe_uint16(data);
			head->font_direction_hint = *(int16_m*)&u16;
			data += 2;

			// indexToLocFormat
			u16 = mu_rbe_uint16(data);
			head->index_to_loc_format = *(int16_m*)&u16;
			if (head->index_to_loc_format != 0 && head->index_to_loc_format != 1) {
				return MUTT_INVALID_HEAD_INDEX_TO_LOC_FORMAT;
			}
			data += 2;

			// glyphDataFormat
			u16 = mu_rbe_uint16(data);
			head->glyph_data_format = *(int16_m*)&u16;
			if (head->glyph_data_format != 0) {
				return MUTT_INVALID_HEAD_GLYPH_DATA_FORMAT;
			}

			return MUTT_SUCCESS; if (font) {}
		}

	/* Hhea */

		// Req: maxp
		muttResult mutt_load_hhea(muttFont* font, muttHhea* hhea, muByte* data, uint32_m length) {
			uint16_m u16;

			// Ensure length
			if (length < 36) {
				return MUTT_INVALID_HHEA_LENGTH;
			}

			// Verify major version
			if (mu_rbe_uint16(data) != 1) {
				return MUTT_INVALID_HHEA_VERSION;
			}
			data += 2;
			// + minor version
			if (mu_rbe_uint16(data) != 0) {
				return MUTT_INVALID_HHEA_VERSION;
			}
			data += 2;

			// ascender
			u16 = mu_rbe_uint16(data);
			hhea->ascender = *(int16_m*)&u16;
			data += 2;

			// descender
			u16 = mu_rbe_uint16(data);
			hhea->descender = *(int16_m*)&u16;
			data += 2;

			// lineGap
			u16 = mu_rbe_uint16(data);
			hhea->line_gap = *(int16_m*)&u16;
			data += 2;

			// advanceWidthMax
			hhea->advance_width_max = mu_rbe_uint16(data);
			data += 2;

			// minLeftSideBearing
			u16 = mu_rbe_uint16(data);
			hhea->min_left_side_bearing = *(int16_m*)&u16;
			data += 2;

			// minRightSideBearing
			u16 = mu_rbe_uint16(data);
			hhea->min_right_side_bearing = *(int16_m*)&u16;
			data += 2;

			// xMaxExtent
			u16 = mu_rbe_uint16(data);
			hhea->x_max_extent = *(int16_m*)&u16;
			data += 2;

			// caretSlopeRise
			u16 = mu_rbe_uint16(data);
			hhea->caret_slope_rise = *(int16_m*)&u16;
			data += 2;

			// caretSlopeRun
			u16 = mu_rbe_uint16(data);
			hhea->caret_slope_run = *(int16_m*)&u16;
			data += 2;

			// caretOffset
			u16 = mu_rbe_uint16(data);
			hhea->caret_offset = *(int16_m*)&u16;
			data += 2;

			// metricDataFormat
			u16 = mu_rbe_uint16(data);
			if (u16 != 0) {
				return MUTT_INVALID_HHEA_METRIC_DATA_FORMAT;
			}
			hhea->metric_data_format = *(int16_m*)&u16;
			data += 2;

			// numberOfHMetrics
			hhea->number_of_hmetrics = mu_rbe_uint16(data);
			// : Verify that numGlyphs-numberOfHMetrics >= 0
			if (hhea->number_of_hmetrics > font->maxp->num_glyphs) {
				return MUTT_INVALID_HHEA_NUMBER_OF_HMETRICS;
			}

			return MUTT_SUCCESS;
		}

	/* Hmtx */

		// Req: maxp, hhea
		muttResult mutt_load_hmtx(muttFont* font, muttHmtx* hmtx, muByte* data, uint32_m length) {
			// Verify length
			uint16_m lsb_count = font->maxp->num_glyphs-font->hhea->number_of_hmetrics;
			if (length < (uint32_m)(4*font->hhea->number_of_hmetrics + 2*lsb_count)) {
				return MUTT_INVALID_HMTX_LENGTH;
			}

			muttResult res;
			uint16_m u16;

			// Allocate hMetrics
			if (font->hhea->number_of_hmetrics == 0) {
				hmtx->hmetrics = 0;
			} else {
				hmtx->hmetrics = (muttLongHorMetric*)&font->mem[font->memcur];
				res = mutt_get_mem(font, sizeof(muttLongHorMetric)*font->hhea->number_of_hmetrics);
				if (res != MUTT_SUCCESS) {
					return res;
				}
			}

			// Allocate leftSideBearings
			if (lsb_count == 0) {
				hmtx->left_side_bearings = 0;
			} else {
				hmtx->left_side_bearings = (int16_m*)&font->mem[font->memcur];
				res = mutt_get_mem(font, sizeof(int16_m)*lsb_count);
				if (res != MUTT_SUCCESS) {
					return res;
				}
			}

			// Loop through each hMetric
			for (uint16_m m = 0; m < font->hhea->number_of_hmetrics; m++) {
				// advanceWidth
				hmtx->hmetrics[m].advance_width = mu_rbe_uint16(data);
				data += 2;
				// lsb
				u16 = mu_rbe_uint16(data);
				hmtx->hmetrics[m].lsb = *(int16_m*)&u16;
				data += 2;
			}

			// Loop through each leftSideBearing
			for (uint16_m l = 0; l < lsb_count; l++) {
				u16 = mu_rbe_uint16(data);
				hmtx->left_side_bearings[l] = *(int16_m*)&u16;
				data += 2;
			}

			return MUTT_SUCCESS;
		}

	/* Loca */

		// Req: head, maxp
		muttResult mutt_load_loca(muttFont* font, muttLoca* loca, muByte* data, uint32_m length) {
			muttResult res;
			uint32_m needed_length; // (in bytes)

			// - Offset16 -

			if (font->head->index_to_loc_format == MUTT_LOCA_FORMAT_OFFSET16) {
				// Verify length
				needed_length = 2*(font->maxp->num_glyphs+1);
				if (length < needed_length) {
					return MUTT_INVALID_LOCA_LENGTH;
				}

				// Allocate length
				loca->offsets16 = (uint16_m*)&font->mem[font->memcur];
				res = mutt_get_mem(font, needed_length);
				if (res != MUTT_SUCCESS) {
					return res;
				}

				// Loop through each offset
				for (uint32_m i = 0; i < (uint32_m)(font->maxp->num_glyphs+1); i++) {
					loca->offsets16[i] = mu_rbe_uint16(data);
					data += 2;
				}

				return MUTT_SUCCESS;
			}

			// - Offset32 -

			// Verify length
			needed_length = 4*(font->maxp->num_glyphs+1);
			if (length < needed_length) {
				return MUTT_INVALID_LOCA_LENGTH;
			}

			// Allocate length
			loca->offsets32 = (uint32_m*)&font->mem[font->memcur];
			res = mutt_get_mem(font, needed_length);
			if (res != MUTT_SUCCESS) {
				return res;
			}

			// Loop through each offset
			for (uint32_m i = 0; i < (uint32_m)(font->maxp->num_glyphs+1); i++) {
				loca->offsets32[i] = mu_rbe_uint32(data);
				data += 4;
			}

			return MUTT_SUCCESS;
		}

	/* Post */

		muttResult mutt_load_post(muttFont* font, muttPost* post, muByte* data, uint32_m length) {
			// Verify main table length
			if (length < 32) {
				return MUTT_INVALID_POST_LENGTH;
			}

			uint32_m u32;
			uint16_m u16;
			muttResult res;

			// version high bytes
			post->version_high = mu_rbe_uint16(data);
			data += 2;

			// version low bytes
			post->version_low = mu_rbe_uint16(data);
			data += 2;

			// Verify version
			if (!(post->version_high == 1 && post->version_low == 0) &&
				!(post->version_high == 2 && post->version_low == 0) &&
				!(post->version_high == 2 && post->version_low == 5) &&
				!(post->version_high == 3 && post->version_low == 0)
			) {
				return MUTT_INVALID_POST_VERSION;
			}

			// italicAngle
			u32 = mu_rbe_uint32(data);
			post->italic_angle = *(int16_m*)&u32;
			data += 4;

			// underlinePosition
			u16 = mu_rbe_uint16(data);
			post->underline_position = *(int16_m*)&u16;
			data += 2;

			// underlineThickness
			u16 = mu_rbe_uint16(data);
			post->underline_thickness = *(int16_m*)&u16;
			data += 2;

			// isFixedPitch
			post->is_fixed_pitch = mu_rbe_uint32(data);
			data += 4;

			// minMemType42
			post->min_mem_type42 = mu_rbe_uint32(data);
			data += 4;

			// maxMemType42
			post->max_mem_type42 = mu_rbe_uint32(data);
			data += 4;

			// minMemType1
			post->min_mem_type1 = mu_rbe_uint32(data);
			data += 4;

			// maxMemType1
			post->max_mem_type1 = mu_rbe_uint32(data);
			data += 4;

			// version handling

			// : 2.0
			if (post->version_high == 2 && post->version_low == 0) {
				// Verify length for numGlyphs
				if (length < 34) {
					return MUTT_INVALID_POST_LENGTH;
				}

				// numGlyphs
				post->subtable.v20.num_glyphs = mu_rbe_uint16(data);
				data += 2;

				if (post->subtable.v20.num_glyphs == 0) {
					return MUTT_SUCCESS;
				}

				// Verify length for glyphNameIndex
				if (length < (uint32_m)(34 + (2*post->subtable.v20.num_glyphs))) {
					return MUTT_INVALID_POST_LENGTH;
				}
				length -= (34 + (2*post->subtable.v20.num_glyphs));

				// glyphNameIndex allocation
				post->subtable.v20.glyph_name_index = (uint16_m*)&font->mem[font->memcur];
				res = mutt_get_mem(font, 2*post->subtable.v20.num_glyphs);
				if (res != MUTT_SUCCESS) {
					return res;
				}

				// Allocate stringData
				post->subtable.v20.string_data = (uint8_m*)&font->mem[font->memcur];
				res = mutt_get_mem(font, length);
				if (res != MUTT_SUCCESS) {
					return res;
				}

				// Copy stringData over
				mu_memcpy(post->subtable.v20.string_data, &data[2*post->subtable.v20.num_glyphs], length);

				// Loop through each number in glyphNameIndex
				for (uint16_m i = 0; i < post->subtable.v20.num_glyphs; i++) {
					// glyphNameIndex[i]
					post->subtable.v20.glyph_name_index[i] = mu_rbe_uint16(data);

					// Verify index
					if (post->subtable.v20.glyph_name_index[i] >= 258) {
						uint16_m index = post->subtable.v20.glyph_name_index[i]-258;
						uint8_m* string_data = post->subtable.v20.string_data;
						uint32_m string_length = 0;

						for (uint16_m c = 0; c < index; c++) {
							string_length += string_data[0] + 1;
							if (string_length > length) {
								return MUTT_INVALID_POST_GLYPH_NAME_INDEX;
							}
							string_data += string_data[0] + 1;
						}
					}

					data += 2;
				}
			}

			// : 2.5
			else if (post->version_high == 2 && post->version_low == 5) {
				// Verify length for numGlyphs
				if (length < 34) {
					return MUTT_INVALID_POST_LENGTH;
				}

				// numGlyphs
				post->subtable.v25.num_glyphs = mu_rbe_uint16(data);
				data += 2;

				if (post->subtable.v25.num_glyphs == 0) {
					return MUTT_SUCCESS;
				}

				// Verify length for offset
				if (length < (uint32_m)(34+post->subtable.v25.num_glyphs)) {
					return MUTT_INVALID_POST_LENGTH;
				}

				// Allocate offset
				post->subtable.v25.offset = (int8_m*)&font->mem[font->memcur];
				res = mutt_get_mem(font, post->subtable.v25.num_glyphs);
				if (res != MUTT_SUCCESS) {
					return res;
				}

				// Copy offset data
				mu_memcpy(post->subtable.v25.offset, data, post->subtable.v25.num_glyphs);
			}

			return MUTT_SUCCESS;
		}

	/* Name */

		muttResult mutt_load_name(muttFont* font, muttName* name, muByte* data, uint32_m length) {
			// Verify length for version, count, and storageOffset
			if (length < 6) {
				return MUTT_INVALID_NAME_LENGTH;
			}

			muttResult res;

			// version
			name->version = mu_rbe_uint16(data);
			if (name->version > 1) {
				return MUTT_INVALID_NAME_VERSION;
			}
			// : Verify version length
			uint32_m base_length = 6; // (version, count, storageOffset)
			if (name->version == 1) {
				base_length = 8; // (version, count, storageOffset, langTagCount)
				if (length < base_length) {
					return MUTT_INVALID_NAME_LENGTH;
				}
			}
			data += 2;

			// count
			name->count = mu_rbe_uint16(data);
			if (name->count == 0) {
				return MUTT_SUCCESS;
			}
			data += 2;

			// storageOffset
			name->storage_offset = mu_rbe_uint16(data);
			if (name->storage_offset >= length) {
				return MUTT_INVALID_NAME_STORAGE_OFFSET;
			}
			data += 2;

			uint32_m storage_length = length - name->storage_offset;

			// Allocate storage
			name->storage = (muByte*)&font->mem[font->memcur];
			res = mutt_get_mem(font, storage_length);
			if (res != MUTT_SUCCESS) {
				return res;
			}

			// Verify length for nameRecord
			uint32_m req_length = base_length + (name->count*12);
			if (length < req_length) {
				return MUTT_INVALID_NAME_LENGTH;
			}

			// Allocate nameRecord
			name->name_record = (muttNameRecord*)&font->mem[font->memcur];
			res = mutt_get_mem(font, name->count*sizeof(muttNameRecord));
			if (res != MUTT_SUCCESS) {
				return res;
			}

			// Loop through each nameRecord
			for (uint16_m n = 0; n < name->count; n++) {
				muttNameRecord* record = &name->name_record[n];

				// platformID
				record->platform_id = mu_rbe_uint16(data);
				if (record->platform_id > 3 || record->platform_id == 2) {
					return MUTT_INVALID_NAME_PLATFORM_ID;
				}
				data += 2;

				// encodingID
				record->encoding_id = mu_rbe_uint16(data);
				// : Verify Unicode encoding
				if (record->platform_id == 0 && record->encoding_id > 4) {
					return MUTT_INVALID_NAME_ENCODING_ID;
				}
				// : Verify Macintosh encoding
				else if (record->platform_id == 1 && record->encoding_id > 32) {
					return MUTT_INVALID_NAME_ENCODING_ID;
				}
				// : Verify Windows encoding
				else if (record->platform_id == 3 && (
					(record->encoding_id > 10) ||
					(record->encoding_id >= 7 && record->encoding_id <= 9)
				)) {
					return MUTT_INVALID_NAME_ENCODING_ID;
				}
				data += 2;

				// languageID
				record->language_id = mu_rbe_uint16(data);
				// : Verify Macintosh
				if (record->platform_id == 1 && (
					(record->language_id > 94 && record->language_id < 128) ||
					(record->language_id > 150)
				)) {
					return MUTT_INVALID_NAME_LANGUAGE_ID;
				}
				// : Verify Windows (@TODO)
				data += 2;

				// nameID
				record->name_id = mu_rbe_uint16(data);
				data += 2;

				// length
				record->length = mu_rbe_uint16(data);
				data += 2;

				// stringOffset
				record->string_offset = mu_rbe_uint16(data);
				if ((uint32_m)(record->string_offset+record->length) > storage_length) {
					return MUTT_INVALID_NAME_STRING_OFFSET;
				}
				data += 2;
			}

			if (name->version == 0) {
				mu_memcpy(name->storage, data, storage_length);
				return MUTT_SUCCESS;
			}

			// langTagCount (length for this is already guaranteed)
			name->lang_tag_count = mu_rbe_uint16(data);
			data += 2;

			// Verify length for langTagRecord
			req_length += name->lang_tag_count*4;
			if (length < req_length) {
				return MUTT_INVALID_NAME_LENGTH;
			}

			// Allocate langTagRecord
			name->lang_tag_record = (muttLangTagRecord*)&font->mem[font->memcur];
			res = mutt_get_mem(font, name->lang_tag_count*sizeof(muttLangTagRecord));
			if (res != MUTT_SUCCESS) {
				return res;
			}

			// Loop through each langTagRecord
			for (uint16_m l = 0; l < name->lang_tag_count; l++) {
				muttLangTagRecord* record = &name->lang_tag_record[l];

				// length
				record->length = mu_rbe_uint16(data);
				data += 2;

				// langTagOffset
				record->lang_tag_offset = mu_rbe_uint16(data);
				if ((uint32_m)(record->lang_tag_offset+record->length) > storage_length) {
					return MUTT_INVALID_NAME_LANG_TAG_OFFSET;
				}
				data += 2;
			}

			mu_memcpy(name->storage, data, storage_length);

			return MUTT_SUCCESS;
		}

	/* Full loading */

		#define MUTT_LOAD_IT_TYPE muttResult (*)(muttFont*, void*, uint8_t*, uint32_t)
		void mutt_load_individual_table(muttFont* font, void** p, muttResult* res, muttResult (*load)(muttFont* font, void* table, muByte* data, uint32_m length), muByte* data, uint32_m length, size_m size) {
			if (*p != 0) {
				*p = 0;
				*res = MUTT_DUPLICATE_TABLE;
				return;
			}
			// Allocate
			*p = (void*)&font->mem[font->memcur];
			*res = mutt_get_mem(font, size);

			// If allocation failed:
			if (*res != MUTT_SUCCESS) {
				// The table is 0
				*p = 0;
			// If we have allocated:
			} else {
				// Try loading the table
				*res = load(font, *p, data, length);
				// If it failed, it's 0
				if (*res != MUTT_SUCCESS) {
					*p = 0;
				}
			}
		}

		// - Macros for the table loading -

			// Determines if a table has already been loaded or attempted such,
			// or if we don't even need to load it
			#define MUTT_SKIP_PROCESSED_TABLE(up_this_table) \
				if (!(font->load_flags & MUTT_LOAD_##up_this_table)) { \
					break; \
				} \
				if (!(first) && !(*skipped & MUTT_LOAD_##up_this_table)) { \
					break; \
				}

			// Verifies a load flag
			#define MUTT_VERIFY_LOAD_FLAG(up_load_table, up_this_table, low_this_table) \
				if (!(font->load_flags & MUTT_LOAD_##up_load_table)) { \
					font->low_this_table##_res = MUTT_##up_this_table##_REQUIRES_##up_load_table; \
					break; \
				}

			// Handles a table with dependencies not existing yet
			#define MUTT_VERIFY_TABLE(up_load_table, low_load_table, up_this_table, low_this_table) \
				if (!(font->low_load_table)) { \
					if ((*skipped) & MUTT_LOAD_##up_load_table) { \
						*skipped |= MUTT_LOAD_##up_this_table; \
						break; \
					} \
					if (first && (font->low_load_table##_res == MUTT_UNFOUND_TABLE)) { \
						*skipped |= MUTT_LOAD_##up_this_table; \
						break; \
					} \
					font->low_this_table##_res = MUTT_##up_this_table##_REQUIRES_##up_load_table; \
					break; \
				}

			// Unmarks a table as skipped if necessary
			#define MUTT_UNSKIP_TABLE(up_this_table) \
				if (*skipped & MUTT_LOAD_##up_this_table) { \
					*skipped ^= MUTT_LOAD_##up_this_table; \
				}

			// Loads a table
			#define MUTT_LOAD_TABLE(first_up_this_table, low_this_table) \
				if (font->low_this_table##_res != MUTT_DUPLICATE_TABLE) {\
					mutt_load_individual_table(font, (void**)&font->low_this_table, &font->low_this_table##_res, (MUTT_LOAD_IT_TYPE)mutt_load_##low_this_table, &data[record->offset], record->length, sizeof(mutt##first_up_this_table)); \
				}

		void mutt_load_tables(muttFont* font, muByte* data, uint32_m* skipped, muBool first) {
			// Loop through each table

			for (uint16_m i = 0; i < font->directory->num_tables; i++) {
				// Get tag as a number

				muttTableRecord* record = &font->directory->table_records[i];
				uint32_m tag = mu_rbe_uint32(((muByte*)&record->table_tag));

				switch (tag) {
					default: break;

					// maxp
					case 0x6D617870: {
						MUTT_SKIP_PROCESSED_TABLE(MAXP)
						MUTT_LOAD_TABLE(Maxp, maxp)
					} break;

					// head
					case 0x68656164: {
						MUTT_SKIP_PROCESSED_TABLE(HEAD)
						MUTT_LOAD_TABLE(Head, head)
					} break;

					// hhea
					case 0x68686561: {
						MUTT_SKIP_PROCESSED_TABLE(HHEA)

						// Req: maxp
						MUTT_VERIFY_LOAD_FLAG(MAXP, HHEA, hhea)
						MUTT_VERIFY_TABLE(MAXP, maxp, HHEA, hhea)

						MUTT_UNSKIP_TABLE(HHEA)

						// Load
						MUTT_LOAD_TABLE(Hhea, hhea)
					} break;

					// hmtx
					case 0x686D7478: {
						MUTT_SKIP_PROCESSED_TABLE(HMTX)

						// Req: maxp, hhea
						MUTT_VERIFY_LOAD_FLAG(MAXP, HMTX, hmtx)
						MUTT_VERIFY_LOAD_FLAG(HHEA, HMTX, hmtx)
						MUTT_VERIFY_TABLE(MAXP, maxp, HMTX, hmtx)
						MUTT_VERIFY_TABLE(HHEA, hhea, HMTX, hmtx)

						MUTT_UNSKIP_TABLE(HMTX)

						// Load
						MUTT_LOAD_TABLE(Hmtx, hmtx)
					} break;

					// loca
					case 0x6C6F6361: {
						MUTT_SKIP_PROCESSED_TABLE(LOCA)

						// Req: head, maxp
						MUTT_VERIFY_LOAD_FLAG(HEAD, LOCA, loca)
						MUTT_VERIFY_LOAD_FLAG(MAXP, LOCA, loca)
						MUTT_VERIFY_TABLE(HEAD, head, LOCA, loca)
						MUTT_VERIFY_TABLE(MAXP, maxp, LOCA, loca)

						MUTT_UNSKIP_TABLE(LOCA)

						// Load
						MUTT_LOAD_TABLE(Loca, loca)
					} break;

					// post
					case 0x706F7374: {
						MUTT_SKIP_PROCESSED_TABLE(POST)
						MUTT_LOAD_TABLE(Post, post)
					} break;

					// name
					case 0x6E616D65: {
						MUTT_SKIP_PROCESSED_TABLE(NAME)
						MUTT_LOAD_TABLE(Name, name)
					} break;
				}
			}
		}

		MUDEF muttResult mutt_load(muByte* data, size_m datalen, muttFont* font, uint32_m load_flags) {
			muttResult res;
			font->load_flags = load_flags;

			/* Memory */

				// mem
				font->mem = (muByte*)mu_malloc(datalen);
				if (font->mem == 0) {
					return MUTT_FAILED_MALLOC;
				}

				// memlen
				font->memlen = datalen;
				// memcur
				font->memcur = 0;

			/* Loading directory */

				// Get memory required
				font->directory = (muttDirectory*)&font->mem[font->memcur];
				res = mutt_get_mem(font, sizeof(muttDirectory));

				// If allocation failed:
				if (res != MUTT_SUCCESS) {
					// : Return as such (we need the directory)
					mu_free(font->mem);
					return res;
				// If it didn't:
				} else {
					// : Load it
					res = mutt_load_directory(font, data, datalen, font->directory);
					// : Return if failed (we need the directory)
					if (res != MUTT_SUCCESS) {
						mu_free(font->mem);
						return res;
					}
				}

			/* Set all requested tables to automatically unfound */

				if (load_flags & MUTT_LOAD_MAXP) {
					font->maxp = 0;
					font->maxp_res = MUTT_UNFOUND_TABLE;
				}
				if (load_flags & MUTT_LOAD_HEAD) {
					font->head = 0;
					font->head_res = MUTT_UNFOUND_TABLE;
				}
				if (load_flags & MUTT_LOAD_HHEA) {
					font->hhea = 0;
					font->hhea_res = MUTT_UNFOUND_TABLE;
				}
				if (load_flags & MUTT_LOAD_HMTX) {
					font->hmtx = 0;
					font->hmtx_res = MUTT_UNFOUND_TABLE;
				}
				if (load_flags & MUTT_LOAD_LOCA) {
					font->loca = 0;
					font->loca_res = MUTT_UNFOUND_TABLE;
				}
				if (load_flags & MUTT_LOAD_POST) {
					font->post = 0;
					font->post_res = MUTT_UNFOUND_TABLE;
				}
				if (load_flags & MUTT_LOAD_NAME) {
					font->name = 0;
					font->name_res = MUTT_UNFOUND_TABLE;
				}

			/* Find tables */

				// Used to mark what tables have been skipped;
				// multiple passes are needed due to table dependencies.
				uint32_m skipped_flags = 0;

				mutt_load_tables(font, data, &skipped_flags, MU_TRUE);

				while (skipped_flags) {
					mutt_load_tables(font, data, &skipped_flags, MU_FALSE);
				}

			return MUTT_SUCCESS;
		}

		MUDEF void mutt_deload(muttFont* font) {
			mu_free(font->mem);
		}

#ifdef MUTT_IMPLEMENTATION

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

