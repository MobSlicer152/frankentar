/**
 * @file write.h
 * @author MobSlicer152 (brambleclaw1414@gmail.com)
 * @brief Writing functions for Frankentar archives
 * 
 * @copyright Copyright (c) MobSlicer152 2021
 * This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
 * 1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#pragma once

#ifndef FRANKENTAR_WRITE_H
#define FRANKENTAR_WRITE_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "frankentar.h"

/**
 * @brief Converts an entry into a buffer
 * 
 * @param ent is the entry to convert
 * @param len_ret returns the length of the returned buffer or -1 (error)
 * 
 * @return Returns either `NULL` or a buffer containing the information of the
 *  entry in a form suitable for writing to a file
 */
extern void *ftar_ent_to_raw(struct ftar_ent *ent, size_t *len_ret);

/**
 * @brief Converts the given Frankentar structure into a buffer
 * 
 * @param tar is the structure to convert
 * @param len_ret returns the length of the buffer or -1 (error)
 * 
 * @return Returns `NULL` or the buffer
 */
extern void *ftar_to_raw(struct ftar *tar, size_t *len_ret);

#ifdef __cplusplus
}
#endif

#endif /* !FRANKENTAR_WRITE_H */
