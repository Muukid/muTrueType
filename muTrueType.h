/*
muTrueType.h - Muukid
Public domain single-file C library for reading, rendering, and placing TrueType data.
https://github.com/Muukid/muTrueType
No warranty implied; use at your own risk.

Licensed under MIT License or public domain, whichever you prefer.
More explicit license information at the end of file.

@DOCBEGIN

# muTrueType v1.0.0

muTrueType (acronymized to "mutt") is a public domain single-file C library for retrieving data from the TrueType file format via its tables (the "[low-level API](#low-level-api)"), rasterizing glyphs to a bitmap (the "[raster API](#raster-api)"), and handling the layout/placement/spacing of multiple glyphs in a graphical context (the "[layout API](#layout-api)"). Its header is automatically defined upon inclusion if not already included (`MUTT_H`), and the source code is defined if `MUTT_IMPLEMENTATION` is defined, following the internal structure of:

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

## Limited cmap format support

Currently, mutt only supports loading cmap formats 0, 4, and 12. This should be okay for most cases, with Apple's TrueType spec. saying, "Modern font generation tools might not need to be able to write general-purpose cmaps in formats other than 4 and 12."

## Optimal cmap conversions

mutt does not use the most efficient algorithms to convert codepoints to glyph IDs and vice versa for cmap formats.

# TrueType documentation

Involved usage of the low-level API of mutt necessitates an understanding of the TrueType documentation. Terms from the TrueType documentation will be used with the assumption that the user has read it and understands these terms.

mutt is developed primarily off of these sources of documentation:

* [OpenType spec](https://learn.microsoft.com/en-us/typography/opentype/spec/).

* [TrueType reference manual](https://developer.apple.com/fonts/TrueType-Reference-Manual/).

@DOCEND
*/

