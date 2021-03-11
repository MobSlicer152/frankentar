/**
 * @file util.h
 * @author MobSlicer152 (brambleclaw1414@gmail.com)
 * @brief Internal utility functions
 * 
 * @copyright Copyright (c) MobSlicer152 2021
 * This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
 * 1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#pragma once

#ifndef FRANKENTAR_UTIL_H
#define FRANKENTAR_UTIL_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#include "stb_sprintf.h"

/**
 * @brief Get the base name of `path`
 */
#if _WIN32 && _MSC_VER /*
			* MSVC is the only (supported) Windows compiler that
			* uses backslashes in __FILE__
			*/
#define FTAR_GET_BASENAME(path) \
	(strrchr(path, '\\') ? strrchr(path, '\\') + 1 : path)
#else
#define FTAR_GET_BASENAME(path) \
	(strrchr(path, '/') ? strrchr(path, '/') + 1 : path)
#endif

/**
 * @brief Formats text as `vsprintf` would
 * 
 * @param len_ret will receive the length of the buffer (if it's -1, returns `fmt`)
 * @param fmt is the `printf`-style format string to be formatted
 * @param args is the variable argument structure that would be given to 
 *  vsprintf`
 * 
 * @return Returns either a buffer containing the formatted string, or, in the
 *  event that that doesn't work, the value of `fmt`
 * 
 * This function can be used as a simpler way of formatting text. It returns
 *  a buffer of either the necessary length for the formatted string or a
 *  large value that should be good enough as a fallback.  Always `free` the buffer,
 *  unless len_ret is -1 and `fmt` can't be freed in that way.
 */
extern char *fmt_text_va(size_t *len_ret, const char *fmt, va_list args);

/**
 * @brief Formats text as `sprintf` would
 * 
 * @param len_ret will receive the length of the buffer (if it's -1, returns `fmt`)
 * @param fmt is the `printf`-style format string to be formatted
 * 
 * @return Returns either a buffer containing the formatted string, or, in the
 *  event that that doesn't work, the value of `fmt`
 * 
 * This function is a convenient way of formatting text. It gives you a buffer
 *  large enough for the formatted message, which is more convenient than
 *  `sprintf`. Always `free` the buffer, unless len_ret is -1 and `fmt`
 *  can't be freed in that way.
 */
extern char *fmt_text(size_t *len_ret, const char *fmt, ...);

/**
 * @brief Print and error message and exit
 * 
 * @param code is the code to exit with
 * @param msg is the message to print (`printf`-style formatting available)
 */
extern void
#ifdef _MSC_VER
__declspec(noreturn)
#else
__attribute__((noreturn))
#endif
err_exit(int code, const char *msg, ...);

/**
 * @brief Get a yes or no response from `stdin`
 * 
 * @param message is the question to ask
 * 
 * @return Returns `true` or `false` depending on the user's response, or if
 *  there's an error
 */
extern bool get_y_or_n(const char *message, ...);

#ifdef __cplusplus
}
#endif

#endif /* !FRANKENTAR_UTIL_H */
