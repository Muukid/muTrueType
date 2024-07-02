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
			// @DOCLINE * `@NLFT`: the task succeeded.
			MUTT_SUCCESS,
			// @DOCLINE * `@NLFT`: a call to malloc failed; memory was insufficient to perform the operation
			MUTT_FAILED_MALLOC,
			// @DOCLINE * `@NLFT`: a call to realloc failled; memory was insufficient to perform the operation.
			MUTT_FAILED_REALLOC,
			// @DOCLINE * `@NLFT`: the table could not be located within the data.
			MUTT_UNFOUND_TABLE,
			// @DOCLINE * `@NLFT`: the length of the given TrueType data is not enough for the table directory. Likely the length is incorrect or the data given is not TrueType data.
			MUTT_INVALID_TABLE_DIRECTORY_LENGTH,
			// @DOCLINE * `@NLFT`: the value for "sfntVersion" in the table directory was invalid. Since this is the first value read when loading TrueType data, this most likely means that rather the data given is corrupt, not TrueType data, or is under another incompatible wrapper (such as fonts that use CFF data).
			MUTT_INVALID_TABLE_DIRECTORY_SFNT_VERSION,
			// @DOCLINE * `@NLFT`: the value for "searchRange" in the table directory was invalid.
			MUTT_INVALID_TABLE_DIRECTORY_SEARCH_RANGE,
			// @DOCLINE * `@NLFT`: the value for "entrySelector" in the table directory was invalid.
			MUTT_INVALID_TABLE_DIRECTORY_ENTRY_SELECTOR,
			// @DOCLINE * `@NLFT`: the value for "rangeShift" in the table directory was invalid.
			MUTT_INVALID_TABLE_DIRECTORY_RANGE_SHIFT,
			// @DOCLINE * `@NLFT`: the value for "offset" in a table record was out of range.
			MUTT_INVALID_TABLE_RECORD_OFFSET,
			// @DOCLINE * `@NLFT`: the value for "length" in a table record was out of range.
			MUTT_INVALID_TABLE_RECORD_LENGTH,
			// @DOCLINE * `@NLFT`: the value for "checksum" in a table record was invalid.
			MUTT_INVALID_TABLE_RECORD_CHECKSUM,
			// @DOCLINE * `@NLFT`: the value for the table length of maxp was invalid.
			MUTT_INVALID_MAXP_LENGTH,
			// @DOCLINE * `@NLFT`: the value for "version" in the maxp table was invalid.
			MUTT_INVALID_MAXP_VERSION,
			// @DOCLINE * `@NLFT`: the value for "maxZones" in the maxp table was invalid.
			MUTT_INVALID_MAX_ZONES,
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
				// @DOCLINE * `directory`: a pointer to a directory listing all of the tables provided by the given font, defined below: @NLNT
				muttDirectory* directory;

				// @DOCLINE * `maxp`: a pointer to the maxp table, defined below: @NLNT
				muttMaxp* maxp;
				// @DOCLINE * `maxp_res`: the result of loading the member `maxp`, defined below: @NLNT
				muttResult maxp_res;

				// @DOCLINE * `mem`: the inner allocated memory used for holding necessary data, defined below: @NLNT
				muByte* mem;
				// @DOCLINE * `memlen`: the length of the allocated memory, in bytes, defined below: @NLNT
				size_m memlen;
				// @DOCLINE * `memcur`: offset to the latest unused memory in `mem`, in bytes, defined below: @NLNT
				size_m memcur;
			};

			// @DOCLINE Most of the members are in pairs of pointers and result values. If a requested pointer is 0, it could not be loaded, and its corresponding result value will indicate the result enumerator indicating what went wrong.

			// @DOCLINE The contents of a pointer and result pair for information not included in the load flags are undefined.

			// @DOCLINE Note that if the directory fails to load, the entire loading function fails, and what went wrong is returned in the loading function; this is why there is no respective result for the member `directory`.

		// @DOCLINE ## Directory information

			// @DOCLINE The struct `muttDirectory` is used to list all of the tables provided by a TrueType font. It is stored in the struct `muttFont` as `muttFont->directory`, and is similar to TrueType's table directory. It is loaded permanently with the flag `MUTT_LOAD_DIRECTORY`.

			typedef struct muttTableRecord muttTableRecord;

			// @DOCLINE Its members are:
			struct muttDirectory {
				// @DOCLINE * `num_tables`: equivalent to "numTables" in the table directory, defined below: @NLNT
				uint16_m num_tables;
				// @DOCLINE * `search_range`: equivalent to "searchRange" in the table directory, defined below: @NLNT
				uint16_m search_range;
				// @DOCLINE * `entry_selector`: equivalent to "entrySelector" in the table directory, defined below: @NLNT
				uint16_m entry_selector;
				// @DOCLINE * `range_shift`: equivalent to "rangeShift" in the table directory, defined below: @NLNT
				uint16_m range_shift;
				// @DOCLINE * `table_records`: equivalent to "tableRecords" in the table directory, defined below: @NLNT
				muttTableRecord* table_records;
			};

			// @DOCLINE The struct `muttTableRecord` is similar to TrueType's table record, and has the following members:

			struct muttTableRecord {
				// @DOCLINE * `table_tag`: equivalent to "tableTag" in the table record, defined below: @NLNT
				uint8_m table_tag[4];
				// @DOCLINE * `checksum`: equivalent to "checksum" in the table record, defined below: @NLNT
				uint32_m checksum;
				// @DOCLINE * `offset`: equivalent to "offset" in the table record, defined below: @NLNT
				uint32_m offset;
				// @DOCLINE * `length`: equivalent to "length" in the table record, defined below: @NLNT
				uint32_m length;
			};

		// @DOCLINE ## Maxp information

			// @DOCLINE The struct `muttMaxp` is used to represent the maxp table provided by a TrueType font, stored in the struct `muttFont` as `muttFont->maxp`, and loaded with the flag `MUTT_LOAD_MAXP`.

			// @DOCLINE Its members are:
			struct muttMaxp {
				// @DOCLINE * `version_high`: equivalent to the high bytes of "version" in the maxp table, defined below: @NLNT
				uint16_m version_high;
				// @DOCLINE * `version_low`: equivalent to the low bytes "version" in the maxp table, defined below: @NLNT
				uint16_m version_low;
				// @DOCLINE * `num_glyphs`: equivalent to "numGlyphs" in the maxp table, defined below: @NLNT
				uint16_m num_glyphs;
				// @DOCLINE * `max_points`: equivalent to "maxPoints" in the maxp table, defined below: @NLNT
				uint16_m max_points;
				// @DOCLINE * `max_contours`: equivalent to "maxContours" in the maxp table, defined below: @NLNT
				uint16_m max_contours;
				// @DOCLINE * max_composite_points`: equivalent to "maxCompositePoints" in the maxp table, defined below: @NLNT
				uint16_m max_composite_points;
				// @DOCLINE * `max_composite_contours`: equivalent to "maxCompositeContours" in the maxp table, defined below: @NLNT
				uint16_m max_composite_contours;
				// @DOCLINE * `max_zones`: equivalent to "maxZones" in the maxp table, defined below: @NLNT
				uint16_m max_zones;
				// @DOCLINE * `max_twilight_points`: equivalent to "maxTwilightPoints" in the maxp table, defined below: @NLNT
				uint16_m max_twilight_points;
				// @DOCLINE * `max_storage`: equivalent to "maxStorage" in the maxp table, defined below: @NLNT
				uint16_m max_storage;
				// @DOCLINE * `max_function_defs`: equivalent to "maxFunctionDefs" in the maxp table, defined below: @NLNT
				uint16_m max_function_defs;
				// @DOCLINE * `max_instruction_defs`: equivalent to "maxInstructionDefs" in the maxp table, defined below: @NLNT
				uint16_m max_instruction_defs;
				// @DOCLINE * `max_stack_elements`: equivalent to "maxStackElements" in the maxp table, defined below: @NLNT
				uint16_m max_stack_elements;
				// @DOCLINE * `max_size_of_instructions`: equivalent to "maxSizeOfInstructions" in the maxp table, defined below: @NLNT
				uint16_m max_size_of_instructions;
				// @DOCLINE * `max_component_elements`: equivalent to "maxComponentElements" in the maxp table, defined below: @NLNT
				uint16_m max_component_elements;
				// @DOCLINE * `max_component_depth`: equivalent to "maxComponentDepth" in the maxp table, defined below: @NLNT
				uint16_m max_component_depth;
			};

	// @DOCLINE ## C standard library dependencies

		// @DOCLINE mutt has several C standard library dependencies not provided by its other library dependencies, all of which are overridable by defining them before the inclusion of its header. This is a list of all of those dependencies.

		#if !defined(mu_malloc) || \
			!defined(mu_free) || \
			!defined(mu_realloc)

			// @DOCLINE ## `stdlib.h` dependencies
			#include <stdlib.h>

			// @DOCLINE * `mu_malloc`: equivalent to `malloc`.
			#ifndef mu_malloc
				#define mu_malloc malloc
			#endif

			// @DOCLINE * `mu_free`: equivalent to `free`.
			#ifndef mu_free
				#define mu_free free
			#endif

			// @DOCLINE * `mu_realloc`: equivalent to `realloc`.
			#ifndef mu_realloc
				#define mu_realloc realloc
			#endif

		#endif

		#if !defined(mu_memcpy)

			// @DOCLINE ## `string.h` dependencies
			#include <string.h>

			// @DOCLINE * `mu_memcpy`: equivalent to `memcpy`.
			#ifndef mu_memcpy
				#define mu_memcpy memcpy
			#endif

		#endif

		#if !defined(mu_pow)

			// @DOCLINE ## `math.h` dependencies
			#include <math.h>

			// @DOCLINE * `mu_pow`: equivalent to `pow`.
			#ifndef mu_pow
				#define mu_pow pow
			#endif

		#endif

	#ifdef __cplusplus
	}
	#endif