#ifndef MUTT_H
	#define MUTT_H
	
	// @DOCLINE # Other library dependencies
		// @DOCLINE mutt has a dependency on:
		
		// @DOCLINE * [muUtility v2.0.1](https://github.com/Muukid/muUtility/releases/tag/v2.0.1).
		// @IGNORE
			#if !defined(MU_CHECK_VERSION_MISMATCHING) && defined(MUU_H) && \
				(MUU_VERSION_MAJOR != 2 || MUU_VERSION_MINOR != 0 || MUU_VERSION_PATCH != 1)

				#pragma message("[MUTT] muUtility's header has already been defined, but version doesn't match the version that this library is built for. This may lead to errors, warnings, or unexpected behavior. Define MU_CHECK_VERSION_MISMATCHING before this to turn off this message.")

			#endif

			#ifndef MUU_H
				#define MUU_H

				// @DOCLINE # Version
					// @DOCLINE The macros `MUU_VERSION_MAJOR`, `MUU_VERSION_MINOR`, and `MUU_VERSION_PATCH` are defined to match its respective release version, following the formatting of `MAJOR.MINOR.PATCH`.
					
					#define MUU_VERSION_MAJOR 2
					#define MUU_VERSION_MINOR 0
					#define MUU_VERSION_PATCH 1

				// @DOCLINE # `MUDEF`
					// @DOCLINE The `MUDEF` macro is used by virtually all mu libraries, and is generally added before a header-defined variable or function. Its default value is `extern`, but can be changed to `static` by defining `MU_STATIC` before the header section of muUtility is defined. Its value can also be overwritten entirely to anything else by directly defining `MUDEF`.
					
					#ifndef MUDEF
						#ifdef MU_STATIC
							#define MUDEF static
						#else
							#define MUDEF extern
						#endif
					#endif
				
				// @DOCLINE # Secure warnings
					// @DOCLINE mu libraries often use non-secure functions that will trigger warnings on certain compilers. These warnings are, to put it lightly, dumb, so muUtility defines `_CRT_SECURE_NO_WARNINGS`. However, it is not guaranteed that this definition will actually turn the warnings off, which, at that point, they have to be manually turned off by the user. This functionality can be turned off by defining `MU_SECURE_WARNINGS`.
					#if !defined(MU_SECURE_WARNINGS) && !defined(_CRT_SECURE_NO_WARNINGS)
						#define _CRT_SECURE_NO_WARNINGS
					#endif
				
				// @DOCLINE # C++ extern
					// @DOCLINE Every mu library's primary code externs "C" within the main chunks of their code; macros `MU_CPP_EXTERN_START` and `MU_CPP_EXTERN_END` are defined to make this process easier, and would read like this:
					/* @DOCBEGIN
					```
					MU_CPP_EXTERN_START

					// Library code goes here...

					MU_CPP_EXTERN_END
					```
					@DOCEND */
					#ifdef __cplusplus
						#define MU_CPP_EXTERN_START extern "C" {
						#define MU_CPP_EXTERN_END   }
					#else
						#define MU_CPP_EXTERN_START
						#define MU_CPP_EXTERN_END
					#endif
				
				MU_CPP_EXTERN_START

				// @DOCLINE # C standard library dependencies

					// @DOCLINE muUtility has several C standard library dependencies, all of which are overridable by defining them before the inclusion of the file. The following is a list of those dependencies.

					// @DOCLINE ## `stdint.h` dependencies
					#if !defined(int8_m) || \
						!defined(uint8_m) || \
						!defined(int16_m) || \
						!defined(uint16_m) || \
						!defined(int32_m) || \
						!defined(uint32_m) || \
						!defined(int64_m) || \
						!defined(uint64_m) || \
						!defined(MU_SIZE_MAX)

						#define __STDC_LIMIT_MACROS
						#define __STDC_CONSTANT_MACROS
						#include <stdint.h>
						
						// @DOCLINE * `int8_m` - equivalent to `int8_t` if `INT8_MAX` is defined; `char` if otherwise.
						#ifndef int8_m
							#ifdef INT8_MAX
								#define int8_m int8_t
							#else
								#define int8_m char
							#endif
						#endif

						// @DOCLINE * `uint8_m` - equivalent to `uint8_t` if `UINT8_MAX` is defined; `unsigned char` if otherwise.
						#ifndef uint8_m
							#ifdef UINT8_MAX
								#define uint8_m uint8_t
							#else
								#define uint8_m unsigned char
							#endif
						#endif

						// @DOCLINE * `int16_m` - equivalent to `int16_t` if `INT16_MAX` is defined; `short` if otherwise.
						#ifndef int16_m
							#ifdef INT16_MAX
								#define int16_m int16_t
							#else
								#define int16_m short
							#endif
						#endif

						// @DOCLINE * `uint16_m` - equivalent to `uint16_t` if `UINT16_MAX` is defined; `unsigned short` if otherwise.
						#ifndef uint16_m
							#ifdef UINT16_MAX
								#define uint16_m uint16_t
							#else
								#define uint16_m unsigned short
							#endif
						#endif

						// @DOCLINE * `int32_m` - equivalent to `int32_t` if `INT32_MAX` is defined; `long` if otherwise.
						#ifndef int32_m
							#ifdef INT32_MAX
								#define int32_m int32_t
							#else
								#define int32_m long
							#endif
						#endif

						// @DOCLINE * `uint32_m` - equivalent to `uint32_t` if `UINT32_MAX` is defined; `unsigned long` if otherwise.
						#ifndef uint32_m
							#ifdef UINT32_MAX
								#define uint32_m uint32_t
							#else
								#define uint32_m unsigned long
							#endif
						#endif

						// @DOCLINE * `int64_m` - equivalent to `int64_t` if `INT64_MAX` is defined; `long long` if otherwise.
						#ifndef int64_m
							#ifdef INT64_MAX
								#define int64_m int64_t
							#else
								#define int64_m long long
							#endif
						#endif

						// @DOCLINE * `uint64_m` - equivalent to `uint64_t` if `UINT64_MAX` is defined; `unsigned long long` if otherwise.
						#ifndef uint64_m
							#ifdef UINT64_MAX
								#define uint64_m uint64_t
							#else
								#define uint64_m unsigned long long
							#endif
						#endif

						// @DOCLINE * `MU_SIZE_MAX` - equivalent to `SIZE_MAX`.
						#ifndef MU_SIZE_MAX
							#define MU_SIZE_MAX SIZE_MAX
						#endif

					#endif /* stdint.h */

					// @DOCLINE ## `stddef.h` dependencies
					#if !defined(size_m)

						#include <stddef.h>

						// @DOCLINE * `size_m` - equivalent to `size_t`.
						#ifndef size_m
							#define size_m size_t
						#endif

					#endif /* stddef.h */

					// @DOCLINE ## `stdbool.h` dependencies
					#if !defined(muBool) || \
						!defined(MU_TRUE) || \
						!defined(MU_FALSE)

						#include <stdbool.h>

						// @DOCLINE * `muBool` - equivalent to `bool`.
						#ifndef muBool
							#define muBool bool
						#endif

						// @DOCLINE * `MU_TRUE` - equivalent to `true`.
						#ifndef MU_TRUE
							#define MU_TRUE true
						#endif

						// @DOCLINE * `MU_FALSE` - equivalent to `false`.
						#ifndef MU_FALSE
							#define MU_FALSE false
						#endif

					#endif /* stdbool.h */

				// @DOCLINE # Zero struct

					// @DOCLINE There are two macros, `MU_ZERO_STRUCT` and `MU_ZERO_STRUCT_CONST`, which are functions used to zero-out a struct's contents, with their only parameter being the struct type. The reason this needs to be defined is because the way C and C++ syntax handles an empty struct are different, and need to be adjusted for. These macros are overridable by defining them before `muUtility.h` is included.

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

				// @DOCLINE # Byte manipulation

					// @DOCLINE muUtility defines several inline functions that read a value from a given array of bytes. Internally, they're all defined with the prefix `muu_...`, and then a macro is defined for them as `MU_...` (with change in capitalization after the prefix as well). The macros for these functions can be overridden, and, in such case, the original function will go undefined. For example, the function `muu_rleu8` is primarily referenced via the macro `MU_RLEU8`, and if `MU_RLEU8` is overridden, `muu_rleu8` is never defined and is not referenceable.

					// @DOCLINE All reading functions take in a pointer of bytes as their only parameter and have a return type of the fixed-width size of bits in question; for example, `muu_rleu8` is defined as:

					/* @DOCBEGIN
					```
					MUDEF inline uint8_m muu_rleu8(muByte* b);
					```
					@DOCEND */

					// @DOCLINE All writing functions take in a pointer of bytes as their first parameter, the number to be written as the second parameter, and have a return type of void; for example, `muu_wleu8` is defined as:

					/* @DOCBEGIN
					```
					MUDEF inline void muu_wleu8(muByte* b, uint8_m n);
					```
					@DOCEND */

					// @DOCLINE The exception to this is 24-bit, in which case, the fixed-width types are 32-bit (`uint32_m` / `int32_m`).

					// @DOCLINE ## Byte type

						// @DOCLINE muUtility defines the type `muByte` to refer to a byte. It is defined as `uint8_m`, and is overridable.
						#ifndef muByte
							#define muByte uint8_m
						#endif

					// @DOCLINE ## 8-bit

						// @DOCLINE The following macros exist for byte manipulation regarding 8-bit integers:

						// @DOCLINE * `MU_RLEU8` - reads an unsigned 8-bit integer from little-endian byte data; overridable macro to `muu_rleu8`.
						#ifndef MU_RLEU8
							MUDEF inline uint8_m muu_rleu8(muByte* b) {
								return b[0];
							}
							#define MU_RLEU8 muu_rleu8
						#endif

						// @DOCLINE * `MU_WLEU8` - writes an unsigned 8-bit integer to little-endian byte data; overridable macro to `muu_wleu8`.
						#ifndef MU_WLEU8
							MUDEF inline void muu_wleu8(muByte* b, uint8_m n) {
								b[0] = n;
							}
							#define MU_WLEU8 muu_wleu8
						#endif

						// @DOCLINE * `MU_RLES8` - reads a signed 8-bit integer from little-endian byte data; overridable macro to `muu_rles8`.
						#ifndef MU_RLES8
							MUDEF inline int8_m muu_rles8(muByte* b) {
								// I'm PRETTY sure this is okay...
								return *(int8_m*)b;
							}
							#define MU_RLES8 muu_rles8
						#endif

						// @DOCLINE * `MU_WLES8` - writes a signed 8-bit integer to little-endian byte data; overridable macro to `muu_wles8`.
						#ifndef MU_WLES8
							MUDEF inline void muu_wles8(muByte* b, int8_m n) {
								((int8_m*)(b))[0] = n;
							}
							#define MU_WLES8 muu_wles8
						#endif

						// @DOCLINE * `MU_RBEU8` - reads an unsigned 8-bit integer from big-endian byte data; overridable macro to `muu_rbeu8`.
						#ifndef MU_RBEU8
							MUDEF inline uint8_m muu_rbeu8(muByte* b) {
								return b[0];
							}
							#define MU_RBEU8 muu_rbeu8
						#endif

						// @DOCLINE * `MU_WBEU8` - writes an unsigned 8-bit integer to big-endian byte data; overridable macro to `muu_wbeu8`.
						#ifndef MU_WBEU8
							MUDEF inline void muu_wbeu8(muByte* b, uint8_m n) {
								b[0] = n;
							}
							#define MU_WBEU8 muu_wbeu8
						#endif

						// @DOCLINE * `MU_RBES8` - reads a signed 8-bit integer from big-endian byte data; overridable macro to `muu_rbes8`.
						#ifndef MU_RBES8
							MUDEF inline int8_m muu_rbes8(muByte* b) {
								return *(int8_m*)b;
							}
							#define MU_RBES8 muu_rbes8
						#endif

						// @DOCLINE * `MU_WBES8` - writes a signed 8-bit integer to big-endian byte data; overridable macro to `muu_wbes8`.
						#ifndef MU_WBES8
							MUDEF inline void muu_wbes8(muByte* b, int8_m n) {
								((int8_m*)(b))[0] = n;
							}
							#define MU_WBES8 muu_wbes8
						#endif

					// @DOCLINE ## 16-bit

						// @DOCLINE The following macros exist for byte manipulation regarding 16-bit integers:

						// @DOCLINE * `MU_RLEU16` - reads an unsigned 16-bit integer from little-endian byte data; overridable macro to `muu_rleu16`.
						#ifndef MU_RLEU16
							MUDEF inline uint16_m muu_rleu16(muByte* b) {
								return (
									((uint16_m)(b[0]) << 0) |
									((uint16_m)(b[1]) << 8)
								);
							}
							#define MU_RLEU16 muu_rleu16
						#endif

						// @DOCLINE * `MU_WLEU16` - writes an unsigned 16-bit integer to little-endian byte data; overridable macro to `muu_wleu16`.
						#ifndef MU_WLEU16
							MUDEF inline void muu_wleu16(muByte* b, uint16_m n) {
								b[0] = (uint8_m)(n >> 0);
								b[1] = (uint8_m)(n >> 8);
							}
							#define MU_WLEU16 muu_wleu16
						#endif

						// @DOCLINE * `MU_RLES16` - reads a signed 16-bit integer from little-endian byte data; overridable macro to `muu_rles16`.
						#ifndef MU_RLES16
							MUDEF inline int16_m muu_rles16(muByte* b) {
								uint16_m u16 = muu_rleu16(b);
								return *(int16_m*)&u16;
							}
							#define MU_RLES16 muu_rles16
						#endif

						// @DOCLINE * `MU_WLES16` - writes a signed 16-bit integer to little-endian byte data; overridable macro to `muu_wles16`.
						#ifndef MU_WLES16
							MUDEF inline void muu_wles16(muByte* b, int16_m n) {
								uint16_m un = *(uint16_m*)&n;
								b[0] = (uint8_m)(un >> 0);
								b[1] = (uint8_m)(un >> 8);
							}
							#define MU_WLES16 muu_wles16
						#endif

						// @DOCLINE * `MU_RBEU16` - reads an unsigned 16-bit integer from big-endian byte data; overridable macro to `muu_rbeu16`.
						#ifndef MU_RBEU16
							MUDEF inline uint16_m muu_rbeu16(muByte* b) {
								return (
									((uint16_m)(b[1]) << 0) |
									((uint16_m)(b[0]) << 8)
								);
							}
							#define MU_RBEU16 muu_rbeu16
						#endif

						// @DOCLINE * `MU_WBEU16` - writes an unsigned 16-bit integer to big-endian byte data; overridable macro to `muu_wbeu16`.
						#ifndef MU_WBEU16
							MUDEF inline void muu_wbeu16(muByte* b, uint16_m n) {
								b[1] = (uint8_m)(n >> 0);
								b[0] = (uint8_m)(n >> 8);
							}
							#define MU_WBEU16 muu_wbeu16
						#endif

						// @DOCLINE * `MU_RBES16` - reads a signed 16-bit integer from big-endian byte data; overridable macro to `muu_rbes16`.
						#ifndef MU_RBES16
							MUDEF inline int16_m muu_rbes16(muByte* b) {
								uint16_m u16 = muu_rbeu16(b);
								return *(int16_m*)&u16;
							}
							#define MU_RBES16 muu_rbes16
						#endif

						// @DOCLINE * `MU_WBES16` - writes a signed 16-bit integer to big-endian byte data; overridable macro to `muu_wbes16`.
						#ifndef MU_WBES16
							MUDEF inline void muu_wbes16(muByte* b, int16_m n) {
								uint16_m un = *(uint16_m*)&n;
								b[1] = (uint8_m)(un >> 0);
								b[0] = (uint8_m)(un >> 8);
							}
							#define MU_WBES16 muu_wbes16
						#endif

					// @DOCLINE ## 24-bit

						// @DOCLINE The following macros exist for byte manipulation regarding 24-bit integers:

						// @DOCLINE * `MU_RLEU24` - reads an unsigned 24-bit integer from little-endian byte data; overridable macro to `muu_rleu24`.
						#ifndef MU_RLEU24
							MUDEF inline uint32_m muu_rleu24(muByte* b) {
								return (
									((uint32_m)(b[0]) << 0) |
									((uint32_m)(b[1]) << 8) |
									((uint32_m)(b[2]) << 16)
								);
							}
							#define MU_RLEU24 muu_rleu24
						#endif

						// @DOCLINE * `MU_WLEU24` - writes an unsigned 24-bit integer to little-endian byte data; overridable macro to `muu_wleu24`.
						#ifndef MU_WLEU24
							MUDEF inline void muu_wleu24(muByte* b, uint32_m n) {
								b[0] = (uint8_m)(n >> 0);
								b[1] = (uint8_m)(n >> 8);
								b[2] = (uint8_m)(n >> 16);
							}
							#define MU_WLEU24 muu_wleu24
						#endif

						// @DOCLINE * `MU_RLES24` - reads a signed 24-bit integer from little-endian byte data; overridable macro to `muu_rles24`.
						#ifndef MU_RLES24
							MUDEF inline int32_m muu_rles24(muByte* b) {
								uint32_m u24 = muu_rleu24(b);
								return *(int32_m*)&u24;
							}
							#define MU_RLES24 muu_rles24
						#endif

						// @DOCLINE * `MU_WLES24` - writes a signed 24-bit integer to little-endian byte data; overridable macro to `muu_wles24`.
						#ifndef MU_WLES24
							MUDEF inline void muu_wles24(muByte* b, int32_m n) {
								// Probably definitely doesn't work with signed integers; fix later
								uint32_m un = *(uint32_m*)&n;
								b[0] = (uint8_m)(un >> 0);
								b[1] = (uint8_m)(un >> 8);
								b[2] = (uint8_m)(un >> 16);
							}
							#define MU_WLES24 muu_wles24
						#endif

						// @DOCLINE * `MU_RBEU24` - reads an unsigned 24-bit integer from big-endian byte data; overridable macro to `muu_rbeu24`.
						#ifndef MU_RBEU24
							MUDEF inline uint32_m muu_rbeu24(muByte* b) {
								return (
									((uint32_m)(b[2]) << 0) |
									((uint32_m)(b[1]) << 8) |
									((uint32_m)(b[0]) << 16)
								);
							}
							#define MU_RBEU24 muu_rbeu24
						#endif

						// @DOCLINE * `MU_WBEU24` - writes an unsigned 24-bit integer to big-endian byte data; overridable macro to `muu_wbeu24`.
						#ifndef MU_WBEU24
							MUDEF inline void muu_wbeu24(muByte* b, uint32_m n) {
								b[2] = (uint8_m)(n >> 0);
								b[1] = (uint8_m)(n >> 8);
								b[0] = (uint8_m)(n >> 16);
							}
							#define MU_WBEU24 muu_wbeu24
						#endif

						// @DOCLINE * `MU_RBES24` - reads a signed 24-bit integer from big-endian byte data; overridable macro to `muu_rbes24`.
						#ifndef MU_RBES24
							MUDEF inline int32_m muu_rbes24(muByte* b) {
								uint32_m u24 = muu_rbeu24(b);
								return *(int32_m*)&u24;
							}
							#define MU_RBES24 muu_rbes24
						#endif

						// @DOCLINE * `MU_WBES24` - writes a signed 24-bit integer to big-endian byte data; overridable macro to `muu_wbes24`.
						#ifndef MU_WBES24
							MUDEF inline void muu_wbes24(muByte* b, int32_m n) {
								uint32_m un = *(uint32_m*)&n;
								b[2] = (uint8_m)(un >> 0);
								b[1] = (uint8_m)(un >> 8);
								b[0] = (uint8_m)(un >> 16);
							}
							#define MU_WBES24 muu_wbes24
						#endif

					// @DOCLINE ## 32-bit

						// @DOCLINE The following macros exist for byte manipulation regarding 32-bit integers:

						// @DOCLINE * `MU_RLEU32` - reads an unsigned 32-bit integer from little-endian byte data; overridable macro to `muu_rleu32`.
						#ifndef MU_RLEU32
							MUDEF inline uint32_m muu_rleu32(muByte* b) {
								return (
									((uint32_m)(b[0]) << 0)  |
									((uint32_m)(b[1]) << 8)  |
									((uint32_m)(b[2]) << 16) |
									((uint32_m)(b[3]) << 24)
								);
							}
							#define MU_RLEU32 muu_rleu32
						#endif

						// @DOCLINE * `MU_WLEU32` - writes an unsigned 32-bit integer to little-endian byte data; overridable macro to `muu_wleu32`.
						#ifndef MU_WLEU32
							MUDEF inline void muu_wleu32(muByte* b, uint32_m n) {
								b[0] = (uint8_m)(n >> 0);
								b[1] = (uint8_m)(n >> 8);
								b[2] = (uint8_m)(n >> 16);
								b[3] = (uint8_m)(n >> 24);
							}
							#define MU_WLEU32 muu_wleu32
						#endif

						// @DOCLINE * `MU_RLES32` - reads a signed 32-bit integer from little-endian byte data; overridable macro to `muu_rles32`.
						#ifndef MU_RLES32
							MUDEF inline int32_m muu_rles32(muByte* b) {
								uint32_m u32 = muu_rleu32(b);
								return *(int32_m*)&u32;
							}
							#define MU_RLES32 muu_rles32
						#endif

						// @DOCLINE * `MU_WLES32` - writes a signed 32-bit integer to little-endian byte data; overridable macro to `muu_wles32`.
						#ifndef MU_WLES32
							MUDEF inline void muu_wles32(muByte* b, int32_m n) {
								uint32_m un = *(uint32_m*)&n;
								b[0] = (uint8_m)(un >> 0);
								b[1] = (uint8_m)(un >> 8);
								b[2] = (uint8_m)(un >> 16);
								b[3] = (uint8_m)(un >> 24);
							}
							#define MU_WLES32 muu_wles32
						#endif

						// @DOCLINE * `MU_RBEU32` - reads an unsigned 32-bit integer from big-endian byte data; overridable macro to `muu_rbeu32`.
						#ifndef MU_RBEU32
							MUDEF inline uint32_m muu_rbeu32(muByte* b) {
								return (
									((uint32_m)(b[3]) << 0)  |
									((uint32_m)(b[2]) << 8)  |
									((uint32_m)(b[1]) << 16) |
									((uint32_m)(b[0]) << 24)
								);
							}
							#define MU_RBEU32 muu_rbeu32
						#endif

						// @DOCLINE * `MU_WBEU32` - writes an unsigned 32-bit integer to big-endian byte data; overridable macro to `muu_wbeu32`.
						#ifndef MU_WBEU32
							MUDEF inline void muu_wbeu32(muByte* b, uint32_m n) {
								b[3] = (uint8_m)(n >> 0);
								b[2] = (uint8_m)(n >> 8);
								b[1] = (uint8_m)(n >> 16);
								b[0] = (uint8_m)(n >> 24);
							}
							#define MU_WBEU32 muu_wbeu32
						#endif

						// @DOCLINE * `MU_RBES32` - reads a signed 32-bit integer from big-endian byte data; overridable macro to `muu_rbes32`.
						#ifndef MU_RBES32
							MUDEF inline int32_m muu_rbes32(muByte* b) {
								uint32_m u32 = muu_rbeu32(b);
								return *(int32_m*)&u32;
							}
							#define MU_RBES32 muu_rbes32
						#endif

						// @DOCLINE * `MU_WBES32` - writes a signed 32-bit integer to big-endian byte data; overridable macro to `muu_wbes32`.
						#ifndef MU_WBES32
							MUDEF inline void muu_wbes32(muByte* b, int32_m n) {
								uint32_m un = *(uint32_m*)&n;
								b[3] = (uint8_m)(un >> 0);
								b[2] = (uint8_m)(un >> 8);
								b[1] = (uint8_m)(un >> 16);
								b[0] = (uint8_m)(un >> 24);
							}
							#define MU_WBES32 muu_wbes32
						#endif

					// @DOCLINE ## 64-bit

						// @DOCLINE The following macros exist for byte manipulation regarding 64-bit integers:

						// @DOCLINE * `MU_RLEU64` - reads an unsigned 64-bit integer from little-endian byte data; overridable macro to `muu_rleu64`.
						#ifndef MU_RLEU64
							MUDEF inline uint64_m muu_rleu64(muByte* b) {
								return (
									((uint64_m)(b[0]) << 0)  |
									((uint64_m)(b[1]) << 8)  |
									((uint64_m)(b[2]) << 16) |
									((uint64_m)(b[3]) << 24) |
									((uint64_m)(b[4]) << 32) |
									((uint64_m)(b[5]) << 40) |
									((uint64_m)(b[6]) << 48) |
									((uint64_m)(b[7]) << 56)
								);
							}
							#define MU_RLEU64 muu_rleu64
						#endif

						// @DOCLINE * `MU_WLEU64` - writes an unsigned 64-bit integer to little-endian byte data; overridable macro to `muu_wleu64`.
						#ifndef MU_WLEU64
							MUDEF inline void muu_wleu64(muByte* b, uint64_m n) {
								b[0] = (uint8_m)(n >> 0);
								b[1] = (uint8_m)(n >> 8);
								b[2] = (uint8_m)(n >> 16);
								b[3] = (uint8_m)(n >> 24);
								b[4] = (uint8_m)(n >> 32);
								b[5] = (uint8_m)(n >> 40);
								b[6] = (uint8_m)(n >> 48);
								b[7] = (uint8_m)(n >> 56);
							}
							#define MU_WLEU64 muu_wleu64
						#endif

						// @DOCLINE * `MU_RLES64` - reads a signed 64-bit integer from little-endian byte data; overridable macro to `muu_rles64`.
						#ifndef MU_RLES64
							MUDEF inline int64_m muu_rles64(muByte* b) {
								uint64_m u64 = muu_rleu64(b);
								return *(int64_m*)&u64;
							}
							#define MU_RLES64 muu_rles64
						#endif

						// @DOCLINE * `MU_WLES64` - writes a signed 64-bit integer to little-endian byte data; overridable macro to `muu_wles64`.
						#ifndef MU_WLES64
							MUDEF inline void muu_wles64(muByte* b, int64_m n) {
								uint64_m un = *(uint64_m*)&n;
								b[0] = (uint8_m)(un >> 0);
								b[1] = (uint8_m)(un >> 8);
								b[2] = (uint8_m)(un >> 16);
								b[3] = (uint8_m)(un >> 24);
								b[4] = (uint8_m)(un >> 32);
								b[5] = (uint8_m)(un >> 40);
								b[6] = (uint8_m)(un >> 48);
								b[7] = (uint8_m)(un >> 56);
							}
							#define MU_WLES64 muu_wles64
						#endif

						// @DOCLINE * `MU_RBEU64` - reads an unsigned 64-bit integer from big-endian byte data; overridable macro to `muu_rbeu64`.
						#ifndef MU_RBEU64
							MUDEF inline uint64_m muu_rbeu64(muByte* b) {
								return (
									((uint64_m)(b[7]) << 0)  |
									((uint64_m)(b[6]) << 8)  |
									((uint64_m)(b[5]) << 16) |
									((uint64_m)(b[4]) << 24) |
									((uint64_m)(b[3]) << 32) |
									((uint64_m)(b[2]) << 40) |
									((uint64_m)(b[1]) << 48) |
									((uint64_m)(b[0]) << 56)
								);
							}
							#define MU_RBEU64 muu_rbeu64
						#endif

						// @DOCLINE * `MU_WBEU64` - writes an unsigned 64-bit integer to big-endian byte data; overridable macro to `muu_wbeu64`.
						#ifndef MU_WBEU64
							MUDEF inline void muu_wbeu64(muByte* b, uint64_m n) {
								b[7] = (uint8_m)(n >> 0);
								b[6] = (uint8_m)(n >> 8);
								b[5] = (uint8_m)(n >> 16);
								b[4] = (uint8_m)(n >> 24);
								b[3] = (uint8_m)(n >> 32);
								b[2] = (uint8_m)(n >> 40);
								b[1] = (uint8_m)(n >> 48);
								b[0] = (uint8_m)(n >> 56);
							}
							#define MU_WBEU64 muu_wbeu64
						#endif

						// @DOCLINE * `MU_RBES64` - reads a signed 64-bit integer from big-endian byte data; overridable macro to `muu_rbes64`.
						#ifndef MU_RBES64
							MUDEF inline int64_m muu_rbes64(muByte* b) {
								uint64_m u64 = muu_rbeu64(b);
								return *(int64_m*)&u64;
							}
							#define MU_RBES64 muu_rbes64
						#endif

						// @DOCLINE * `MU_WBES64` - writes a signed 64-bit integer to big-endian byte data; overridable macro to `muu_wbes64`.
						#ifndef MU_WBES64
							MUDEF inline void muu_wbes64(muByte* b, int64_m n) {
								uint64_m un = *(uint64_m*)&n;
								b[7] = (uint8_m)(un >> 0);
								b[6] = (uint8_m)(un >> 8);
								b[5] = (uint8_m)(un >> 16);
								b[4] = (uint8_m)(un >> 24);
								b[3] = (uint8_m)(un >> 32);
								b[2] = (uint8_m)(un >> 40);
								b[1] = (uint8_m)(un >> 48);
								b[0] = (uint8_m)(un >> 56);
							}
							#define MU_WBES64 muu_wbes64
						#endif

				// @DOCLINE # Set result

					/* @DOCBEGIN

					The `MU_SET_RESULT(res, val)` macro is an overridable function that checks if the given parameter `res` is a null pointer. If it is, it does nothing, but if it isn't, it dereferences the pointer and sets the value to `val`. This macro saves a lot of code, shrinking down what would be this:

					```c
					if (result) {
						*result = ...;
					}
					```

					into this:

					```c
					MU_SET_RESULT(result, ...)
					```

					@DOCEND */

					#ifndef MU_SET_RESULT
						#define MU_SET_RESULT(res, val) if(res){*res=val;}
					#endif

				// @DOCLINE # Enum

					/* @DOCBEGIN

					The `MU_ENUM(name, ...)` macro is an overridable function used to declare an enumerator. `name` is the name of the enumerator type, and `...` are all of the values. The reason why one would prefer this over the traditional way of declaring enumerators is because it actually makes it a `size_m`, which can avoid errors on certain compilers (looking at you, Microsoft) in regards to treating enumerators like values. It expands like this:

					```c
					enum _##name {
						__VA_ARGS__
					};
					typedef enum _##name _##name;
					typedef size_m name;
					```

					@DOCEND */

					#define MU_ENUM(name, ...) enum _##name{__VA_ARGS__};typedef enum _##name _##name;typedef size_m name;

				// @DOCLINE # Operating system recognition

					/* @DOCBEGIN

					The macros `MU_WIN32` or `MU_LINUX` are defined (if neither were defined before) in order to allow mu libraries to easily check if they're running on a Windows or Linux system.

					`MU_WIN32` will be defined if `WIN32` or `_WIN32` are defined, one of which is usually pre-defined on Windows systems.

					`MU_LINUX` will be defined if `__linux__` is defined.

					@DOCEND */

					#if !defined(MU_WIN32) && !defined(MU_LINUX)
						#if defined(WIN32) || defined(_WIN32)
							#define MU_WIN32
						#endif
						#if defined(__linux__)
							#define MU_LINUX
						#endif
					#endif

				MU_CPP_EXTERN_END

			#endif /* MUU_H */
		// @ATTENTION

		// @DOCLINE > Note that mu libraries store their dependencies within their files, so you don't need to import these dependencies yourself; this section is purely to provide more information about the contents that this file defines. The libraries listed may also have other dependencies that they also include that aren't explicitly listed here.

	MU_CPP_EXTERN_START

	// Typedefs explained later
	typedef uint32_m muttResult;

	// @DOCLINE # Loading a TrueType font

		typedef struct muttFont muttFont;

		// @DOCLINE All major parts of the mutt API rely on loading a TrueType font and then reading data from it, which is encapsulated in the `muttFont` struct, which is described [later in the lower-level API section](#font-struct). Most casual usage of the mutt API only needs to treat `muttFont` as a handle to the font itself.

		// @DOCLINE ## Loading a font

			typedef uint32_m muttLoadFlags;

			// @DOCLINE To load a TrueType font into a `muttFont` struct, the function `mutt_load` is used, defined below: @NLNT
			MUDEF muttResult mutt_load(muByte* data, uint64_m datalen, muttFont* font, muttLoadFlags load_flags);

			// @DOCLINE `data` and `datalen` should be the raw binary data of the font file, and should be loaded by the user themselves. `font` is the `muttFont` struct to be filled in with information after loading. `load_flags` is a value whose bits indicate which tables to load; more elaboration on this is given in the [load flags section](#font-load-flags).

			// @DOCLINE If the result returned by mutt is fatal, the contents of `font` are undefined. If the result returned by mutt isn't fatal, the font has been successfully loaded, and must be deloaded at some point.

			// @DOCLINE Once this function has finished executing, there are no internal dependencies on the pointer to the data given, and can be safely freed.

		// @DOCLINE ## Deloading a font

			// @DOCLINE To deload a font, the function `mutt_deload` is used, defined below: @NLNT
			MUDEF void mutt_deload(muttFont* font);

			// @DOCLINE This function must be called on every successfully loaded font at some point. The contents of `font` are undefined after `mutt_deload` has been called on it.

		// @DOCLINE ## Font load flags

			// @DOCLINE To customize what tables are loaded when loading a TrueType font, the type `muttLoadFlags` exists (typedef for `uint32_m`) whose bits indicate what tables should be loaded. It has the following defined values:

			// @DOCLINE * [0x00000001] `MUTT_LOAD_MAXP` - load the [maxp table](#maxp-table).
			#define MUTT_LOAD_MAXP 0x00000001
			// @DOCLINE * [0x00000002] `MUTT_LOAD_HEAD` - load the [head table](#head-table).
			#define MUTT_LOAD_HEAD 0x00000002
			// @DOCLINE * [0x00000004] `MUTT_LOAD_HHEA` - load the [hhea table](#hhea-table).
			#define MUTT_LOAD_HHEA 0x00000004
			// @DOCLINE * [0x00000008] `MUTT_LOAD_HMTX` - load the [hmtx table](#hmtx-table).
			#define MUTT_LOAD_HMTX 0x00000008
			// @DOCLINE * [0x00000010] `MUTT_LOAD_LOCA` - load the [loca table](#loca-table).
			#define MUTT_LOAD_LOCA 0x00000010
			// @DOCLINE * [0x00000020] `MUTT_LOAD_POST` - load the [post table](#post-table).
			#define MUTT_LOAD_POST 0x00000020
			// @DOCLINE * [0x00000040] `MUTT_LOAD_NAME` - load the [name table](#name-table).
			#define MUTT_LOAD_NAME 0x00000040
			// @DOCLINE * [0x00000080] `MUTT_LOAD_GLYF` - load the [glyf table](#glyf-table).
			#define MUTT_LOAD_GLYF 0x00000080
			// @DOCLINE * [0x00000100] `MUTT_LOAD_CMAP` - load the [cmap table](#cmap-table).
			#define MUTT_LOAD_CMAP 0x00000100

			// @DOCLINE To see which tables successfully loaded, see the [section covering the font struct](#font-struct).

			// @DOCLINE ### Font load flag groups

				// @DOCLINE For most users, it is unnecessary or confusing to specify all the tables they want manually, so several macros are defined that set the bits for several tables. These are the defined flag groups:

				// @DOCLINE * [0x000001FF] `MUTT_LOAD_REQUIRED` - load the tables required by the TrueType specification (maxp, head, hhea, hmtx, loca, post, name, glyf, and cmap).
				#define MUTT_LOAD_REQUIRED 0x000001FF

				// @DOCLINE * [0xFFFFFFFF] `MUTT_LOAD_ALL` - loads all tables that could be supported by mutt.
				#define MUTT_LOAD_ALL 0xFFFFFFFF

	// @DOCLINE # Low-level API

		// @DOCLINE The low-level API of mutt is designed to support reading information from the tables provided by TrueType. It is used internally by all other parts of the mutt API. All values provided by the low-level API have been checked to be valid, and are guaranteed to be valid once given to the user, unless explicitly stated otherwise.

		// @DOCLINE ## Font struct

			typedef struct muttDirectory muttDirectory;
			typedef struct muttMaxp muttMaxp;
			typedef struct muttHead muttHead;
			typedef struct muttHhea muttHhea;
			typedef struct muttHmtx muttHmtx;
			typedef union  muttLoca muttLoca;
			typedef struct muttPost muttPost;
			typedef struct muttName muttName;
			typedef struct muttGlyf muttGlyf;
			typedef struct muttCmap muttCmap;

			// @DOCLINE The font struct, `muttFont`, is the primary way of reading information from TrueType tables, holding pointers to each table's defined data, and is automatically filled using the function [`mutt_load`](#loading-a-font). It has the following members:

			struct muttFont {
				// @DOCLINE * `@NLFT load_flags` - flags indicating which requested tables successfully loaded.
				muttLoadFlags load_flags;
				// @DOCLINE * `@NLFT fail_load_flags` - flags indicating which requested tables did not successfully load.
				muttLoadFlags fail_load_flags;

				// @DOCLINE * `@NLFT* directory` - a pointer to the [font directory](#font-directory).
				muttDirectory* directory;

				// @DOCLINE * `@NLFT* maxp` - a pointer to the [maxp table](#maxp-table).
				muttMaxp* maxp;
				// @DOCLINE * `@NLFT maxp_res` - the result of attempting to load the maxp table.
				muttResult maxp_res;

				// @DOCLINE * `@NLFT* head` - a pointer to the [head table](#head-table).
				muttHead* head;
				// @DOCLINE * `@NLFT head_res` - the result of attempting to load the head table.
				muttResult head_res;

				// @DOCLINE * `@NLFT* hhea` - a pointer to the [hhea table](#hhea-table).
				muttHhea* hhea;
				// @DOCLINE * `@NLFT hhea_res` - the result of attempting to load the hhea table.
				muttResult hhea_res;

				// @DOCLINE * `@NLFT* hmtx` - a pointer to the [hmtx table](#hmtx-table).
				muttHmtx* hmtx;
				// @DOCLINE * `@NLFT hmtx_res` - the result of attempting to load the hmtx table.
				muttResult hmtx_res;

				// @DOCLINE * `@NLFT* loca` - a pointer to the [loca table](#loca-table).
				muttLoca* loca;
				// @DOCLINE * `@NLFT loca_res` - the result of attempting to load the loca table.
				muttResult loca_res;

				// @DOCLINE * `@NLFT* post` - a pointer to the post table.
				muttPost* post;
				// @DOCLINE * `@NLFT post_res` - the result of attempting to load the post table.
				muttResult post_res;

				// @DOCLINE * `@NLFT* name` - a pointer to the [name table](#name-table).
				muttName* name;
				// @DOCLINE * `@NLFT name_res` - the result of attempting to load the name table.
				muttResult name_res;

				// @DOCLINE * `@NLFT* glyf` - a pointer to the [glyf table](#glyf-table).
				muttGlyf* glyf;
				// @DOCLINE * `@NLFT glyf_res` - the result of attempting to load the glyf table.
				muttResult glyf_res;

				// @DOCLINE * `@NLFT* cmap` - a pointer to the [cmap table](#cmap-table).
				muttCmap* cmap;
				// @DOCLINE * `@NLFT cmap_res` - the result of attempting to load the cmap table.
				muttResult cmap_res;
			};

			// @DOCLINE For each optionally-loadable table within the `muttFont` struct, there exists two members: one that exists as a pointer to the table, and a result value storing the result of attempting to load the table itself. If the respective result value is fatal, or the user never requested for the table to be loaded, the pointer to the table will be 0. Otherwise, the member will be a valid pointer to the table information.

		// @DOCLINE ## Font directory

			typedef struct muttTableRecord muttTableRecord;

			// @DOCLINE The struct `muttDirectory` is used to list all of the tables provided by a TrueType font. It is stored in the struct `muttFont` as `muttFont.directory` and is similar to TrueType's table directory. It has the following members:

			struct muttDirectory {
				// @DOCLINE * `@NLFT num_tables` - the amount of tables within the font; equivalent to "numTables" in the table directory.
				uint16_m num_tables;
				// @DOCLINE * `@NLFT* records` - pointer to an array of each [table record](#table-record); similar to "tableRecords" in the table directory. This array is of length `num_tables`.
				muttTableRecord* records;
			};

			// @DOCLINE ### Table record

				// @DOCLINE The struct `muttTableRecord` represents a table record in the table directory. It has the following members:

				struct muttTableRecord {
					// @DOCLINE * `@NLFT table_tag_u8[4]` - the table tag, represented by four consecutive unsigned 8-bit values representing each character of the table tag.
					uint8_m table_tag_u8[4];
					// @DOCLINE * `@NLFT table_tag_u32`- the table tag, represented by an unsigned 32-bit value representing the characters of the table tag read as big-endian.
					uint32_m table_tag_u32;
					// @DOCLINE * `@NLFT checksum` - equivalent to "checksum" in the table record.
					uint32_m checksum;
					// @DOCLINE * `@NLFT offset` - equivalent to "offset" in the table record.
					uint32_m offset;
					// @DOCLINE * `@NLFT length` - equivalent to "length" in the table record.
					uint32_m length;
				};

				// @DOCLINE The checksum value is not validated for the head table, as the head table itself includes a checksum value.

		// @DOCLINE ## Maxp table

			// @DOCLINE The struct `muttMaxp` is used to represent the maxp table provided by a TrueType font, stored in the struct `muttFont` as the pointer member "`maxp`", and loaded with the flag `MUTT_LOAD_MAXP`. It has the following members:

			struct muttMaxp {
				// @DOCLINE * `@NLFT version_high` - the high bytes of "version" in the version 1.0 maxp table.
				uint16_m version_high;
				// @DOCLINE * `@NLFT version_low` - the low bytes of "version" in the version 1.0 maxp table.
				uint16_m version_low;
				// @DOCLINE * `@NLFT num_glyphs` - equivalent to "numGlyphs" in the version 1.0 maxp table.
				uint16_m num_glyphs;
				// @DOCLINE * `@NLFT max_points` - equivalent to "maxPoints" in the version 1.0 maxp table.
				uint16_m max_points;
				// @DOCLINE * `@NLFT max_contours` - equivalent to "maxContours" in the version 1.0 maxp table.
				uint16_m max_contours;
				// @DOCLINE * `@NLFT max_composite_points` - equivalent to "maxCompositePoints" in the version 1.0 maxp table.
				uint16_m max_composite_points;
				// @DOCLINE * `@NLFT max_composite_contours` - equivalent to "maxCompositeContours" in the version 1.0 maxp table.
				uint16_m max_composite_contours;
				// @DOCLINE * `@NLFT max_zones` - equivalent to "maxZones" in the version 1.0 maxp table.
				uint16_m max_zones;
				// @DOCLINE * `@NLFT max_twilight_points` - equivalent to "maxTwilightPoints" in the version 1.0 maxp table.
				uint16_m max_twilight_points;
				// @DOCLINE * `@NLFT max_storage` - equivalent to "maxStorage" in the version 1.0 maxp table.
				uint16_m max_storage;
				// @DOCLINE * `@NLFT max_function_defs` - equivalent to "maxFunctionDefs" in the version 1.0 maxp table.
				uint16_m max_function_defs;
				// @DOCLINE * `@NLFT max_instruction_defs` - equivalent to "maxInstructionDefs" in the version 1.0 maxp table.
				uint16_m max_instruction_defs;
				// @DOCLINE * `@NLFT max_stack_elements` - equivalent to "maxStackElements" in the version 1.0 maxp table.
				uint16_m max_stack_elements;
				// @DOCLINE * `@NLFT max_size_of_instructions` - equivalent to "maxSizeOfInstructions" in the version 1.0 maxp table.
				uint16_m max_size_of_instructions;
				// @DOCLINE * `@NLFT max_component_elements` - equivalent to "maxComponentElements" in the version 1.0 maxp table.
				uint16_m max_component_elements;
				// @DOCLINE * `@NLFT max_component_depth` - equivalent to "maxComponentDepth" in the version 1.0 maxp table.
				uint16_m max_component_depth;
			};

			// @DOCLINE Since most values given in this table are just maximums, there are only checks performed for the version, numGlyph, and maxZones values. All other values dictate maximums that other tables must follow, and checks will be performed on said tables to ensure they stay within the maximums dictated by maxp.

		// @DOCLINE ## Head table

			// @DOCLINE The struct `muttHead` is used to represent the head table provided by a TrueType font, stored in the struct `muttFont` as the pointer member "`head`", and loaded with the flag `MUTT_LOAD_HEAD`. It has the following members:

			struct muttHead {
				// @DOCLINE * `@NLFT font_revision_high` - equivalent to the high bytes of "fontRevision" in the head table.
				int16_m font_revision_high;
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
			};

			// @DOCLINE Currently, the values for "checksumAdjustment" and "fontDirectionHint" are not checked.

			// @DOCLINE ### indexToLocFormat macros

				// @DOCLINE The macros `MUTT_OFFSET_16` (0) and `MUTT_OFFSET_32` (1) are defined to make reading the value of "indexToLocFormat" easier.
				#define MUTT_OFFSET_16 0
				#define MUTT_OFFSET_32 1

		// @DOCLINE ## Hhea table

			// @DOCLINE The struct `muttHhea` is used to represent the hhea table provided by a TrueType font, stored in the struct `muttFont` as the pointer member "`hhea`", and loaded with the flag `MUTT_LOAD_HHEA` (`MUTT_LOAD_MAXP` must also be defined). It has the following members:

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
				// @DOCLINE * `@NLFT number_of_hmetrics` - equivalent to "numberOfHMetrics" in the hhea table.
				uint16_m number_of_hmetrics;
			};

			// @DOCLINE All values provided in the `muttHhea` struct are not checked (besides numberOfHMetrics, since it must be less than or equal to `maxp->num_glyphs` in order to generate a valid array length for "leftSideBearings" within hmtx), as virtually all of them have no technically "incorrect" values (from what I'm aware).

		// @DOCLINE ## Hmtx table

			typedef struct muttLongHorMetric muttLongHorMetric;

			// @DOCLINE The struct `muttHmtx` is used to represent the hmtx table provided by a TrueType font, stored in the struct `muttFont` as the pointer member "`hmtx`", and loaded with the flag `MUTT_LOAD_HMTX` (`MUTT_LOAD_MAXP` and `MUTT_LOAD_HHEA` must also be defined). It has the following members:

			struct muttHmtx {
				// @DOCLINE * `@NLFT* hmetrics` - an array of horizontal metric records; equiavlent to "hMetrics" in the hmtx table. Its length is equivalent to `hhea->number_of_hmetrics`.
				muttLongHorMetric* hmetrics;
				// @DOCLINE * `@NLFT* left_side_bearings` - equivalent to "leftSideBearings" in the hmtx table. Its length is equivalent to `maxp->num_glyphs - hhea->number_of_hmetrics`.
				int16_m* left_side_bearings;
			};

			// @DOCLINE The struct `muttLongHorMetrics` has the following members:

			struct muttLongHorMetric {
				// @DOCLINE * `@NLFT advance_width` - equivalent to "advanceWidth" in the LongHorMetric record.
				uint16_m advance_width;
				// @DOCLINE * `@NLFT lsb` - equivalent to "lsb" in the LongHorMetric record.
				int16_m lsb;
			};

			// @DOCLINE All values provided in the `muttHmtx` struct (AKA the values in `muttLongHorMetrics`) are not checked, as virtually all of them have no technically "incorrect" values (from what I'm aware).

		// @DOCLINE ## Loca table

			// @DOCLINE The union `muttLoca` is used to represent the loca table provided by a TrueType font, stored in the struct `muttFont` as the pointer member "`loca`", and loaded with the flag `MUTT_LOAD_LOCA` (`MUTT_LOAD_MAXP`, `MUTT_LOAD_HEAD`, and `MUTT_LOAD_GLYF` must also be defined). It has the following members:

			union muttLoca {
				// @DOCLINE * `@NLFT* offsets16` - equivalent to the short-format offsets array in the loca table. This member is to be read from if `head->index_to_loc_format` is equal to `MUTT_OFFSET_16`.
				uint16_m* offsets16;
				// @DOCLINE * `@NLFT* offsets32` - equivalent to the long-format offsets array in the loca table. This member is to be read from if `head->index_to_loc_format` is equal to `MUTT_OFFSET_32`.
				uint32_m* offsets32;
			};

			// @DOCLINE The offsets are verified to be within range of the glyf table, along with all of the other rules within the specification.

		// @DOCLINE ## Name table

			typedef struct muttNameRecord muttNameRecord;
			typedef struct muttLangTagRecord muttLangTagRecord;

			// @DOCLINE The struct `muttName` is used to represent the name table provided by a TrueType font, stored in the struct `muttFont` as the pointer member "`name`", and loaded with the flag `MUTT_LOAD_NAME`. It has the following members:

			struct muttName {
				// @DOCLINE * `@NLFT version` - equivalent to "version" in the naming table header.
				uint16_m version;
				// @DOCLINE * `@NLFT count` - the amount of name records specified; equivalent to "count" in the naming table header.
				uint16_m count;
				// @DOCLINE * `@NLFT* name_records` - all [name records](#name-record) provided (length `count`); equivalent to "nameRecord" in the naming table header.
				muttNameRecord* name_records;
				// @DOCLINE * `@NLFT lang_tag_count` - the amount of language tags specified; equivalent to "langTagCount" in the naming table header.
				uint16_m lang_tag_count;
				// @DOCLINE * `@NLFT* lang_tag_records` - all [language tag records](#lang-tag-record) provided (length `lang_tag_count`); equivalent to "langTagRecord" in the naming table header.
				muttLangTagRecord* lang_tag_records;
				// @DOCLINE * `@NLFT* string_data` - the raw string data provided by the name table. All pointers to strings provided by the name table are pointers to parts of this data.
				muByte* string_data;
			};

			// @DOCLINE ### Name record

				// @DOCLINE The struct `muttNameRecord` represents a name record in TrueType. It has the following members:

				struct muttNameRecord {
					// @DOCLINE * `@NLFT platform_id` - the [platform ID](#platform-id); equivalent to "platformID" in the name record.
					uint16_m platform_id;
					// @DOCLINE * `@NLFT encoding_id` - the [encoding ID](#encoding-id); equivalent to "encodingID" in the name record.
					uint16_m encoding_id;
					// @DOCLINE * `@NLFT language_id` - the [language ID](#language-id); equivalent to "languageID" in the name record.
					uint16_m language_id;
					// @DOCLINE * `@NLFT name_id` - the [name ID](#name-id); equivalent to "nameID" in the name record.
					uint16_m name_id;
					// @DOCLINE * `@NLFT length` - the length of the string, in bytes; equivalent to "length" in the name record.
					uint16_m length;
					// @DOCLINE * `@NLFT* string` - a pointer to the string data stored within `muttName->string_data` for this given name record.
					muByte* string;
				};

				// @DOCLINE No platform, encoding, language, or name IDs give bad result values unless the specification explicitly states that the range of values that it's within will never be supported. The provided pointer for `string` is checked to be a pointer to valid data for the given length.

			// @DOCLINE ### Lang tag record

				// @DOCLINE The struct `muttLangTagRecord` represents a language tag in TrueType. It has the following members:

				struct muttLangTagRecord {
					// @DOCLINE * `@NLFT length` - the length of the string, in bytes; equivalent to "length" in the lang tag record.
					uint16_m length;
					// @DOCLINE * `@NLFT* lang_tag` - a pointer to the string data stored within `muttName->string_data` for this given name record.
					muByte* lang_tag;
				};

				// @DOCLINE The provided pointer for `lang_tag` is checked to be a pointer to valid data for the given length.

		// @DOCLINE ## Glyf table

				typedef struct muttGlyphHeader muttGlyphHeader;
				typedef struct muttSimpleGlyph muttSimpleGlyph;
				typedef struct muttCompositeGlyph muttCompositeGlyph;

				// @DOCLINE The struct `muttGlyf` is used to represent the glyf table provided by a TrueType font, stored in the struct `muttFont` as the pointer member "`glyf`", and loaded with the flag `MUTT_LOAD_GLYF` (`MUTT_LOAD_MAXP`, `MUTT_LOAD_HEAD`, and `MUTT_LOAD_LOCA` must also be defined). It has the following members:

				struct muttGlyf {
					// @DOCLINE * `@NLFT len` - the length of the glyf table, in bytes.
					uint32_m len;
					// @DOCLINE * `@NLFT* data` - the raw byte data of the glyf table (length `len`).
					muByte* data;
				};

				// @DOCLINE Unlike most low-level table structs, `muttGlyf` provides virtually no information about any glyphs upfront. This is because expanding every single glyph's information can be taxing, so instead, an API is provided to load individual glyph information using the `muttGlyf` struct, which is described below.

				// @DOCLINE ### Glyph header

					// @DOCLINE Every glyph, simple or composite, is described initially by its header, which is represented in mutt with the struct `muttGlyphHeader`, which has the following members:

					struct muttGlyphHeader {
						// @DOCLINE * `@NLFT number_of_contours` - equivalent to "numberOfContours" in the glyph header; if this value is negative, the glyph is composite, and if positive or zero, it is simple.
						int16_m number_of_contours;
						// @DOCLINE * `@NLFT x_min` - equivalent to "xMin" in the glyph header; minimum for x-coordinate data.
						int16_m x_min;
						// @DOCLINE * `@NLFT y_min` - equivalent to "yMin" in the glyph header; minimum for y-coordinate data.
						int16_m y_min;
						// @DOCLINE * `@NLFT x_max` - equivalent to "xMax" in the glyph header; maximum for x-coordinate data.
						int16_m x_max;
						// @DOCLINE * `@NLFT y_max` - equivalent to "yMax" in the glyph header; maximum for y-coordinate data.
						int16_m y_max;
						// @DOCLINE * `@NLFT* data` - a pointer to byte data in `glyf->data` after the header. This is primarily used internally by mutt.
						muByte* data;
						// @DOCLINE * `@NLFT length` - the length of the data after the header in bytes. If this member is equal to 0, the given glyph has no outline, and should not be called with any functions.
						uint32_m length;
					};

					// @DOCLINE The minimums and maximums for x- and y-coordinates within the glyph header are not checked initially (besides making sure the minimums are less than or equal to the maximums, and that they're within range of the values provided by the head table); if the actual glyph coordinates are not confined within the given minimums and maximums, a bad result will be provided upon loading the simple glyph data.

					// @DOCLINE #### Get glyph header

						// @DOCLINE In order to load a glyph header for a given glyph ID, the function `mutt_glyph_header` is used, defined below: @NLNT
						MUDEF muttResult mutt_glyph_header(muttFont* font, uint16_m glyph_id, muttGlyphHeader* header);

						// @DOCLINE Upon a non-fatal result, `header` is filled with valid header information for the given glyph ID. Upon a fatal result, the contents of `header` are undefined. The given header information is only valid for as long as `font` is not deloaded.

						// @DOCLINE `glyph_id` must be a valid glyph ID for the given font (AKA less than `font->head->num_glyphs`).

				// @DOCLINE ### Simple glyph

					typedef struct muttGlyphPoint muttGlyphPoint;

					// @DOCLINE The struct `muttSimpleGlyph` represents a simple glyph in mutt, and has the following members:

					struct muttSimpleGlyph {
						// @DOCLINE * `@NLFT* end_pts_of_contours` - equivalent to "endPtsOfContours" in the simple glyph table.
						uint16_m* end_pts_of_contours;
						// @DOCLINE * `@NLFT instruction_length` - equivalent to "instructionLength" in the simple glyph table; the length of `instructions`, in bytes.
						uint16_m instruction_length;
						// @DOCLINE * `@NLFT* instructions` - equivalent to "instructions" in the simple glyph table; the instructions for the given glyph.
						muByte* instructions;
						// @DOCLINE * `@NLFT* points` - each point for the simple glyph. The number of points is equal to `end_pts_of_contours[muttGlyphHeader->number_of_contours-1]+1` if `muttGlyphHeader->number_of_contours` is over 0; if `muttGlyphHeader->number_of_contours` is equal to 0, `points` will be equal to 0 as well.
						muttGlyphPoint* points;
					};

					// @DOCLINE The struct `muttGlyphPoint` represents a point in a simple glyph, and has the following members:
					struct muttGlyphPoint {
						// @DOCLINE * `@NLFT flags` - equivalent to a value within the "flags" array in the simple glyph table; the [flags of the given point](#glyph-point-flags).
						uint8_m flags;
						// @DOCLINE * `@NLFT x` - the x-coordinate of the point, in FUnits.
						int16_m x;
						// @DOCLINE * `@NLFT y` - the y-coordinate of the point, in FUnits.
						int16_m y;
					};

					// @DOCLINE #### Glyph point flags

						// @DOCLINE The following macros are defined for bitmasking a glyph point's flags:

						// @DOCLINE * [0x01] `MUTT_ON_CURVE_POINT`
						#define MUTT_ON_CURVE_POINT 0x01
						// @DOCLINE * [0x02] `MUTT_X_SHORT_VECTOR`
						#define MUTT_X_SHORT_VECTOR 0x02
						// @DOCLINE * [0x04] `MUTT_Y_SHORT_VECTOR`
						#define MUTT_Y_SHORT_VECTOR 0x04
						// @DOCLINE * [0x08] `MUTT_REPEAT_FLAG`
						#define MUTT_REPEAT_FLAG 0x08
						// @DOCLINE * [0x10] `MUTT_X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR`
						#define MUTT_X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR 0x10
						// @DOCLINE * [0x20] `MUTT_Y_IS_SAME_OR_POSITIVE_Y_SHORT_VECTOR`
						#define MUTT_Y_IS_SAME_OR_POSITIVE_Y_SHORT_VECTOR 0x20
						// @DOCLINE * [0x40] `MUTT_OVERLAP_SIMPLE`
						#define MUTT_OVERLAP_SIMPLE 0x40

						// @DOCLINE Note that since the value of `flags` is directly copied from the raw TrueType data, usage of these macros is optional, and the user can bitmask as they please in accordance to the TrueType specification.

					// @DOCLINE #### Load simple glyph

						// @DOCLINE In order to load a simple glyph, the function `mutt_simple_glyph` is used, defined below: @NLNT
						MUDEF muttResult mutt_simple_glyph(muttFont* font, muttGlyphHeader* header, muttSimpleGlyph* glyph, muByte* data, uint32_m* written);

						// @DOCLINE Upon a non-fatal result, `glyph` is filled with valid simple glyph information for the given glyph ID using memory from `data`. Upon a fatal result, the contents of `glyph` and `data` are undefined. The given glyph information is only valid for as long as `font` is not deloaded, and as long as `data` goes unmodified.

						// @DOCLINE This function follows the format of a user-allocated function. For an explanation of how `data` and `written` are supposed to be used within this function, see [the user-allocated function section](#user-allocated-functions).

						// @DOCLINE > This function checks if all of the values are compliant with information from other tables (especially maxp) and compliant with TrueType's specification with a few exceptions: as far as I'm aware, it's invalid to have a flag that uses values from a prior point (such as X_IS_SAME...) when the current flag is the first flag specified, since in that case, there's no "previous value" to repeat from. This is done in several common fonts, however, so mutt permits this, setting the value to 0 in this case.

						// @DOCLINE > It's also invalid (from what I'm aware) to have the first point be off-curve, but in the case that such happens, mutt permits this, pretending that the previous point was an on-curve point at (0,0). It's also invalid (from what I'm aware) to have a repeat flag count that exceeds the amount of points, but since it's easy to internally make sure to simply not go over the point count, mutt permits this.

					// @DOCLINE #### Simple glyph memory maximum

						// @DOCLINE The maximum amount of memory that will be needed for loading a simple glyph, in bytes, is provided by the function `mutt_simple_glyph_max_size`, defined below: @NLNT
						MUDEF uint32_m mutt_simple_glyph_max_size(muttFont* font);

				// @DOCLINE ### Composite glyph

					typedef struct muttComponentGlyph muttComponentGlyph;

					// @DOCLINE The struct `muttCompositeGlyph` represents a composite glyph in mutt, and has the following members:

					struct muttCompositeGlyph {
						// @DOCLINE * `@NLFT component_count` - the number of components within the composite glyph.
						uint16_m component_count;
						// @DOCLINE * `@NLFT* components` - an array of each component within the composite glyph.
						muttComponentGlyph* components;
						// @DOCLINE * `@NLFT instruction_length` - the length of the instructions for the composite glyph, in bytes.
						uint16_m instruction_length;
						// @DOCLINE * `@NLFT* instructions` - the instructions for the composite glyph.
						muByte* instructions;
					};

					// @DOCLINE The struct `muttComponentGlyph` represents a component in a composite glyph, and has the following members:
					struct muttComponentGlyph {
						// @DOCLINE * `@NLFT flags` - equivalent to "flags" in the component glyph record; the [flags for the given component glyph](#glyph-component-flags).
						uint16_m flags;
						// @DOCLINE * `@NLFT glyph_index` - equivalent to "glyphIndex" in the component glyph record; the glyph ID of the given component.
						uint16_m glyph_index;
						// @DOCLINE * `@NLFT argument1` - equivalent to "argument1" in the component glyph record.
						int32_m argument1;
						// @DOCLINE * `@NLFT argument2` - equivalent to "argument2" in the component glyph record.
						int32_m argument2;
						// @DOCLINE * `@NLFT scales[4]` - the transform data of the component.
						float scales[4];
					};

					// @DOCLINE The data of `scales` depends on the value of `flags` (see TrueType/OpenType documentation for more information on how this data works); the following conditions exist:

					// @DOCLINE * If the `MUTT_WE_HAVE_A_SCALE` bit is 1, `scales[0]` is the scale; the contents of all other float indexes are undefined.
					// @DOCLINE * If the `MUTT_WE_HAVE_AN_X_AND_Y_SCALE` bit is 1, `scales[0]` and `scales[1]` are the x- and y-scales respectively; the contents of all other float indexes are undefined.
					// @DOCLINE * If the `MUTT_WE_HAVE_A_TWO_BY_TWO` bit is 1, `scales[0]`, `scales[1]`, `scales[2]`, and `scales[3]` are the 2-by-2 affine transformation values (xscale, scale01, scale10, yscale respectively).
					// @DOCLINE * If none of the bits mentioned above are 1, the values of `scales` are undefined.

					// @DOCLINE The value for `glyph_index` is not verified to be a non-infinite loop of composite glyphs, and must be manually checked for by the user, unless being converted to a pixel glyph, in which case the conversion checks for this case.

					// @DOCLINE #### Glyph component flags

						// @DOCLINE The following macros are defined for bitmasking a glyph component's flags:

						// @DOCLINE * [0x0001] `MUTT_ARG_1_AND_2_ARE_WORDS`
						#define MUTT_ARG_1_AND_2_ARE_WORDS 0x0001
						// @DOCLINE * [0x0002] `MUTT_ARGS_ARE_XY_VALUES`
						#define MUTT_ARGS_ARE_XY_VALUES 0x0002
						// @DOCLINE * [0x0004] `MUTT_ROUND_XY_TO_GRID`
						#define MUTT_ROUND_XY_TO_GRID 0x0004
						// @DOCLINE * [0x0008] `MUTT_WE_HAVE_A_SCALE`
						#define MUTT_WE_HAVE_A_SCALE 0x0008
						// @DOCLINE * [0x0020] `MUTT_MORE_COMPONENTS`
						#define MUTT_MORE_COMPONENTS 0x0020
						// @DOCLINE * [0x0040] `MUTT_WE_HAVE_AN_X_AND_Y_SCALE`
						#define MUTT_WE_HAVE_AN_X_AND_Y_SCALE 0x0040
						// @DOCLINE * [0x0080] `MUTT_WE_HAVE_A_TWO_BY_TWO`
						#define MUTT_WE_HAVE_A_TWO_BY_TWO 0x0080
						// @DOCLINE * [0x0100] `MUTT_WE_HAVE_INSTRUCTIONS`
						#define MUTT_WE_HAVE_INSTRUCTIONS 0x0100
						// @DOCLINE * [0x0200] `MUTT_USE_MY_METRICS`
						#define MUTT_USE_MY_METRICS 0x0200
						// @DOCLINE * [0x0400] `MUTT_OVERLAP_COMPOUND`
						#define MUTT_OVERLAP_COMPOUND 0x0400
						// @DOCLINE * [0x0800] `MUTT_SCALED_COMPONENT_OFFSET`
						#define MUTT_SCALED_COMPONENT_OFFSET 0x0800
						// @DOCLINE * [0x1000] `MUTT_UNSCALED_COMPONENT_OFFSET`
						#define MUTT_UNSCALED_COMPONENT_OFFSET 0x1000

						// @DOCLINE Note that since the value of `flags` is retrieved from the TrueType data, usage of these macros is optional, and the user can bitmask as they please in accordance to the TrueType specification.

					// @DOCLINE #### Load composite glyph

						// @DOCLINE In order to load a composite glyph, the function `mutt_composite_glyph` is used, defined below: @NLNT
						MUDEF muttResult mutt_composite_glyph(muttFont* font, muttGlyphHeader* header, muttCompositeGlyph* glyph, muByte* data, uint32_m* written);

						// @DOCLINE Upon a non-fatal result, `glyph` is filled with valid composite glyph information for the given glyph ID using memory from `data`. Upon a fatal result, the contents of `glyph` and `data` are undefined. The given glyph information is only valid for as long as `font` is not deloaded, and as long as `data` goes unmodified.

						// @DOCLINE This function follows the format of a user-allocated function. For an explanation of how `data` and `written` are supposed to be used within this function, see [the user-allocated function section](#user-allocated-functions).

						// @DOCLINE This function performs no checks on the validity of the components' range within the minimum/maximum coordinate ranges specified for the glyph in the respective header. Therefore, this function does allow composite glyphs to successfully load that have points that are out of range. This is due to the fact that properly verifying the points' coordinates would entail fully decompressing the composite glyph's components, which is not performed in the lower-level API of mutt.

					// @DOCLINE #### Composite glyph memory maximum

						// @DOCLINE The maximum amount of memory that will be needed for loading a composite glyph, in bytes, is provided by the function `mutt_composite_glyph_max_size`, defined below: @NLNT
						MUDEF uint32_m mutt_composite_glyph_max_size(muttFont* font);

				// @DOCLINE ### Max glyph size

					// @DOCLINE The maximum amount of memory that will be needed for loading a glyph, simple or composite, in bytes, is provided by the function `mutt_glyph_max_size`, defined below: @NLNT
					MUDEF uint32_m mutt_glyph_max_size(muttFont* font);

					// @DOCLINE This function returns the largest value between `mutt_simple_glyph_max_size` and `mutt_composite_glyph_max_size`.

		// @DOCLINE ## Cmap table

			typedef struct muttEncodingRecord muttEncodingRecord;
			typedef union muttCmapFormat muttCmapFormat;
			typedef struct muttCmap0 muttCmap0;
			typedef struct muttCmap4 muttCmap4;
			typedef struct muttCmap12 muttCmap12;

			// @DOCLINE The struct `muttCmap` is used to represent the cmap table provided by a TrueType font, stored in the struct `muttFont` as the pointer member "`cmap`", and loaded with the flag `MUTT_LOAD_CMAP` (`MUTT_LOAD_MAXP` must also be defined). It has the following members:

			struct muttCmap {
				// @DOCLINE * `@NLFT num_tables` - equivalent to "numTables" in the cmap header; the number of encoding records in the `encoding_records` array.
				uint16_m num_tables;
				// @DOCLINE * `@NLFT* encoding_records` - equivalent to "encodingRecords" in the cmap header; an array of each encoding record in the cmap table.
				muttEncodingRecord* encoding_records;
			};

			// @DOCLINE The union `muttCmapFormat` represents a cmap format for a cmap encoding record. It has the following members:
			union muttCmapFormat {
				// @DOCLINE * `@NLFT* f0` - [format 0](#format-0).
				muttCmap0* f0;
				// @DOCLINE * `@NLFT* f4` - [format 4](#format-4).
				muttCmap4* f4;
				// @DOCLINE * `@NLFT* f12` - [format 12](#format-12).
				muttCmap12* f12;
			};

			// @DOCLINE The struct `muttEncodingRecord` represents an encoding record in the cmap table. It has the following members:
			struct muttEncodingRecord {
				// @DOCLINE * `@NLFT platform_id` - equivalent to "platformID" in the cmap encoding record; the platform ID of the encoding record.
				uint16_m platform_id;
				// @DOCLINE * `@NLFT encoding_id` - equivalent to "encodingID" in the cmap encoding record; the encoding ID of the encoding record.
				uint16_m encoding_id;
				// @DOCLINE * `@NLFT format` - equivalent to "format" in a given cmap subtable format; the format of the encoding record.
				uint16_m format;
				// @DOCLINE * `@NLFT encoding` - a union holding a pointer to the information for the cmap subtable format.
				muttCmapFormat encoding;
				// @DOCLINE * `@NLFT result` - the result of attempting to load the cmap subtable format.
				muttResult result;
			};

			// @DOCLINE If `result` is a fatal result (most commonly due to the format not being supported (`MUTT_INVALID_CMAP_ENCODING_RECORD_FORMAT`)), the relevant member of `encoding` is 0, unless the format is unsupported, in which case the value of `encoding` is undefined.

			// @DOCLINE The following sections detail how to convert codepoint values to glyph ID values and vice versa, using (at the highest level) the cmap table as a whole, and (at the lowest level) reading data from each format's struct. ***WARNING:*** The glyph ID values returned by a cmap format subtable are *not* checked to be valid upon loading; they are checked to be valid once a function is called for conversion. This means that if the user is using the structs of each format to get glyph ID values instead of using the respective functions, they need to check if the glyph ID values that they retrieve are valid or not, since they are unchecked. Normal checks for all values being within data range are still performed; for example, the loading of format 4 checks if all values within its segments generate a valid index into its glyphIdArray, it just doesn't check if the values within glyphIdArray are valid glyph IDs, that it performed in `mutt_cmap4_get_glyph`.

			// @DOCLINE Codepoint values passed into functions for conversion can be invalid; 0 shall just be returned. Codepoint values returned by a conversion function may not be valid codepoint values for the relevant encoding; the user must check these values themselves. Invalid glyph ID values should not be passed into conversion functions, and conversion functions should not return invalid glyph ID values.

			// @DOCLINE ### Top-level cmap

				// @DOCLINE Every implemented cmap format in mutt can retrieve a glyph ID based on a given codepoint and vice versa.

				// @DOCLINE The function `mutt_get_glyph` searches each cmap encoding record specified for the given font and attempts to convert the given codepoint value to a valid glyph ID, defined below: @NLNT
				MUDEF uint16_m mutt_get_glyph(muttFont* font, uint32_m codepoint);

				// @DOCLINE The function `mutt_get_codepoint` searches each cmap encoding record specified for the given font and attempts to convert the given glyph ID to a codepoint value, defined below: @NLNT
				MUDEF uint32_m mutt_get_codepoint(muttFont* font, uint16_m glyph_id);

				// @DOCLINE Both functions return 0 if no equivalent could be found in the conversion process for any cmap encoding record.

			// @DOCLINE ### Cmap encoding

				// @DOCLINE The function `mutt_cmap_encoding_get_glyph` converts a given codepoint value to a glyph ID value using the given cmap encoding record, defined below: @NLNT
				MUDEF uint16_m mutt_cmap_encoding_get_glyph(muttFont* font, muttEncodingRecord* record, uint32_m codepoint);

				// @DOCLINE The function `mutt_cmap_encoding_get_codepoint` converts a given glyph ID to a codepoint value using the given cmap encoding record, defined below: @NLNT
				MUDEF uint32_m mutt_cmap_encoding_get_codepoint(muttFont* font, muttEncodingRecord* record, uint16_m glyph_id);

				// @DOCLINE Both functions return 0 if no equivalent could be found in the conversion process for the format of the given encoding record.

			// @DOCLINE ### Format 0

				// @DOCLINE The struct `muttCmap0` represents a cmap format 0 subtable, and has the following members:
				struct muttCmap0 {
					// @DOCLINE * `@NLFT language` - equivalent to "language" in the cmap format 0 subtable.
					uint16_m language;
					// @DOCLINE * `@NLFT glyph_ids[256]` - equivalent to "glyphIdArray" in the cmap format 0 subtable.
					uint8_m glyph_ids[256];
				};

				// @DOCLINE The function `mutt_cmap0_get_glyph` converts a given codepoint value to a glyph ID value using the given format 0 cmap subtable, defined below: @NLNT
				MUDEF uint16_m mutt_cmap0_get_glyph(muttFont* font, muttCmap0* f0, uint8_m codepoint);

				// @DOCLINE The function `mutt_cmap0_get_codepoint` converts a given glyph ID to a codepoint value using the given format 0 cmap subtable, defined below: @NLNT
				MUDEF uint8_m mutt_cmap0_get_codepoint(muttFont* font, muttCmap0* f0, uint16_m glyph);

				// @DOCLINE Both functions return 0 if no equivalent could be found in the conversion process.

			// @DOCLINE ### Format 4

				typedef struct muttCmap4Segment muttCmap4Segment;

				// @DOCLINE The struct `muttCmap4` represents a cmap format 4 subtable, and has the following members:
				struct muttCmap4 {
					// @DOCLINE * `@NLFT language` - equivalent to "language" in the cmap format 4 subtable.
					uint16_m language;
					// @DOCLINE * `@NLFT seg_count` - equivalent to half of "segCountX2" in the cmap format 4 subtable; the amount of segments in the `seg` array.
					uint16_m seg_count;
					// @DOCLINE * `@NLFT* seg` - an array of each segment within the cmap format 4 subtable.
					muttCmap4Segment* seg;
					// @DOCLINE * `@NLFT* glyph_ids` - equivalent to "glyphIdArray" in the cmap format 4 subtable; the glyph index array that each segment should return indexes into.
					uint16_m* glyph_ids;
				};

				// @DOCLINE Internally, mutt does not verify or use the values for "searchRange", "entrySelector", or "rangeShift".

				// @DOCLINE The struct `muttCmap4Segment` represents a segment in the cmap format 4 subtable, and has the following members:
				struct muttCmap4Segment {
					// @DOCLINE * `@NLFT end_code` - equivalent to the value for the given segment in the "endCode" array in the cmap format 4 subtable; the end character code for the given segment.
					uint16_m end_code;
					// @DOCLINE * `@NLFT start_code` - equivalent to the value for the given segment in the "startCode" array in the cmap format 4 subtable; the start character code for the given segment.
					uint16_m start_code;
					// @DOCLINE * `@NLFT id_delta` - equivalent to the value for the given segment in the "idDelta" array in the cmap format 4 subtable; the delta for the character codes of the given segment.
					int16_m id_delta;
					// @DOCLINE * `@NLFT id_range_offset` - equivalent to the value for the given segment in the "idRangeOffset" array in the cmap format 4 subtable, but divided by 2 and with (`muttCmap4->seg_count` - the index for the given segment) subtracted; the start code index offset into `muttCmap4->glyph_ids`.
					uint16_m id_range_offset;
					// @DOCLINE * `@NLFT start_glyph_id` - the calculated first glyph ID of the segment. This is not checked to be a valid glyph ID, and is used when converting glyph IDs into codepoints.
					uint16_m start_glyph_id;
					// @DOCLINE * `@NLFT end_glyph_id` - the calculated last glyph ID of the segment. This is not checked to be a valid glyph ID, and is used when converting glyph IDs into codepoints.
					uint16_m end_glyph_id;
				};

				// @DOCLINE > In TrueType, idRangeOffset is stored as an offset into the glyphIdArray for the start code, to which every code in the segment adds an offset to get the next value (`codepoint-start_code` incrementally offsets, beginning at the start code as 0). However, this offset is stored relative to `&idRangeOffset[segment]`, so this offset is accounted for when loading the format in mutt by subtracting `seg_count-segment` for each value in idRangeOffset. The value is also first divided by 2 (before the subtraction) since, internally, the value is accessed by directly indexing into `glyph_ids`, but idRangeOffset values store actual byte offsets, so the 2 bytes per glyph ID must be accounted for. Glyph IDs are then internally retrieved (after being verified to be valid offsets) via `f4->glyph_ids[seg->id_range_offset + (codepoint - seg->start_code)]` (followed by delta logic), and this is how the user should also retrieve them when manually reading from the `muttCmap4` struct.

				// @DOCLINE The function `mutt_cmap4_get_glyph` converts a given codepoint value to a glyph ID value using the given format 4 cmap subtable, defined below: @NLNT
				MUDEF uint16_m mutt_cmap4_get_glyph(muttFont* font, muttCmap4* f4, uint16_m codepoint);

				// @DOCLINE The function `mutt_cmap4_get_codepoint` converts a given glyph ID to a codepoint value using the given format 4 cmap subtable, defined below: @NLNT
				MUDEF uint16_m mutt_cmap4_get_codepoint(muttFont* font, muttCmap4* f4, uint16_m glyph);

				// @DOCLINE Both functions return 0 if no equivalent could be found in the conversion process.

			// @DOCLINE ### Format 12

				typedef struct muttCmap12Group muttCmap12Group;

				// @DOCLINE The struct `muttCmap12` represents a cmap format 12 subtable, and has the following members:
				struct muttCmap12 {
					// @DOCLINE * `@NLFT language` - equivalent to "language" in the cmap format 12 subtable.
					uint32_m language;
					// @DOCLINE * `@NLFT num_groups` - equivalent to "numGroups" in the cmap format 12 subtable; the amount of groups in the `groups` array.
					uint32_m num_groups;
					// @DOCLINE * `@NLFT* groups` - equivalent to "groups" in the cmap format 12 subtable; an array of each map group.
					muttCmap12Group* groups;
				};

				// @DOCLINE The struct `muttCmap12Group` represents a sequential map group in the cmap format 12 subtable, and has the following members:
				struct muttCmap12Group {
					// @DOCLINE * `@NLFT start_char_code` - equivalent to "startCharCode" in the sequential map group record; the first character code for the given group.
					uint32_m start_char_code;
					// @DOCLINE * `@NLFT end_char_code` - equivalent to "endCharCode" in the sequential map group record; the last character code for the given group.
					uint32_m end_char_code;
					// @DOCLINE * `@NLFT start_glyph_id` - equivalent to "startGlyphID" in the sequential map group record; the glyph ID for the first character code.
					uint32_m start_glyph_id;
				};

				// @DOCLINE The function `mutt_cmap12_get_glyph` converts a given codepoint value to a glyph ID value using the given format 12 cmap subtable, defined below: @NLNT
				MUDEF uint16_m mutt_cmap12_get_glyph(muttFont* font, muttCmap12* f12, uint32_m codepoint);

				// @DOCLINE The function `mutt_cmap12_get_codepoint` converts a given glyph ID to a codepoint value using the given format 12 cmap subtable, defined below: @NLNT
				MUDEF uint32_m mutt_cmap12_get_codepoint(muttFont* font, muttCmap12* f12, uint16_m glyph);

				// @DOCLINE Both functions return 0 if no equivalent could be found in the conversion process.

		// @DOCLINE ## User allocated functions

			/* @DOCBEGIN
			"User-allocated functions" are functions used in mutt to allow the user to handle allocation of memory necessary to perform certain low-level operations. These functions usually have two distinct members that make this possible, `muByte* data` and `uint32_m* written`, with the functions usually following the format of:

			```c
			MUDEF muttResult mutt_whatever(muttFont* font, ... muByte* data, uint32_m* written);
			```

			The way these functions work depends on the value of `data`:

			* If `data` is 0, `written` is dereferenced and set to how many bytes are needed within `data` in order to perform the operation.
			
			* If `data` is not 0, `data` is used as allocated memory to perform whatever task is needed, and assumes that `data` is large enough to perform the operation. If `written` is not 0 in this instance, `written` is dereferenced and set to how much data was used in the operation.

			These functions are setup so that the user has full control over the allocation of the data for the given process.

			If these functions are ever used to fill information about something (in particular, if it's filling up a struct like `mutt_simple_glyph` or `mutt_composite_glyph`), the filled-out structs use `data` as memory for the information provided by them. This means that the information with the struct is only valid for as long as `data` goes unmodified by the user.

			User-allocated functions do not expect the given memory to be zero'd out, and the contents of `data`, once filled in, are undefined.

			User-allocated functions usually also have provided maximums for the maximum amount of memory that will be needed to perform the operation for any given variables, which can be used to pre-allocate and use the same memory for multiple passes of the operation. For example, `mutt_simple_glyph` has a memory maximum provided by the function `mutt_simple_glyph_max_size`, which means that a user can allocate memory of byte-length `mutt_simple_glyph_max_size(...)` and use that same memory for a call to `mutt_simple_glyph` for any glyphs that they need to load, avoiding the need to reallocate any memory when processing simple glyphs, although at the cost of only being able to process one simple glyph at a time.
			@DOCEND */

		// @DOCLINE ## String macros

			// @DOCLINE This section covers macros defined for platform, encoding, language, and name IDs. Note that values may be given that don't fit into any of the given macros.

			// @DOCLINE ### Platform ID

				// @DOCLINE The following macros are defined for platform IDs:

				// @DOCLINE * [0x0000] `MUTT_PLATFORM_UNICODE` - [Unicode platform](#unicode-encoding).
				#define MUTT_PLATFORM_UNICODE 0x0000
				// @DOCLINE * [0x0001] `MUTT_PLATFORM_MACINTOSH` - [Macintosh platform](#macintosh-encoding).
				#define MUTT_PLATFORM_MACINTOSH 0x0001
				// @DOCLINE * [0x0002] `MUTT_PLATFORM_ISO` - [ISO platform](#iso-encoding).
				#define MUTT_PLATFORM_ISO 0x0002
				// @DOCLINE * [0x0003] `MUTT_PLATFORM_WINDOWS` - [Windows platform](#windows-encoding).
				#define MUTT_PLATFORM_WINDOWS 0x0003
				// @DOCLINE * [0x0004] `MUTT_PLATFORM_CUSTOM` - custom encoding.
				#define MUTT_PLATFORM_CUSTOM 0x0004

				#ifdef MUTT_NAMES
				// @DOCLINE #### Platform ID names

					// @DOCLINE The name function `mutt_platform_get_name` returns a `const char*` representation of a given platform ID (for example, `MUTT_PLATFORM_UNICODE` returns "MUTT_PLATFORM_UNICODE"), defined below: @NLNT
					MUDEF const char* mutt_platform_get_name(uint16_m platform_id);
					// @DOCLINE This function returns "MU_UNKNOWN" in the case that `platform_id` is an unrecognized value.

					// @DOCLINE The name function `mutt_platform_get_nice_name` does the same thing, but returns a more readable version of it (for example, `MUTT_PLATFORM_UNICODE` returns "Unicode"), defined below: @NLNT
					MUDEF const char* mutt_platform_get_nice_name(uint16_m platform_id);
					// @DOCLINE This function returns "Unknown" in the case that `platform_id` is an unrecognized value.

					// @DOCLINE > Note that these are name functions, and are only defined if `MUTT_NAMES` is also defined.
				#endif /* MUTT_NAMES */

			// @DOCLINE ### Encoding ID

				// @DOCLINE The following macros are defined for various platform encoding IDs:

				// @DOCLINE #### Unicode encoding

					// @DOCLINE * [0x0000] `MUTT_UNICODE_1_0` - Unicode 1.0.
					#define MUTT_UNICODE_1_0 0x0000
					// @DOCLINE * [0x0001] `MUTT_UNICODE_1_1` - Unicode 1.1.
					#define MUTT_UNICODE_1_1 0x0001
					// @DOCLINE * [0x0002] `MUTT_UNICODE_ISO_IEC_10646` - ISO/IEC 10646.
					#define MUTT_UNICODE_ISO_IEC_10646 0x0002
					// @DOCLINE * [0x0003] `MUTT_UNICODE_2_0_BMP` - Unicode 2.0, BMP only.
					#define MUTT_UNICODE_2_0_BMP 0x0003
					// @DOCLINE * [0x0004] `MUTT_UNICODE_2_0` - Unicode 2.0.
					#define MUTT_UNICODE_2_0 0x0004
					// @DOCLINE * [0x0005] `MUTT_UNICODE_VARIATION` - Unicode variation sequences.
					#define MUTT_UNICODE_VARIATION 0x0005
					// @DOCLINE * [0x0006] `MUTT_UNICODE_FULL` - Unicode "full repertoire".
					#define MUTT_UNICODE_FULL 0x0006

					#ifdef MUTT_NAMES
					// @DOCLINE ##### Unicode encoding names

						// @DOCLINE The name function `mutt_unicode_encoding_get_name` returns a `const char*` representation of a given Unicode encoding ID (for example, `MUTT_UNICODE_1_0` returns "MUTT_UNICODE_1_0"), defined below: @NLNT
						MUDEF const char* mutt_unicode_encoding_get_name(uint16_m encoding_id);
						// @DOCLINE This function returns "MU_UNKNOWN" in the case that `encoding_id` is an unrecognized value.

						// @DOCLINE The name function `mutt_unicode_encoding_get_nice_name` does the same thing, but returns a more readable version of it (for example, `MUTT_UNICODE_1_0` returns "Unicode 1.0"), defined below: @NLNT
						MUDEF const char* mutt_unicode_encoding_get_nice_name(uint16_m encoding_id);
						// @DOCLINE This function returns "Unknown" in the case that `encoding_id` is an unrecognized value.

						// @DOCLINE > Note that these are name functions, and are only defined if `MUTT_NAMES` is also defined.
					#endif /* MUTT_NAMES */

				// @DOCLINE #### Macintosh encoding

					// @DOCLINE * [0x0000] `MUTT_MACINTOSH_ROMAN` - Roman.
					#define MUTT_MACINTOSH_ROMAN 0x0000
					// @DOCLINE * [0x0001] `MUTT_MACINTOSH_JAPANESE` - Japanese.
					#define MUTT_MACINTOSH_JAPANESE 0x0001
					// @DOCLINE * [0x0002] `MUTT_MACINTOSH_CHINESE_TRADITIONAL` - Chinese (Traditional).
					#define MUTT_MACINTOSH_CHINESE_TRADITIONAL 0x0002
					// @DOCLINE * [0x0003] `MUTT_MACINTOSH_KOREAN` - Korean.
					#define MUTT_MACINTOSH_KOREAN 0x0003
					// @DOCLINE * [0x0004] `MUTT_MACINTOSH_ARABIC` - Arabic.
					#define MUTT_MACINTOSH_ARABIC 0x0004
					// @DOCLINE * [0x0005] `MUTT_MACINTOSH_HEBREW` - Hebrew.
					#define MUTT_MACINTOSH_HEBREW 0x0005
					// @DOCLINE * [0x0006] `MUTT_MACINTOSH_GREEK` - Greek.
					#define MUTT_MACINTOSH_GREEK 0x0006
					// @DOCLINE * [0x0007] `MUTT_MACINTOSH_RUSSIAN` - Russian.
					#define MUTT_MACINTOSH_RUSSIAN 0x0007
					// @DOCLINE * [0x0008] `MUTT_MACINTOSH_RSYMBOL` - RSymbol.
					#define MUTT_MACINTOSH_RSYMBOL 0x0008
					// @DOCLINE * [0x0009] `MUTT_MACINTOSH_DEVANAGARI` - Devanagari.
					#define MUTT_MACINTOSH_DEVANAGARI 0x0009
					// @DOCLINE * [0x000A] `MUTT_MACINTOSH_GURMUKHI` - Gurmukhi.
					#define MUTT_MACINTOSH_GURMUKHI 0x000A
					// @DOCLINE * [0x000B] `MUTT_MACINTOSH_GUJARATI` - Gujarati.
					#define MUTT_MACINTOSH_GUJARATI 0x000B
					// @DOCLINE * [0x000C] `MUTT_MACINTOSH_ODIA` - Odia.
					#define MUTT_MACINTOSH_ODIA 0x000C
					// @DOCLINE * [0x000D] `MUTT_MACINTOSH_BANGLA` - Bangla.
					#define MUTT_MACINTOSH_BANGLA 0x000D
					// @DOCLINE * [0x000E] `MUTT_MACINTOSH_TAMIL` - Tamil.
					#define MUTT_MACINTOSH_TAMIL 0x000E
					// @DOCLINE * [0x000F] `MUTT_MACINTOSH_TELUGU` - Telugu.
					#define MUTT_MACINTOSH_TELUGU 0x000F
					// @DOCLINE * [0x0010] `MUTT_MACINTOSH_KANNADA` - Kannada.
					#define MUTT_MACINTOSH_KANNADA 0x0010
					// @DOCLINE * [0x0011] `MUTT_MACINTOSH_MALAYALAM` - Malayalam.
					#define MUTT_MACINTOSH_MALAYALAM 0x0011
					// @DOCLINE * [0x0012] `MUTT_MACINTOSH_SINHALESE` - Sinhalese.
					#define MUTT_MACINTOSH_SINHALESE 0x0012
					// @DOCLINE * [0x0013] `MUTT_MACINTOSH_BURMESE` - Burmese.
					#define MUTT_MACINTOSH_BURMESE 0x0013
					// @DOCLINE * [0x0014] `MUTT_MACINTOSH_KHMER` - Khmer.
					#define MUTT_MACINTOSH_KHMER 0x0014
					// @DOCLINE * [0x0015] `MUTT_MACINTOSH_THAI` - Thai.
					#define MUTT_MACINTOSH_THAI 0x0015
					// @DOCLINE * [0x0016] `MUTT_MACINTOSH_LAOTIAN` - Laotian.
					#define MUTT_MACINTOSH_LAOTIAN 0x0016
					// @DOCLINE * [0x0017] `MUTT_MACINTOSH_GEORGIAN` - Georgian.
					#define MUTT_MACINTOSH_GEORGIAN 0x0017
					// @DOCLINE * [0x0018] `MUTT_MACINTOSH_ARMENIAN` - Armenian.
					#define MUTT_MACINTOSH_ARMENIAN 0x0018
					// @DOCLINE * [0x0019] `MUTT_MACINTOSH_CHINESE_SIMPLIFIED` - Chinese (Simplified).
					#define MUTT_MACINTOSH_CHINESE_SIMPLIFIED 0x0019
					// @DOCLINE * [0x001A] `MUTT_MACINTOSH_TIBETAN` - Tibetan.
					#define MUTT_MACINTOSH_TIBETAN 0x001A
					// @DOCLINE * [0x001B] `MUTT_MACINTOSH_MONGOLIAN` - Mongolian.
					#define MUTT_MACINTOSH_MONGOLIAN 0x001B
					// @DOCLINE * [0x001C] `MUTT_MACINTOSH_GEEZ` - Geez.
					#define MUTT_MACINTOSH_GEEZ 0x001C
					// @DOCLINE * [0x001D] `MUTT_MACINTOSH_SLAVIC` - Slavic.
					#define MUTT_MACINTOSH_SLAVIC 0x001D
					// @DOCLINE * [0x001E] `MUTT_MACINTOSH_VIETNAMESE` - Vietnamese.
					#define MUTT_MACINTOSH_VIETNAMESE 0x001E
					// @DOCLINE * [0x001F] `MUTT_MACINTOSH_SINDHI` - Sindhi.
					#define MUTT_MACINTOSH_SINDHI 0x001F
					// @DOCLINE * [0x0020] `MUTT_MACINTOSH_UNINTERPRETED` - Uninterpreted.
					#define MUTT_MACINTOSH_UNINTERPRETED 0x0020

					#ifdef MUTT_NAMES
					// @DOCLINE ##### Macintosh encoding names

						// @DOCLINE The name function `mutt_macintosh_encoding_get_name` returns a `const char*` representation of a given Macintosh encoding ID (for example, `MUTT_MACINTOSH_ROMAN` returns "MUTT_MACINTOSH_ROMAN"), defined below: @NLNT
						MUDEF const char* mutt_macintosh_encoding_get_name(uint16_m encoding_id);
						// @DOCLINE This function returns "MU_UNKNOWN" in the case that `encoding_id` is an unrecognized value.

						// @DOCLINE The name function `mutt_macintosh_encoding_get_nice_name` does the same thing, but returns a more readable version of it (for example, `MUTT_MACINTOSH_ROMAN` returns "Roman"), defined below: @NLNT
						MUDEF const char* mutt_macintosh_encoding_get_nice_name(uint16_m encoding_id);
						// @DOCLINE This function returns "Unknown" in the case that `encoding_id` is an unrecognized value.

						// @DOCLINE > Note that these are name functions, and are only defined if `MUTT_NAMES` is also defined.
					#endif /* MUTT_NAMES */

				// @DOCLINE #### ISO encoding

					// @DOCLINE * [0x0000] `MUTT_ISO_7_BIT_ASCII` - 7-bit ASCII.
					#define MUTT_ISO_7_BIT_ASCII 0x0000
					// @DOCLINE * [0x0001] `MUTT_ISO_10646` - ISO 10646.
					#define MUTT_ISO_10646 0x0001
					// @DOCLINE * [0x0002] `MUTT_ISO_8859_1` - ISO 8859-1.
					#define MUTT_ISO_8859_1 0x0002

				// @DOCLINE #### Windows encoding

					// @DOCLINE * [0x0000] `MUTT_WINDOWS_SYMBOL` - Symbol.
					#define MUTT_WINDOWS_SYMBOL 0x0000
					// @DOCLINE * [0x0001] `MUTT_WINDOWS_UNICODE_BMP` - Unicode BMP.
					#define MUTT_WINDOWS_UNICODE_BMP 0x0001
					// @DOCLINE * [0x0002] `MUTT_WINDOWS_SHIFTJIS` - ShiftJIS.
					#define MUTT_WINDOWS_SHIFTJIS 0x0002
					// @DOCLINE * [0x0003] `MUTT_WINDOWS_PRC` - PRC.
					#define MUTT_WINDOWS_PRC 0x0003
					// @DOCLINE * [0x0004] `MUTT_WINDOWS_BIG5` - Big5.
					#define MUTT_WINDOWS_BIG5 0x0004
					// @DOCLINE * [0x0005] `MUTT_WINDOWS_WANSUNG` - Wansung.
					#define MUTT_WINDOWS_WANSUNG 0x0005
					// @DOCLINE * [0x0006] `MUTT_WINDOWS_JOHAB` - Johab.
					#define MUTT_WINDOWS_JOHAB 0x0006
					// @DOCLINE * [0x000A] `MUTT_WINDOWS_UNICODE` - Unicode full repertoire.
					#define MUTT_WINDOWS_UNICODE 0x000A

					#ifdef MUTT_NAMES
					// @DOCLINE ##### Windows encoding names

						// @DOCLINE The name function `mutt_windows_encoding_get_name` returns a `const char*` representation of a given Windows encoding ID (for example, `MUTT_WINDOWS_SYMBOL` returns "MUTT_WINDOWS_SYMBOL"), defined below: @NLNT
						MUDEF const char* mutt_windows_encoding_get_name(uint16_m encoding_id);
						// @DOCLINE This function returns "MU_UNKNOWN" in the case that `encoding_id` is an unrecognized value.

						// @DOCLINE The name function `mutt_windows_encoding_get_nice_name` does the same thing, but returns a more readable version of it (for example, `MUTT_WINDOWS_SYMBOL` returns "Symbol"), defined below: @NLNT
						MUDEF const char* mutt_windows_encoding_get_nice_name(uint16_m encoding_id);
						// @DOCLINE This function returns "Unknown" in the case that `encoding_id` is an unrecognized value.

						// @DOCLINE > Note that these are name functions, and are only defined if `MUTT_NAMES` is also defined.
					#endif /* MUTT_NAMES */

			// @DOCLINE ### Language ID

				// @DOCLINE The following macros are defined for various language IDs:

				// @DOCLINE #### Macintosh language

					// @DOCLINE * [0x0000] `MUTT_MACINTOSH_LANG_ENGLISH` - English.
					#define MUTT_MACINTOSH_LANG_ENGLISH 0x0000
					// @DOCLINE * [0x0001] `MUTT_MACINTOSH_LANG_FRENCH` - French.
					#define MUTT_MACINTOSH_LANG_FRENCH 0x0001
					// @DOCLINE * [0x0002] `MUTT_MACINTOSH_LANG_GERMAN` - German.
					#define MUTT_MACINTOSH_LANG_GERMAN 0x0002
					// @DOCLINE * [0x0003] `MUTT_MACINTOSH_LANG_ITALIAN` - Italian.
					#define MUTT_MACINTOSH_LANG_ITALIAN 0x0003
					// @DOCLINE * [0x0004] `MUTT_MACINTOSH_LANG_DUTCH` - Dutch.
					#define MUTT_MACINTOSH_LANG_DUTCH 0x0004
					// @DOCLINE * [0x0005] `MUTT_MACINTOSH_LANG_SWEDISH` - Swedish.
					#define MUTT_MACINTOSH_LANG_SWEDISH 0x0005
					// @DOCLINE * [0x0006] `MUTT_MACINTOSH_LANG_SPANISH` - Spanish.
					#define MUTT_MACINTOSH_LANG_SPANISH 0x0006
					// @DOCLINE * [0x0007] `MUTT_MACINTOSH_LANG_DANISH` - Danish.
					#define MUTT_MACINTOSH_LANG_DANISH 0x0007
					// @DOCLINE * [0x0008] `MUTT_MACINTOSH_LANG_PORTUGUESE` - Portuguese.
					#define MUTT_MACINTOSH_LANG_PORTUGUESE 0x0008
					// @DOCLINE * [0x0009] `MUTT_MACINTOSH_LANG_NORWEGIAN` - Norwegian.
					#define MUTT_MACINTOSH_LANG_NORWEGIAN 0x0009
					// @DOCLINE * [0x000A] `MUTT_MACINTOSH_LANG_HEBREW` - Hebrew.
					#define MUTT_MACINTOSH_LANG_HEBREW 0x000A
					// @DOCLINE * [0x000B] `MUTT_MACINTOSH_LANG_JAPANESE` - Japanese.
					#define MUTT_MACINTOSH_LANG_JAPANESE 0x000B
					// @DOCLINE * [0x000C] `MUTT_MACINTOSH_LANG_ARABIC` - Arabic.
					#define MUTT_MACINTOSH_LANG_ARABIC 0x000C
					// @DOCLINE * [0x000D] `MUTT_MACINTOSH_LANG_FINNISH` - Finnish.
					#define MUTT_MACINTOSH_LANG_FINNISH 0x000D
					// @DOCLINE * [0x000E] `MUTT_MACINTOSH_LANG_GREEK` - Greek.
					#define MUTT_MACINTOSH_LANG_GREEK 0x000E
					// @DOCLINE * [0x000F] `MUTT_MACINTOSH_LANG_ICELANDIC` - Icelandic.
					#define MUTT_MACINTOSH_LANG_ICELANDIC 0x000F
					// @DOCLINE * [0x0010] `MUTT_MACINTOSH_LANG_MALTESE` - Maltese.
					#define MUTT_MACINTOSH_LANG_MALTESE 0x0010
					// @DOCLINE * [0x0011] `MUTT_MACINTOSH_LANG_TURKISH` - Turkish.
					#define MUTT_MACINTOSH_LANG_TURKISH 0x0011
					// @DOCLINE * [0x0012] `MUTT_MACINTOSH_LANG_CROATIAN` - Croatian.
					#define MUTT_MACINTOSH_LANG_CROATIAN 0x0012
					// @DOCLINE * [0x0013] `MUTT_MACINTOSH_LANG_CHINESE_TRADITIONAL` - Chinese (traditional).
					#define MUTT_MACINTOSH_LANG_CHINESE_TRADITIONAL 0x0013
					// @DOCLINE * [0x0014] `MUTT_MACINTOSH_LANG_URDU` - Urdu.
					#define MUTT_MACINTOSH_LANG_URDU 0x0014
					// @DOCLINE * [0x0015] `MUTT_MACINTOSH_LANG_HINDI` - Hindi.
					#define MUTT_MACINTOSH_LANG_HINDI 0x0015
					// @DOCLINE * [0x0016] `MUTT_MACINTOSH_LANG_THAI` - Thai.
					#define MUTT_MACINTOSH_LANG_THAI 0x0016
					// @DOCLINE * [0x0017] `MUTT_MACINTOSH_LANG_KOREAN` - Korean.
					#define MUTT_MACINTOSH_LANG_KOREAN 0x0017
					// @DOCLINE * [0x0018] `MUTT_MACINTOSH_LANG_LITHUANIAN` - Lithuanian.
					#define MUTT_MACINTOSH_LANG_LITHUANIAN 0x0018
					// @DOCLINE * [0x0019] `MUTT_MACINTOSH_LANG_POLISH` - Polish.
					#define MUTT_MACINTOSH_LANG_POLISH 0x0019
					// @DOCLINE * [0x001A] `MUTT_MACINTOSH_LANG_HUNGARIAN` - Hungarian.
					#define MUTT_MACINTOSH_LANG_HUNGARIAN 0x001A
					// @DOCLINE * [0x001B] `MUTT_MACINTOSH_LANG_ESTONIAN` - Estonian.
					#define MUTT_MACINTOSH_LANG_ESTONIAN 0x001B
					// @DOCLINE * [0x001C] `MUTT_MACINTOSH_LANG_LATVIAN` - Latvian.
					#define MUTT_MACINTOSH_LANG_LATVIAN 0x001C
					// @DOCLINE * [0x001D] `MUTT_MACINTOSH_LANG_SAMI` - Sami.
					#define MUTT_MACINTOSH_LANG_SAMI 0x001D
					// @DOCLINE * [0x001E] `MUTT_MACINTOSH_LANG_FAROESE` - Faroese.
					#define MUTT_MACINTOSH_LANG_FAROESE 0x001E
					// @DOCLINE * [0x001F] `MUTT_MACINTOSH_LANG_FARSI_PERSIAN` - Farsi/Persian.
					#define MUTT_MACINTOSH_LANG_FARSI_PERSIAN 0x001F
					// @DOCLINE * [0x0020] `MUTT_MACINTOSH_LANG_RUSSIAN` - Russian.
					#define MUTT_MACINTOSH_LANG_RUSSIAN 0x0020
					// @DOCLINE * [0x0021] `MUTT_MACINTOSH_LANG_CHINESE_SIMPLIFIED` - Chinese (simplified).
					#define MUTT_MACINTOSH_LANG_CHINESE_SIMPLIFIED 0x0021
					// @DOCLINE * [0x0022] `MUTT_MACINTOSH_LANG_FLEMISH` - Flemish.
					#define MUTT_MACINTOSH_LANG_FLEMISH 0x0022
					// @DOCLINE * [0x0023] `MUTT_MACINTOSH_LANG_IRISH_GAELIC` - Irish Gaelic.
					#define MUTT_MACINTOSH_LANG_IRISH_GAELIC 0x0023
					// @DOCLINE * [0x0024] `MUTT_MACINTOSH_LANG_ALBANIAN` - Albanian.
					#define MUTT_MACINTOSH_LANG_ALBANIAN 0x0024
					// @DOCLINE * [0x0025] `MUTT_MACINTOSH_LANG_ROMANIAN` - Romanian.
					#define MUTT_MACINTOSH_LANG_ROMANIAN 0x0025
					// @DOCLINE * [0x0026] `MUTT_MACINTOSH_LANG_CZECH` - Czech.
					#define MUTT_MACINTOSH_LANG_CZECH 0x0026
					// @DOCLINE * [0x0027] `MUTT_MACINTOSH_LANG_SLOVAK` - Slovak.
					#define MUTT_MACINTOSH_LANG_SLOVAK 0x0027
					// @DOCLINE * [0x0028] `MUTT_MACINTOSH_LANG_SLOVENIAN` - Slovenian.
					#define MUTT_MACINTOSH_LANG_SLOVENIAN 0x0028
					// @DOCLINE * [0x0029] `MUTT_MACINTOSH_LANG_YIDDISH` - Yiddish.
					#define MUTT_MACINTOSH_LANG_YIDDISH 0x0029
					// @DOCLINE * [0x002A] `MUTT_MACINTOSH_LANG_SERBIAN` - Serbian.
					#define MUTT_MACINTOSH_LANG_SERBIAN 0x002A
					// @DOCLINE * [0x002B] `MUTT_MACINTOSH_LANG_MACEDONIAN` - Macedonian.
					#define MUTT_MACINTOSH_LANG_MACEDONIAN 0x002B
					// @DOCLINE * [0x002C] `MUTT_MACINTOSH_LANG_BULGARIAN` - Bulgarian.
					#define MUTT_MACINTOSH_LANG_BULGARIAN 0x002C
					// @DOCLINE * [0x002D] `MUTT_MACINTOSH_LANG_UKRANIAN` - Ukrainian.
					#define MUTT_MACINTOSH_LANG_UKRANIAN 0x002D
					// @DOCLINE * [0x002E] `MUTT_MACINTOSH_LANG_BYELORUSSIAN` - Byelorussian.
					#define MUTT_MACINTOSH_LANG_BYELORUSSIAN 0x002E
					// @DOCLINE * [0x002F] `MUTT_MACINTOSH_LANG_UZBEK` - Uzbek.
					#define MUTT_MACINTOSH_LANG_UZBEK 0x002F
					// @DOCLINE * [0x0030] `MUTT_MACINTOSH_LANG_KAZAKH` - Kazakh.
					#define MUTT_MACINTOSH_LANG_KAZAKH 0x0030
					// @DOCLINE * [0x0031] `MUTT_MACINTOSH_LANG_AZERBAIJANI_CYRILLIC` - Azerbaijani (Cyrillic script).
					#define MUTT_MACINTOSH_LANG_AZERBAIJANI_CYRILLIC 0x0031
					// @DOCLINE * [0x0032] `MUTT_MACINTOSH_LANG_AZERBAIJANI_ARABIC` - Azerbaijani (Arabic script).
					#define MUTT_MACINTOSH_LANG_AZERBAIJANI_ARABIC 0x0032
					// @DOCLINE * [0x0033] `MUTT_MACINTOSH_LANG_ARMENIAN` - Armenian.
					#define MUTT_MACINTOSH_LANG_ARMENIAN 0x0033
					// @DOCLINE * [0x0034] `MUTT_MACINTOSH_LANG_GEORGIAN` - Georgian.
					#define MUTT_MACINTOSH_LANG_GEORGIAN 0x0034
					// @DOCLINE * [0x0035] `MUTT_MACINTOSH_LANG_MOLDAVIAN` - Moldavian.
					#define MUTT_MACINTOSH_LANG_MOLDAVIAN 0x0035
					// @DOCLINE * [0x0036] `MUTT_MACINTOSH_LANG_KIRGHIZ` - Kirghiz.
					#define MUTT_MACINTOSH_LANG_KIRGHIZ 0x0036
					// @DOCLINE * [0x0037] `MUTT_MACINTOSH_LANG_TAJIKI` - Tajiki.
					#define MUTT_MACINTOSH_LANG_TAJIKI 0x0037
					// @DOCLINE * [0x0038] `MUTT_MACINTOSH_LANG_TURKMEN` - Turkmen.
					#define MUTT_MACINTOSH_LANG_TURKMEN 0x0038
					// @DOCLINE * [0x0039] `MUTT_MACINTOSH_LANG_MONGOLIAN` - Mongolian (Mongolian script).
					#define MUTT_MACINTOSH_LANG_MONGOLIAN 0x0039
					// @DOCLINE * [0x003A] `MUTT_MACINTOSH_LANG_MONGOLIAN_CYRILLIC` - Mongolian (Cyrillic script).
					#define MUTT_MACINTOSH_LANG_MONGOLIAN_CYRILLIC 0x003A
					// @DOCLINE * [0x003B] `MUTT_MACINTOSH_LANG_PASHTO` - Pashto.
					#define MUTT_MACINTOSH_LANG_PASHTO 0x003B
					// @DOCLINE * [0x003C] `MUTT_MACINTOSH_LANG_KURDISH` - Kurdish.
					#define MUTT_MACINTOSH_LANG_KURDISH 0x003C
					// @DOCLINE * [0x003D] `MUTT_MACINTOSH_LANG_KASHMIRI` - Kashmiri.
					#define MUTT_MACINTOSH_LANG_KASHMIRI 0x003D
					// @DOCLINE * [0x003E] `MUTT_MACINTOSH_LANG_SINDHI` - Sindhi.
					#define MUTT_MACINTOSH_LANG_SINDHI 0x003E
					// @DOCLINE * [0x003F] `MUTT_MACINTOSH_LANG_TIBETAN` - Tibetan.
					#define MUTT_MACINTOSH_LANG_TIBETAN 0x003F
					// @DOCLINE * [0x0040] `MUTT_MACINTOSH_LANG_NEPALI` - Nepali.
					#define MUTT_MACINTOSH_LANG_NEPALI 0x0040
					// @DOCLINE * [0x0041] `MUTT_MACINTOSH_LANG_SANSKIRT` - Sanskrit.
					#define MUTT_MACINTOSH_LANG_SANSKIRT 0x0041
					// @DOCLINE * [0x0042] `MUTT_MACINTOSH_LANG_MARATHI` - Marathi.
					#define MUTT_MACINTOSH_LANG_MARATHI 0x0042
					// @DOCLINE * [0x0043] `MUTT_MACINTOSH_LANG_BENGALI` - Bengali.
					#define MUTT_MACINTOSH_LANG_BENGALI 0x0043
					// @DOCLINE * [0x0044] `MUTT_MACINTOSH_LANG_ASSAMESE` - Assamese.
					#define MUTT_MACINTOSH_LANG_ASSAMESE 0x0044
					// @DOCLINE * [0x0045] `MUTT_MACINTOSH_LANG_GUJARATI` - Gujarati.
					#define MUTT_MACINTOSH_LANG_GUJARATI 0x0045
					// @DOCLINE * [0x0046] `MUTT_MACINTOSH_LANG_PUNJABI` - Punjabi.
					#define MUTT_MACINTOSH_LANG_PUNJABI 0x0046
					// @DOCLINE * [0x0047] `MUTT_MACINTOSH_LANG_ORIYA` - Oriya.
					#define MUTT_MACINTOSH_LANG_ORIYA 0x0047
					// @DOCLINE * [0x0048] `MUTT_MACINTOSH_LANG_MALAYALAM` - Malayalam.
					#define MUTT_MACINTOSH_LANG_MALAYALAM 0x0048
					// @DOCLINE * [0x0049] `MUTT_MACINTOSH_LANG_KANNADA` - Kannada.
					#define MUTT_MACINTOSH_LANG_KANNADA 0x0049
					// @DOCLINE * [0x004A] `MUTT_MACINTOSH_LANG_TAMIL` - Tamil.
					#define MUTT_MACINTOSH_LANG_TAMIL 0x004A
					// @DOCLINE * [0x004B] `MUTT_MACINTOSH_LANG_TELUGU` - Telugu.
					#define MUTT_MACINTOSH_LANG_TELUGU 0x004B
					// @DOCLINE * [0x004C] `MUTT_MACINTOSH_LANG_SINHALESE` - Sinhalese.
					#define MUTT_MACINTOSH_LANG_SINHALESE 0x004C
					// @DOCLINE * [0x004D] `MUTT_MACINTOSH_LANG_BURMESE` - Burmese.
					#define MUTT_MACINTOSH_LANG_BURMESE 0x004D
					// @DOCLINE * [0x004E] `MUTT_MACINTOSH_LANG_KHMER` - Khmer.
					#define MUTT_MACINTOSH_LANG_KHMER 0x004E
					// @DOCLINE * [0x004F] `MUTT_MACINTOSH_LANG_LAO` - Lao.
					#define MUTT_MACINTOSH_LANG_LAO 0x004F
					// @DOCLINE * [0x0050] `MUTT_MACINTOSH_LANG_VIETNAMESE` - Vietnamese.
					#define MUTT_MACINTOSH_LANG_VIETNAMESE 0x0050
					// @DOCLINE * [0x0051] `MUTT_MACINTOSH_LANG_INDONESIAN` - Indonesian.
					#define MUTT_MACINTOSH_LANG_INDONESIAN 0x0051
					// @DOCLINE * [0x0052] `MUTT_MACINTOSH_LANG_TAGALOG` - Tagalog.
					#define MUTT_MACINTOSH_LANG_TAGALOG 0x0052
					// @DOCLINE * [0x0053] `MUTT_MACINTOSH_LANG_MALAY_ROMAN` - Malay (Roman script).
					#define MUTT_MACINTOSH_LANG_MALAY_ROMAN 0x0053
					// @DOCLINE * [0x0054] `MUTT_MACINTOSH_LANG_MALAY_ARABIC` - Malay (Arabic script).
					#define MUTT_MACINTOSH_LANG_MALAY_ARABIC 0x0054
					// @DOCLINE * [0x0055] `MUTT_MACINTOSH_LANG_AMHARIC` - Amharic.
					#define MUTT_MACINTOSH_LANG_AMHARIC 0x0055
					// @DOCLINE * [0x0056] `MUTT_MACINTOSH_LANG_TIGRINYA` - Tigrinya.
					#define MUTT_MACINTOSH_LANG_TIGRINYA 0x0056
					// @DOCLINE * [0x0057] `MUTT_MACINTOSH_LANG_GALLA` - Galla.
					#define MUTT_MACINTOSH_LANG_GALLA 0x0057
					// @DOCLINE * [0x0058] `MUTT_MACINTOSH_LANG_SOMALI` - Somali.
					#define MUTT_MACINTOSH_LANG_SOMALI 0x0058
					// @DOCLINE * [0x0059] `MUTT_MACINTOSH_LANG_SWAHILI` - Swahili.
					#define MUTT_MACINTOSH_LANG_SWAHILI 0x0059
					// @DOCLINE * [0x005A] `MUTT_MACINTOSH_LANG_KINYARWANDA_RUANDA` - Kinyarwanda/Ruanda.
					#define MUTT_MACINTOSH_LANG_KINYARWANDA_RUANDA 0x005A
					// @DOCLINE * [0x005B] `MUTT_MACINTOSH_LANG_RUNDI` - Rundi.
					#define MUTT_MACINTOSH_LANG_RUNDI 0x005B
					// @DOCLINE * [0x005C] `MUTT_MACINTOSH_LANG_NYANJA_CHEWA` - Nyanja/Chewa.
					#define MUTT_MACINTOSH_LANG_NYANJA_CHEWA 0x005C
					// @DOCLINE * [0x005D] `MUTT_MACINTOSH_LANG_MALAGASY` - Malagasy.
					#define MUTT_MACINTOSH_LANG_MALAGASY 0x005D
					// @DOCLINE * [0x005E] `MUTT_MACINTOSH_LANG_ESPERANTO` - Esperanto.
					#define MUTT_MACINTOSH_LANG_ESPERANTO 0x005E
					// @DOCLINE * [0x0080] `MUTT_MACINTOSH_LANG_WELSH` - Welsh.
					#define MUTT_MACINTOSH_LANG_WELSH 0x0080
					// @DOCLINE * [0x0081] `MUTT_MACINTOSH_LANG_BASQUE` - Basque.
					#define MUTT_MACINTOSH_LANG_BASQUE 0x0081
					// @DOCLINE * [0x0082] `MUTT_MACINTOSH_LANG_CATALAN` - Catalan.
					#define MUTT_MACINTOSH_LANG_CATALAN 0x0082
					// @DOCLINE * [0x0083] `MUTT_MACINTOSH_LANG_LATIN` - Latin.
					#define MUTT_MACINTOSH_LANG_LATIN 0x0083
					// @DOCLINE * [0x0084] `MUTT_MACINTOSH_LANG_QUECHUA` - Quechua.
					#define MUTT_MACINTOSH_LANG_QUECHUA 0x0084
					// @DOCLINE * [0x0085] `MUTT_MACINTOSH_LANG_GUARANI` - Guarani.
					#define MUTT_MACINTOSH_LANG_GUARANI 0x0085
					// @DOCLINE * [0x0086] `MUTT_MACINTOSH_LANG_AYMARA` - Aymara.
					#define MUTT_MACINTOSH_LANG_AYMARA 0x0086
					// @DOCLINE * [0x0087] `MUTT_MACINTOSH_LANG_TATAR` - Tatar.
					#define MUTT_MACINTOSH_LANG_TATAR 0x0087
					// @DOCLINE * [0x0088] `MUTT_MACINTOSH_LANG_UIGHUR` - Uighur.
					#define MUTT_MACINTOSH_LANG_UIGHUR 0x0088
					// @DOCLINE * [0x0089] `MUTT_MACINTOSH_LANG_DZONGKHA` - Dzongkha.
					#define MUTT_MACINTOSH_LANG_DZONGKHA 0x0089
					// @DOCLINE * [0x008A] `MUTT_MACINTOSH_LANG_JAVANESE_ROMAN` - Javanese (Roman script).
					#define MUTT_MACINTOSH_LANG_JAVANESE_ROMAN 0x008A
					// @DOCLINE * [0x008B] `MUTT_MACINTOSH_LANG_SUNDANESE_ROMAN` - Sundanese (Roman script).
					#define MUTT_MACINTOSH_LANG_SUNDANESE_ROMAN 0x008B
					// @DOCLINE * [0x008C] `MUTT_MACINTOSH_LANG_GALICIAN` - Galician.
					#define MUTT_MACINTOSH_LANG_GALICIAN 0x008C
					// @DOCLINE * [0x008D] `MUTT_MACINTOSH_LANG_AFRIKAANS` - Afrikaans.
					#define MUTT_MACINTOSH_LANG_AFRIKAANS 0x008D
					// @DOCLINE * [0x008E] `MUTT_MACINTOSH_LANG_BRETON` - Breton.
					#define MUTT_MACINTOSH_LANG_BRETON 0x008E
					// @DOCLINE * [0x008F] `MUTT_MACINTOSH_LANG_INUKTITUT` - Inuktitut.
					#define MUTT_MACINTOSH_LANG_INUKTITUT 0x008F
					// @DOCLINE * [0x0090] `MUTT_MACINTOSH_LANG_SCOTTISH_GAELIC` - Scottish Gaelic.
					#define MUTT_MACINTOSH_LANG_SCOTTISH_GAELIC 0x0090
					// @DOCLINE * [0x0091] `MUTT_MACINTOSH_LANG_MANX_GAELIC` - Manx Gaelic.
					#define MUTT_MACINTOSH_LANG_MANX_GAELIC 0x0091
					// @DOCLINE * [0x0092] `MUTT_MACINTOSH_LANG_IRISH_GAELIC_DOT_ABOVE` - Irish Gaelic (with dot above).
					#define MUTT_MACINTOSH_LANG_IRISH_GAELIC_DOT_ABOVE 0x0092
					// @DOCLINE * [0x0093] `MUTT_MACINTOSH_LANG_TONGAN` - Tongan.
					#define MUTT_MACINTOSH_LANG_TONGAN 0x0093
					// @DOCLINE * [0x0094] `MUTT_MACINTOSH_LANG_GREEK_POLYTONIC` - Greek (polytonic).
					#define MUTT_MACINTOSH_LANG_GREEK_POLYTONIC 0x0094
					// @DOCLINE * [0x0095] `MUTT_MACINTOSH_LANG_GREENLANDIC` - Greenlandic.
					#define MUTT_MACINTOSH_LANG_GREENLANDIC 0x0095
					// @DOCLINE * [0x0096] `MUTT_MACINTOSH_LANG_AZERBAIJANI_ROMAN` - Azerbaijani (Roman script).
					#define MUTT_MACINTOSH_LANG_AZERBAIJANI_ROMAN 0x0096

			// @DOCLINE ### Name ID

				// @DOCLINE The follwing macros are defined for various name IDs:

				// @DOCLINE * [0x0000] `MUTT_NAME_COPYRIGHT_NOTICE` - "Copyright notice."
				#define MUTT_NAME_COPYRIGHT_NOTICE 0x0000
				// @DOCLINE * [0x0001] `MUTT_NAME_FONT_FAMILY` - "Font Family name."
				#define MUTT_NAME_FONT_FAMILY 0x0001
				// @DOCLINE * [0x0002] `MUTT_NAME_FONT_SUBFAMILY` - "Font Subfamily name."
				#define MUTT_NAME_FONT_SUBFAMILY 0x0002
				// @DOCLINE * [0x0003] `MUTT_NAME_FONT_IDENTIFIER` - "Unique font identifier."
				#define MUTT_NAME_FONT_IDENTIFIER 0x0003
				// @DOCLINE * [0x0004] `MUTT_NAME_FONT_FULL` - "Full font name that reflects all family and relevant subfamily descriptors".
				#define MUTT_NAME_FONT_FULL 0x0004
				// @DOCLINE * [0x0005] `MUTT_NAME_VERSION` - "Version string."
				#define MUTT_NAME_VERSION 0x0005
				// @DOCLINE * [0x0006] `MUTT_NAME_POSTSCRIPT` - "PostScript name for the font."
				#define MUTT_NAME_POSTSCRIPT 0x0006
				// @DOCLINE * [0x0007] `MUTT_NAME_TRADEMARK` - "Trademark."
				#define MUTT_NAME_TRADEMARK 0x0007
				// @DOCLINE * [0x0008] `MUTT_NAME_MANUFACTURER` - "Manufacturer Name."
				#define MUTT_NAME_MANUFACTURER 0x0008
				// @DOCLINE * [0x0009] `MUTT_NAME_DESIGNER` - "Designer."
				#define MUTT_NAME_DESIGNER 0x0009
				// @DOCLINE * [0x000A] `MUTT_NAME_DESCRIPTION` - "Description."
				#define MUTT_NAME_DESCRIPTION 0x000A
				// @DOCLINE * [0x000B] `MUTT_NAME_VENDOR_URL` - "URL of Vendor."
				#define MUTT_NAME_VENDOR_URL 0x000B
				// @DOCLINE * [0x000C] `MUTT_NAME_DESIGNER_URL` - "URL of Designer."
				#define MUTT_NAME_DESIGNER_URL 0x000C
				// @DOCLINE * [0x000D] `MUTT_NAME_LICENSE` - "License Description."
				#define MUTT_NAME_LICENSE 0x000D
				// @DOCLINE * [0x000E] `MUTT_NAME_LICENSE_URL` - "License Info URL."
				#define MUTT_NAME_LICENSE_URL 0x000E
				// @DOCLINE * [0x0010] `MUTT_NAME_TYPOGRAPHIC_FAMILY` - "Typographic Family name."
				#define MUTT_NAME_TYPOGRAPHIC_FAMILY 0x0010
				// @DOCLINE * [0x0011] `MUTT_NAME_TYPOGRAPHIC_SUBFAMILY` - "Typographic Subfamily name."
				#define MUTT_NAME_TYPOGRAPHIC_SUBFAMILY 0x0011
				// @DOCLINE * [0x0012] `MUTT_NAME_COMPATIBLE_FULL` - "Compatible Full (Macintosh only)."
				#define MUTT_NAME_COMPATIBLE_FULL 0x0012
				// @DOCLINE * [0x0013] `MUTT_NAME_SAMPLE_TEXT` - "Sample text."
				#define MUTT_NAME_SAMPLE_TEXT 0x0013
				// @DOCLINE * [0x0014] `MUTT_NAME_POSTSCRIPT_CID_FINDFONT` - "PostScript CID findfont name."
				#define MUTT_NAME_POSTSCRIPT_CID_FINDFONT 0x0014
				// @DOCLINE * [0x0015] `MUTT_NAME_WWS_FAMILY` - "WWS Family Name."
				#define MUTT_NAME_WWS_FAMILY 0x0015
				// @DOCLINE * [0x0016] `MUTT_NAME_WWS_SUBFAMILY` - "WWS Subfamily Name."
				#define MUTT_NAME_WWS_SUBFAMILY 0x0016
				// @DOCLINE * [0x0017] `MUTT_NAME_LIGHT_BACKGROUND_PALETTE` - "Light Background Palette."
				#define MUTT_NAME_LIGHT_BACKGROUND_PALETTE 0x0017
				// @DOCLINE * [0x0018] `MUTT_NAME_DARK_BACKGROUND_PALETTE` - "Dark Background Palette."
				#define MUTT_NAME_DARK_BACKGROUND_PALETTE 0x0018

				#ifdef MUTT_NAMES
				// @DOCLINE #### Name ID names

					// @DOCLINE The name function `mutt_name_id_get_name` returns a `const char*` representation of a given name ID (for example, `MUTT_NAME_COPYRIGHT_NOTICE` returns "MUTT_NAME_COPYRIGHT_NOTICE"), defined below: @NLNT
					MUDEF const char* mutt_name_id_get_name(uint16_m name_id);
					// @DOCLINE This function returns "MU_UNKNOWN" in the case that `name_id` is an unrecognized value.

					// @DOCLINE The name function `mutt_name_id_get_nice_name` does the same thing, but returns a more readable version of it (for example, `MUTT_NAME_COPYRIGHT_NOTICE` returns "Copyright notice"), defined below: @NLNT
					MUDEF const char* mutt_name_id_get_nice_name(uint16_m name_id);
					// @DOCLINE This function returns "Unknown" in the case that `name_id` is an unrecognized value.

					// @DOCLINE > Note that these are name functions, and are only defined if `MUTT_NAMES` is also defined.
				#endif /* MUTT_NAMES */

		// @DOCLINE ## Internally used low-level functionality

			// @DOCLINE mutt uses several internally-defined low-level things to make certain things easier to perform. This section is a list of them.

			// @DOCLINE ### Reading F2DOT14 values

				// @DOCLINE The macro function `MUTT_F2DOT14` creates an expression for a float equivalent of a given array that stores 2 bytes representing a big-endian F2DOT14, defined below: @NLNT
				#define MUTT_F2DOT14(b) (((float)((*(int8_m*)&b[1]) & 0xC0)) + (((float)(MU_RBEU16(b) & 0xFFFF)) / 16384.f))

			// @DOCLINE ### Delta logic

				// @DOCLINE Some cmap formats use fairly weird logic when using "idDelta" values. The function `mutt_id_delta` figures this logic out automatically, defined below: @NLNT
				MUDEF uint16_m mutt_id_delta(uint16_m character_code, int16_m delta);

	// @DOCLINE # Raster API

		// @DOCLINE The raster API has the ability to [rasterize](#rasterize-glyph) [TrueType-like glyphs](#raster-glyph) onto [a bitmap](#raster-bitmap).

		// @DOCLINE ## Raster glyph

			// @DOCLINE A "raster glyph" (often shortened to "rglyph", respective struct [`muttRGlyph`](#rglyph-struct)) is a glyph described for rasterization in the raster API, being similar to how a simple glyph is defined in the low-level API, and is heavily based on how glyphs are specified in TrueType.

			// @DOCLINE The most common usage of rglyphs is for [rasterizing given glyphs in a TrueType font](#rasterization-of-truetype-glyphs). This can be achieved via converting a simple or composite glyph retrieved from the low-level API to an rglyph equivalent, which mutt has built-in support for, and can do [automatically via rendering a glyph purely based on its glyph ID](#rasterize-glyph-id).

			// @DOCLINE Rglyphs don't necessarily need to come from a simple or composite glyph, however. The user can pass in their own rglyphs, and as long as they use the struct correctly, the raster API will rasterize it correctly.

			// @DOCLINE ### Rglyph struct

				typedef uint8_m muttRFlags;
				typedef struct muttRPoint muttRPoint;
				typedef struct muttRGlyph muttRGlyph;

				// @DOCLINE An rglyph is represented via the struct `muttRGlyph`, which has the following members:

				struct muttRGlyph {
					// @DOCLINE * `@NLFT num_points` - the number of points in the `points` array. This value must be at least 1.
					uint16_m num_points;
					// @DOCLINE * `@NLFT* points` - each point for the glyph.
					muttRPoint* points;
					// @DOCLINE * `@NLFT num_contours` - the number of contours in the glyph.
					uint16_m num_contours;
					// @DOCLINE * `@NLFT* contour_ends` - the last point index of each contour, in increasing order. `contour_ends[num_contours-1]+1` must equal `num_points`.
					uint16_m* contour_ends;
					// @DOCLINE * `@NLFT x_max` - the greatest x-coordinate value of any point within the glyph.
					float x_max;
					// @DOCLINE * `@NLFT y_max` - the greatest y-coordinate value of any point within the glyph.
					float y_max;
				};

				// @DOCLINE A point in an rglyph is represented with the struct `muttRPoint`, which has the following members:
				struct muttRPoint {
					// @DOCLINE * `@NLFT x` - the x-coordinate of the point, in [pixel units](#raster-bitmap).
					float x;
					// @DOCLINE * `@NLFT y` - the y-coordinate of the point, in [pixel units](#raster-bitmap).
					float y;
					// @DOCLINE * `@NLFT flags` - the [flags](#rglyph-flags) of the point.
					muttRFlags flags;
				};

				// @DOCLINE No coordinate values in any point within an rglyph should be negative, or exceed the values indicated by `x_max` and `y_max`.

				// @DOCLINE The ordering of points should follow the non-zero winding number rule that TrueType glyphs also follow: "[Points that have a non-zero winding number are inside the glyph. All other points are outside the glyph.](https://developer.apple.com/fonts/TrueType-Reference-Manual/RM02/Chap2.html#distinguishing)"

				// @DOCLINE All contours must start with an on-curve point.

				// @DOCLINE Some rendering methods have the possibility of "[bleeding](#raster-bleeding)" over pixels that mathematically are completely outside of the glyph, but are one pixel away from another pixel who is at least partially inside of the glyph. For this reason, it is recommended to have each points' coordinates offset by at least 1 pixel, so that no pixel coordinate is exactly at 0. This is automatically performed when converting simple and composite glyphs from the low-level API to an rglyph, and should be done by the user when creating/modifying rglyphs.

			// @DOCLINE #### Rglyph flags

				// @DOCLINE The type `muttRFlags` (typedef for `uint8_m`) represents the flags of a given point in an rglyph. It has the following defined values for bitmasking:

				// @DOCLINE * [0x01] `MUTTR_ON_CURVE` - represents whether or not the point is on (1) or off (0) the curve; equivalent to "ON_CURVE_POINT" for simple glyphs in TrueType.
				#define MUTTR_ON_CURVE 0x01

				// @DOCLINE No other bits other than the ones defined above are read for any point in an rglyph.

		// @DOCLINE ## Raster bitmap

			typedef struct muttRBitmap muttRBitmap;
			typedef uint8_m muttRIOColor;
			typedef uint16_m muttRChannels;

			// @DOCLINE Rasterization of an rglyph is performed on a bitmap. The information about the bitmap is provided by the struct `muttRBitmap`, which has the following members:

			struct muttRBitmap {
				// @DOCLINE * `@NLFT width` - the width of the bitmap, in pixels.
				uint32_m width;
				// @DOCLINE * `@NLFT height` - the height of the bitmap, in pixels.
				uint32_m height;
				// @DOCLINE * `@NLFT channels` - the [channels](#raster-channels) of the bitmap.
				muttRChannels channels;
				// @DOCLINE * `@NLFT stride` - the amount of bytes to move by for each horizontal row of pixels.
				uint32_m stride;
				// @DOCLINE * `@NLFT* pixels` - the pixel data for the bitmap to be filled in, stored from left to right, top to bottom. All values within the pixel data are expected to be pre-initialized to the appropriate out-of-glyph color indicated by `io_color`.
				uint8_m* pixels;
				// @DOCLINE * `@NLFT io_color` - the [in/out color](#raster-in-out-color) of the bitmap.
				muttRIOColor io_color;
			};

			// @DOCLINE ### Raster channels

				// @DOCLINE The type `muttRChannels` (typedef for `uint16_m`) represents the channels of a bitmap. It has the following defined values:

				// @DOCLINE * [0x0000] `MUTTR_R` - one color channel per pixel, corresponding to one value representing how far a pixel is *in* or *out* of the glyph.
				#define MUTTR_R 0x0000
				// @DOCLINE * [0x0002] `MUTTR_RGB` - three color channels: red, green, and blue in that order per pixel.
				#define MUTTR_RGB 0x0002
				// @DOCLINE * [0x0003] `MUTTR_RGBA` - four color channels: red, green, blue, and alpha in that order per pixel.
				#define MUTTR_RGBA 0x0003

				// @DOCLINE How non-singular channel values represent how far a pixel is *in* or *out* of the glyph is dependent on the [raster method](#raster-method).

			// @DOCLINE ### Raster in out color

				// @DOCLINE The type `muttRIOColor` (typedef for `uint8_m`) represents what values indicate whether or not a pixel is *inside* of the glyph or *outside* of the glyph (and the corresponding possible mixing between the two values). It has the following defined values:

				// @DOCLINE * [0x00] `MUTTR_BW` - a smaller value indicates being more outside the glyph, and a larger value indicates being more inside the glyph.
				#define MUTTR_BW 0x00
				// @DOCLINE * [0x01] `MUTTR_WB` - a larger value indicates being more outside the glyph, and a smaller value indicates being more inside the glyph.
				#define MUTTR_WB 0x01

				// @DOCLINE The rules of these values applies to all channels, including alpha.

		// @DOCLINE ## Rasterize glyph

			typedef uint16_m muttRMethod;

			// @DOCLINE Rasterizing a glyph is performed with the function `mutt_raster_glyph`, defined below: @NLNT
			MUDEF muttResult mutt_raster_glyph(muttRGlyph* glyph, muttRBitmap* bitmap, muttRMethod method);

			// @DOCLINE ### Raster method

				// @DOCLINE The type `muttRMethod` (typedef for `uint16_m`) represents what rasterization method to use when rasterizing a glyph. It has the following defined values:

				// @DOCLINE * [0x0000] `MUTTR_FULL_PIXEL_BI_LEVEL` - [full-pixel](#full-pixel) [bi-level](#bi-level) rasterization.
				#define MUTTR_FULL_PIXEL_BI_LEVEL 0x0000
				// @DOCLINE * [0x0001] `MUTTR_FULL_PIXEL_AA2X2` - [full-pixel](#full-pixel) two-by-two [anti-aliased](#anti-aliasing) rasterization.
				#define MUTTR_FULL_PIXEL_AA2X2 0x0001

				// @DOCLINE Most of the terms used to describe these rendering methods are taken from terms used in [The Raster Tragedy](http://rastertragedy.com).

			// @DOCLINE ### Full-pixel

				// @DOCLINE The term "full-pixel" means that each pixel is used as one value indicating how much a pixel is *inside* or *outside* of the glyph. Each pixel is treated pixel-coordinate-wise as being directly in the center of a pixel in the pixel coordinate grid; for example, the coordinates of the top-leftest pixel in a bitmap is (0.5, 0.5) when internally calculating how much a pixel is inside or outside of the glyph.

			// @DOCLINE ### Bi-level

				// @DOCLINE The term "bi-level" means that each pixel is rather fully inside or outside of the glyph, with no possibility of intermediate values.

			// @DOCLINE ### Anti-aliasing

				// @DOCLINE Anti-aliasing is used in rasterization to smooth jagged edges, taking multiple samples per pixel, calculating whether or not each one is inside or outside of the glyph, and averaging all of those values for the calculated value of a given pixel. This allows for pixels to exist that are *partially* inside or outside of the glyph, and whose pixel values indicate as such, which is the opposite of [bi-level rasterization](#bi-level).

				// @DOCLINE The amount of samples per pixel in the x- and y-direction is controlled by its dimensions, splitting up the pixel into multiple sub-pixels to then be individually calculated. For example, two-by-two anti-aliasing implies taking two samples on the x- and y-axis per pixel, so the top-leftest pixel (coordinates (0.5, 0.5)) would be split up into coordinates (0.25, 0.25), (0.75, 0.25), (0.25, 0.75), and (0.75, 0.75), in no particular order, and individually calculated & averaged for the final pixel value.

			// @DOCLINE ### Raster bleeding

				// @DOCLINE Some rasterization methods have a possibility of setting pixels as (at least partially) inside of the glyph that aren't mathematically inside of the glyph to any degree, but are one pixel away from another pixel that *is* (at least partially) inside of the glyph. This effect is called "bleeding", and can cause pixels to be inside of the glyph that are outside of the range of the glyph's coordinates.

				// @DOCLINE In terms of rglyphs, this is prevented by offsetting the coordinates of each point by 1 pixel, ensuring that there is at least a single pixel to the left and top and of any pixel that is mathematically inside of the glyph, and thus can catch any theoretical bleeding. This is automatically performed when converting simple and composite glyphs from the low-level API to an rglyph, and should be done by the user when creating/modifying rglyphs.

				// @DOCLINE In terms of rasterization, this is prevented by increasing the width and height of the bitmap to be 1 pixel greater than the maximum x- and y-coordinates within the glyph (`glyph->x_max` and `glyph->y_max`). The conversion from the decimal values of `x_max` and `y_max` to an integer width and height should be performed via a ceiling of the final result.

		// @DOCLINE ## Rasterization of TrueType glyphs

			// @DOCLINE The raster API gives access to rasterizing TrueType glyphs by converting them to an rglyph, which can then be [rasterized directly](#rasterize-glyph). This conversion can be done rather by the user directly [giving a simple glyph](#simple-glyph-to-rglyph), [giving a composite glyph](#composite-glyph-to-rglyph), or by [giving the header of a simple or composite glyph](#glyph-header-to-rglyph).

			// @DOCLINE This conversion can also automatically be performed internally via [rasterizing the glyph based on a given glyph ID](#rasterize-glyph-id), handling all of the allocation and conversions. This can be inefficient to call on large groups of glyphs, as new memory has to be repeatedly allocated and deallocated.

			// @DOCLINE ### Font units to pixel units

				// @DOCLINE The rasterization of any TrueType glyph involves converting the Truetype "font units" (FUnits) to pixel units (which is what a raster glyph uses). This conversion requires a [point size](https://en.wikipedia.org/wiki/Point_(typography)) and the [pixels per inch](https://en.wikipedia.org/wiki/Pixel_density), or PPI, of the display (usually 72 or 96). These two variables allow the coordinates of an rglyph to be rasterized at a predictable and calculatable physical size when displayed.

				// @DOCLINE The function `mutt_funits_to_punits` performs the conversion described above for a given font based on its unitsPerEm value (stored in the head table), defined below: @NLNT
				MUDEF float mutt_funits_to_punits(muttFont* font, float funits, float point_size, float ppi);

				// @DOCLINE Although the font unit range in TrueType can be expressed with a signed 16-bit integer, `funits` is a `float` for the sake of being able to perform the conversion on transformed coordinates in composite glyphs, which can result in decimal numbers.

			// @DOCLINE ### Simple glyph to rglyph

				// @DOCLINE The function `mutt_simple_rglyph` converts a simple glyph to an rglyph, defined below: @NLNT
				MUDEF muttResult mutt_simple_rglyph(muttFont* font, muttGlyphHeader* header, muttSimpleGlyph* glyph, muttRGlyph* rglyph, float point_size, float ppi, muByte* data, uint32_m* written);

				// @DOCLINE Upon a non-fatal result, `rglyph` is filled with valid raster glyph information for the given simple glyph using memory from `data`. Upon a fatal result, the contents of `rglyph` and `data` are undefined. The given rglyph information is only valid for as long as `font` is not deloaded, and as long as `data` goes unmodified.

				// @DOCLINE The given simple glyph must have at least one contour, and that one contour must have points. The simple glyph given must be valid.

				// @DOCLINE This function follows the format of a user-allocated function. For an explanation of how `data` and `written` are supposed to be used within this function, see [the user-allocated function section](#user-allocated-functions).

				// @DOCLINE #### Simple glyph to rglyph memory maximum

					// @DOCLINE The maximum amount of memory that will be needed for converting a simple glyph to a raster glyph for a given font, in bytes, is provided by the function `mutt_simple_rglyph_max`, defined below: @NLNT
					MUDEF uint32_m mutt_simple_rglyph_max(muttFont* font);

			// @DOCLINE ### Composite glyph to rglyph

				// @DOCLINE The function `mutt_composite_rglyph` converts a composite glyph to an rglyph, defined below: @NLNT
				MUDEF muttResult mutt_composite_rglyph(muttFont* font, muttGlyphHeader* header, muttCompositeGlyph* glyph, muttRGlyph* rglyph, float point_size, float ppi, muByte* data, uint32_m* written);

				// @DOCLINE Upon a non-fatal result, `rglyph` is filled with valid raster glyph information for the given composite glyph using memory from `data`. Upon a fatal result, the contents of `rglyph` and `data` are undefined. The given rglyph information is only valid for as long as `font` is not deloaded, and as long as `data` goes unmodified.

				// @DOCLINE The given composite glyph must have at least one contour, and that one contour must have points. The composite glyph given must be valid.

				// @DOCLINE This function follows the format of a user-allocated function. For an explanation of how `data` and `written` are supposed to be used within this function, see [the user-allocated function section](#user-allocated-functions).

				// @DOCLINE #### Composite glyph to rglyph memory maximum

					// @DOCLINE The maximum amount of memory that will be needed for converting a composite glyph to a raster glyph for a given font, in bytes, is provided by the function `mutt_composite_rglyph_max`, defined below: @NLNT
					MUDEF uint32_m mutt_composite_rglyph_max(muttFont* font);

			// @DOCLINE ### Glyph header to rglyph

				// @DOCLINE The function `mutt_header_rglyph` converts a glyph header to a glyph, defined below: @NLNT
				MUDEF muttResult mutt_header_rglyph(muttFont* font, muttGlyphHeader* header, muttRGlyph* rglyph, float point_size, float ppi, muByte* data, uint32_m* written);

				// @DOCLINE Upon a non-fatal result, `rglyph` is filled with valid raster glyph information for the given glyph based on its header, using memory from `data`. Upon a fatal result, the contents of `rglyph` and `data` are undefined. The given rglyph information is only valid for as long as `font` is not deloaded, and as long as `data` goes unmodified.

				// @DOCLINE The given glyph must have at least one contour, and that one contour must have points. The glyph header given must be valid.

				// @DOCLINE This function follows the format of a user-allocated function. For an explanation of how `data` and `written` are supposed to be used within this function, see [the user-allocated function section](#user-allocated-functions).

				// @DOCLINE #### Glyph header to rglyph memory maximum

					// @DOCLINE The maximum amount of memory that will be needed for converting a glyph header to a raster glyph for a given font, in bytes, is provided by the function `mutt_header_rglyph_max`, defined below: @NLNT
					MUDEF uint32_m mutt_header_rglyph_max(muttFont* font);

					// @DOCLINE This function rather returns (the sum of `mutt_simple_glyph_max_size` and `mutt_simple_rglyph_max`) or (the sum of `mutt_composite_glyph_max_size` and `mutt_composite_rglyph_max`), whichever is greater. All the table loading requirements of these functions apply.

	// @DOCLINE # Result

		// @DOCLINE The type `muttResult` (typedef for `uint32_m`) is defined to represent how a task went. Result values can be "fatal" (meaning that the task completely failed to execute, and the program will continue as if the task had never been attempted), "non-fatal" (meaning that the task partially failed, but was still able to complete the task), and "successful" (meaning that the task fully succeeded).

		// @DOCLINE ## Result values

		// @DOCLINE The following values are defined for `muttResult` (all values not explicitly stated as being fatal, non-fatal, or successful are assumed to be fatal):

		// @DOCLINE ### General result values
		// 0 -> 63 //

			// @DOCLINE * `MUTT_SUCCESS` - the task was successfully completed; real value 0.
			#define MUTT_SUCCESS 0
			// @DOCLINE * `MUTT_FAILED_MALLOC` - a call to malloc failed, implying insufficient available memory to perform the task.
			#define MUTT_FAILED_MALLOC 1
			// @DCOLINE * `MUTT_FAILED_REALLOC` - a call to realloc failed, implying insufficient available memory to perform the task.
			#define MUTT_FAILED_REALLOC 2
			// @DOCLINE * `MUTT_FAILED_FIND_TABLE` - the table could not be located, and is likely not included in the font file.
			#define MUTT_FAILED_FIND_TABLE 3

		// @DOCLINE ### Directory result values
		// 64 -> 127 //

			// @DOCLINE * `MUTT_INVALID_DIRECTORY_LENGTH` - the length of the table directory was invalid. This is the first check performed on the length of the font file data, meaning that if this result is given, it is likely that the data given is not font file data.
			#define MUTT_INVALID_DIRECTORY_LENGTH 64
			// @DOCLINE * `MUTT_INVALID_DIRECTORY_SFNT_VERSION` - the value of "sfntVersion" in the table directory was invalid/unsupported. This is the first check performed on the values within the font file data, meaning that if this result is given, it is likely that the data given is not TrueType font file data. This can also be triggered by the TrueType font using CFF data, which is currently unsupported.
			#define MUTT_INVALID_DIRECTORY_SFNT_VERSION 65
			// @DOCLINE * `MUTT_INVALID_DIRECTORY_NUM_TABLES` - the value of "numTables" in the table directory was invalid; the number of tables must be at least 9 to store all tables required in TrueType.
			#define MUTT_INVALID_DIRECTORY_NUM_TABLES 66
			// @DOCLINE * `MUTT_INVALID_DIRECTORY_RECORD_OFFSET` - the value of "offset" in a table record within the table directory was out of range.
			#define MUTT_INVALID_DIRECTORY_RECORD_OFFSET 67
			// @DOCLINE * `MUTT_INVALID_DIRECTORY_RECORD_LENGTH` - the value of "length" in a table record within the table directory was out of range.
			#define MUTT_INVALID_DIRECTORY_RECORD_LENGTH 68
			// @DOCLINE * `MUTT_INVALID_DIRECTORY_RECORD_CHECKSUM` - the value of "checksum" in a table record within the table directory was invalid, implying that the table data was incorrect.
			#define MUTT_INVALID_DIRECTORY_RECORD_CHECKSUM 69
			// @DOCLINE * `MUTT_INVALID_DIRECTORY_RECORD_TABLE_TAG` - the table tag of a table within the table directory was a duplicate of a previous one.
			#define MUTT_INVALID_DIRECTORY_RECORD_TABLE_TAG 70
			// @DOCLINE * `MUTT_MISSING_DIRECTORY_RECORD_TABLE_TAGS` - one or more tables required by TrueType standards could not be found in the table directory.
			#define MUTT_MISSING_DIRECTORY_RECORD_TABLE_TAGS 71

		// @DOCLINE ### Maxp result values
		// 128 -> 191 //

			// @DOCLINE * `MUTT_INVALID_MAXP_LENGTH` - the length of the maxp table was invalid.
			#define MUTT_INVALID_MAXP_LENGTH 128
			// @DOCLINE * `MUTT_INVALID_MAXP_VERSION` - the version of the maxp table given was invalid/unsupported. This likely means that the font has a CFF/CFF2 font outline, which is currently unsupported.
			#define MUTT_INVALID_MAXP_VERSION 129
			// @DOCLINE * `MUTT_INVALID_MAXP_NUM_GLYPHS` - the value for "numGlyphs" given in the maxp table was invalid.
			#define MUTT_INVALID_MAXP_NUM_GLYPHS 130
			// @DOCLINE * `MUTT_INVALID_MAXP_MAX_ZONES` - the value for "maxZones" given in the maxp table was invalid.
			#define MUTT_INVALID_MAXP_MAX_ZONES 131

		// @DOCLINE ### Head result values
		// 192 -> 255 //

			// @DOCLINE * `MUTT_INVALID_HEAD_LENGTH` - the length of the head table was invalid.
			#define MUTT_INVALID_HEAD_LENGTH 192
			// @DOCLINE * `MUTT_INVALID_HEAD_VERSION` - the version indicated for the head table was invalid/unsupported.
			#define MUTT_INVALID_HEAD_VERSION 193
			// @DOCLINE * `MUTT_INVALID_HEAD_MAGIC_NUMBER` - the value for the magic number in the head table was invalid.
			#define MUTT_INVALID_HEAD_MAGIC_NUMBER 194
			// @DOCLINE * `MUTT_INVALID_HEAD_UNITS_PER_EM` - the value for the units per em in the head table was not within the correct range of 16 to 16384.
			#define MUTT_INVALID_HEAD_UNITS_PER_EM 195
			// @DOCLINE * `MUTT_INVALID_HEAD_X_MIN_COORDINATES` - the value "xMin" in the head table was not within the valid TrueType grid coordinate range of -16384 to 16383.
			#define MUTT_INVALID_HEAD_X_MIN_COORDINATES 196
			// @DOCLINE * `MUTT_INVALID_HEAD_Y_MIN_COORDINATES` - the value "yMin" in the head table was not within the valid TrueType grid coordinate range of -16384 to 16383.
			#define MUTT_INVALID_HEAD_Y_MIN_COORDINATES 197
			// @DOCLINE * `MUTT_INVALID_HEAD_X_MAX_COORDINATES` - the value "xMax" in the head table was not within the valid TrueType grid coordinate range of -16384 to 16383.
			#define MUTT_INVALID_HEAD_X_MAX_COORDINATES 198
			// @DOCLINE * `MUTT_INVALID_HEAD_Y_MAX_COORDINATES` - the value "yMax" in the head table was not within the valid TrueType grid coordinate range of -16384 to 16383.
			#define MUTT_INVALID_HEAD_Y_MAX_COORDINATES 199
			// @DOCLINE * `MUTT_INVALID_HEAD_X_MIN_MAX` - the value established minimum x-value within the head table was greater than the established maximum x-value.
			#define MUTT_INVALID_HEAD_X_MIN_MAX 200
			// @DOCLINE * `MUTT_INVALID_HEAD_Y_MIN_MAX` - the value established minimum y-value within the head table was greater than the established maximum y-value.
			#define MUTT_INVALID_HEAD_Y_MIN_MAX 201
			// @DOCLINE * `MUTT_INVALID_HEAD_INDEX_TO_LOC_FORMAT` - the value for "indexToLocFormat" within the head table was invalid/unsupported; it was not one of the expected values 0 (Offset16) or 1 (Offset32).
			#define MUTT_INVALID_HEAD_INDEX_TO_LOC_FORMAT 202
			// @DOCLINE * `MUTT_INVALID_HEAD_GLYPH_DATA_FORMAT` - the value for "glyphDataFormat" within the head table was invalid/unsupported; it was not the expected value 0.
			#define MUTT_INVALID_HEAD_GLYPH_DATA_FORMAT 203

		// @DOCLINE ### Hhea result values
		// 256 -> 319 //

			// @DOCLINE * `MUTT_INVALID_HHEA_LENGTH` - the length of the hhea table was invalid.
			#define MUTT_INVALID_HHEA_LENGTH 256
			// @DOCLINE * `MUTT_INVALID_HHEA_VERSION` - the version indicated for the hhea table was invalid/unsupported.
			#define MUTT_INVALID_HHEA_VERSION 257
			// @DOCLINE * `MUTT_INVALID_HHEA_METRIC_DATA_FORMAT` - the value for "metricDataFormat" within the hhea table was invalid/unsupported; it was not the expected value 0.
			#define MUTT_INVALID_HHEA_METRIC_DATA_FORMAT 258
			// @DOCLINE * `MUTT_INVALID_HHEA_NUMBER_OF_HMETRICS` - the value for "numberOfHMetrics" within the hhea table was invalid; numberOfHMetrics must be less than or equal to "numGlyphs" in order to generate a valid array length for "leftSideBearings" within hmtx.
			#define MUTT_INVALID_HHEA_NUMBER_OF_HMETRICS 259
			// @DOCLINE * `MUTT_HHEA_REQUIRES_MAXP` - the maxp table rather failed to load or was not requested for loading, and hhea requires maxp to be loaded.
			#define MUTT_HHEA_REQUIRES_MAXP 260

		// @DOCLINE ### Hmtx result values
		// 320 -> 383 //

			// @DOCLINE * `MUTT_INVALID_HMTX_LENGTH` - the length of the hmtx table was invalid.
			#define MUTT_INVALID_HMTX_LENGTH 320
			// @DOCLINE * `MUTT_HMTX_REQUIRES_MAXP` - the maxp table rather failed to load or was not requested for loading, and hmtx requires maxp to be loaded.
			#define MUTT_HMTX_REQUIRES_MAXP 321
			// @DOCLINE * `MUTT_HMTX_REQUIRES_HHEA` - the hhea table rather failed to load or was not requested for loading, and hmtx requires hhea to be loaded.
			#define MUTT_HMTX_REQUIRES_HHEA 322

		// @DOCLINE ### Loca result values
		// 384 -> 447 //

			// @DOCLINE * `MUTT_INVALID_LOCA_LENGTH` - the length of the loca table was invalid.
			#define MUTT_INVALID_LOCA_LENGTH 284
			// @DOCLINE * `MUTT_INVALID_LOCA_OFFSET` - an offset in the loca table was invalid. This could mean that the offset's range was invalid for the glyf table, or that the rule of incremental offsets was violated.
			#define MUTT_INVALID_LOCA_OFFSET 285
			// @DOCLINE * `MUTT_LOCA_REQUIRES_MAXP` - the maxp table rather failed to load or was not requested for loading, and loca requires maxp to be loaded.
			#define MUTT_LOCA_REQUIRES_MAXP 286
			// @DOCLINE * `MUTT_LOCA_REQUIRES_HEAD` - the head table rather failed to load or was not requested for loading, and loca requires head to be loaded.
			#define MUTT_LOCA_REQUIRES_HEAD 287
			// @DOCLINE * `MUTT_LOCA_REQUIRES_GLYF` - the glyf table rather failed to load or was not requested for loading, and loca requires glyf to be loaded.
			#define MUTT_LOCA_REQUIRES_GLYF 288

		// @DOCLINE ### Name result values
		// 448 -> 511 //

			// @DOCLINE * `MUTT_INVALID_NAME_LENGTH` - the length of the name table was invalid.
			#define MUTT_INVALID_NAME_LENGTH 448
			// @DOCLINE * `MUTT_INVALID_NAME_VERSION` - the version of the name table was invalid/unsupported.
			#define MUTT_INVALID_NAME_VERSION 449
			// @DOCLINE * `MUTT_INVALID_NAME_STORAGE_OFFSET` - the offset given for the storage of string data within the name table was invalid / out of range.
			#define MUTT_INVALID_NAME_STORAGE_OFFSET 450
			// @DOCLINE * `MUTT_INVALID_NAME_LENGTH_OFFSET` - the length and offset given for a name record within the name table was invalid / out of range.
			#define MUTT_INVALID_NAME_LENGTH_OFFSET 451

		// @DOCLINE ### Glyf result values
		// 512 -> 575 //

			// @DOCLINE * `MUTT_INVALID_GLYF_HEADER_LENGTH` - the glyph header length given from values by the loca table were invalid; they were above 0, implying an outline, yet the length given was insufficient to store a glyph header.
			#define MUTT_INVALID_GLYF_HEADER_LENGTH 512
			// @DOCLINE * `MUTT_INVALID_GLYF_HEADER_NUMBER_OF_CONTOURS` - the number of contours within the glyph header exceeded the maximum set by the maxp table.
			#define MUTT_INVALID_GLYF_HEADER_NUMBER_OF_CONTOURS 513
			// @DOCLINE * `MUTT_INVALID_GLYF_HEADER_X_MIN` - the glyph header's xMin value was not in range of the head table's listed corresponding value.
			#define MUTT_INVALID_GLYF_HEADER_X_MIN 514
			// @DOCLINE * `MUTT_INVALID_GLYF_HEADER_Y_MIN` - the glyph header's yMin value was not in range of the head table's listed corresponding value.
			#define MUTT_INVALID_GLYF_HEADER_Y_MIN 515
			// @DOCLINE * `MUTT_INVALID_GLYF_HEADER_X_MAX` - the glyph header's xMax value was not in range of the head table's listed corresponding value.
			#define MUTT_INVALID_GLYF_HEADER_X_MAX 516
			// @DOCLINE * `MUTT_INVALID_GLYF_HEADER_Y_MAX` - the glyph header's yMax value was not in range of the head table's listed corresponding value.
			#define MUTT_INVALID_GLYF_HEADER_Y_MAX 517
			// @DOCLINE * `MUTT_INVALID_GLYF_HEADER_X_MIN_MAX` - the glyph header's xMin value was greater than its xMax value or vice versa, which does not make sense.
			#define MUTT_INVALID_GLYF_HEADER_X_MIN_MAX 518
			// @DOCLINE * `MUTT_INVALID_GLYF_HEADER_Y_MIN_MAX` - the glyph header's yMin value was greater than its yMax value or vice versa, which does not make sense.
			#define MUTT_INVALID_GLYF_HEADER_Y_MIN_MAX 519

			// @DOCLINE * `MUTT_INVALID_GLYF_SIMPLE_LENGTH` - the length of the simple glyph description is invalid/insufficient to describe the simple glyph.
			#define MUTT_INVALID_GLYF_SIMPLE_LENGTH 520
			// @DOCLINE * `MUTT_INVALID_GLYF_SIMPLE_END_PTS_OF_CONTOURS` - a value within the endPtsOfContours array of the simple glyph was invalid; rather the value was non-incremental, or the last index was the invalid value 0xFFFF.
			#define MUTT_INVALID_GLYF_SIMPLE_END_PTS_OF_CONTOURS 521
			// @DOCLINE * `MUTT_INVALID_GLYF_SIMPLE_POINT_COUNT` - the amount of points specified within the simple glyph exceeded the maximum set by the maxp table.
			#define MUTT_INVALID_GLYF_SIMPLE_POINT_COUNT 522
			// @DOCLINE * `MUTT_INVALID_GLYF_SIMPLE_INSTRUCTION_LENGTH` - the instruction length given by the simple glyph exceeded the maximum set by the maxp table.
			#define MUTT_INVALID_GLYF_SIMPLE_INSTRUCTION_LENGTH 523
			// @DOCLINE * `MUTT_INVALID_GLYF_SIMPLE_X_COORD` - an x-coordinate within the simple glyph was out of range for its minimum/maximum values.
			#define MUTT_INVALID_GLYF_SIMPLE_X_COORD 524
			// @DOCLINE * `MUTT_INVALID_GLYF_SIMPLE_Y_COORD` - a y-coordinate within the simple glyph was out of range for its minimum/maximum values.
			#define MUTT_INVALID_GLYF_SIMPLE_Y_COORD 525

			// @DOCLINE * `MUTT_INVALID_GLYF_COMPOSITE_LENGTH` - the length of the composite glyph description is invalid/insufficient to describe the composite glyph.
			#define MUTT_INVALID_GLYF_COMPOSITE_LENGTH 526
			// @DOCLINE * `MUTT_INVALID_GLYF_COMPOSITE_INSTRUCTION_LENGTH` - the instruction length given by the composite glyph exceeded the maximum set by the maxp table.
			#define MUTT_INVALID_GLYF_COMPOSITE_INSTRUCTION_LENGTH 527
			// @DOCLINE * `MUTT_INVALID_GLYF_COMPOSITE_COMPONENT_COUNT` - the amount of components given in the composite glyph exceeded the maximum set by the maxp table.
			#define MUTT_INVALID_GLYF_COMPOSITE_COMPONENT_COUNT 528
			// @DOCLINE * `MUTT_INVALID_GLYF_COMPOSITE_GLYPH_INDEX` - the value for "glyphIndex" in a component within the composite glyph was an invalid glyph index (out of range for the number of glyphs specified in maxp).
			#define MUTT_INVALID_GLYF_COMPOSITE_GLYPH_INDEX 529
			// @DOCLINE * `MUTT_INVALID_GLYF_COMPOSITE_FLAGS` - the flags in a component within the composite glyph were invalid (multiple mutually exclusive transform data flags were set).
			#define MUTT_INVALID_GLYF_COMPOSITE_FLAGS 530

		// @DOCLINE ### Cmap result values
		// 576 -> 639 //

			// @DOCLINE * `MUTT_INVALID_CMAP_LENGTH` - the length of the cmap table was invalid.
			#define MUTT_INVALID_CMAP_LENGTH 576
			// @DOCLINE * `MUTT_INVALID_CMAP_VERSION` - the version of the cmap table was invalid/unsupported.
			#define MUTT_INVALID_CMAP_VERSION 577

			// @DOCLINE * `MUTT_INVALID_CMAP_ENCODING_RECORD_OFFSET` - an encoding record's subtable offset was invalid (AKA out of range for the cmap table).
			#define MUTT_INVALID_CMAP_ENCODING_RECORD_OFFSET 578
			// @DOCLINE * `MUTT_INVALID_CMAP_ENCODING_RECORD_LENGTH` - an encoding record's subtable length was invalid (AKA the offset's distance from the end of the cmap table was not long enough to figure out its format).
			#define MUTT_INVALID_CMAP_ENCODING_RECORD_LENGTH 579
			// @DOCLINE * `MUTT_INVALID_CMAP_ENCODING_RECORD_FORMAT` - the encoding record's format was invalid/unsupported.
			#define MUTT_INVALID_CMAP_ENCODING_RECORD_FORMAT 580

			// @DOCLINE * `MUTT_INVALID_CMAP0_LENGTH` - the length of the cmap format 0 subtable was invalid.
			#define MUTT_INVALID_CMAP0_LENGTH 581

			// @DOCLINE * `MUTT_INVALID_CMAP4_LENGTH` - the length of the cmap format 4 subtable was invalid/insufficient to define the data needed.
			#define MUTT_INVALID_CMAP4_LENGTH 582
			// @DOCLINE * `MUTT_INVALID_CMAP4_SEG_COUNT_X2` - the value given for "segCountX2" in the cmap format 4 subtable was not divisible by 2, and was therefore an invalid value.
			#define MUTT_INVALID_CMAP4_SEG_COUNT_X2 583
			// @DOCLINE * `MUTT_INVALID_CMAP4_END_CODE` - an "endCode" value for a segment in the cmap format 4 subtable broke the incremental order of the endCode values.
			#define MUTT_INVALID_CMAP4_END_CODE 584
			// @DOCLINE * `MUTT_INVALID_CMAP4_LAST_END_CODE` - the last "endCode" value in the cmap format 4 subtable was not 0xFFFF, which is required in the TrueType specification.
			#define MUTT_INVALID_CMAP4_LAST_END_CODE 585
			// @DOCLINE * `MUTT_INVALID_CMAP4_START_CODE` - a "startCode" value for a segment in the cmap format 4 subtable was greater than its endCode.
			#define MUTT_INVALID_CMAP4_START_CODE 586
			// @DOCLINE * `MUTT_INVALID_CMAP4_ID_RANGE_OFFSET` - an "idRangeOffset" value for a segment in the cmap format 4 subtable was rather out of range for indexes into glyphIdArray, or was not divisible by 2 (which it must be, since it's a byte-offset into a 2-byte-integer array, starting from a 2-byte-integer array).
			#define MUTT_INVALID_CMAP4_ID_RANGE_OFFSET 587

			// @DOCLINE * `MUTT_INVALID_CMAP12_LENGTH` - the length of the cmap format 12 subtable was invalid/in sufficient to define the data needed.
			#define MUTT_INVALID_CMAP12_LENGTH 588
			// @DOCLINE * `MUTT_INVALID_CMAP12_START_CHAR_CODE` - a "startCharCode" value for a map group in the cmap format 12 subtable was not incremental compared to the previous group.
			#define MUTT_INVALID_CMAP12_START_CHAR_CODE 589
			// @DOCLINE * `MUTT_INVALID_CMAP12_END_CHAR_CODE` - an "endCharCode" value for a map group in the cmap format 12 subtable was not less than the startCharCode value of the next group.
			#define MUTT_INVALID_CMAP12_END_CHAR_CODE 590

			// @DOCLINE * `MUTT_CMAP_REQUIRES_MAXP` - the maxp table rather failed to load or was not requested for loading, and cmap requires maxp to be loaded.
			#define MUTT_CMAP_REQUIRES_MAXP 639

		// @DOCLINE ### Rasterization result values
		// 640 -> 703 //

			// @DOCLINE * `MUTT_UNKNOWN_RASTER_METHOD` - the given raster method value was unrecognized.
			#define MUTT_UNKNOWN_RASTER_METHOD 640

		// @DOCLINE ## Check if result is fatal

			// @DOCLINE The function `mutt_result_is_fatal` returns whether or not a given `muttResult` value is fatal, defined below: @NLNT
			MUDEF muBool mutt_result_is_fatal(muttResult result);

			// @DOCLINE This function returns `MU_TRUE` if the value of `result` is invalid.

		// @DOCLINE ## Result name

			#ifdef MUTT_NAMES

			// @DOCLINE The function `mutt_result_get_name` returns a `const char*` representation of a given result value (for example, `MUTT_SUCCESS` returns "MUTT_SUCCESS"), defined below: @NLNT
			MUDEF const char* mutt_result_get_name(muttResult result);

			// @DOCLINE This function returns "MU_UNKNOWN" in the case that `result` is an invalid result value.

			// @DOCLINE > This function is a "name" function, and therefore is only defined if `MUTT_NAMES` is also defined.

			#endif

	// @DOCLINE # C standard library dependencies

		// @DOCLINE mutt has several C standard library dependencies, all of which are overridable by defining them before the inclusion of its header. The following is a list of those dependencies.

		#if !defined(mu_malloc) || \
			!defined(mu_free) || \
			!defined(mu_realloc) || \
			!defined(mu_qsort)

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

			// @DOCLINE * `mu_qsort` equivalent to `qsort`.
			#ifndef mu_qsort
				#define mu_qsort qsort
			#endif

		#endif /* stdlib.h */

		#if !defined(mu_memcpy) || \
			!defined(mu_memset)

			// @DOCLINE ## `string.h` dependencies
			#include <string.h>

			// @DOCLINE * `mu_memcpy` - equivalent to `memcpy`.
			#ifndef mu_memcpy
				#define mu_memcpy memcpy
			#endif

			// @DOCLINE * `mu_memset`- equivalent to `memset`.
			#ifndef mu_memset
				#define mu_memset memset
			#endif

		#endif /* string.h */

		#if !defined(mu_fabsf)

			// @DOCLINE ## `math.h` dependencies
			#include <math.h>

			// @DOCLINE * `mu_fabsf` - equivalent to `fabsf`.
			#ifndef mu_fabsf
				#define mu_fabsf fabsf
			#endif

		#endif /* math.h */

	MU_CPP_EXTERN_END
