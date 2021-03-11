/**
 * @file frankentar.h
 * @author MobSlicer152 (brambleclaw1414@gmail.com)
 * @brief Frankentar is an archive format designed for use in the Purpl game
 *  engine. It is essentially a modified version of the tar format.
 * 
 * @copyright Copyright (c) MobSlicer152 2021
 * This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
 * 1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 * 
 * All numbers are little endian because only little endian arm64 and x86_64
 *  are supported (most other architectures are embedded or dead anyways)
 */

#pragma once

#ifndef FRANKENTAR_H
#define FRANKENTAR_H 1

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Magic value to be put at the start of the file (\0 is implied in
 *  string literal)
 */
#define FTAR_MAGIC "frankentar"

/**
 * @brief Length of magic value
 */
#define FTAR_MAGIC_LEN 11

/**
 * @brief The size of one block
 */
#define FTAR_BLOCK_SIZE 512

/** File mode macros */
#define FTAR_MODE_EXEC (1) /** Executable */
#define FTAR_MODE_WRITE (1 << 1) /** Readable */
#define FTAR_MODE_READ (1 << 2) /** Writable */
#define FTAR_MODE_RDWR (FTAR_MODE_READ | FTAR_MODE_WRITE) /** Read-write */
#define FTAR_MODE_RDEX (FTAR_MODE_READ | FTAR_MODE_EXEC) /** Read-exec */
#define FTAR_MODE_FULL \
	(FTAR_MODE_READ | FTAR_MODE_WRITE | FTAR_MODE_EXEC) /** Full access */

#define FTAR_SET_MODE_USER(mode) (((mode) & 0b111) << 6) /** Set the user mode bit */
#define FTAR_SET_MODE_GROUP(mode) (((mode) & 0b111) << 3) /** Set the group mode bit */
#define FTAR_SET_MODE_OTHERS(mode) ((mode) & 0b111) /** Set the bit for everyone */

#define FTAR_GET_MODE_USER(mode) ((mode & 0b111000000) >> 6) /** Get the value of the user mode bit */
#define FTAR_GET_MODE_GROUP(mode) ((mode & 0b111000) >> 3) /** Get the value of the group mode bit */
#define FTAR_GET_MODE_OTHERS(mode) ((mode & 0b111)) /** Get the value of the mode bit for everyone */

/**
 * @brief A Frankentar entry (pretty much the same as tar but some fields are
 *  size_t instead of `char` arrays, since only LE will be dealt with, and
 *  owner UID/GID aren't present since Windows doesn't work like that)
 */
struct ftar_ent {
	char name[100]; /**< File name */
	short mode; /**< File mode */
	size_t size; /**< File size in bytes */
	long mtime; /**< Last modification time */
	long checksum; /**< Checksum of above values */
	char type; /**< File type flag */
	char link[100]; /**< Link name */
	char *data; /**< The file itself (obviously not stored as a pointer) */
};

struct ftar {
	char magic[FTAR_MAGIC_LEN]; /**< Magic signature */
	size_t ent_count; /**< The number of entries found in the archive */
	struct ftar_ent **entries; /**< The entries in the archive */
};

#ifdef __cplusplus
}
#endif

#endif /* !FRANKENTAR_H */
