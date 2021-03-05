/**
 * @file read.h
 * @author MobSlicer152 (brambleclaw1414@gmail.com)
 * @brief Reading functions for Frankentar archives
 * 
 * @copyright Copyright (c) MobSlicer152 2021
 * This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
 * 1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#pragma once

#ifndef FRANKENTAR_READ_H
#define FRANKENTAR_READ_H 1

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#include "frankentar.h"
#include "util.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Load a Frankentar archive from `tar`
 * 
 * @param tar is a pointer to the start of an archive present in memory
 * @param tar_len is the length of the memory containing the archive
 * 
 * @return Returns a pointer to a filled out `ftar` structure or `NULL`
 */
extern struct ftar *ftar_load(void *tar, size_t tar_len);

/**
 * @brief Find an entry with the given name in `tar`
 * 
 * @param tar is the Frankentar archive structure to search
 * @param index is, if non-`NULL`, the index of the file if located
 * @param name is the name of the file to locate
 * 
 * @return Returns a Frankentar entry or `NULL` depending on whether the entry
 *  could be found
 */
extern struct ftar_ent *ftar_find(struct ftar *tar, long *index, const char *name, ...);

/**
 * @brief Gets the checksum for a given entry.
 * 
 * @param ent is the entry to checksum.
 *
 * @return Returns the checksum for the entry, or, in the event
 *  that the entry's structure has its `checksum` field filled in,
 *  1 or 0 to state whether the checksums match. If `ent` is NULL,
 *  returns -1.
 */
extern long ftar_checksum(struct ftar_ent *ent);

/**
 * @brief Free a Frankentar structure
 *
 * @param tar is the Frankentar structure to free
 */
extern void ftar_free(struct ftar *tar);

#ifdef __cplusplus
}
#endif

#endif /* !FRANKENTAR_READ_H */
