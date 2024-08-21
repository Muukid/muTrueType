/*
muTrueType.h - Muukid
Public domain single-file C library for reading, rendering, and placing TrueType data.
https://github.com/Muukid/muTrueType
No warranty implied; use at your own risk.

Licensed under MIT License or public domain, whichever you prefer.
More explicit license information at the end of file.

@DOCBEGIN

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

# Known bugs and limitations

This section covers all of the known bugs and limitations of mutt.

## Instruction support

Currently, mutt does not have any built-in way to execute any TrueType instructions.

## Licensing of demo resources

The resources used by the demos may differ from licensing of the demos themselves; in that context, their licenses apply, with licensing of each file available as a separate file with the same name, but with no filename extension.

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
			typedef struct muttLoca muttLoca;
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

				// @DOCLINE * `@NLFT* post` - a pointer to the [post table](#post-table).
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

		/* Loading / Deloading */

			// Initializes all flag/result states of each table to "failed to find"
			void mutt_InitTables(muttFont* font, muttLoadFlags load_flags) {
				// maxp
				font->maxp_res = (load_flags & MUTT_LOAD_MAXP) ? MUTT_FAILED_FIND_TABLE : 0;
				font->fail_load_flags |= (load_flags & MUTT_LOAD_MAXP);
				// head
				font->head_res = (load_flags & MUTT_LOAD_HEAD) ? MUTT_FAILED_FIND_TABLE : 0;
				font->fail_load_flags |= (load_flags & MUTT_LOAD_HEAD);
			}

			// Does one pass through each table load
			void mutt_LoadTables(muttFont* font, muByte* data) {
				// Loop through each table
				for (uint16_m i = 0; i < font->directory->num_tables; ++i) {
					// Get record information
					muttTableRecord rec = font->directory->records[i];

					// Do things based on table tag
					switch (rec.table_tag_u32) {
						default: break;

						// maxp
						case 0x6D617870: {
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
								font->fail_load_flags ^= MUTT_LOAD_MAXP;
							} else {
								font->fail_load_flags |= MUTT_LOAD_MAXP;
								font->load_flags ^= MUTT_LOAD_MAXP;
							}
						} break;

						// head
						case 0x68656164: {
							// Skip if already processed
							if (font->head_res != MUTT_FAILED_FIND_TABLE) {
								break;
							}

							// Load
							font->head_res = mutt_LoadHead(font, &data[rec.offset], rec.length);
							if (font->head) {
								font->load_flags |= MUTT_LOAD_HEAD;
								font->fail_load_flags ^= MUTT_LOAD_HEAD;
							} else {
								font->fail_load_flags |= MUTT_LOAD_HEAD;
								font->load_flags ^= MUTT_LOAD_HEAD;
							}
						} break;
					}
				}
			}

			// Deallocates all loaded tables
			void mutt_DeloadTables(muttFont* font) {
				// maxp + head
				if (font->maxp) {
					mu_free(font->maxp);
				}
				if (font->head) {
					mu_free(font->head);
				}
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
				mutt_LoadTables(font, data);

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

