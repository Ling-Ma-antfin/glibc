/* Test for the long double conversions in *warn* functions.
   Copyright (C) 2018 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#include <err.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <support/check.h>
#include <support/xmemstream.h>

enum {WARN, WARNX, VWARN, VWARNX};

static void
do_one_test (int select, const char *format, va_list args,
	     long double arg1, double arg2, long double arg3,
	     double arg4, const char *expected)
{
  /* Prepare in-memory buffer to hold the output.  */
  struct xmemstream stream;
  xopen_memstream (&stream);
  FILE *old_stderr = stderr;
  stderr = stream.out;

  /* Write to the buffer using one of the *warn* functions.  */
  errno = 0;
  switch (select)
    {
      case WARN:
	warn (format, arg1, arg2, arg3, arg4);
	break;
      case WARNX:
	warnx (format, arg1, arg2, arg3, arg4);
	break;
      case VWARN:
	vwarn (format, args);
	break;
      case VWARNX:
	vwarnx (format, args);
	break;
    }

  stderr = old_stderr;

  /* Close the in-memory stream and check the output buffer.  */
  xfclose_memstream (&stream);
  if (strcmp (stream.buffer, expected) != 0)
    FAIL_EXIT1 ("error:\n"
		"    expected: '%s'\n"
		"      actual: '%s'\n",
		expected, stream.buffer);

  if (stream.buffer != NULL)
    free (stream.buffer);
}

static void
do_test_call_varg (const char *format, ...)
{
  va_list args;

  va_start (args, format);
  do_one_test (VWARN, format, args, 0, 0, 0, 0,
	       "tst-ldbl-warn: "
	       "-1.000000 - -2.000000 - -3.000000 - -4.000000: Success\n");
  va_end (args);

  va_start (args, format);
  do_one_test (VWARNX, format, args, 0, 0, 0, 0,
	       "tst-ldbl-warn: "
	       "-1.000000 - -2.000000 - -3.000000 - -4.000000\n");
  va_end (args);
}

static void
do_test_call_rarg (const char *format, long double arg1, double arg2,
		   long double arg3, double arg4)
{
  va_list args;
  memset (&args, 0, sizeof (args));
  do_one_test (WARN, format, args, arg1, arg2, arg3, arg4,
	       "tst-ldbl-warn: "
	       "-1.000000 - -2.000000 - -3.000000 - -4.000000: Success\n");
  do_one_test (WARNX, format, args, arg1, arg2, arg3, arg4,
	       "tst-ldbl-warn: "
	       "-1.000000 - -2.000000 - -3.000000 - -4.000000\n");
}

static int
do_test (void)
{
  long double arg1 = -1;
  long double arg3 = -3;
  double arg2 = -2;
  double arg4 = -4;

  do_test_call_rarg ("%Lf - %f - %Lf - %f", arg1, arg2, arg3, arg4);
  do_test_call_varg ("%Lf - %f - %Lf - %f", arg1, arg2, arg3, arg4);

  return 0;
}

#include <support/test-driver.c>
