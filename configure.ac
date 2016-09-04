# Copyright Sayed Adel. All rights reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
# IN THE SOFTWARE.

#########
# Install
#############

AC_INIT(libmep, 1.0.0, libmep@imavr.com, libmep, https://github.com/imavr/libmep)
AC_PREREQ(2.52)

AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([foreign subdir-objects])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/mep.c])

#########
# Check Programs
###################
AC_PROG_CC
AC_C_CONST
AC_C_INLINE

#########
# Check Headers
###################
AC_CHECK_HEADERS([stddef.h stdint.h stdlib.h string.h unistd.h])

#########
# Check Types
###################
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T

#########
# Check Functions
###################
AC_CHECK_FUNC(posix_memalign, AC_DEFINE(MEP_HAVE_POSIX_ALIGN))
AC_CHECK_FUNC(memalign, AC_DEFINE(MEP_HAVE_MEMALIGN))


#########
# Memory Alignment
###################
AC_ARG_ENABLE(alignment,
        AC_HELP_STRING([--set-alignment], [Set memory alignment (default=16)]),, set_alignment=16)

if test x$set_alignment != x16; then
    AC_DEFINE([MEP_ALIGN_SIZE], [x$set_alignment])
fi

#########
# Enable Max Size
###################
AC_ARG_ENABLE(debug,
        AC_HELP_STRING([--enable-maxsize], Increase line and allocation size to max [(default=no)]),, enable_maxsize=no)

if test x$enable_maxsize = xyes; then
    AC_DEFINE(MEP_MAX_SIZE)
fi


#########
# Enable Debug
###################
AC_ARG_ENABLE(debug,
        AC_HELP_STRING([--enable-debug], [Build debug library (default=no)]),, enable_debug=no)

if test x$enable_debug = xyes; then
    GCJFLAGS="-g -O0"
    CFLAGS="-O0 -Wall -Werror"
else
    CFLAGS="-O3"
    AC_DEFINE(NDEBUG)
fi

AC_SUBST(CFLAGS)

#########
# Enable Tests
###################

# ---------------------------------------------------------------------
# Conditional variables
# ---------------------------------------------------------------------

AC_ARG_ENABLE(tests,
        AC_HELP_STRING( [--enable-tests], [Build test programs (default=no)]),, enable_tests=no)

AM_CONDITIONAL(MEP_TESTS, test x$enable_tests = xyes)

#########
# Enable Valgrind
###################

AC_ARG_ENABLE(valgrind,
        AC_HELP_STRING( [--enable-valgrind], [Pass mep allocators to valgrind (default=no)]),, enable_valgrind=no)

if test x$enable_valgrind = xyes; then
    AC_DEFINE(MEP_VALGRIND)
fi


AM_MAINTAINER_MODE

##########
# Output
###################
LT_INIT
AC_CONFIG_FILES(Makefile)
AC_OUTPUT

echo
echo
echo "Configuration:"
echo ""
echo "  LIBDIR:               $libdir"
echo "  DEBUG:                $enable_debug"
echo "  VALGRIND:             $enable_valgrind"
echo "  TESTS:                $enable_tests"
echo "  MAX SIZE:             $enable_maxsize"
echo "  MEMORY ALIGNMENT:     $set_alignment"