#endif /* MUTT_H */

#ifdef MUTT_IMPLEMENTATION
	MU_CPP_EXTERN_START

	/* Lower-level API */

		/* Checksum logic */

			// Verfifies the checksum of a given table
			muBool mutt_VerifyTableChecksum(muByte* table, uint32_m length, uint32_m checksum) {
				// Initial vars
				uint32_m current_checksum = 0;
				uint32_m off = length%4;

				// Calculate end of table
				muByte* end = &table[length];

				// Go through each u32 in the table data
				while (table < end) {
					// Handle if we're cutting into the end of the table unevenly:
					if (off != 0 && (table+4) >= end) {
						// 16-bit
						if (off == 2) {
							current_checksum += MU_RBEU16(table) << 16;
						}
						// 24-bit
						else if (off == 3) {
							current_checksum += MU_RBEU24(table) << 8;
						}
						// 8-bit
						else {
							current_checksum += table[0] << 24;
						}
					}
					// Normal cut
					else {
						current_checksum += MU_RBEU32(table);
					}

					// Go to next u32
					table += 4;
				}

				// Verify that checksum matches
				if (checksum != current_checksum) {
					return MU_FALSE;
				}
				return MU_TRUE;
			}

		/* Table directory */

			// Loads the table directory
			// Note: if fails, still call mutt_DeloadTableDirectory
			muttResult mutt_LoadTableDirectory(muttDirectory* dir, muByte* data, uint64_m datalen) {
				muByte* orig_data = data;

				// Verify min. length
				if (datalen < 12) {
					return MUTT_INVALID_DIRECTORY_LENGTH;
				}

				// Read & verify sfntVersion
				if (MU_RBEU32(data) != 0x00010000) {
					return MUTT_INVALID_DIRECTORY_SFNT_VERSION;
				}

				// Read & verify numTables
				dir->num_tables = MU_RBEU16(data+4);
				// - Must be at least 9 for 9 required tables
				if (dir->num_tables < 9) {
					return MUTT_INVALID_DIRECTORY_NUM_TABLES;
				}

				// Verify length based on numTables
				if (datalen < (uint64_m)(12+(dir->num_tables*16))) {
					return MUTT_INVALID_DIRECTORY_LENGTH;
				}

				// Allocate table records
				dir->records = (muttTableRecord*)mu_malloc(sizeof(muttTableRecord)*dir->num_tables);
				if (!dir->records) {
					return MUTT_FAILED_MALLOC;
				}

				// Set up flag for noting which tables we found, which will be used
				// for ensuring that all required tables were found
				muttLoadFlags load_flags = 0;

				// Loop through each table record:
				data += 12;
				muttTableRecord* rec_end = dir->records+dir->num_tables;
				for (muttTableRecord* rec = dir->records; rec < rec_end; ++rec) {
					// Read tableTag
					// - table_tag_u8
					mu_memcpy(rec->table_tag_u8, data, 4);
					// - table_tag_u32
					rec->table_tag_u32 = MU_RBEU32(data);

					// Set load flag based on tag
					switch (rec->table_tag_u32) {
						default: break;
						case 0x6D617870: load_flags |= MUTT_LOAD_MAXP; break;
						case 0x68656164: load_flags |= MUTT_LOAD_HEAD; break;
						case 0x68686561: load_flags |= MUTT_LOAD_HHEA; break;
						case 0x686D7478: load_flags |= MUTT_LOAD_HMTX; break;
						case 0x6C6F6361: load_flags |= MUTT_LOAD_LOCA; break;
						case 0x706F7374: load_flags |= MUTT_LOAD_POST; break;
						case 0x6E616D65: load_flags |= MUTT_LOAD_NAME; break;
						case 0x676C7966: load_flags |= MUTT_LOAD_GLYF; break;
						case 0x636D6170: load_flags |= MUTT_LOAD_CMAP; break;
					}

					// Make sure this table tag is not a duplicate
					for (muttTableRecord* c = dir->records; c < rec; ++c) {
						if (rec->table_tag_u32 == c->table_tag_u32) {
							return MUTT_INVALID_DIRECTORY_RECORD_TABLE_TAG;
						}
					}

					// Read checksum
					rec->checksum = MU_RBEU32(data+4);

					// Read & verify offset
					rec->offset = MU_RBEU32(data+8);
					// - Make sure offset is in range
					if (rec->offset >= datalen) {
						return MUTT_INVALID_DIRECTORY_RECORD_OFFSET;
					}

					// Read & verify length
					rec->length = MU_RBEU32(data+12);
					// - Make sure length+offset is in range
					if (rec->offset+rec->length > datalen) {
						return MUTT_INVALID_DIRECTORY_RECORD_LENGTH;
					}

					// Verify checksum
					// Note: we're not performing this for head, its checksum HAS a checksum in it so it's weird
					if (rec->table_tag_u32 != 0x68656164 &&
						!mutt_VerifyTableChecksum(&orig_data[rec->offset], rec->length, rec->checksum)
					) {
						return MUTT_INVALID_DIRECTORY_RECORD_CHECKSUM;
					}

					// Increment data
					data += 16;
				}

				// Check that all required tables were loaded
				if ((load_flags & MUTT_LOAD_REQUIRED) != MUTT_LOAD_REQUIRED) {
					return MUTT_MISSING_DIRECTORY_RECORD_TABLE_TAGS;
				}

				return MUTT_SUCCESS;
			}

			// Deloads the table directory
			void mutt_DeloadTableDirectory(muttDirectory* dir) {
				// Free table records if they exist
				if (dir->records) {
					mu_free(dir->records);
				}
			}

		/* Basic tables */

			// Loads the maxp table
			muttResult mutt_LoadMaxp(muttFont* font, muByte* data, uint32_m datalen) {
				// Allocate maxp
				muttMaxp* maxp = (muttMaxp*)mu_malloc(sizeof(muttMaxp));
				if (!maxp) {
					return MUTT_FAILED_MALLOC;
				}

				// Verify min. length for version
				if (datalen < 4) {
					mu_free(maxp);
					return MUTT_INVALID_MAXP_LENGTH;
				}

				// Version high
				maxp->version_high = MU_RBEU16(data);
				if (maxp->version_high != 0x0001) {
					mu_free(maxp);
					return MUTT_INVALID_MAXP_VERSION;
				}
				// Version low
				maxp->version_low = MU_RBEU16(data+2);
				if (maxp->version_low != 0x0000) {
					mu_free(maxp);
					return MUTT_INVALID_MAXP_VERSION;
				}

				// Verify min. length
				if (datalen < 32) {
					mu_free(maxp);
					return MUTT_INVALID_MAXP_LENGTH;
				}

				// numGlyphs
				maxp->num_glyphs = MU_RBEU16(data+4);
				if (maxp->num_glyphs < 2) {
					mu_free(maxp);
					return MUTT_INVALID_MAXP_NUM_GLYPHS;
				}

				// maxPoints
				maxp->max_points = MU_RBEU16(data+6);
				// maxContours
				maxp->max_contours = MU_RBEU16(data+8);
				// maxCompositePoints
				maxp->max_composite_points = MU_RBEU16(data+10);
				// maxCompositeContours
				maxp->max_composite_contours = MU_RBEU16(data+12);
				// maxZones
				maxp->max_zones = MU_RBEU16(data+14);
				if (maxp->max_zones != 1 && maxp->max_zones != 2) {
					mu_free(maxp);
					return MUTT_INVALID_MAXP_MAX_ZONES;
				}
				// maxTwilightPoints
				maxp->max_twilight_points = MU_RBEU16(data+16);
				// maxStorage
				maxp->max_storage = MU_RBEU16(data+18);
				// maxFunctionDefs
				maxp->max_function_defs = MU_RBEU16(data+20);
				// maxInstructionDefs
				maxp->max_instruction_defs = MU_RBEU16(data+22);
				// maxStackElements
				maxp->max_stack_elements = MU_RBEU16(data+24);
				// maxSizeOfInstructions
				maxp->max_size_of_instructions = MU_RBEU16(data+26);
				// maxComponentElements
				maxp->max_component_elements = MU_RBEU16(data+28);
				// maxComponentDepth
				maxp->max_component_depth = MU_RBEU16(data+30);

				font->maxp = maxp;
				return MUTT_SUCCESS;
			}

			// Loads the head table
			muttResult mutt_LoadHead(muttFont* font, muByte* data, uint32_m datalen) {
				// Allocate head
				muttHead* head = (muttHead*)mu_malloc(sizeof(muttHead));
				if (!head) {
					return MUTT_FAILED_MALLOC;
				}

				// Verify min. length for version
				if (datalen < 4) {
					mu_free(head);
					return MUTT_INVALID_HEAD_LENGTH;
				}

				// Verify version
				if (MU_RBEU16(data) != 1 || MU_RBEU16(data+2) != 0) {
					mu_free(head);
					return MUTT_INVALID_HEAD_VERSION;
				}

				// Verify min. length for rest of table
				if (datalen < 54) {
					mu_free(head);
					return MUTT_INVALID_HEAD_LENGTH;
				}

				// fontRevision high+low
				head->font_revision_high = MU_RBES16(data+4);
				head->font_revision_low  = MU_RBEU16(data+6);
				// checksumAdjustment
				head->checksum_adjustment = MU_RBEU32(data+8);

				// magicNumber
				if (MU_RBEU32(data+12) != 0x5F0F3CF5) {
					mu_free(head);
					return MUTT_INVALID_HEAD_MAGIC_NUMBER;
				}

				// flags
				head->flags = MU_RBEU16(data+16);

				// unitsPerEm
				head->units_per_em = MU_RBEU16(data+18);
				if (head->units_per_em < 16 || head->units_per_em > 16384) {
					mu_free(head);
					return MUTT_INVALID_HEAD_UNITS_PER_EM;
				}

				// created + modified
				head->created = MU_RBES64(data+20);
				head->modified = MU_RBES64(data+28);

				// xMin + yMin
				head->x_min = MU_RBES16(data+36);
				if (head->x_min < -16384 || head->x_min > 16383) {
					mu_free(head);
					return MUTT_INVALID_HEAD_X_MIN_COORDINATES;
				}
				head->y_min = MU_RBES16(data+38);
				if (head->y_min < -16384 || head->y_min > 16383) {
					mu_free(head);
					return MUTT_INVALID_HEAD_Y_MIN_COORDINATES;
				}
				// xMax + yMax
				head->x_max = MU_RBES16(data+40);
				if (head->x_max < -16384 || head->x_max > 16383) {
					mu_free(head);
					return MUTT_INVALID_HEAD_X_MAX_COORDINATES;
				}
				head->y_max = MU_RBES16(data+42);
				if (head->y_max < -16384 || head->y_max > 16383) {
					mu_free(head);
					return MUTT_INVALID_HEAD_Y_MAX_COORDINATES;
				}
				// + Verify min/max
				if (head->x_min > head->x_max) {
					mu_free(head);
					return MUTT_INVALID_HEAD_X_MIN_MAX;
				}
				if (head->y_min > head->y_max) {
					mu_free(head);
					return MUTT_INVALID_HEAD_Y_MIN_MAX;
				}

				// macStyle
				head->mac_style = MU_RBEU16(data+44);
				// lowestRecPPEM
				head->lowest_rec_ppem = MU_RBEU16(data+46);
				// fontDirectionHint
				head->font_direction_hint = MU_RBES16(data+48);
				// indexToLocFormat
				head->index_to_loc_format = MU_RBES16(data+50);
				if (head->index_to_loc_format != 0 && head->index_to_loc_format != 1) {
					mu_free(head);
					return MUTT_INVALID_HEAD_INDEX_TO_LOC_FORMAT;
				}

				// glyphDataFormat
				if (MU_RBES16(data+52) != 0) {
					mu_free(head);
					return MUTT_INVALID_HEAD_GLYPH_DATA_FORMAT;
				}

				font->head = head;
				return MUTT_SUCCESS;
			}

			// Loads the hhea table
			// Req: maxp
			muttResult mutt_LoadHhea(muttFont* font, muByte* data, uint32_m datalen) {
				// Allocate hhea
				muttHhea* hhea = (muttHhea*)mu_malloc(sizeof(muttHhea));
				if (!hhea) {
					return MUTT_FAILED_MALLOC;
				}

				// Verify min. length for version
				if (datalen < 4) {
					mu_free(hhea);
					return MUTT_INVALID_HHEA_LENGTH;
				}

				// Verify version
				if (MU_RBEU16(data) != 1 || MU_RBEU16(data+2) != 0) {
					mu_free(hhea);
					return MUTT_INVALID_HHEA_VERSION;
				}

				// Verify min. length
				if (datalen < 36) {
					mu_free(hhea);
					return MUTT_INVALID_HHEA_LENGTH;
				}

				// ascender + descender
				hhea->ascender = MU_RBES16(data+4);
				hhea->descender = MU_RBES16(data+6);
				// lineGap
				hhea->line_gap = MU_RBES16(data+8);
				// advanceWidthMax
				hhea->advance_width_max = MU_RBEU16(data+10);
				// min(Left/Right)SideBearing
				hhea->min_left_side_bearing = MU_RBES16(data+12);
				hhea->min_right_side_bearing = MU_RBES16(data+14);
				// xMaxExtent
				hhea->x_max_extent = MU_RBES16(data+16);
				// caretSlope(Rise/Run)
				hhea->caret_slope_rise = MU_RBES16(data+18);
				hhea->caret_slope_run = MU_RBES16(data+20);
				// caretOffset
				hhea->caret_offset = MU_RBES16(data+22);

				// metricDataFormat
				if (MU_RBES16(data+32) != 0) {
					mu_free(hhea);
					return MUTT_INVALID_HHEA_METRIC_DATA_FORMAT;
				}

				// numberOfHMetrics
				hhea->number_of_hmetrics = MU_RBEU16(data+34);
				// - numGlyphs-numberOfHMetrics must be valid for leftSideBearings in hmtx
				if (hhea->number_of_hmetrics > font->maxp->num_glyphs) {
					mu_free(hhea);
					return MUTT_INVALID_HHEA_NUMBER_OF_HMETRICS;
				}

				font->hhea = hhea;
				return MUTT_SUCCESS;
			}

		/* Allocated tables */

			// Loads the hmtx table
			// Req: maxp, hhea
			void mutt_DeloadHmtx(muttHmtx* hmtx);
			muttResult mutt_LoadHmtx(muttFont* font, muByte* data, uint32_m datalen) {
				// Verify length
				if (datalen <
					// hMetrics
					(uint64_m)(4*font->hhea->number_of_hmetrics)
					// leftSideBearings
					+(uint64_m)(2*(font->maxp->num_glyphs-font->hhea->number_of_hmetrics))
				) {
					return MUTT_INVALID_HMTX_LENGTH;
				}

				// Allocate
				muttHmtx* hmtx = (muttHmtx*)mu_malloc(sizeof(muttHmtx));
				if (!hmtx) {
					// mutt_DeloadHmtx(hmtx);
					return MUTT_FAILED_MALLOC;
				}
				hmtx->hmetrics = 0;
				hmtx->left_side_bearings = 0;

				// Allocate hMetrics
				if (font->hhea->number_of_hmetrics == 0) {
					hmtx->hmetrics = 0;
				} else {
					hmtx->hmetrics = (muttLongHorMetric*)mu_malloc(sizeof(muttLongHorMetric)*font->hhea->number_of_hmetrics);
					if (!hmtx->hmetrics) {
						mutt_DeloadHmtx(hmtx);
						return MUTT_FAILED_MALLOC;
					}
				}

				// Allocate leftSideBearings
				uint16_m lsb_len = font->maxp->num_glyphs - font->hhea->number_of_hmetrics;
				if (lsb_len == 0) {
					hmtx->left_side_bearings = 0;
				} else {
					hmtx->left_side_bearings = (int16_m*)mu_malloc(lsb_len*2);
					if (!hmtx->left_side_bearings) {
						mutt_DeloadHmtx(hmtx);
						return MUTT_FAILED_MALLOC;
					}
				}

				// Loop through each hMetrics index
				for (uint16_m h = 0; h < font->hhea->number_of_hmetrics; ++h) {
					// advanceWidth
					hmtx->hmetrics[h].advance_width = MU_RBEU16(data);
					// lsb
					hmtx->hmetrics[h].lsb = MU_RBES16(data+2);
					// Increment data
					data += 4;
				}

				// Loop through each leftSideBearings index
				for (uint16_m l = 0; l < lsb_len; ++l) {
					// leftSideBearings[l]
					hmtx->left_side_bearings[l] = MU_RBES16(data);
					// Increment data
					data += 2;
				}

				font->hmtx = hmtx;
				return MUTT_SUCCESS;
			}

			// Deloads the hmtx table
			void mutt_DeloadHmtx(muttHmtx* hmtx) {
				if (hmtx) {
					if (hmtx->hmetrics) {
						mu_free(hmtx->hmetrics);
					}
					if (hmtx->left_side_bearings) {
						mu_free(hmtx->left_side_bearings);
					}
					mu_free(hmtx);
				}
			}

			// Loads the loca table
			// Req: maxp, head, glyf
			void mutt_DeloadLoca(muttLoca* loca);
			muttResult mutt_LoadLoca(muttFont* font, muByte* data, uint32_m datalen) {
				// Allocate
				muttLoca* loca = (muttLoca*)mu_malloc(sizeof(muttLoca));
				if (!loca) {
					// mutt_DeloadLoca(loca);
					return MUTT_FAILED_MALLOC;
				}
				mu_memset(loca, 0, sizeof(muttLoca));

				// Get offsets count
				uint32_m offsets = ((uint32_m)font->maxp->num_glyphs) + 1;

				// Allocate offsets
				// - 16-bit
				if (font->head->index_to_loc_format == MUTT_OFFSET_16) {
					// - Verify 16-bit length
					if (datalen < offsets*2) {
						mutt_DeloadLoca(loca);
						return MUTT_INVALID_LOCA_LENGTH;
					}
					// - Allocate
					loca->offsets16 = (uint16_m*)mu_malloc(offsets*2);
					if (!loca->offsets16) {
						mutt_DeloadLoca(loca);
						return MUTT_FAILED_MALLOC;
					}
				}
				// 32-bit
				else {
					// - Verify 32-bit length
					if (datalen < offsets*4) {
						mutt_DeloadLoca(loca);
						return MUTT_INVALID_LOCA_LENGTH;
					}
					// - Allocate
					loca->offsets32 = (uint32_m*)mu_malloc(offsets*4);
					if (!loca->offsets32) {
						mutt_DeloadLoca(loca);
						return MUTT_FAILED_MALLOC;
					}
				}

				// Loop through each offset
				// - 16-bit
				if (font->head->index_to_loc_format == MUTT_OFFSET_16) {
					for (uint32_m o = 0; o < offsets; ++o) {
						// Read value and go to next
						loca->offsets16[o] = MU_RBEU16(data);
						data += 2;
						// Verify incremental order
						if (o > 0 && loca->offsets16[o-1] > loca->offsets16[o]) {
							mutt_DeloadLoca(loca);
							return MUTT_INVALID_LOCA_OFFSET;
						}
						// Verify offset is within range of glyf
						uint32_m offset = ((uint32_m)loca->offsets16[o]) * 2;
						if (offset > font->glyf->len) {
							mutt_DeloadLoca(loca);
							return MUTT_INVALID_LOCA_OFFSET;
						}
					}
				}
				// - 32-bit
				else {
					for (uint32_m o = 0; o < offsets; ++o) {
						// Read value and go to next
						loca->offsets32[o] = MU_RBEU32(data);
						data += 4;
						// Verify incremental order
						if (o > 0 && loca->offsets32[o-1] > loca->offsets32[o]) {
							mutt_DeloadLoca(loca);
							return MUTT_INVALID_LOCA_OFFSET;
						}
						// Verify offset is within range of glyf
						if (loca->offsets32[o] > font->glyf->len) {
							mutt_DeloadLoca(loca);
							return MUTT_INVALID_LOCA_OFFSET;
						}
					}
				}

				font->loca = loca;
				return MUTT_SUCCESS;
			}

			// Deloads the loca table
			void mutt_DeloadLoca(muttLoca* loca) {
				if (loca) {
					// I THINK this works with 32 as well...
					if (loca->offsets16) {
						mu_free(loca->offsets16);
					}
					mu_free(loca);
				}
			}

			// Loads the name table
			void mutt_DeloadName(muttName* name);
			muttResult mutt_LoadName(muttFont* font, muByte* data, uint32_m datalen) {
				// Allocate
				muttName* name = (muttName*)mu_malloc(sizeof(muttName));
				if (!name) {
					return MUTT_FAILED_MALLOC;
				}
				mu_memset(name, 0, sizeof(muttName));

				// Verify min. length for version
				if (datalen < 2) {
					mutt_DeloadName(name);
					return MUTT_INVALID_NAME_LENGTH;
				}

				// Verify version
				name->version = MU_RBEU16(data);
				if (name->version != 0 && name->version != 1) {
					mutt_DeloadName(name);
					return MUTT_INVALID_NAME_VERSION;
				}

				// Verify min. length for version, count, and storage offset
				if (datalen < 6) {
					mutt_DeloadName(name);
					return MUTT_INVALID_NAME_LENGTH;
				}

				// Read count + storageOffset
				name->count = MU_RBEU16(data+2);
				uint16_m storage_offset = MU_RBEU16(data+4);
				// Verify length for storage offset
				if (storage_offset > datalen) {
					mutt_DeloadName(name);
					return MUTT_INVALID_NAME_STORAGE_OFFSET;
				}
				// Verify length for nameRecord and storage
				uint32_m req = 6+(name->count*12)+(datalen-storage_offset);
				if (datalen < req) {
					mutt_DeloadName(name);
					return MUTT_INVALID_NAME_LENGTH;
				}

				// Allocate and fill storage (if necessary)
				uint32_m storage_len = datalen-storage_offset;
				if (storage_len) {
					name->string_data = (muByte*)mu_malloc(storage_len);
					if (!name->string_data) {
						mutt_DeloadName(name);
						return MUTT_FAILED_MALLOC;
					}
					mu_memcpy(name->string_data, &data[storage_offset], storage_len);
				}

				// Move up data
				data += 6;

				// If we have name records:
				if (name->count) {
					// Allocate name records
					name->name_records = (muttNameRecord*)mu_malloc(sizeof(muttNameRecord)*((size_m)name->count));
					if (!name->name_records) {
						mutt_DeloadName(name);
						return MUTT_FAILED_MALLOC;
					}

					// Loop through each name record
					for (uint16_m r = 0; r < name->count; ++r) {
						muttNameRecord* rp = &name->name_records[r];

						// Basic numbers
						rp->platform_id = MU_RBEU16(data);
						rp->encoding_id = MU_RBEU16(data+2);
						rp->language_id = MU_RBEU16(data+4);
						rp->name_id = MU_RBEU16(data+6);
						rp->length = MU_RBEU16(data+8);

						// Get & verify offset + length
						uint16_m offset = MU_RBEU16(data+10);
						if (offset+rp->length > storage_len) {
							mutt_DeloadName(name);
							return MUTT_INVALID_NAME_LENGTH_OFFSET;
						}

						// Get string
						rp->string = (storage_len) ?(name->string_data+offset) :(0);
						// Increment data
						data += 12;
					}
				}

				// For verson 1:
				if (name->version == 1) {
					// Verify length for langTagCount
					req += 2;
					if (datalen < req) {
						mutt_DeloadName(name);
						return MUTT_INVALID_NAME_LENGTH;
					}
					// Get langTagCount
					name->lang_tag_count = MU_RBEU16(data);
					data += 2;
					// Verify length for langTagRecord
					req += name->lang_tag_count*4;
					if (datalen < req) {
						mutt_DeloadName(name);
						return MUTT_INVALID_NAME_LENGTH;
					}
				}

				// If we have lang tags:
				if (name->lang_tag_count) {
					// Allocate lang tags
					name->lang_tag_records = (muttLangTagRecord*)mu_malloc(sizeof(muttLangTagRecord)*((size_m)name->lang_tag_count));
					if (!name->lang_tag_records) {
						mutt_DeloadName(name);
						return MUTT_FAILED_MALLOC;
					}

					// Loop through each lang tag record
					for (uint16_m l = 0; l < name->lang_tag_count; ++l) {
						muttLangTagRecord* lr = &name->lang_tag_records[l];

						// Read length
						lr->length = MU_RBEU16(data);

						// Get & verify offset + length
						uint16_m offset = MU_RBEU16(data+2);
						if (offset+lr->length > storage_len) {
							mutt_DeloadName(name);
							return MUTT_INVALID_NAME_LENGTH_OFFSET;
						}

						// Get lang tag
						lr->lang_tag = (storage_len) ?(name->string_data+offset) :(0);
						// Increment data
						data += 4;
					}
				}

				font->name = name;
				return MUTT_SUCCESS;
			}

			// Frees all allocated data for name
			void mutt_DeloadName(muttName* name) {
				if (name) {
					if (name->name_records) {
						mu_free(name->name_records);
					}
					if (name->lang_tag_records) {
						mu_free(name->lang_tag_records);
					}
					if (name->string_data) {
						mu_free(name->string_data);
					}
					mu_free(name);
				}
			}

			// Loads the glyf table
			void mutt_DeloadGlyf(muttGlyf* glyf);
			muttResult mutt_LoadGlyf(muttFont* font, muByte* data, uint32_m datalen) {
				// Allocate glyf
				muttGlyf* glyf = (muttGlyf*)mu_malloc(sizeof(muttGlyf));
				if (!glyf) {
					return MUTT_FAILED_MALLOC;
				}
				mu_memset(glyf, 0, sizeof(muttGlyf));

				// Get glyf length
				glyf->len = datalen;

				if (glyf->len) {
					// Allocate glyf data
					glyf->data = (muByte*)mu_malloc(glyf->len);
					if (!glyf->data) {
						mutt_DeloadGlyf(glyf);
						return MUTT_FAILED_MALLOC;
					}

					// Copy over table memory
					mu_memcpy(glyf->data, data, glyf->len);
				}

				font->glyf = glyf;
				return MUTT_SUCCESS;
			}

			// Frees all allocated data for glyf
			void mutt_DeloadGlyf(muttGlyf* glyf) {
				if (glyf) {
					if (glyf->data) {
						mu_free(glyf->data);
					}
					mu_free(glyf);
				}
			}

		/* Cmap stuff */

			/* Format 0 */

				// Loads format 0
				muttResult mutt_f0Load(muttFont* font, muttCmap0* f0, muByte* data, uint32_m datalen) {
					// Verify length for everything
					if (datalen < 262) {
						return MUTT_INVALID_CMAP0_LENGTH;
					}

					// length
					if (MU_RBEU16(data+2) > datalen) {
						return MUTT_INVALID_CMAP0_LENGTH;
					}
					// language
					f0->language = MU_RBEU16(data+4);
					// glyphIdArray
					mu_memcpy(f0->glyph_ids, data+6, 256);

					return MUTT_SUCCESS; if (font) {}
				}

				// Codepoint -> glyph ID
				MUDEF uint16_m mutt_cmap0_get_glyph(muttFont* font, muttCmap0* f0, uint8_m codepoint) {
					// Get value for glyph ID from array
					// (codepoint will always be a valid index since u8)
					uint16_m glyph = (uint16_m)f0->glyph_ids[codepoint];

					// Return if valid; 0 if invalid
					return (glyph < font->maxp->num_glyphs) ?(glyph) :(0);
				}

				// Glyph ID -> codepoint
				MUDEF uint8_m mutt_cmap0_get_codepoint(muttFont* font, muttCmap0* f0, uint16_m glyph) {
					// Loop through each element in glyph ID array
					for (uint16_m i = 0; i < 256; ++i) {
						// If glyph ID is found:
						if (f0->glyph_ids[i] == glyph) {
							// Return index (codepoint)
							return i;
						}
					}

					// No glyph ID equivalent found; return nada
					return 0; if (font) {}
				}

			/* Format 4 */

				// Loads format 4
				void mutt_f4Deload(muttCmap4* f4);
				muttResult mutt_f4Load(muttFont* font, muttCmap4* f4, muByte* data, uint32_m datalen) {
					// Verify length for format...rangeShift
					// 32 is ok because length is u16 in subtable
					uint32_m req = 14;
					if (datalen < req) {
						return MUTT_INVALID_CMAP4_LENGTH;
					}
					// length
					uint16_m length = MU_RBEU16(data+2);
					if (length > datalen || length < req) {
						return MUTT_INVALID_CMAP4_LENGTH;
					}

					// language
					f4->language = MU_RBEU16(data+4);

					// segCountX2
					f4->seg_count = MU_RBEU16(data+6);
					if ((f4->seg_count%2) != 0) {
						return MUTT_INVALID_CMAP4_SEG_COUNT_X2;
					}
					f4->seg_count /= 2;
					// - Edge case for seg_count being 0
					if (f4->seg_count == 0) {
						f4->seg = 0;
						f4->glyph_ids = 0;
						return MUTT_SUCCESS;
					}

					// Verify length for endCode...idRangeOffset
					req += 2+(8*f4->seg_count);
					if (length < req) {
						return MUTT_INVALID_CMAP4_LENGTH;
					}

					// glyphIdArray length
					uint16_m glyph_id_array_len = length-req;
					if ((glyph_id_array_len%2) != 0) {
						//return MUTT_INVALID_CMAP4_GLYPH_ID_ARRAY_LENGTH;
						// Subtracting 1 instead, just feels better...
						glyph_id_array_len -= 1;
					}
					glyph_id_array_len /= 2;

					// Allocate segments
					f4->seg = (muttCmap4Segment*)mu_malloc(f4->seg_count*sizeof(muttCmap4Segment));
					if (!f4->seg) {
						return MUTT_FAILED_MALLOC;
					}
					// Allocate glyph IDs
					f4->glyph_ids = (uint16_m*)mu_malloc(glyph_id_array_len*2);
					if (!f4->glyph_ids) {
						mu_free(f4->seg);
						return MUTT_FAILED_MALLOC;
					}

					// Placeholder values for progress in data:
					muByte* end_code = data+14;
					muByte* start_code = end_code+(f4->seg_count*2)+2;
					muByte* id_delta = start_code+(f4->seg_count*2);
					muByte* id_range_offset = id_delta+(f4->seg_count*2);
					muByte* glyph_id_array = id_range_offset+(f4->seg_count*2);

					// Loop through each glyphIdArray element
					for (uint16_m g = 0; g < glyph_id_array_len; ++g) {
						// Read value
						f4->glyph_ids[g] = MU_RBEU16(glyph_id_array);
						// Move to next element
						glyph_id_array += 2;
					}

					// Loop through each segment
					for (uint16_m s = 0; s < f4->seg_count; ++s) {
						// Get segment
						muttCmap4Segment* ps = &f4->seg[s];

						// endCode
						ps->end_code = MU_RBEU16(end_code);
						// - Verify incremental
						if (s > 0) {
							if (ps->end_code <= (ps-1)->end_code) {
								mutt_f4Deload(f4);
								return MUTT_INVALID_CMAP4_END_CODE;
							}
						}
						// - Verify last = 0xFFFF
						if (s+1 == f4->seg_count && ps->end_code != 0xFFFF) {
							mutt_f4Deload(f4);
							return MUTT_INVALID_CMAP4_LAST_END_CODE;
						}

						// startCode
						ps->start_code = MU_RBEU16(start_code);
						// - Verify startCode <= endCode
						if (ps->start_code > ps->end_code) {
							mutt_f4Deload(f4);
							return MUTT_INVALID_CMAP4_START_CODE;
						}

						// idDelta
						ps->id_delta = MU_RBES16(id_delta);

						// idRangeOffset
						ps->id_range_offset = MU_RBEU16(id_range_offset);
						// Further verification is only cared about if idRangeOffset is not 0,
						// since logic with it is only performed under that circumstance
						if (ps->id_range_offset != 0) {
							// - Divide by 2 to account for 2-byte-per offset
							if ((ps->id_range_offset%2) != 0) {
								// Not divisible by 2 :L
								mutt_f4Deload(f4);
								return MUTT_INVALID_CMAP4_ID_RANGE_OFFSET;
							}
							ps->id_range_offset /= 2;
							// - Subtract seg_count-s to counteract offset being relative to &idRangeOffset[s]
							//   (only applies if idRangeOffset is not 0)
							if (ps->id_range_offset < f4->seg_count-s) {
								// Offset is out of range (before glyphIdArray)
								mutt_f4Deload(f4);
								return MUTT_INVALID_CMAP4_ID_RANGE_OFFSET;
							}
							ps->id_range_offset -= f4->seg_count-s;
							// - Ensure within range of glyphIdArray at max distance from it
							if (ps->id_range_offset + (ps->start_code - ps->end_code) >= glyph_id_array_len) {
								mutt_f4Deload(f4);
								return MUTT_INVALID_CMAP4_ID_RANGE_OFFSET;
							}

							// Calculate starting glyph ID
							ps->start_glyph_id = f4->glyph_ids[ps->id_range_offset];
							// - If it's not 0, idDelta logic is performed
							if (ps->start_glyph_id != 0) {
								ps->start_glyph_id = mutt_id_delta(ps->start_glyph_id, ps->id_delta);
							}
							// + Ending glyph ID
							ps->end_glyph_id = f4->glyph_ids[ps->id_range_offset+(ps->start_code-ps->end_code)];
							if (ps->end_glyph_id != 0) {
								ps->end_glyph_id = mutt_id_delta(ps->end_glyph_id, ps->id_delta);
							}
						}
						// If idRangeOffset IS 0, calculaing starting/ending glyph ID is just idDelta
						else {
							ps->start_glyph_id = mutt_id_delta(ps->start_code, ps->id_delta);
							ps->end_glyph_id = mutt_id_delta(ps->end_code, ps->id_delta);
						}
						// Now, we can be sure that for any segment:
						// glyphIdArray[idRangeOffset+(codepoint-startCode)]
						// is a valid index into glyphIdArray (besides idRangeOffset==0).
						// idDelta logic goes unverified at this point.

						// Increment relevant placeholder values
						end_code += 2;
						start_code += 2;
						id_delta += 2;
						id_range_offset += 2;
					}

					return MUTT_SUCCESS; if (font) {}
				}

				// Deloads format 4
				void mutt_f4Deload(muttCmap4* f4) {
					if (f4) {
						if (f4->seg) {
							mu_free(f4->seg);
						}
						if (f4->glyph_ids) {
							mu_free(f4->glyph_ids);
						}
						// (Purposely not freeing f4)
					}
				}

				// Codepoint -> glyph ID
				MUDEF uint16_m mutt_cmap4_get_glyph(muttFont* font, muttCmap4* f4, uint16_m codepoint) {
					// Loop through each segment
					for (uint16_m s = 0; s < f4->seg_count; ++s) {
						// Continue on if the codepoint is not within this segment
						if (codepoint < f4->seg[s].start_code || codepoint > f4->seg[s].end_code) {
							continue;
						}
						// If we're here, the segment contains our codepoint
						uint16_m glyph;
						muttCmap4Segment* seg = &f4->seg[s];

						// If idRangeOffset is 0, we're only performing delta logic
						if (seg->id_range_offset == 0) {
							glyph = mutt_id_delta(codepoint, seg->id_delta);
						}
						// If idRangeOffset isn't 0, we must index into glyphIdArray
						else {
							// (This is verified to be valid in mutt_f4Load)
							glyph = f4->glyph_ids[seg->id_range_offset+(codepoint-seg->start_code)];
							// + idDelta if not 0
							if (glyph != 0) {
								glyph = mutt_id_delta(glyph, seg->id_delta);
							}
						}

						// Return glyph if it's valid and not 0
						if (glyph < font->maxp->num_glyphs && glyph != 0) {
							return glyph;
						}
					}

					// Codepoint does not lie within a defined segment
					return 0;
				}

				// Glyph ID -> codepoint
				MUDEF uint16_m mutt_cmap4_get_codepoint(muttFont* font, muttCmap4* f4, uint16_m glyph) {
					// Loop through each segment
					for (uint16_m s = 0; s < f4->seg_count; ++s) {
						// If segment is fully continuous:
						if (f4->seg[s].start_glyph_id <= f4->seg[s].end_glyph_id) {
							// If glyph ID is within this range:
							if (glyph >= f4->seg[s].start_glyph_id && glyph <= f4->seg[s].end_glyph_id) {
								// Just return character code based on offset into glyph ID range
								return f4->seg[s].start_code + (glyph - f4->seg[s].start_glyph_id);
							}
						}
						// For non-continuous segments:
						else {
							// @TODO
						}
					}

					// Glyph ID is not given in any segment
					return 0; if (font) {}
				}

			/* Format 12 */

				// Loads format 12
				void mutt_f12Deload(muttCmap12* f12);
				muttResult mutt_f12Load(muttFont* font, muttCmap12* f12, muByte* data, uint32_m datalen) {
					// Verify length for format...numGroups
					uint64_m req = 16;
					if (datalen < req) {
						return MUTT_INVALID_CMAP12_LENGTH;
					}
					// length
					uint32_m length = MU_RBEU32(data+4);
					if (length > datalen || length < req) {
						return MUTT_INVALID_CMAP12_LENGTH;
					}

					// language
					f12->language = MU_RBEU32(data+8);

					// numGroups
					f12->num_groups = MU_RBEU32(data+12);
					// Exit early if no groups
					if (f12->num_groups == 0) {
						f12->groups = 0;
						return MUTT_SUCCESS;
					}

					// Verify length for groups
					req += f12->num_groups*12;
					if (length < req) {
						return MUTT_INVALID_CMAP12_LENGTH;
					}
					// Allocate groups
					f12->groups = (muttCmap12Group*)mu_malloc(sizeof(muttCmap12Group)*f12->num_groups);
					if (!f12->groups) {
						return MUTT_FAILED_MALLOC;
					}

					// Loop through each group
					data += 16;
					for (uint32_m g = 0; g < f12->num_groups; ++g) {
						// startCharCode
						f12->groups[g].start_char_code = MU_RBEU32(data);
						// endCharCode
						f12->groups[g].end_char_code = MU_RBEU32(data+4);

						// Logic for groups that have a previous group:
						if (g > 0) {
							// Verify incremental startCharCode ordering
							if (f12->groups[g].start_char_code <= f12->groups[g-1].start_char_code) {
								mutt_f12Deload(f12);
								return MUTT_INVALID_CMAP12_START_CHAR_CODE;
							}
							// Verify [-1].end < [0].start
							if (f12->groups[g-1].end_char_code >= f12->groups[g].start_char_code) {
								mutt_f12Deload(f12);
								return MUTT_INVALID_CMAP12_END_CHAR_CODE;
							}
						}

						// startGlyphID
						f12->groups[g].start_glyph_id = MU_RBEU32(data+8);
						// Increment data to next group
						data += 12;
					}

					return MUTT_SUCCESS; if (font) {}
				}

				// Deloads format 12
				void mutt_f12Deload(muttCmap12* f12) {
					if (f12->groups) {
						mu_free(f12->groups);
						// (Purposely not freeing f12)
					}
				}

				// Codepoint -> glyph ID
				MUDEF uint16_m mutt_cmap12_get_glyph(muttFont* font, muttCmap12* f12, uint32_m codepoint) {
					// Loop through each group
					for (uint32_m g = 0; g < f12->num_groups; ++g) {
						// Pass if codepoint is not in range
						if (codepoint < f12->groups[g].start_char_code || codepoint > f12->groups[g].end_char_code) {
							continue;
						}

						// Get glyph ID via start glyph ID and distance from startCharCode
						uint32_m glyph = f12->groups[g].start_glyph_id + (codepoint - f12->groups[g].start_char_code);
						// Return if non-zero and in range
						if (glyph != 0 && glyph < font->maxp->num_glyphs) {
							return (uint16_m)glyph;
						}
					}

					// No good glyph value could be found
					return 0;
				}

				// Glyph ID -> codepoint
				MUDEF uint32_m mutt_cmap12_get_codepoint(muttFont* font, muttCmap12* f12, uint16_m glyph) {
					// Loop through each group
					for (uint32_m g = 0; g < f12->num_groups; ++g) {
						// Pass if glyph is not in range
						if (
							glyph < f12->groups[g].start_glyph_id || 
							glyph > f12->groups[g].start_glyph_id + (f12->groups[g].end_char_code - f12->groups[g].start_char_code)
						) {
							continue;
						}

						// Return codepoint via startCharCode and distance from startGlyphID
						return f12->groups[g].start_char_code + (f12->groups[g].start_glyph_id - glyph);
					}

					// Glyph value was never specified in a range
					return 0; if (font) {}
				}

			/* All formats */

				// Loads a given format
				muttResult mutt_LoadCmapFormat(muttFont* font, uint16_m format, muttCmapFormat* pformat, muByte* data, uint32_m datalen) {
					switch (format) {
						// Unsupported format
						default: {
							return MUTT_INVALID_CMAP_ENCODING_RECORD_FORMAT;
						} break;

						// Format 0
						case 0: {
							// Allocate
							pformat->f0 = (muttCmap0*)mu_malloc(sizeof(muttCmap0));
							if (!pformat->f0) {
								return MUTT_FAILED_MALLOC;
							}
							// Load
							muttResult res = mutt_f0Load(font, pformat->f0, data, datalen);
							if (mutt_result_is_fatal(res)) {
								mu_free(pformat->f0);
								pformat->f0 = 0;
							}
							return res;
						} break;

						// Format 4
						case 4: {
							// Allocate
							pformat->f4 = (muttCmap4*)mu_malloc(sizeof(muttCmap4));
							if (!pformat->f4) {
								return MUTT_FAILED_MALLOC;
							}
							// Load
							muttResult res = mutt_f4Load(font, pformat->f4, data, datalen);
							if (mutt_result_is_fatal(res)) {
								mu_free(pformat->f4);
								pformat->f4 = 0;
							}
							return res;
						} break;

						// Format 12
						case 12: {
							// Allocate
							pformat->f12 = (muttCmap12*)mu_malloc(sizeof(muttCmap12));
							if (!pformat->f12) {
								return MUTT_FAILED_MALLOC;
							}
							// Load
							muttResult res = mutt_f12Load(font, pformat->f12, data, datalen);
							if (mutt_result_is_fatal(res)) {
								mu_free(pformat->f12);
								pformat->f12 = 0;
							}
							return res;
						} break;
					}
				}

				// Deloads a given format
				void mutt_DeloadCmapFormat(uint16_m format, muttCmapFormat* pformat) {
					switch (format) {
						default: break;

						// Format 0
						case 0: {
							// Deallocate
							mu_free(pformat->f0);
						} break;

						// Format 4
						case 4: {
							// Deload
							mutt_f4Deload(pformat->f4);
							// Deallocate
							mu_free(pformat->f4);
						} break;

						// Format 12
						case 12: {
							// Deload
							mutt_f12Deload(pformat->f12);
							// Deallocate
							mu_free(pformat->f12);
						} break;
					}
				}

				// Glyph -> codepoint, for a given format
				MUDEF uint16_m mutt_cmap_encoding_get_glyph(muttFont* font, muttEncodingRecord* record, uint32_m codepoint) {
					switch (record->format) {
						// Unknown
						default: return 0; break;

						// Format 0
						case 0: return mutt_cmap0_get_glyph(font, record->encoding.f0, codepoint); break;
						// Format 4
						case 4: return mutt_cmap4_get_glyph(font, record->encoding.f4, codepoint); break;
						// Format 12
						case 12: return mutt_cmap12_get_glyph(font, record->encoding.f12, codepoint); break;
					}
				}

				// Codepoint -> glyph, for a given format
				MUDEF uint32_m mutt_cmap_encoding_get_codepoint(muttFont* font, muttEncodingRecord* record, uint16_m glyph_id) {
					switch (record->format) {
						// Unknown
						default: return 0; break;

						// Format 0
						case 0: return mutt_cmap0_get_codepoint(font, record->encoding.f0, glyph_id); break;
						// Format 4
						case 4: return mutt_cmap4_get_codepoint(font, record->encoding.f4, glyph_id); break;
						// Format 12
						case 12: return mutt_cmap12_get_codepoint(font, record->encoding.f12, glyph_id); break;
					}
				}

			/* Cmap */

				// Loads the cmap table
				void mutt_DeloadCmap(muttCmap* cmap);
				muttResult mutt_LoadCmap(muttFont* font, muByte* data, uint32_m datalen) {
					// Placeholder values
					muByte* orig_data = data;

					// Verify length for version
					if (datalen < 2) {
						return MUTT_INVALID_CMAP_LENGTH;
					}
					// Verify version
					if (MU_RBEU16(data) != 0) {
						return MUTT_INVALID_CMAP_VERSION;
					}

					// Verify length for version and numTables
					if (datalen < 4) {
						return MUTT_INVALID_CMAP_LENGTH;
					}
					// Allocate cmap
					muttCmap* cmap = (muttCmap*)mu_malloc(sizeof(muttCmap));
					if (!cmap) {
						return MUTT_FAILED_MALLOC;
					}
					mu_memset(cmap, 0, sizeof(muttCmap));

					// numTables
					cmap->num_tables = MU_RBEU16(data+2);
					// Terminate early if no tables specified
					if (cmap->num_tables == 0) {
						font->cmap = cmap;
						return MUTT_SUCCESS;
					}
					// Verify length for tables
					if (datalen < (4 + (uint32_m)(cmap->num_tables*8))) {
						mutt_DeloadCmap(cmap);
						return MUTT_INVALID_CMAP_LENGTH;
					}

					// Allocate tables
					cmap->encoding_records = (muttEncodingRecord*)mu_malloc(sizeof(muttEncodingRecord)*cmap->num_tables);
					if (!cmap->encoding_records) {
						mutt_DeloadCmap(cmap);
						return MUTT_FAILED_MALLOC;
					}
					mu_memset(cmap->encoding_records, 0, sizeof(muttEncodingRecord)*cmap->num_tables);

					// Loop through each table
					data += 4;
					for (uint16_m t = 0; t < cmap->num_tables; ++t) {
						// Get record
						muttEncodingRecord* r = &cmap->encoding_records[t];

						// platformID
						r->platform_id = MU_RBEU16(data);
						// encodingID
						r->encoding_id = MU_RBEU16(data+2);

						// subtableOffset
						uint32_m offset = MU_RBEU32(data+4);
						if (offset >= datalen) {
							mutt_DeloadCmap(cmap);
							return MUTT_INVALID_CMAP_ENCODING_RECORD_OFFSET;
						}
						
						// Length of subtable
						uint32_m subtable_len = datalen-offset;
						// Verify length for format
						if (subtable_len < 2) {
							mutt_DeloadCmap(cmap);
							return MUTT_INVALID_CMAP_ENCODING_RECORD_LENGTH;
						}
						// format
						r->format = MU_RBEU16(orig_data+offset);
						// Load format
						r->result = mutt_LoadCmapFormat(font, r->format, &r->encoding, orig_data+offset, subtable_len);

						// Increment to next table
						data += 8;
					}

					font->cmap = cmap;
					return MUTT_SUCCESS;
				}

				// Deloads the cmap table
				void mutt_DeloadCmap(muttCmap* cmap) {
					// cmap:
					if (cmap) {
						// Encoding records:
						if (cmap->encoding_records) {
							// Per encoding record:
							for (uint16_m t = 0; t < cmap->num_tables; ++t) {
								// Deload format if necessary
								if (!mutt_result_is_fatal(cmap->encoding_records[t].result)) {
									mutt_DeloadCmapFormat(cmap->encoding_records[t].format, &cmap->encoding_records[t].encoding);
								}
							}
							mu_free(cmap->encoding_records);
						}
						mu_free(cmap);
					}
				}

				// Glyph -> codepoint, for all cmap formats
				MUDEF uint16_m mutt_get_glyph(muttFont* font, uint32_m codepoint) {
					// Loop through each encoding record
					for (uint16_m t = 0; t < font->cmap->num_tables; ++t) {
						// Try getting glyph ID for the encoding record
						uint16_m glyph = mutt_cmap_encoding_get_glyph(font, &font->cmap->encoding_records[t], codepoint);
						// Return if not 0
						if (glyph != 0) {
							return glyph;
						}
					}

					// Non-zero glyph ID couldn't be found
					return 0;
				}

				// Codepoint -> glyph, for all cmap formats
				MUDEF uint32_m mutt_get_codepoint(muttFont* font, uint16_m glyph_id) {
					// Loop through each encoding record
					for (uint16_m t = 0; t < font->cmap->num_tables; ++t) {
						// Try getting codepoint for the encoding record
						uint32_m codepoint = mutt_cmap_encoding_get_codepoint(font, &font->cmap->encoding_records[t], glyph_id);
						// Return if not 0
						if (codepoint != 0) {
							return codepoint;
						}
					}

					// Non-zero codepoint couldn't be found
					return 0;
				}

		/* Loading / Deloading */

			// Initializes all flag/result states of each table to "failed to find"
			void mutt_InitTables(muttFont* font, muttLoadFlags load_flags) {
				// maxp
				font->maxp_res = (load_flags & MUTT_LOAD_MAXP) ? MUTT_FAILED_FIND_TABLE : 0;
				font->fail_load_flags |= (load_flags & MUTT_LOAD_MAXP);
				// head
				font->head_res = (load_flags & MUTT_LOAD_HEAD) ? MUTT_FAILED_FIND_TABLE : 0;
				font->fail_load_flags |= (load_flags & MUTT_LOAD_HEAD);
				// hhea
				font->hhea_res = (load_flags & MUTT_LOAD_HHEA) ? MUTT_FAILED_FIND_TABLE : 0;
				font->fail_load_flags |= (load_flags & MUTT_LOAD_HHEA);
				// hmtx
				font->hmtx_res = (load_flags & MUTT_LOAD_HMTX) ? MUTT_FAILED_FIND_TABLE : 0;
				font->fail_load_flags |= (load_flags & MUTT_LOAD_HMTX);
				// loca
				font->loca_res = (load_flags & MUTT_LOAD_LOCA) ? MUTT_FAILED_FIND_TABLE : 0;
				font->fail_load_flags |= (load_flags & MUTT_LOAD_LOCA);
				// name
				font->name_res = (load_flags & MUTT_LOAD_NAME) ? MUTT_FAILED_FIND_TABLE : 0;
				font->fail_load_flags |= (load_flags & MUTT_LOAD_NAME);
				// glyf
				font->glyf_res = (load_flags & MUTT_LOAD_GLYF) ? MUTT_FAILED_FIND_TABLE : 0;
				font->fail_load_flags |= (load_flags & MUTT_LOAD_GLYF);
				// cmap
				font->cmap_res = (load_flags & MUTT_LOAD_CMAP) ? MUTT_FAILED_FIND_TABLE : 0;
				font->fail_load_flags |= (load_flags & MUTT_LOAD_CMAP);
			}

			// Does one pass through each table load
			void mutt_LoadTables(muttFont* font, muByte* data, muttLoadFlags* first, muBool dep_pass, muttLoadFlags* waiting) {
				// Loop through each table
				for (uint16_m i = 0; i < font->directory->num_tables; ++i) {
					// Get record information
					muttTableRecord rec = font->directory->records[i];

					// Do things based on table tag
					switch (rec.table_tag_u32) {
						default: break;

						// maxp
						case 0x6D617870: {
							// Account for first
							if (dep_pass) {
								*first |= MUTT_LOAD_MAXP;
							}
							// Skip if already processed
							// This works because all tables to be processed are initialized
							// to "MUTT_FAILED_FIND_TABLE" and set to something else once
							// processed.
							if (font->maxp_res != MUTT_FAILED_FIND_TABLE) {
								break;
							}

							// Load
							font->maxp_res = mutt_LoadMaxp(font, &data[rec.offset], rec.length);
							if (font->maxp) {
								font->load_flags |= MUTT_LOAD_MAXP;
								font->fail_load_flags &= ~MUTT_LOAD_MAXP;
							} else {
								font->fail_load_flags |= MUTT_LOAD_MAXP;
								font->load_flags &= ~MUTT_LOAD_MAXP;
							}
						} break;

						// head
						case 0x68656164: {
							// Account for first
							if (dep_pass) {
								*first |= MUTT_LOAD_HEAD;
							}
							// Skip if already processed
							if (font->head_res != MUTT_FAILED_FIND_TABLE) {
								break;
							}

							// Load
							font->head_res = mutt_LoadHead(font, &data[rec.offset], rec.length);
							if (font->head) {
								font->load_flags |= MUTT_LOAD_HEAD;
								font->fail_load_flags &= ~MUTT_LOAD_HEAD;
							} else {
								font->fail_load_flags |= MUTT_LOAD_HEAD;
								font->load_flags &= ~MUTT_LOAD_HEAD;
							}
						} break;

						// hhea; req maxp
						case 0x68686561: {
							// Account for first
							if (dep_pass) {
								*first |= MUTT_LOAD_HHEA;
							}
							// Skip if already processed
							if (font->hhea_res != MUTT_FAILED_FIND_TABLE) {
								break;
							}

							// Give bad result if missing dependency
							if (!dep_pass && !(*first & MUTT_LOAD_MAXP)) {
								font->hhea_res = MUTT_HHEA_REQUIRES_MAXP;
								break;
							}
							// Continue if dependencies aren't processed
							if (!font->maxp) {
								*waiting |= MUTT_LOAD_HHEA;
								break;
							}
							// Mark as no longer waiting
							*waiting &= ~MUTT_LOAD_HHEA;

							// Load
							font->hhea_res = mutt_LoadHhea(font, &data[rec.offset], rec.length);
							if (font->hhea) {
								font->load_flags |= MUTT_LOAD_HHEA;
								font->fail_load_flags &= ~MUTT_LOAD_HHEA;
							} else {
								font->fail_load_flags |= MUTT_LOAD_HHEA;
								font->load_flags &= ~MUTT_LOAD_HHEA;
							}
						} break;

						// hmtx; req maxp, hhea
						case 0x686D7478: {
							// Account for first
							if (dep_pass) {
								*first |= MUTT_LOAD_HMTX;
							}
							// Skip if already processed
							if (font->hmtx_res != MUTT_FAILED_FIND_TABLE) {
								break;
							}

							// Give bad result if missing dependency
							if (!dep_pass) {
								// maxp
								if (!(*first & MUTT_LOAD_MAXP)) {
									font->hmtx_res = MUTT_HMTX_REQUIRES_MAXP;
									break;
								}
								// hhea
								if (!(*first & MUTT_LOAD_HHEA)) {
									font->hmtx_res = MUTT_HMTX_REQUIRES_HHEA;
									break;
								}
							}
							// Continue if dependencies aren't processed
							if (!(font->maxp) || !(font->hhea)) {
								*waiting |= MUTT_LOAD_HMTX;
								break;
							}
							// Mark as no longer waiting
							*waiting &= ~MUTT_LOAD_HMTX;

							// Load
							font->hmtx_res = mutt_LoadHmtx(font, &data[rec.offset], rec.length);
							if (font->hmtx) {
								font->load_flags |= MUTT_LOAD_HMTX;
								font->fail_load_flags &= ~MUTT_LOAD_HMTX;
							} else {
								font->fail_load_flags |= MUTT_LOAD_HMTX;
								font->load_flags &= ~MUTT_LOAD_HMTX;
							}
						} break;

						// loca; req maxp, head, glyf
						case 0x6C6F6361: {
							// Account for first
							if (dep_pass) {
								*first |= MUTT_LOAD_LOCA;
							}
							// Skip if already processed
							if (font->loca_res != MUTT_FAILED_FIND_TABLE) {
								break;
							}

							// Give bad result if missing dependency
							if (!dep_pass) {
								// maxp
								if (!(*first & MUTT_LOAD_MAXP)) {
									font->loca_res = MUTT_LOCA_REQUIRES_MAXP;
									break;
								}
								// head
								if (!(*first & MUTT_LOAD_HEAD)) {
									font->loca_res = MUTT_LOCA_REQUIRES_HEAD;
									break;
								}
								// glyf
								if (!(*first & MUTT_LOAD_GLYF)) {
									font->loca_res = MUTT_LOCA_REQUIRES_GLYF;
									break;
								}
							}
							// Continue if dependencies aren't processed
							if (!(font->maxp) || !(font->head) || !(font->glyf)) {
								*waiting |= MUTT_LOAD_LOCA;
								break;
							}
							// Mark as no longer waiting
							*waiting &= ~MUTT_LOAD_LOCA;

							// Load
							font->loca_res = mutt_LoadLoca(font, &data[rec.offset], rec.length);
							if (font->loca) {
								font->load_flags |= MUTT_LOAD_LOCA;
								font->fail_load_flags &= ~MUTT_LOAD_LOCA;
							} else {
								font->fail_load_flags |= MUTT_LOAD_LOCA;
								font->load_flags &= ~MUTT_LOAD_LOCA;
							}
						} break;

						// name
						case 0x6E616D65: {
							// Account for first
							if (dep_pass) {
								*first |= MUTT_LOAD_NAME;
							}
							// Skip if already processed
							if (font->name_res != MUTT_FAILED_FIND_TABLE) {
								break;
							}

							// Load
							font->name_res = mutt_LoadName(font, &data[rec.offset], rec.length);
							if (font->name) {
								font->load_flags |= MUTT_LOAD_NAME;
								font->fail_load_flags &= ~MUTT_LOAD_NAME;
							} else {
								font->fail_load_flags |= MUTT_LOAD_NAME;
								font->load_flags &= ~MUTT_LOAD_NAME;
							}
						} break;

						// glyf
						case 0x676C7966: {
							// Account for first
							if (dep_pass) {
								*first |= MUTT_LOAD_GLYF;
							}
							// Skip if already processed
							if (font->glyf_res != MUTT_FAILED_FIND_TABLE) {
								break;
							}

							// Load
							font->glyf_res = mutt_LoadGlyf(font, &data[rec.offset], rec.length);
							if (font->glyf) {
								font->load_flags |= MUTT_LOAD_GLYF;
								font->fail_load_flags &= ~MUTT_LOAD_GLYF;
							} else {
								font->fail_load_flags |= MUTT_LOAD_GLYF;
								font->load_flags &= ~MUTT_LOAD_GLYF;
							}
						} break;

						// cmap
						case 0x636D6170: {
							// Account for first
							if (dep_pass) {
								*first |= MUTT_LOAD_CMAP;
							}
							// Skip if already processed
							if (font->cmap_res != MUTT_FAILED_FIND_TABLE) {
								break;
							}

							// Give bad result if missing dependency
							if (!dep_pass && !(*first & MUTT_LOAD_MAXP)) {
								font->cmap_res = MUTT_CMAP_REQUIRES_MAXP;
								break;
							}
							// Continue if dependencies aren't processed
							if (!font->maxp) {
								*waiting |= MUTT_LOAD_CMAP;
								break;
							}
							// Mark as no longer waiting
							*waiting &= ~MUTT_LOAD_CMAP;

							// Load
							font->cmap_res = mutt_LoadCmap(font, &data[rec.offset], rec.length);
							if (font->cmap) {
								font->load_flags |= MUTT_LOAD_CMAP;
								font->fail_load_flags &= ~MUTT_LOAD_CMAP;
							} else {
								font->fail_load_flags |= MUTT_LOAD_CMAP;
								font->load_flags &= ~MUTT_LOAD_CMAP;
							}
						} break;
					}
				}
			}

			// Deallocates all loaded tables
			void mutt_DeloadTables(muttFont* font) {
				// Basic tables
				if (font->maxp) {
					mu_free(font->maxp);
				}
				if (font->head) {
					mu_free(font->head);
				}
				if (font->hhea) {
					mu_free(font->hhea);
				}

				// Allocated tables
				mutt_DeloadHmtx(font->hmtx);
				mutt_DeloadLoca(font->loca);
				mutt_DeloadName(font->name);
				mutt_DeloadGlyf(font->glyf);
				mutt_DeloadCmap(font->cmap);
			}

			MUDEF muttResult mutt_load(muByte* data, uint64_m datalen, muttFont* font, muttLoadFlags load_flags) {
				muttResult res;

				// Zero-out font
				mu_memset(font, 0, sizeof(muttFont));

				// Load table directory
				// - Allocate
				font->directory = (muttDirectory*)mu_malloc(sizeof(muttDirectory));
				if (!font->directory) {
					// mutt_deload(font);
					return MUTT_FAILED_MALLOC;
				}
				// - Load
				res = mutt_LoadTableDirectory(font->directory, data, datalen);
				if (mutt_result_is_fatal(res)) {
					mutt_deload(font);
					return res;
				}

				// Init and load tables
				mutt_InitTables(font, load_flags);

				muttLoadFlags temp_flags = 0;
				muttLoadFlags wait_flags = 0;
				mutt_LoadTables(font, data, &temp_flags, MU_TRUE, &wait_flags);
				while (wait_flags) {
					mutt_LoadTables(font, data, &temp_flags, MU_FALSE, &wait_flags);
				}

				return MUTT_SUCCESS;
			}

			MUDEF void mutt_deload(muttFont* font) {
				// Deload tables
				mutt_DeloadTables(font);
				// Deload table directory
				if (font->directory) {
					mutt_DeloadTableDirectory(font->directory);
					mu_free(font->directory);
				}
			}

		/* Glyf stuff */

			// Fills in the "muttGlyphHeader" struct
			MUDEF muttResult mutt_glyph_header(muttFont* font, uint16_m glyph_id, muttGlyphHeader* header) {
				// Get length and data of glyph
				// - 16-bit
				if (font->head->index_to_loc_format == 0) {
					header->data = &font->glyf->data[((uint32_m)font->loca->offsets16[glyph_id])*2];
					header->length = (((uint32_m)font->loca->offsets16[glyph_id+1])*2) - (((uint32_m)font->loca->offsets16[glyph_id])*2);
				}
				// - 32-bit
				else {
					header->data = &font->glyf->data[font->loca->offsets32[glyph_id]];
					header->length = font->loca->offsets32[glyph_id+1] - font->loca->offsets32[glyph_id];
				}

				// If header length is 0, we set everything to 0 and return
				// (because no outline)
				if (header->length == 0) {
					mu_memset(header, 0, sizeof(muttGlyphHeader));
					return MUTT_SUCCESS;
				}

				// Verify minimum length for header
				if (header->length < 10) {
					return MUTT_INVALID_GLYF_HEADER_LENGTH;
				}

				// numberOfContours
				header->number_of_contours = MU_RBES16(header->data);
				if (header->number_of_contours > font->maxp->max_contours) {
					return MUTT_INVALID_GLYF_HEADER_NUMBER_OF_CONTOURS;
				}

				// xMin
				header->x_min = MU_RBES16(header->data+2);
				if (header->x_min < font->head->x_min) {
					return MUTT_INVALID_GLYF_HEADER_X_MIN;
				}
				// yMin
				header->y_min = MU_RBES16(header->data+4);
				if (header->y_min < font->head->y_min) {
					return MUTT_INVALID_GLYF_HEADER_Y_MIN;
				}
				// xMax
				header->x_max = MU_RBES16(header->data+6);
				if (header->x_max > font->head->x_max) {
					return MUTT_INVALID_GLYF_HEADER_X_MAX;
				}
				if (header->x_max < header->x_min) {
					return MUTT_INVALID_GLYF_HEADER_X_MIN_MAX;
				}
				// yMax
				header->y_max = MU_RBES16(header->data+8);
				if (header->y_max > font->head->y_max) {
					return MUTT_INVALID_GLYF_HEADER_Y_MAX;
				}
				if (header->y_max < header->y_min) {
					return MUTT_INVALID_GLYF_HEADER_Y_MIN_MAX;
				}

				// Subtract header data from length
				header->length -= 10;
				// ...and move data up past header
				header->data += 10;

				return MUTT_SUCCESS;
			}

			// Fills in (or calculates memory needed for) "muttSimpleGlyph" struct
			MUDEF muttResult mutt_simple_glyph(muttFont* font, muttGlyphHeader* header, muttSimpleGlyph* glyph, muByte* data, uint32_m* written) {
				// Verify length for endPtsOfContours and instructionLength
				// (req is u64 to avoid possible overflow)
				uint64_m req = (((uint32_m)header->number_of_contours)*2) + 2;
				if (header->length < req) {
					return MUTT_INVALID_GLYF_SIMPLE_LENGTH;
				}

				// Glyph data:
				muByte* gdata = header->data;

				// Memory size calculation:
				if (!data) {
					// Add endPtsOfContours to memory
					uint32_m min_size = header->number_of_contours*2;

					// To calculate amount of points:
					uint16_m points = 0;
					if (header->number_of_contours != 0) {
						// Skip to last element of endPtsOfContours and read
						gdata += (uint32_m)(header->number_of_contours-1)*2;
						points = MU_RBEU16(gdata);

						// Verify last element of endPtsOfContours
						if (points == 0xFFFF) {
							return MUTT_INVALID_GLYF_SIMPLE_END_PTS_OF_CONTOURS;
						}

						// Skip past last element
						++points;
						gdata += 2;
					}

					// Verify point count
					if (points > font->maxp->max_points) {
						return MUTT_INVALID_GLYF_SIMPLE_POINT_COUNT;
					}
					// Add points to memory
					min_size += ((uint32_m)points)*sizeof(muttGlyphPoint);

					// Add instructions to memory
					uint16_m instruction_length = MU_RBEU16(gdata);
					if (instruction_length > font->maxp->max_size_of_instructions) {
						return MUTT_INVALID_GLYF_SIMPLE_INSTRUCTION_LENGTH;
					}
					min_size += instruction_length;

					// Check for size fitting
					// This should not need to be checked, as it is calculated via maxp,
					// to which all of these values are also checked to not exceed.
					/*if (min_size > mutt_simple_glyph_max_size(font)) {
						return MUTT_INVALID_GLYF_SIMPLE_;
					}*/

					// Set memory req
					*written = min_size;
					return MUTT_SUCCESS;
				}

				// Store pointer to original data
				muByte* orig_data = data;

				// Allocate endPtsOfContours
				glyph->end_pts_of_contours = (uint16_m*)data;
				data += ((uint32_m)header->number_of_contours)*2;

				// Loop through each contour
				uint16_m points = 0;
				for (uint16_m c = 0; c < header->number_of_contours; ++c) {
					// Copy value for endPtsOfContour
					glyph->end_pts_of_contours[c] = MU_RBEU16(gdata);

					// Verify increasing order
					if (c != 0) {
						if (glyph->end_pts_of_contours[c] <= glyph->end_pts_of_contours[c-1]) {
							return MUTT_INVALID_GLYF_SIMPLE_END_PTS_OF_CONTOURS;
						}
					}

					// Get point count if we're on the last index
					if (c == header->number_of_contours-1) {
						points = glyph->end_pts_of_contours[c];
						// Verify last element
						if (points == 0xFFFF) {
							return MUTT_INVALID_GLYF_SIMPLE_END_PTS_OF_CONTOURS;
						}
						// Increment points by one to get count (since last value is an index)
						++points;
					}

					// Move to next entry in endPtsOfContours
					gdata += 2;
				}

				// Verify point count
				if (points > font->maxp->max_points) {
					return MUTT_INVALID_GLYF_SIMPLE_POINT_COUNT;
				}

				// instructionLength
				glyph->instruction_length = MU_RBEU16(gdata);
				gdata += 2;
				// + Verify instructionLength
				if (glyph->instruction_length > font->maxp->max_size_of_instructions) {
					return MUTT_INVALID_GLYF_SIMPLE_INSTRUCTION_LENGTH;
				}

				// For instructions:
				if (glyph->instruction_length != 0) {
					// Verify length for instructions
					req += glyph->instruction_length;
					if (header->length < req) {
						return MUTT_INVALID_GLYF_SIMPLE_LENGTH;
					}
					// Allocate
					glyph->instructions = (uint8_m*)data;
					// Copy over
					mu_memcpy(glyph->instructions, gdata, glyph->instruction_length);
					// Move past instructions
					data += glyph->instruction_length;
					gdata += glyph->instruction_length;
				}

				// Allocate points
				glyph->points = (muttGlyphPoint*)data;
				data += points*sizeof(muttGlyphPoint);

				// Loop through each flag
				uint16_m pi = 0; // (point index)
				uint16_m contour_id = 0;
				while (pi < points) {
					// Increment contour ID if we've gone over the end point of the current contour
					if (pi > glyph->end_pts_of_contours[contour_id]) {
						++contour_id;
					}
					// Verify length for this flag
					if (header->length < ++req) {
						return MUTT_INVALID_GLYF_SIMPLE_LENGTH;
					}

					// Get flag
					uint8_m flags = *gdata;
					glyph->points[pi].flags = flags;

					// Verify that flag doesn't rely on prior values if this is the first flag
					// (Some common fonts get this messed up; we're assuming 0 in these cases)
					// This is brought up in the documentation.
					// ...

					// Verify that first point is on curve
					// (Not checking for similar reasons above, also brought up in doc)
					// ...

					// Get implied length of x- and y-coordinates based on flag
					uint8_m coord_length = 0;
					// - x
					if (flags & MUTT_X_SHORT_VECTOR) {
						// 1-byte x-coordinate
						coord_length += 1;
					} else if (!(flags & MUTT_X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR)) {
						// 2-byte x-coordinate
						coord_length += 2;
					}
					// -y
					if (flags & MUTT_Y_SHORT_VECTOR) {
						// 1-byte y-coordinate
						coord_length += 1;
					} else if (!(flags & MUTT_Y_IS_SAME_OR_POSITIVE_Y_SHORT_VECTOR)) {
						// 2-byte y-coordinate
						coord_length += 2;
					}

					// Verify length based on coord length
					req += coord_length;
					if (header->length < req) {
						return MUTT_INVALID_GLYF_SIMPLE_LENGTH;
					}

					// Increment past this point/flag
					++pi;
					++gdata;

					// Handle repeating flags:
					if (flags & MUTT_REPEAT_FLAG) {
						// Verify length for next flag
						if (header->length < ++req) {
							return MUTT_INVALID_GLYF_SIMPLE_LENGTH;
						}

						// Get next flag as length
						uint8_m next = *gdata++;
						// (required length is checked next loop)
						// - If next is 0, we don't repeat at all, and pretend
						// like this never happened
						if (next == 0) {
							continue;
						}

						// Verify length for repeated coordinate flags
						req += ((uint32_m)coord_length) * ((uint32_m)next-1);
						if (header->length < req) {
							return MUTT_INVALID_GLYF_SIMPLE_LENGTH;
						}

						// Verify repeat count
						// Not doing this since we can easily pretend like it didn't
						// happen to no consequence.
						// This is mentioned in documentation
						/*if (pi+next > points) {
							return MUTT_INVALID_GLYF_SIMPLE_FLAG_REPEAT_COUNT;
						}*/

						// Go through each repeat flag and copy it down
						for (uint8_m r = 0; r < next && pi < points; ++r) {
							glyph->points[pi++].flags = flags;
						}
					}
				}

				// Loop through each x-coordinate
				pi = 0;
				while (pi < points) {
					// Get flags
					uint8_m flags = glyph->points[pi].flags;

					// Get x-coordinate
					if (flags & MUTT_X_SHORT_VECTOR) {
						// 1-byte
						if (flags & MUTT_X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR) {
							// Positive
							glyph->points[pi].x = (int16_m)*gdata++;
						} else {
							// Negative
							glyph->points[pi].x = -((int16_m)*gdata++);
						}
					} else {
						if (flags & MUTT_X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR) {
							// Repeated x-coordinate
							glyph->points[pi].x = (pi!=0) ?(glyph->points[pi-1].x) :(0);
							++pi;
							// (Continue because no vector stuff is applied)
							continue;
						} else {
							// 2-bytes, signed
							glyph->points[pi].x = MU_RBES16(gdata);
							gdata += 2;
						}
					}

					// Add as vector to prior value
					// - Get previous x value
					int16_m prev_x = (pi!=0) ?(glyph->points[pi-1].x) :(0);
					// - Compute the current coordinate based on previous
					int32_m test_val = prev_x + glyph->points[pi].x;
					// - Verify that the point is within range
					if (test_val < header->x_min || test_val > header->x_max) {
						return MUTT_INVALID_GLYF_SIMPLE_X_COORD;
					}
					// - Assign
					glyph->points[pi++].x = (int16_m)test_val;
				}

				// Loop through each y-coordinate
				pi = 0;
				while (pi < points) {
					// Get flags
					uint8_m flags = glyph->points[pi].flags;

					// Get y-coordinate
					if (flags & MUTT_Y_SHORT_VECTOR) {
						// 1-byte
						if (flags & MUTT_Y_IS_SAME_OR_POSITIVE_Y_SHORT_VECTOR) {
							// Positive
							glyph->points[pi].y = (int16_m)*gdata++;
						} else {
							// Negative
							glyph->points[pi].y = -((int16_m)*gdata++);
						}
					} else {
						if (flags & MUTT_Y_IS_SAME_OR_POSITIVE_Y_SHORT_VECTOR) {
							// Repeated y-coordinate
							glyph->points[pi].y = (pi!=0) ?(glyph->points[pi-1].y) :(0);
							++pi;
							// (Continue because no vector stuff is applied)
							continue;
						} else {
							// 2-bytes, signed
							glyph->points[pi].y = MU_RBES16(gdata);
							gdata += 2;
						}
					}

					// Add as vector to prior value
					// - Get previous y value
					int16_m prev_y = (pi!=0) ?(glyph->points[pi-1].y) :(0);
					// - Compute the current coordinate based on previous
					int32_m test_val = prev_y + glyph->points[pi].y;
					// - Verify that the point is within range
					if (test_val < header->y_min || test_val > header->y_max) {
						return MUTT_INVALID_GLYF_SIMPLE_Y_COORD;
					}
					// - Assign
					glyph->points[pi++].y = (int16_m)test_val;
				}

				// Write how many bytes of data were used
				if (written) {
					*written = data-orig_data;
				}
				return MUTT_SUCCESS;
			}

			// Simple glyph memory maximum
			MUDEF uint32_m mutt_simple_glyph_max_size(muttFont* font) {
				return
					// endPtsOfContours
					(font->maxp->max_contours*2)
					// instructions
					+(font->maxp->max_size_of_instructions)
					// maxPoints
					+(font->maxp->max_points*sizeof(muttGlyphPoint))
				;
			}

			// Fills in (or calculates memory needed for) "muttCompositeGlyph" struct
			MUDEF muttResult mutt_composite_glyph(muttFont* font, muttGlyphHeader* header, muttCompositeGlyph* glyph, muByte* data, uint32_m* written) {
				// Placeholder variables
				uint32_m req = 0; // Required length
				muByte* gdata = header->data; // Progressive data pointer

				// For memory calculations:
				if (!data) {
					// Loop through each record
					uint16_m flags = (header->length != 0) ?(MUTT_MORE_COMPONENTS) :(0);
					uint32_m written_amt = 0;
					muBool instructions = MU_FALSE;
					uint32_m component_count = 0;

					while (flags & MUTT_MORE_COMPONENTS) {
						// Verify component count
						if (++component_count > font->maxp->max_component_elements) {
							return MUTT_INVALID_GLYF_COMPOSITE_COMPONENT_COUNT;
						}
						// Add component to mem req
						written_amt += sizeof(muttComponentGlyph);

						// Verify length for flags and glyphIndex
						req += 4;
						if (header->length < req) {
							return MUTT_INVALID_GLYF_COMPOSITE_LENGTH;
						}
						// Read flags for if we have more components and instructions
						uint16_m this_flags = MU_RBEU16(gdata);
						flags = this_flags & MUTT_MORE_COMPONENTS;
						if (this_flags & MUTT_WE_HAVE_INSTRUCTIONS) {
							instructions = MU_TRUE;
						}
						// Skip past flags and glyphIndex
						gdata += 4;

						// Add arguments to req length and skip past
						if (this_flags & MUTT_ARG_1_AND_2_ARE_WORDS) {
							// 2 bytes per
							req += 4;
							gdata += 4;
						}
						else {
							// 1 byte per
							req += 2;
							gdata += 2;
						}

						// Add transform to req length and skip past
						// We're not checking for mutual exclusivity here, as
						// it's not relevant to mem req
						if (this_flags & MUTT_WE_HAVE_A_SCALE) {
							// One F2DOT14
							req += 2;
							gdata += 2;
						}
						else if (this_flags & MUTT_WE_HAVE_AN_X_AND_Y_SCALE) {
							// Two F2DOT14s
							req += 4;
							gdata += 4;
						}
						else if (this_flags & MUTT_WE_HAVE_A_TWO_BY_TWO) {
							// Four F2DOT14s
							req += 8;
							gdata += 8;
						}

						// Verify length for arguments and transform
						if (header->length < req) {
							return MUTT_INVALID_GLYF_COMPOSITE_LENGTH;
						}
					}

					// Instruction handling
					if (instructions) {
						// Verify length for instruction length
						req += 2;
						if (header->length < req) {
							return MUTT_INVALID_GLYF_COMPOSITE_LENGTH;
						}

						// Read instruction length
						uint16_m instruction_length = MU_RBEU16(gdata);
						// Verify with maxp
						if (instruction_length > font->maxp->max_size_of_instructions) {
							return MUTT_INVALID_GLYF_COMPOSITE_INSTRUCTION_LENGTH;
						}
						// Verify length for instructions
						// Unneeded here since we're just calculating mem req
						// ...

						// Add instructions to mem req
						written_amt += instruction_length;
					}

					// Give mem req and exit
					*written = written_amt;
					return MUTT_SUCCESS;
				}

				// Placeholder variables:
				muByte* orig_data = data; // (Used for memory usage at the end)
				uint32_m component_count = 0; // (Used for counting components; u32 to avoid overflow)
				muBool instructions = MU_FALSE; // (To see if we've found instructions)
				uint16_m flags = (header->length != 0) ?(MUTT_MORE_COMPONENTS) :(0);

				// Point components array to current data (what we're filling in)
				glyph->components = (muttComponentGlyph*)data;
				muttComponentGlyph* c = glyph->components;

				// Loop through each component:
				while (flags & MUTT_MORE_COMPONENTS) {
					// Verify component count
					if (component_count > font->maxp->max_component_elements) {
						return MUTT_INVALID_GLYF_COMPOSITE_COMPONENT_COUNT;
					}

					// Verify length for flags and glyphIndex
					req += 4;
					if (header->length < req) {
						return MUTT_INVALID_GLYF_COMPOSITE_LENGTH;
					}

					// Read flags
					c->flags = MU_RBEU16(gdata);
					gdata += 2;
					// Mark next component
					flags = c->flags & MUTT_MORE_COMPONENTS;
					// Mark instructions flag
					if (c->flags & MUTT_WE_HAVE_INSTRUCTIONS) {
						instructions = MU_TRUE;
					}

					// Read glyphIndex
					c->glyph_index = MU_RBEU16(gdata);
					gdata += 2;
					// Verify index range
					if (c->glyph_index >= font->maxp->num_glyphs) {
						return MUTT_INVALID_GLYF_COMPOSITE_GLYPH_INDEX;
					}

					// Arguments handling:
					if (c->flags & MUTT_ARG_1_AND_2_ARE_WORDS) {
						// 2 bytes per argument

						// Verify length
						req += 4;
						if (header->length < req) {
							return MUTT_INVALID_GLYF_COMPOSITE_LENGTH;
						}

						// Read values
						if (c->flags & MUTT_ARGS_ARE_XY_VALUES) {
							// int16 xy values
							c->argument1 = MU_RBES16(gdata);
							gdata += 2;
							c->argument2 = MU_RBES16(gdata);
							gdata += 2;
						} else {
							// uint16 point numbers
							c->argument1 = MU_RBEU16(gdata);
							gdata += 2;
							c->argument2 = MU_RBEU16(gdata);
							gdata += 2;
						}
					} else {
						// 1 byte per argument

						// Verify length
						req += 2;
						if (header->length < req) {
							return MUTT_INVALID_GLYF_COMPOSITE_LENGTH;
						}

						// Read values
						if (c->flags & MUTT_ARGS_ARE_XY_VALUES) {
							// int8 xy values
							c->argument1 = MU_RBES8(gdata++);
							c->argument2 = MU_RBES8(gdata++);
						} else {
							// uint8 point numbers
							c->argument1 = *gdata++;
							c->argument2 = *gdata++;
						}
					}

					// Transform data handling
					if (c->flags & MUTT_WE_HAVE_A_SCALE) {
						// One F2DOT14

						// Ensure flag exclusivity
						if ((c->flags & MUTT_WE_HAVE_AN_X_AND_Y_SCALE) || (c->flags & MUTT_WE_HAVE_A_TWO_BY_TWO)) {
							return MUTT_INVALID_GLYF_COMPOSITE_FLAGS;
						}

						// Verify length
						req += 2;
						if (header->length < req) {
							return MUTT_INVALID_GLYF_COMPOSITE_LENGTH;
						}

						// Read
						c->scales[0] = MUTT_F2DOT14(gdata);
						gdata += 2;
					}
					else if (c->flags & MUTT_WE_HAVE_AN_X_AND_Y_SCALE) {
						// Two F2DOT14s

						// Ensure flag exclusivity
						if ((c->flags & MUTT_WE_HAVE_A_SCALE) || (c->flags & MUTT_WE_HAVE_A_TWO_BY_TWO)) {
							return MUTT_INVALID_GLYF_COMPOSITE_FLAGS;
						}

						// Verify length
						req += 4;
						if (header->length < req) {
							return MUTT_INVALID_GLYF_COMPOSITE_LENGTH;
						}

						// Read
						c->scales[0] = MUTT_F2DOT14(gdata);
						gdata += 2;
						c->scales[1] = MUTT_F2DOT14(gdata);
						gdata += 2;
					}
					else if (c->flags & MUTT_WE_HAVE_A_TWO_BY_TWO) {
						// Four F2DOT14s

						// Ensure flag exclusivity
						if ((c->flags & MUTT_WE_HAVE_A_SCALE) || (c->flags & MUTT_WE_HAVE_AN_X_AND_Y_SCALE)) {
							return MUTT_INVALID_GLYF_COMPOSITE_FLAGS;
						}

						// Verify length
						req += 8;
						if (header->length < req) {
							return MUTT_INVALID_GLYF_COMPOSITE_LENGTH;
						}

						// Read
						c->scales[0] = MUTT_F2DOT14(gdata);
						gdata += 2;
						c->scales[1] = MUTT_F2DOT14(gdata);
						gdata += 2;
						c->scales[2] = MUTT_F2DOT14(gdata);
						gdata += 2;
						c->scales[3] = MUTT_F2DOT14(gdata);
						gdata += 2;
					}

					// Increment handle for component and component count
					++c;
					++component_count;
				}
				// Verify component count
				if (component_count > font->maxp->max_component_elements) {
					return MUTT_INVALID_GLYF_COMPOSITE_COMPONENT_COUNT;
				}
				// Move past component array
				data += sizeof(muttComponentGlyph) * component_count;

				// Instruction handling
				if (instructions) {
					// Point to data
					glyph->instructions = data;

					// Verify length for instruction length
					req += 2;
					if (header->length < req) {
						return MUTT_INVALID_GLYF_COMPOSITE_LENGTH;
					}

					// Read instruction length
					glyph->instruction_length = MU_RBEU16(gdata);
					gdata += 2;
					// Verify with maxp
					if (glyph->instruction_length > font->maxp->max_size_of_instructions) {
						return MUTT_INVALID_GLYF_COMPOSITE_INSTRUCTION_LENGTH;
					}

					if (glyph->instruction_length > 0) {
						// Verify length for instructions
						req += glyph->instruction_length;
						if (header->length < req) {
							return MUTT_INVALID_GLYF_COMPOSITE_LENGTH;
						}

						// Memcpy instructions
						mu_memcpy(glyph->instructions, gdata, glyph->instruction_length);
						// Move data past instructions
						data += glyph->instruction_length;
					}
				} else {
					glyph->instruction_length = 0;
					glyph->instructions = 0;
				}

				// Copy over component count
				glyph->component_count = (uint16_m)component_count;
				// Write data written
				if (written) {
					*written = data-orig_data;
				}
				return MUTT_SUCCESS;
			}

			// Composite glyph memory maximum
			MUDEF uint32_m mutt_composite_glyph_max_size(muttFont* font) {
				return
					// components
					(font->maxp->max_component_elements*sizeof(muttComponentGlyph))
					// instructions
					+(font->maxp->max_size_of_instructions)
				;
			}

			// Glyph memory maximum
			MUDEF uint32_m mutt_glyph_max_size(muttFont* font) {
				// Simple:
				uint32_m s = mutt_simple_glyph_max_size(font);
				// Composite:
				uint32_m c = mutt_composite_glyph_max_size(font);
				// Return greatest
				return (s>c) ?(s) :(c);
			}

		/* Misc. */

			// Performs idDelta logic
			MUDEF uint16_m mutt_id_delta(uint16_m character_code, int16_m delta) {
				// Just return if idDelta is 0
				if (delta == 0) {
					return character_code;
				}

				// Do modulo 65536 on a larger type to add without overflow
				int32_m big_id = (int32_m)(character_code + delta) % 65536;
				// Prevent negative cases by adding 65536 for cases under 0
				if (big_id < 0) {
					big_id += 65536;
				}
				// Return that but now u16, which (I think) big_id is guaranteed to fit in
				return (uint16_m)big_id;
			}

	/* Raster API */

		/* Math */

			// Float epsilon macro used for countering floating point imprecision in line calculations
			// This value is unabashedly stolen from stb_truetype, but it works real well
			#define MUTTR_LINE_EPSILON32 (1.f/1024.f)

			// Double epsilon macro used for same purpose as MUTTR_LINE_EPSILON32; not used as of right now
			// This value should be tested and vetted more thoroughly if it's going to be used
			#define MUTTR_LINE_EPSILON64 0.00000001

			// Calculates IF a ray intersects a line
			static inline muBool muttR_LineRay(float ry, float y0, float y1) {
				// Any of these conditions must be true for intersection:
				return
					// Ray y is between y0 and y1 with forgiving epsilons:
					(ry >= y0-MUTTR_LINE_EPSILON32 && ry <= y1+MUTTR_LINE_EPSILON32) ||
					(ry >= y1-MUTTR_LINE_EPSILON32 && ry <= y0+MUTTR_LINE_EPSILON32)
					// Line is horizontal:
					|| (y1-y0 == 0.f)
				;
			}

			// Calculates the x-value intersection of a ray and a line
			static inline float muttR_LineRayHit(float ry, float x0, float y0, float x1, float y1) {
				// (https://math.stackexchange.com/a/2297532)
				return x0 + (
					((ry-y0) * (x1-x0))
					/
					(y1-y0)
				);
			}

			// Calculates the x- and y-value of a point on a Bezier curve (on, off, on) given t.
			static inline void muttR_Bezier(float t,
				float x0, float y0, float x1, float y1, float x2, float y2,
				float* x, float* y
			) {
				// I think this can be optimized, but idk...
				*x = ((1.f-t)*(1.f-t)*x0) + (2.f*(1.f-t)*t*x1) + (t*t*x2);
				*y = ((1.f-t)*(1.f-t)*y0) + (2.f*(1.f-t)*t*y1) + (t*t*y2);
			}

		/* Raster shape setup */

			/* Definitions */

				// A raster shape is made up of lines, a line is defined by two points
				struct muttR_Line {
					// 0 is bottom, 1 is top (y0 <= y1)
					float x0;
					float y0;
					float x1;
					float y1;
					// vec = y1-y0 (for direction)
					float vec;
				};
				typedef struct muttR_Line muttR_Line;

				// Used to sort the lines in DECREASING order of top point
				int muttR_CompareLines(const void* p, const void* q) {
					// Get lines
					muttR_Line* l0 = (muttR_Line*)p, * l1 = (muttR_Line*)q;
					// Return value to indicate smallest y1 value
					if (l0->y1 < l1->y1) {
						return 1;
					} else if (l0->y1 > l1->y1) {
						return -1;
					}
					return 0;
				}

				// Calculates the winding for a given line
				static inline int8_m muttR_LineWinding(float ry, muttR_Line* line) {
					// Don't consider winding if we're directly intersecting the high point of line
					if (mu_fabsf(ry - line->y0) <= MUTTR_LINE_EPSILON32) {
						return 0;
					}

					// Return winding order based on vector; down is +1, up is -1, neither is 0
					if (line->vec < 0.f) {
						return 1;
					} else if (line->vec > 0.f) {
						return -1;
					}
					return 0;
				}

				// A raster shape is just a collection of lines representing a glyph
				struct muttR_Shape {
					uint32_m num_lines;
					muttR_Line* lines;
					float x_max;
					float y_max;
				};
				typedef struct muttR_Shape muttR_Shape;

				// Sorts the lines of a shape in DECREASING order of top point
				void muttR_ShapeSort(muttR_Shape* shape) {
					// Just call qsort for the job
					mu_qsort(shape->lines, shape->num_lines, sizeof(muttR_Line), muttR_CompareLines);
				}

			/* Conversions */

				// Converts two points to a line
				void muttR_GlyphLine(muttR_Line* line, float x0, float y0, float x1, float y1) {
					// Determine lowest y0 value
					if (y0 < y1) {
						line->x0 = x0;
						line->y0 = y0;
						line->x1 = x1;
						line->y1 = y1;
					}
					else {
						line->x0 = x1;
						line->y0 = y1;
						line->x1 = x0;
						line->y1 = y0;
					}
					// Calculate vec
					line->vec = y1-y0;
				}

				// Converts three-point Bezier (on, off, on) to lines
				// - Constants:
				#define MUTTR_LINES_PER_BEZIER 25
				static const float MUTTR_LINES_PER_BEZIER_F = ((float)MUTTR_LINES_PER_BEZIER);
				static const float MUTTR_LINES_PER_BEZIER_INV = 1.f / ((float)MUTTR_LINES_PER_BEZIER);
				// - Function:
				static inline void muttR_GlyphCurve(muttR_Line* line, float x0, float y0, float x1, float y1, float x2, float y2) {
					// Loop through each line per Bezier
					for (uint32_m l = 0; l < MUTTR_LINES_PER_BEZIER; ++l) {
						// Calculate Bezier for the first point
						float t = ((float)l) / MUTTR_LINES_PER_BEZIER_F;
						float bx0, by0;
						muttR_Bezier(t, x0, y0, x1, y1, x2, y2, &bx0, &by0);
						
						// + Second point
						float bx1, by1;
						muttR_Bezier(t+MUTTR_LINES_PER_BEZIER_INV, x0, y0, x1, y1, x2, y2, &bx1, &by1);

						// Make line based on this strip of the Bezier
						muttR_GlyphLine(line++, bx0, by0, bx1, by1);
					}
				}

				// Gets the next point of a glyph based on the contour ends
				static inline muttRPoint* muttR_GlyphNextPoint(muttRGlyph* glyph, uint32_m p, uint32_m i, uint32_m c) {
					// The next point is p incremented
					muttRPoint* pn = &glyph->points[p+i];
					// ...unless this is the last point of the contour:
					if (p+i > glyph->contour_ends[c]) {
						// ..in which case the next point wraps back around to the first
						// point of the contour
						pn -= glyph->contour_ends[c]+1;
						if (c > 0) {
							pn += glyph->contour_ends[c-1]+1;
						}
					}
					return pn;
				}

				// Calculates the amount of lines needed to represent a glyph
				uint32_m muttR_GlyphLineCount(muttRGlyph* glyph) {
					// Keep count of lines:
					uint32_m count = 0;
					// Keep count of contours:
					uint16_m c = 0;

					// Loop through each point
					for (uint32_m p = 0; p < glyph->num_points;) {
						// Increment contour ID if necessary
						if (p > glyph->contour_ends[c]) {
							++c;
						}

						// Get current point
						muttRPoint* p0 = &glyph->points[p];

						// If current point is ON curve (ON...):
						if (p0->flags & MUTTR_ON_CURVE) {
							// Get next point
							muttRPoint* p1 = muttR_GlyphNextPoint(glyph, p, 1, c);
							// If next point is ON curve (ON, ON):
							if (p1->flags & MUTTR_ON_CURVE) {
								// One more line, go to next point
								++count;
								++p;
								continue;
							}

							// If we're here, the next point is OFF curve (ON, OFF...)
							// It must be a Bezier, and we must go forward two points
							count += MUTTR_LINES_PER_BEZIER;
							p += 2;
							continue;
						}

						// If we're here, we started on an OFF point
						// It must be a Bezier, and we must go forward one point
						count += MUTTR_LINES_PER_BEZIER;
						++p;
						//continue;
					}

					// Return final count
					return count;
				}

				// Converts an rglyph to a shape
				muttResult muttR_ShapeCreate(muttRGlyph* glyph, muttR_Shape* shape) {
					// Calculate number of lines needed
					shape->num_lines = muttR_GlyphLineCount(glyph);
					// Allocate lines
					shape->lines = (muttR_Line*)mu_malloc(sizeof(muttR_Line)*shape->num_lines);
					if (!shape->lines) {
						return MUTT_FAILED_MALLOC;
					}

					// Loop through each point
					uint32_m c = 0; // (contour tracker)
					muttR_Line* l = shape->lines; // (shape tracker)
					for (uint32_m p = 0; p < glyph->num_points;) {
						// Increment contour ID if necessary
						if (p > glyph->contour_ends[c]) {
							++c;
						}

						// Get current point
						muttRPoint* p0 = &glyph->points[p];

						// If current point is ON curve (ON...):
						if (p0->flags & MUTTR_ON_CURVE) {
							// Get next point
							muttRPoint* p1 = muttR_GlyphNextPoint(glyph, p, 1, c);
							// If next point is ON curve (ON, ON):
							if (p1->flags & MUTTR_ON_CURVE) {
								// Form line between two points and move on by one
								muttR_GlyphLine(l++, p0->x, p0->y, p1->x, p1->y);
								++p;
								continue;
							}

							// If we're here, the next point is OFF curve (ON, OFF...)
							// Get next-next point
							muttRPoint* p2 = muttR_GlyphNextPoint(glyph, p, 2, c);
							// If next-next point is ON the curve (ON, OFF, ON):
							if (p2->flags & MUTTR_ON_CURVE) {
								// Form basic Bezier and move on
								muttR_GlyphCurve(l, p0->x, p0->y, p1->x, p1->y, p2->x, p2->y);
								l += MUTTR_LINES_PER_BEZIER;
								p += 2;
								continue;
							}

							// If we're here, the next-next point is OFF the curve: (ON, OFF, OFF):
							else {
								// Bezier, with last point needing to be mid-pointed
								muttR_GlyphCurve(l, p0->x, p0->y, p1->x, p1->y,
									(p1->x + p2->x) / 2.f, (p1->y + p2->y) / 2.f
								);
								l += MUTTR_LINES_PER_BEZIER;
								p += 2;
								continue;
							}
						}

						// If we're here, the current point is OFF curve (OFF...)
						// From that, we know that the previous point has to also
						// be OFF curve (OFF, OFF[0]...) due to the logic of this
						// loop.
						// Get previous point:
						muttRPoint* pn1 = &glyph->points[p-1];
						// Get next point:
						muttRPoint* p1 = muttR_GlyphNextPoint(glyph, p, 1, c);

						// x0 and y0 must be midpointed between pn1 and p0 since
						// both are OFF curve:
						float x0 = (pn1->x + p0->x) / 2.f;
						float y0 = (pn1->y + p0->y) / 2.f;
						// x1 and y1 are just p0 since it's OFF curve:
						float x1 = p0->x;
						float y1 = p0->y;
						// x2 and y2 are just p1:
						float x2 = p1->x;
						float y2 = p1->y;
						// ...unless it's OFF curve, in which case it must be
						// midpointed:
						if (!(p1->flags & MUTTR_ON_CURVE)) {
							x2 = (x1 + x2) / 2.f;
							y2 = (y1 + y2) / 2.f;
						}

						// Bezier and move on:
						muttR_GlyphCurve(l, x0, y0, x1, y1, x2, y2);
						l += MUTTR_LINES_PER_BEZIER;
						++p;
						//continue;
					}

					// Sort all lines
					muttR_ShapeSort(shape);
					// Set max x/y
					shape->x_max = glyph->x_max;
					shape->y_max = glyph->y_max;

					return MUTT_SUCCESS;
				}

				// Frees all memory used by a shape
				void muttR_ShapeDestroy(muttR_Shape* shape) {
					if (shape->lines) {
						mu_free(shape->lines);
					}
				}

		/* Intersection/Hit logic */

			// A hit:
			struct muttR_Hit {
				// The x-value of the intersection
				float x;
				// The line which it intersected with
				uint32_m l;
			};
			typedef struct muttR_Hit muttR_Hit;

			// Used to sort intersections in order of increasing x-value
			int muttR_HitCompare(const void* p, const void* q) {
				// Get hits
				muttR_Hit* h0 = (muttR_Hit*)p, * h1 = (muttR_Hit*)q;
				// Indicate which hit's x-value is greater
				if (h0->x < h1->x) {
					return -1;
				} else if (h0->x > h1->x) {
					return 1;
				}
				return 0;
			}

			// Updates the active line list based on the ray
			static inline void muttR_ActiveLines(muttR_Line* lines, uint32_m num_lines, uint32_m* first, uint32_m* len, float ry) {
				// Increase length to include possible new in-rage lines
				while (*first + *len < num_lines) {
					if (ry <= lines[*first + *len].y1 + MUTTR_LINE_EPSILON32) {
						++*len;
					} else {
						break;
					}
				}

				// Move up the first active line until we're in range of the next nearest line
				while (*len != 0) {
					if (ry <= lines[*first].y0 - MUTTR_LINE_EPSILON32) {
						++*first;
						--*len;
					} else {
						break;
					}
				}
			}

			// Calculates all hits for a given ray and its active lines
			// Returns amount of hits, and gives winding as well
			static inline uint32_m muttR_Hits(muttR_Line* lines, uint32_m num_lines, float ry, muttR_Hit* hits, int32_m* winding) {
				uint32_m num_hits = 0;

				// Loop through each line
				for (uint32_m l = 0; l < num_lines; ++l) {
					// If ray intersects with line:
					if (muttR_LineRay(ry, lines[l].y0, lines[l].y1)) {
						// Add intersection values
						float x = muttR_LineRayHit(ry, lines[l].x0, lines[l].y0, lines[l].x1, lines[l].y1);
						// Add if intersection is valid
						if (x >= 0.f) {
							hits[num_hits  ].x = x;
							hits[num_hits++].l = l;
						}
					}
				}

				// Sort hits
				mu_qsort(hits, num_hits, sizeof(muttR_Hit), muttR_HitCompare);

				// Calculate total winding order
				*winding = 0;
				for (uint32_m hitw = 0; hitw < num_hits; ++hitw) {
					winding += muttR_LineWinding(ry, &lines[hits[hitw].l]);
				}

				return num_hits;
			}

		/* Rasterization per method */

			// MUTTR_FULL_PIXEL_BI_LEVEL
			muttResult muttR_FullPixelBiLevel(muttR_Shape* shape, muttRBitmap* bitmap, uint8_m adv, uint8_m in, uint8_m out) {
				// Allocate hit tracker
				muttR_Hit* hits = (muttR_Hit*)mu_malloc(shape->num_lines*sizeof(muttR_Hit));
				if (!hits) {
					return MUTT_FAILED_MALLOC;
				}

				// Initialize active lines
				uint32_m first_line = 0;
				uint32_m line_len = 0;

				// Loop through each horizontal strip
				for (uint32_m h = 0; h < bitmap->height; ++h) {
					// Calculate horizontal pixel offset
					uint64_m hpix_offset = bitmap->stride*((bitmap->height-h)-1);

					// Just fill all x-values with out if the height is now outside of the glyph range
					// (+ double-pixel extra for bleeding and ceiling)
					if (h > shape->y_max+2) {
						mu_memset(&bitmap->pixels[hpix_offset], out, bitmap->width*adv);
						continue;
					}

					// Calculate y-value of ray (middle of pixel)
					float ray_y = ((float)h) + .5f;

					// Update active line list
					muttR_ActiveLines(shape->lines, shape->num_lines, &first_line, &line_len, ray_y);
					// Calculate all hits with active lines (+ winding)
					int32_m winding;
					uint32_m num_hits = muttR_Hits(shape->lines, shape->num_lines, ray_y, hits, &winding);

					// Loop through each x-value
					uint32_m ih = 0; // (Upcoming hit)
					for (uint32_m w = 0; w < bitmap->width; ++w) {
						// Just fill all remaining x-values with out if width is now outside of glyph range
						// (+ double-pixel extra for bleeding and ceiling)
						if (w > shape->x_max+2) {
							mu_memset(&bitmap->pixels[hpix_offset+(w*adv)], out, (bitmap->width-w)*adv);
							continue;
						}

						// Calculate x-coordinate (middle of pixel)
						float ray_x = ((float)w) + .5f;

						// Skip over every hit we've passed, and remove their windings
						// "ray_x > ..." ensures rule 2 of scan converting:
						// "If a contour falls exactly on a pixel’s center, that pixel is turned on."
						while (ih < num_hits && ray_x > hits[ih].x) {
							winding -= muttR_LineWinding(ray_y, &shape->lines[hits[ih++].l]);
						}

						// Set pixel to whether or not we're in glyph
						// Winding == 0 means in the glyph, and vice versa
						bitmap->pixels[hpix_offset+(w*adv)] = (winding==0) ?(out) :(in);
					}
				}

				mu_free(hits);
				return MUTT_SUCCESS;
			}

		/* Rasterization */

			// Converts channels to advance
			uint8_m muttR_ChannelsAdv(muttRChannels channels) {
				switch (channels) {
					default: return 1; break;
					case MUTTR_R: return 1; break;
					case MUTTR_RGB: return 3; break;
					case MUTTR_RGBA: return 4; break;
				}
			}

			// Rasterizes a glyph
			MUDEF muttResult mutt_raster_glyph(muttRGlyph* glyph, muttRBitmap* bitmap, muttRMethod method) {
				// Convert rglyph to shape
				muttR_Shape shape;
				muttResult res = muttR_ShapeCreate(glyph, &shape);
				if (mutt_result_is_fatal(res)) {
					return res;
				}

				// Per-pixel advance based on channels:
				uint8_m adv = muttR_ChannelsAdv(bitmap->channels);
				// In/Out value based on io_color:
				uint8_m in  = (bitmap->io_color == MUTTR_BW) ?(255) :(0);
				uint8_m out = ~in;

				// Render shape based on method
				switch (method) {
					// Unrecognized method:
					default: {
						res = MUTT_UNKNOWN_RASTER_METHOD;
					} break;

					// Full-pixel bi-level
					case MUTTR_FULL_PIXEL_BI_LEVEL: {
						res = muttR_FullPixelBiLevel(&shape, bitmap, adv, in, out);
					} break;
				}

				// Free resources and return latest non-fatal result
				muttR_ShapeDestroy(&shape);
				return res;
			}

		/* Conversion */

			// FUnits to pixel-units
			MUDEF float mutt_funits_to_punits(muttFont* font, float funits, float point_size, float ppi) {
				return point_size * funits * ppi / (72.f * font->head->units_per_em);
			}

			/* Simple */

				// Simple glyph -> raster glyph
				MUDEF muttResult mutt_simple_rglyph(muttFont* font, muttGlyphHeader* header, muttSimpleGlyph* glyph, muttRGlyph* rglyph, float point_size, float ppi, muByte* data, uint32_m* written) {
					// For written calculations:
					if (!data) {
						uint32_m write = 0;
						// points
						write += sizeof(muttRPoint) * (glyph->end_pts_of_contours[header->number_of_contours-1] + 1);
						// contour_ends
						write += sizeof(uint16_m) * header->number_of_contours;
						// Write data needed
						*written = write;
						return MUTT_SUCCESS;
					}

					// Get data for arrays
					muByte* orig_data = data;
					// - points
					rglyph->num_points = glyph->end_pts_of_contours[header->number_of_contours-1] + 1;
					rglyph->points = (muttRPoint*)data;
					data += sizeof(muttRPoint) * ((uint32_m)rglyph->num_points);
					// - contour_ends
					rglyph->num_contours = header->number_of_contours;
					rglyph->contour_ends = (uint16_m*)data;
					data += sizeof(uint16_m) * ((uint32_m)rglyph->num_contours);
					// Write written data amount
					if (written) {
						*written = data-orig_data;
					}

					// Calculate point offsets based on glyph's min/max values
					float px = -mutt_funits_to_punits(font, header->x_min, point_size, ppi) + 1.f;
					float py = -mutt_funits_to_punits(font, header->y_min, point_size, ppi) + 1.f;

					// Loop through each point
					for (uint16_m p = 0; p < rglyph->num_points; ++p) {
						// X and Y
						rglyph->points[p].x = px + mutt_funits_to_punits(font, glyph->points[p].x, point_size, ppi);
						rglyph->points[p].y = py + mutt_funits_to_punits(font, glyph->points[p].y, point_size, ppi);
						// Flags
						rglyph->points[p].flags = (glyph->points[p].flags & MUTT_ON_CURVE_POINT) ?(MUTTR_ON_CURVE) :(0);
					}

					// Copy over contour ends
					mu_memcpy(rglyph->contour_ends, glyph->end_pts_of_contours, sizeof(uint16_m) * ((uint32_m)rglyph->num_contours));

					// Caclculate x_max and y_max
					rglyph->x_max = px + mutt_funits_to_punits(font, header->x_max, point_size, ppi);
					rglyph->y_max = py + mutt_funits_to_punits(font, header->y_max, point_size, ppi);

					return MUTT_SUCCESS;
				}

				// Memory maximum
				MUDEF uint32_m mutt_simple_rglyph_max(muttFont* font) {
					return
						// points
						(sizeof(muttRPoint) * font->maxp->max_points)
						// contour_ends
						+ (sizeof(uint16_m) * font->maxp->max_contours)
					;
				}

			/* Composite */

				// Composite glyph -> raster glyph
				MUDEF muttResult mutt_composite_rglyph(muttFont* font, muttGlyphHeader* header, muttCompositeGlyph* glyph, muttRGlyph* rglyph, float point_size, float ppi, muByte* data, uint32_m* written);

				// Memory maximum
				MUDEF uint32_m mutt_composite_rglyph_max(muttFont* font) {
					return 0; if (font) {}
				}

			/* Header */

				// Glyph header -> raster glyph
				MUDEF muttResult mutt_header_rglyph(muttFont* font, muttGlyphHeader* header, muttRGlyph* rglyph, float point_size, float ppi, muByte* data, uint32_m* written) {
					muttResult res = MUTT_SUCCESS;
					uint32_m write0 = 0, write1 = 0;

					// Just memory calculations:
					if (!data) {
						// Simple:
						if (header->number_of_contours >= 0) {
							// Glyph data:
							res = mutt_simple_glyph(font, header, 0, 0, &write0);
							if (mutt_result_is_fatal(res)) {
								return res;
							}
							// Rglyph data:
							res = mutt_simple_rglyph(font, header, 0, 0, point_size, ppi, 0, &write1);
							if (mutt_result_is_fatal(res)) {
								return res;
							}
						}
						// Composite:
						else {
							// Glyph data:
							res = mutt_composite_glyph(font, header, 0, 0, &write0);
							if (mutt_result_is_fatal(res)) {
								return res;
							}
							// Rglyph data:
							//res = mutt_composite_rglyph(font, header, 0, 0, point_size, ppi, 0, &write1);
							if (mutt_result_is_fatal(res)) {
								return res;
							}
						}

						// Write sum of memory needed
						*written = write0 + write1;
						return res;
					}

					// Simple:
					if (header->number_of_contours >= 0) {
						// Load simple glyph
						muttSimpleGlyph glyph;
						res = mutt_simple_glyph(font, header, &glyph, data, &write0);
						if (mutt_result_is_fatal(res)) {
							return res;
						}
						data += write0;

						// Convert to rglyph
						res = mutt_simple_rglyph(font, header, &glyph, rglyph, point_size, ppi, data, &write1);
						if (mutt_result_is_fatal(res)) {
							return res;
						}
						//data += write1;
					}
					// Composite:
					else {
						// @TODO
						return MUTT_UNKNOWN_RASTER_METHOD;
					}

					// Write written
					if (written) {
						*written = write0 + write1;
					}
					return MUTT_SUCCESS;
				}

				MUDEF uint32_m mutt_header_rglyph_max(muttFont* font) {
					// Simple:
					uint32_m sim = mutt_simple_glyph_max_size(font) + mutt_simple_rglyph_max(font);
					// Composite:
					uint32_m com = mutt_composite_glyph_max_size(font) + mutt_composite_rglyph_max(font);
					// Return greater
					return (sim > com) ?(sim) :(com);
				}

	/* Result */

		MUDEF muBool mutt_result_is_fatal(muttResult result) {
			switch (result) {
				default: return MU_TRUE; break;
				case MUTT_SUCCESS: return MU_FALSE; break;
			}
		}

	/* Names */

		#ifdef MUTT_NAMES

		MUDEF const char* mutt_result_get_name(muttResult result) {
			switch (result) {
				default: return "MU_UNKNOWN"; break;
				case MUTT_SUCCESS: return "MUTT_SUCCESS"; break;
				case MUTT_FAILED_MALLOC: return "MUTT_FAILED_MALLOC"; break;
				case MUTT_FAILED_REALLOC: return "MUTT_FAILED_REALLOC"; break;
				case MUTT_FAILED_FIND_TABLE: return "MUTT_FAILED_FIND_TABLE"; break;
				case MUTT_INVALID_DIRECTORY_LENGTH: return "MUTT_INVALID_DIRECTORY_LENGTH"; break;
				case MUTT_INVALID_DIRECTORY_SFNT_VERSION: return "MUTT_INVALID_DIRECTORY_SFNT_VERSION"; break;
				case MUTT_INVALID_DIRECTORY_NUM_TABLES: return "MUTT_INVALID_DIRECTORY_NUM_TABLES"; break;
				case MUTT_INVALID_DIRECTORY_RECORD_OFFSET: return "MUTT_INVALID_DIRECTORY_RECORD_OFFSET"; break;
				case MUTT_INVALID_DIRECTORY_RECORD_LENGTH: return "MUTT_INVALID_DIRECTORY_RECORD_LENGTH"; break;
				case MUTT_INVALID_DIRECTORY_RECORD_CHECKSUM: return "MUTT_INVALID_DIRECTORY_RECORD_CHECKSUM"; break;
				case MUTT_INVALID_DIRECTORY_RECORD_TABLE_TAG: return "MUTT_INVALID_DIRECTORY_RECORD_TABLE_TAG"; break;
				case MUTT_MISSING_DIRECTORY_RECORD_TABLE_TAGS: return "MUTT_MISSING_DIRECTORY_RECORD_TABLE_TAGS"; break;
				case MUTT_INVALID_MAXP_LENGTH: return "MUTT_INVALID_MAXP_LENGTH"; break;
				case MUTT_INVALID_MAXP_VERSION: return "MUTT_INVALID_MAXP_VERSION"; break;
				case MUTT_INVALID_MAXP_NUM_GLYPHS: return "MUTT_INVALID_MAXP_NUM_GLYPHS"; break;
				case MUTT_INVALID_MAXP_MAX_ZONES: return "MUTT_INVALID_MAXP_MAX_ZONES"; break;
				case MUTT_INVALID_HEAD_LENGTH: return "MUTT_INVALID_HEAD_LENGTH"; break;
				case MUTT_INVALID_HEAD_VERSION: return "MUTT_INVALID_HEAD_VERSION"; break;
				case MUTT_INVALID_HEAD_MAGIC_NUMBER: return "MUTT_INVALID_HEAD_MAGIC_NUMBER"; break;
				case MUTT_INVALID_HEAD_UNITS_PER_EM: return "MUTT_INVALID_HEAD_UNITS_PER_EM"; break;
				case MUTT_INVALID_HEAD_X_MIN_COORDINATES: return "MUTT_INVALID_HEAD_X_MIN_COORDINATES"; break;
				case MUTT_INVALID_HEAD_Y_MIN_COORDINATES: return "MUTT_INVALID_HEAD_Y_MIN_COORDINATES"; break;
				case MUTT_INVALID_HEAD_X_MAX_COORDINATES: return "MUTT_INVALID_HEAD_X_MAX_COORDINATES"; break;
				case MUTT_INVALID_HEAD_Y_MAX_COORDINATES: return "MUTT_INVALID_HEAD_Y_MAX_COORDINATES"; break;
				case MUTT_INVALID_HEAD_X_MIN_MAX: return "MUTT_INVALID_HEAD_X_MIN_MAX"; break;
				case MUTT_INVALID_HEAD_Y_MIN_MAX: return "MUTT_INVALID_HEAD_Y_MIN_MAX"; break;
				case MUTT_INVALID_HEAD_INDEX_TO_LOC_FORMAT: return "MUTT_INVALID_HEAD_INDEX_TO_LOC_FORMAT"; break;
				case MUTT_INVALID_HEAD_GLYPH_DATA_FORMAT: return "MUTT_INVALID_HEAD_GLYPH_DATA_FORMAT"; break;
				case MUTT_INVALID_HHEA_LENGTH: return "MUTT_INVALID_HHEA_LENGTH"; break;
				case MUTT_INVALID_HHEA_VERSION: return "MUTT_INVALID_HHEA_VERSION"; break;
				case MUTT_INVALID_HHEA_METRIC_DATA_FORMAT: return "MUTT_INVALID_HHEA_METRIC_DATA_FORMAT"; break;
				case MUTT_INVALID_HHEA_NUMBER_OF_HMETRICS: return "MUTT_INVALID_HHEA_NUMBER_OF_HMETRICS"; break;
				case MUTT_HHEA_REQUIRES_MAXP: return "MUTT_HHEA_REQUIRES_MAXP"; break;
				case MUTT_INVALID_HMTX_LENGTH: return "MUTT_INVALID_HMTX_LENGTH"; break;
				case MUTT_HMTX_REQUIRES_MAXP: return "MUTT_HMTX_REQUIRES_MAXP";
				case MUTT_HMTX_REQUIRES_HHEA: return "MUTT_HMTX_REQUIRES_HHEA";
				case MUTT_INVALID_LOCA_LENGTH: return "MUTT_INVALID_LOCA_LENGTH"; break;
				case MUTT_INVALID_LOCA_OFFSET: return "MUTT_INVALID_LOCA_OFFSET"; break;
				case MUTT_LOCA_REQUIRES_MAXP: return "MUTT_LOCA_REQUIRES_MAXP"; break;
				case MUTT_LOCA_REQUIRES_HEAD: return "MUTT_LOCA_REQUIRES_HEAD"; break;
				case MUTT_LOCA_REQUIRES_GLYF: return "MUTT_LOCA_REQUIRES_GLYF"; break;
				case MUTT_INVALID_NAME_LENGTH: return "MUTT_INVALID_NAME_LENGTH"; break;
				case MUTT_INVALID_NAME_VERSION: return "MUTT_INVALID_NAME_VERSION"; break;
				case MUTT_INVALID_NAME_STORAGE_OFFSET: return "MUTT_INVALID_NAME_STORAGE_OFFSET"; break;
				case MUTT_INVALID_NAME_LENGTH_OFFSET: return "MUTT_INVALID_NAME_LENGTH_OFFSET"; break;
				case MUTT_INVALID_GLYF_HEADER_LENGTH: return "MUTT_INVALID_GLYF_HEADER_LENGTH"; break;
				case MUTT_INVALID_GLYF_HEADER_NUMBER_OF_CONTOURS: return "MUTT_INVALID_GLYF_HEADER_NUMBER_OF_CONTOURS"; break;
				case MUTT_INVALID_GLYF_HEADER_X_MIN: return "MUTT_INVALID_GLYF_HEADER_X_MIN"; break;
				case MUTT_INVALID_GLYF_HEADER_Y_MIN: return "MUTT_INVALID_GLYF_HEADER_Y_MIN"; break;
				case MUTT_INVALID_GLYF_HEADER_X_MAX: return "MUTT_INVALID_GLYF_HEADER_X_MAX"; break;
				case MUTT_INVALID_GLYF_HEADER_Y_MAX: return "MUTT_INVALID_GLYF_HEADER_Y_MAX"; break;
				case MUTT_INVALID_GLYF_HEADER_X_MIN_MAX: return "MUTT_INVALID_GLYF_HEADER_X_MIN_MAX"; break;
				case MUTT_INVALID_GLYF_HEADER_Y_MIN_MAX: return "MUTT_INVALID_GLYF_HEADER_Y_MIN_MAX"; break;
				case MUTT_INVALID_GLYF_SIMPLE_LENGTH: return "MUTT_INVALID_GLYF_SIMPLE_LENGTH"; break;
				case MUTT_INVALID_GLYF_SIMPLE_END_PTS_OF_CONTOURS: return "MUTT_INVALID_GLYF_SIMPLE_END_PTS_OF_CONTOURS"; break;
				case MUTT_INVALID_GLYF_SIMPLE_POINT_COUNT: return "MUTT_INVALID_GLYF_SIMPLE_POINT_COUNT"; break;
				case MUTT_INVALID_GLYF_SIMPLE_INSTRUCTION_LENGTH: return "MUTT_INVALID_GLYF_SIMPLE_INSTRUCTION_LENGTH"; break;
				case MUTT_INVALID_GLYF_SIMPLE_X_COORD: return "MUTT_INVALID_GLYF_SIMPLE_X_COORD"; break;
				case MUTT_INVALID_GLYF_SIMPLE_Y_COORD: return "MUTT_INVALID_GLYF_SIMPLE_Y_COORD"; break;
				case MUTT_INVALID_GLYF_COMPOSITE_LENGTH: return "MUTT_INVALID_GLYF_COMPOSITE_LENGTH"; break;
				case MUTT_INVALID_GLYF_COMPOSITE_INSTRUCTION_LENGTH: return "MUTT_INVALID_GLYF_COMPOSITE_INSTRUCTION_LENGTH"; break;
				case MUTT_INVALID_GLYF_COMPOSITE_COMPONENT_COUNT: return "MUTT_INVALID_GLYF_COMPOSITE_COMPONENT_COUNT"; break;
				case MUTT_INVALID_GLYF_COMPOSITE_GLYPH_INDEX: return "MUTT_INVALID_GLYF_COMPOSITE_GLYPH_INDEX"; break;
				case MUTT_INVALID_GLYF_COMPOSITE_FLAGS: return "MUTT_INVALID_GLYF_COMPOSITE_FLAGS"; break;
				case MUTT_INVALID_CMAP_LENGTH: return "MUTT_INVALID_CMAP_LENGTH"; break;
				case MUTT_INVALID_CMAP_VERSION: return "MUTT_INVALID_CMAP_VERSION"; break;
				case MUTT_INVALID_CMAP_ENCODING_RECORD_OFFSET: return "MUTT_INVALID_CMAP_ENCODING_RECORD_OFFSET"; break;
				case MUTT_INVALID_CMAP_ENCODING_RECORD_LENGTH: return "MUTT_INVALID_CMAP_ENCODING_RECORD_LENGTH"; break;
				case MUTT_INVALID_CMAP_ENCODING_RECORD_FORMAT: return "MUTT_INVALID_CMAP_ENCODING_RECORD_FORMAT"; break;
				case MUTT_INVALID_CMAP0_LENGTH: return "MUTT_INVALID_CMAP0_LENGTH"; break;
				case MUTT_INVALID_CMAP4_LENGTH: return "MUTT_INVALID_CMAP4_LENGTH"; break;
				case MUTT_INVALID_CMAP4_SEG_COUNT_X2: return "MUTT_INVALID_CMAP4_SEG_COUNT_X2"; break;
				case MUTT_INVALID_CMAP4_END_CODE: return "MUTT_INVALID_CMAP4_END_CODE"; break;
				case MUTT_INVALID_CMAP4_LAST_END_CODE: return "MUTT_INVALID_CMAP4_LAST_END_CODE"; break;
				case MUTT_INVALID_CMAP4_START_CODE: return "MUTT_INVALID_CMAP4_START_CODE"; break;
				case MUTT_INVALID_CMAP4_ID_RANGE_OFFSET: return "MUTT_INVALID_CMAP4_ID_RANGE_OFFSET"; break;
				case MUTT_INVALID_CMAP12_LENGTH: return "MUTT_INVALID_CMAP12_LENGTH"; break;
				case MUTT_INVALID_CMAP12_START_CHAR_CODE: return "MUTT_INVALID_CMAP12_START_CHAR_CODE"; break;
				case MUTT_INVALID_CMAP12_END_CHAR_CODE: return "MUTT_INVALID_CMAP12_END_CHAR_CODE"; break;
				case MUTT_CMAP_REQUIRES_MAXP: return "MUTT_CMAP_REQUIRES_MAXP"; break;
				case MUTT_UNKNOWN_RASTER_METHOD: return "MUTT_UNKNOWN_RASTER_METHOD"; break;
			}
		}

		MUDEF const char* mutt_platform_get_name(uint16_m platform_id) {
			switch (platform_id) {
				default: return "MU_UNKNOWN"; break;
				case MUTT_PLATFORM_UNICODE: return "MUTT_PLATFORM_UNICODE"; break;
				case MUTT_PLATFORM_MACINTOSH: return "MUTT_PLATFORM_MACINTOSH"; break;
				case MUTT_PLATFORM_ISO: return "MUTT_PLATFORM_ISO"; break;
				case MUTT_PLATFORM_WINDOWS: return "MUTT_PLATFORM_WINDOWS"; break;
				case MUTT_PLATFORM_CUSTOM: return "MUTT_PLATFORM_CUSTOM"; break;
			}
		}
		MUDEF const char* mutt_platform_get_nice_name(uint16_m platform_id) {
			switch (platform_id) {
				default: return "Unknown"; break;
				case MUTT_PLATFORM_UNICODE: return "Unicode"; break;
				case MUTT_PLATFORM_MACINTOSH: return "Macintosh"; break;
				case MUTT_PLATFORM_ISO: return "ISO"; break;
				case MUTT_PLATFORM_WINDOWS: return "Windows"; break;
				case MUTT_PLATFORM_CUSTOM: return "Custom"; break;
			}
		}

		MUDEF const char* mutt_unicode_encoding_get_name(uint16_m encoding_id) {
			switch (encoding_id) {
				default: return "MU_UNKNOWN"; break;
				case MUTT_UNICODE_1_0: return "MUTT_UNICODE_1_0"; break;
				case MUTT_UNICODE_1_1: return "MUTT_UNICODE_1_1"; break;
				case MUTT_UNICODE_ISO_IEC_10646: return "MUTT_UNICODE_ISO_IEC_10646"; break;
				case MUTT_UNICODE_2_0_BMP: return "MUTT_UNICODE_2_0_BMP"; break;
				case MUTT_UNICODE_2_0: return "MUTT_UNICODE_2_0"; break;
				case MUTT_UNICODE_VARIATION: return "MUTT_UNICODE_VARIATION"; break;
				case MUTT_UNICODE_FULL: return "MUTT_UNICODE_FULL"; break;
			}
		}
		MUDEF const char* mutt_unicode_encoding_get_nice_name(uint16_m encoding_id) {
			switch (encoding_id) {
				default: return "Unknown"; break;
				case MUTT_UNICODE_1_0: return "Unicode 1.0"; break;
				case MUTT_UNICODE_1_1: return "Unicode 1.1"; break;
				case MUTT_UNICODE_ISO_IEC_10646: return "ISO/IEC 10646"; break;
				case MUTT_UNICODE_2_0_BMP: return "Unicode 2.0 (BMP only)"; break;
				case MUTT_UNICODE_2_0: return "Unicode 2.0 (full repertoire)"; break;
				case MUTT_UNICODE_VARIATION: return "Unicode variation sequences"; break;
				case MUTT_UNICODE_FULL: return "Unicode full repertoire"; break;
			}
		}

		MUDEF const char* mutt_macintosh_encoding_get_name(uint16_m encoding_id) {
			switch (encoding_id) {
				default: return "MU_UNKNOWN"; break;
				case MUTT_MACINTOSH_ROMAN: return "MUTT_MACINTOSH_ROMAN"; break;
				case MUTT_MACINTOSH_JAPANESE: return "MUTT_MACINTOSH_JAPANESE"; break;
				case MUTT_MACINTOSH_CHINESE_TRADITIONAL: return "MUTT_MACINTOSH_CHINESE_TRADITIONAL"; break;
				case MUTT_MACINTOSH_KOREAN: return "MUTT_MACINTOSH_KOREAN"; break;
				case MUTT_MACINTOSH_ARABIC: return "MUTT_MACINTOSH_ARABIC"; break;
				case MUTT_MACINTOSH_HEBREW: return "MUTT_MACINTOSH_HEBREW"; break;
				case MUTT_MACINTOSH_GREEK: return "MUTT_MACINTOSH_GREEK"; break;
				case MUTT_MACINTOSH_RUSSIAN: return "MUTT_MACINTOSH_RUSSIAN"; break;
				case MUTT_MACINTOSH_RSYMBOL: return "MUTT_MACINTOSH_RSYMBOL"; break;
				case MUTT_MACINTOSH_DEVANAGARI: return "MUTT_MACINTOSH_DEVANAGARI"; break;
				case MUTT_MACINTOSH_GURMUKHI: return "MUTT_MACINTOSH_GURMUKHI"; break;
				case MUTT_MACINTOSH_GUJARATI: return "MUTT_MACINTOSH_GUJARATI"; break;
				case MUTT_MACINTOSH_ODIA: return "MUTT_MACINTOSH_ODIA"; break;
				case MUTT_MACINTOSH_BANGLA: return "MUTT_MACINTOSH_BANGLA"; break;
				case MUTT_MACINTOSH_TAMIL: return "MUTT_MACINTOSH_TAMIL"; break;
				case MUTT_MACINTOSH_TELUGU: return "MUTT_MACINTOSH_TELUGU"; break;
				case MUTT_MACINTOSH_KANNADA: return "MUTT_MACINTOSH_KANNADA"; break;
				case MUTT_MACINTOSH_MALAYALAM: return "MUTT_MACINTOSH_MALAYALAM"; break;
				case MUTT_MACINTOSH_SINHALESE: return "MUTT_MACINTOSH_SINHALESE"; break;
				case MUTT_MACINTOSH_BURMESE: return "MUTT_MACINTOSH_BURMESE"; break;
				case MUTT_MACINTOSH_KHMER: return "MUTT_MACINTOSH_KHMER"; break;
				case MUTT_MACINTOSH_THAI: return "MUTT_MACINTOSH_THAI"; break;
				case MUTT_MACINTOSH_LAOTIAN: return "MUTT_MACINTOSH_LAOTIAN"; break;
				case MUTT_MACINTOSH_GEORGIAN: return "MUTT_MACINTOSH_GEORGIAN"; break;
				case MUTT_MACINTOSH_ARMENIAN: return "MUTT_MACINTOSH_ARMENIAN"; break;
				case MUTT_MACINTOSH_CHINESE_SIMPLIFIED: return "MUTT_MACINTOSH_CHINESE_SIMPLIFIED"; break;
				case MUTT_MACINTOSH_TIBETAN: return "MUTT_MACINTOSH_TIBETAN"; break;
				case MUTT_MACINTOSH_MONGOLIAN: return "MUTT_MACINTOSH_MONGOLIAN"; break;
				case MUTT_MACINTOSH_GEEZ: return "MUTT_MACINTOSH_GEEZ"; break;
				case MUTT_MACINTOSH_SLAVIC: return "MUTT_MACINTOSH_SLAVIC"; break;
				case MUTT_MACINTOSH_VIETNAMESE: return "MUTT_MACINTOSH_VIETNAMESE"; break;
				case MUTT_MACINTOSH_SINDHI: return "MUTT_MACINTOSH_SINDHI"; break;
				case MUTT_MACINTOSH_UNINTERPRETED: return "MUTT_MACINTOSH_UNINTERPRETED"; break;
			}
		}
		MUDEF const char* mutt_macintosh_encoding_get_nice_name(uint16_m encoding_id) {
			switch (encoding_id) {
				default: return "Unknown"; break;
				case MUTT_MACINTOSH_ROMAN: return "Roman"; break;
				case MUTT_MACINTOSH_JAPANESE: return "Japanese"; break;
				case MUTT_MACINTOSH_CHINESE_TRADITIONAL: return "Chinese (Traditional)"; break;
				case MUTT_MACINTOSH_KOREAN: return "Korean"; break;
				case MUTT_MACINTOSH_ARABIC: return "Arabic"; break;
				case MUTT_MACINTOSH_HEBREW: return "Hebrew"; break;
				case MUTT_MACINTOSH_GREEK: return "Greek"; break;
				case MUTT_MACINTOSH_RUSSIAN: return "Russian"; break;
				case MUTT_MACINTOSH_RSYMBOL: return "RSymbol"; break;
				case MUTT_MACINTOSH_DEVANAGARI: return "Devanagari"; break;
				case MUTT_MACINTOSH_GURMUKHI: return "Gurmukhi"; break;
				case MUTT_MACINTOSH_GUJARATI: return "Gujarati"; break;
				case MUTT_MACINTOSH_ODIA: return "Odia"; break;
				case MUTT_MACINTOSH_BANGLA: return "Bangla"; break;
				case MUTT_MACINTOSH_TAMIL: return "Tamil"; break;
				case MUTT_MACINTOSH_TELUGU: return "Telugu"; break;
				case MUTT_MACINTOSH_KANNADA: return "Kannada"; break;
				case MUTT_MACINTOSH_MALAYALAM: return "Malayalam"; break;
				case MUTT_MACINTOSH_SINHALESE: return "Sinhalese"; break;
				case MUTT_MACINTOSH_BURMESE: return "Burmese"; break;
				case MUTT_MACINTOSH_KHMER: return "Khmer"; break;
				case MUTT_MACINTOSH_THAI: return "Thai"; break;
				case MUTT_MACINTOSH_LAOTIAN: return "Laotian"; break;
				case MUTT_MACINTOSH_GEORGIAN: return "Georgian"; break;
				case MUTT_MACINTOSH_ARMENIAN: return "Armenian"; break;
				case MUTT_MACINTOSH_CHINESE_SIMPLIFIED: return "Chinese (Simplified)"; break;
				case MUTT_MACINTOSH_TIBETAN: return "Tibetan"; break;
				case MUTT_MACINTOSH_MONGOLIAN: return "Mongolian"; break;
				case MUTT_MACINTOSH_GEEZ: return "Geez"; break;
				case MUTT_MACINTOSH_SLAVIC: return "Slavic"; break;
				case MUTT_MACINTOSH_VIETNAMESE: return "Vietnamese"; break;
				case MUTT_MACINTOSH_SINDHI: return "Sindhi"; break;
				case MUTT_MACINTOSH_UNINTERPRETED: return "Uninterpreted"; break;
			}
		}

		MUDEF const char* mutt_windows_encoding_get_name(uint16_m encoding_id) {
			switch (encoding_id) {
				default: return "MU_UNKNOWN"; break;
				case MUTT_WINDOWS_SYMBOL: return "MUTT_WINDOWS_SYMBOL"; break;
				case MUTT_WINDOWS_UNICODE_BMP: return "MUTT_WINDOWS_UNICODE_BMP"; break;
				case MUTT_WINDOWS_SHIFTJIS: return "MUTT_WINDOWS_SHIFTJIS"; break;
				case MUTT_WINDOWS_PRC: return "MUTT_WINDOWS_PRC"; break;
				case MUTT_WINDOWS_BIG5: return "MUTT_WINDOWS_BIG5"; break;
				case MUTT_WINDOWS_WANSUNG: return "MUTT_WINDOWS_WANSUNG"; break;
				case MUTT_WINDOWS_JOHAB: return "MUTT_WINDOWS_JOHAB"; break;
				case MUTT_WINDOWS_UNICODE: return "MUTT_WINDOWS_UNICODE"; break;
			}
		}
		MUDEF const char* mutt_windows_encoding_get_nice_name(uint16_m encoding_id) {
			switch (encoding_id) {
				default: return "Unknown"; break;
				case MUTT_WINDOWS_SYMBOL: return "Symbol"; break;
				case MUTT_WINDOWS_UNICODE_BMP: return "Unicode (BMP only)"; break;
				case MUTT_WINDOWS_SHIFTJIS: return "ShiftJIS"; break;
				case MUTT_WINDOWS_PRC: return "PRC"; break;
				case MUTT_WINDOWS_BIG5: return "Big5"; break;
				case MUTT_WINDOWS_WANSUNG: return "Wansung"; break;
				case MUTT_WINDOWS_JOHAB: return "Johab"; break;
				case MUTT_WINDOWS_UNICODE: return "Unicode (full repertoire)"; break;
			}
		}

		MUDEF const char* mutt_name_id_get_name(uint16_m name_id) {
			switch (name_id) {
				default: return "MU_UNKNOWN"; break;
				case MUTT_NAME_COPYRIGHT_NOTICE: return "MUTT_NAME_COPYRIGHT_NOTICE"; break;
				case MUTT_NAME_FONT_FAMILY: return "MUTT_NAME_FONT_FAMILY"; break;
				case MUTT_NAME_FONT_SUBFAMILY: return "MUTT_NAME_FONT_SUBFAMILY"; break;
				case MUTT_NAME_FONT_IDENTIFIER: return "MUTT_NAME_FONT_IDENTIFIER"; break;
				case MUTT_NAME_FONT_FULL: return "MUTT_NAME_FONT_FULL"; break;
				case MUTT_NAME_VERSION: return "MUTT_NAME_VERSION"; break;
				case MUTT_NAME_POSTSCRIPT: return "MUTT_NAME_POSTSCRIPT"; break;
				case MUTT_NAME_TRADEMARK: return "MUTT_NAME_TRADEMARK"; break;
				case MUTT_NAME_MANUFACTURER: return "MUTT_NAME_MANUFACTURER"; break;
				case MUTT_NAME_DESIGNER: return "MUTT_NAME_DESIGNER"; break;
				case MUTT_NAME_DESCRIPTION: return "MUTT_NAME_DESCRIPTION"; break;
				case MUTT_NAME_VENDOR_URL: return "MUTT_NAME_VENDOR_URL"; break;
				case MUTT_NAME_DESIGNER_URL: return "MUTT_NAME_DESIGNER_URL"; break;
				case MUTT_NAME_LICENSE: return "MUTT_NAME_LICENSE"; break;
				case MUTT_NAME_LICENSE_URL: return "MUTT_NAME_LICENSE_URL"; break;
				case MUTT_NAME_TYPOGRAPHIC_FAMILY: return "MUTT_NAME_TYPOGRAPHIC_FAMILY"; break;
				case MUTT_NAME_TYPOGRAPHIC_SUBFAMILY: return "MUTT_NAME_TYPOGRAPHIC_SUBFAMILY"; break;
				case MUTT_NAME_COMPATIBLE_FULL: return "MUTT_NAME_COMPATIBLE_FULL"; break;
				case MUTT_NAME_SAMPLE_TEXT: return "MUTT_NAME_SAMPLE_TEXT"; break;
				case MUTT_NAME_POSTSCRIPT_CID_FINDFONT: return "MUTT_NAME_POSTSCRIPT_CID_FINDFONT"; break;
				case MUTT_NAME_WWS_FAMILY: return "MUTT_NAME_WWS_FAMILY"; break;
				case MUTT_NAME_WWS_SUBFAMILY: return "MUTT_NAME_WWS_SUBFAMILY"; break;
				case MUTT_NAME_LIGHT_BACKGROUND_PALETTE: return "MUTT_NAME_LIGHT_BACKGROUND_PALETTE"; break;
				case MUTT_NAME_DARK_BACKGROUND_PALETTE: return "MUTT_NAME_DARK_BACKGROUND_PALETTE"; break;
			}
		}
		MUDEF const char* mutt_name_id_get_nice_name(uint16_m name_id) {
			switch (name_id) {
				default: return "Unknown"; break;
				case MUTT_NAME_COPYRIGHT_NOTICE: return "Copyright notice"; break;
				case MUTT_NAME_FONT_FAMILY: return "Font family"; break;
				case MUTT_NAME_FONT_SUBFAMILY: return "Font subfamily"; break;
				case MUTT_NAME_FONT_IDENTIFIER: return "Unique font identifier"; break;
				case MUTT_NAME_FONT_FULL: return "Full font name"; break;
				case MUTT_NAME_VERSION: return "Version"; break;
				case MUTT_NAME_POSTSCRIPT: return "Postscript name"; break;
				case MUTT_NAME_TRADEMARK: return "Trademark"; break;
				case MUTT_NAME_MANUFACTURER: return "Manufacturer"; break;
				case MUTT_NAME_DESIGNER: return "Designer"; break;
				case MUTT_NAME_DESCRIPTION: return "Description"; break;
				case MUTT_NAME_VENDOR_URL: return "Vendor URL"; break;
				case MUTT_NAME_DESIGNER_URL: return "Designer URL"; break;
				case MUTT_NAME_LICENSE: return "License"; break;
				case MUTT_NAME_LICENSE_URL: return "License URL"; break;
				case MUTT_NAME_TYPOGRAPHIC_FAMILY: return "Typographic family"; break;
				case MUTT_NAME_TYPOGRAPHIC_SUBFAMILY: return "Typographic subfamily"; break;
				case MUTT_NAME_COMPATIBLE_FULL: return "Compatible full"; break;
				case MUTT_NAME_SAMPLE_TEXT: return "Sample text"; break;
				case MUTT_NAME_POSTSCRIPT_CID_FINDFONT: return "PostScript CID findfont"; break;
				case MUTT_NAME_WWS_FAMILY: return "WWS family"; break;
				case MUTT_NAME_WWS_SUBFAMILY: return "WWS subfamily"; break;
				case MUTT_NAME_LIGHT_BACKGROUND_PALETTE: return "Light background palette"; break;
				case MUTT_NAME_DARK_BACKGROUND_PALETTE: return "Dark background palette"; break;
			}
		}

		#endif /* MUTT_NAMES */

	MU_CPP_EXTERN_END
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

