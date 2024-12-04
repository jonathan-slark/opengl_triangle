#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

void
terminate(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);

    /* Make the trailing newline character optional */
    if (fmt[strlen(fmt) - 1] != '\n')
	fputc('\n', stderr);

    exit(EXIT_FAILURE);
}
