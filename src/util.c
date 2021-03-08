#define STB_SPRINTF_IMPLEMENTATION
#include "frankentar/util.h"

#ifdef __cplusplus
extern "C" {
#endif

char *fmt_text_va(size_t *len_ret, const char *fmt, va_list args)
{
	size_t len;
	char *buf;
	va_list ap;

	errno = 0;

	/* Check our parameters */
	if (!len_ret || !fmt || !args) {
		errno = EINVAL;
		return fmt;
	}

	/* Copy the arglist */
	va_copy(ap, args);

	/* Determine how large the buffer has to be */
	len = stbsp_vsnprintf(NULL, 0, fmt, args) + 1;
	if (!len) {
		errno = E2BIG;
		len = 512;
	}

	/* Now we know how big the buffer will be */
	buf = calloc(len, sizeof(char));
	if (!buf) {
		errno = ENOMEM;
		len = -1;
		return fmt;
	}

	/* Now put the string in the buffer and return */
	stbsp_vsnprintf(buf, len, fmt, ap);
	if (!buf) {
		errno = E2BIG;
		len = -1;
		return fmt;
	}
	*len_ret = len;

	errno = 0;

	return buf;
}

char *fmt_text(size_t *len_ret, const char *fmt, ...)
{
	va_list args;
	char *fmt_ptr;

	errno = 0;

	/* Check everything */
	if (!len_ret || !fmt) {
		errno = EINVAL;
		return fmt;
	}

	va_start(args, fmt);
	fmt_ptr = fmt_text_va(len_ret, fmt, args);
	va_end(args);

	errno = 0;

	return fmt_ptr;
}

void err_exit(int code, const char *msg, ...)
{
	va_list args;
	char *fmt;
	size_t len;

	/* Check args */
	if (!msg)
		fmt = fmt_text(&len, "Unknown error occured\n");

	/* Format the message */
	va_start(args, msg);
	fmt = fmt_text_va(&len, msg, args);
	va_end(args);

	/* Print it out */
	fprintf(stderr, "%s", fmt);

	/* Now exit */
	exit(code);
}

#ifdef __cplusplus
}
#endif