#endif /* MUTT_H */

/* Names */

	MUDEF const char* mutt_result_get_name(muttResult result) {
		switch (result) {
			default: return "MUTT_UNKNOWN"; break;
			case MUTT_SUCCESS: return "MUTT_SUCCESS"; break;
			case MUTT_FAILED_MALLOC: return "MUTT_FAILED_MALLOC"; break;
			case MUTT_FAILED_REALLOC: return "MUTT_FAILED_REALLOC"; break;
			case MUTT_UNFOUND_TABLE: return "MUTT_UNFOUND_TABLE"; break;
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
			case MUTT_INVALID_MAX_ZONES: return "MUTT_INVALID_MAX_ZONES"; break;
		}
	}

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

		muttResult mutt_load_maxp(muttMaxp* maxp, muByte* data, uint32_m length) {
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
				return MUTT_INVALID_MAX_ZONES;
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

			return MUTT_SUCCESS;
		}

	/* Full loading */

		MUDEF muttResult mutt_load(muByte* data, size_m datalen, muttFont* font, uint32_m load_flags) {
			muttResult res;

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

			/* Find tables */

				// Loop through each table

				for (uint16_m i = 0; i < font->directory->num_tables; i++) {
					// Get tag as a number

					muttTableRecord* record = &font->directory->table_records[i];
					uint32_m tag = mu_rbe_uint32(((muByte*)&record->table_tag));

					switch (tag) {
						default: break;

						// maxp
						case 0x6D617870: {
							// Allocate
							font->maxp = (muttMaxp*)&font->mem[font->memcur];
							font->maxp_res = mutt_get_mem(font, sizeof(font->maxp));

							// If allocation failed:
							if (font->maxp_res != MUTT_SUCCESS) {
								// The table is 0
								font->maxp = 0;
							// If we have allocated:
							} else {
								// Try loading the table
								font->maxp_res = mutt_load_maxp(font->maxp, &data[record->offset], record->length);
								// If it failed, it's 0
								if (font->maxp_res != MUTT_SUCCESS) {
									font->maxp = 0;
								}
							}
						} break;
					}
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

