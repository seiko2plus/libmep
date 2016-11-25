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

########
# libmep
##################

ACLOCAL_AMFLAGS     = -I m4
include_HEADERS     = include/mep.h

AM_CPPFLAGS         = -I$(top_srcdir)/include \
                      -I$(top_srcdir)/src
CLEANFILES          =

lib_LTLIBRARIES     = libmep.la
libmep_la_CFLAGS    = @CFLAGS@
libmep_la_LDFLAGS   = -no-undefined -version-info 1:0:0
libmep_la_SOURCES   = src/mep.c \
                      src/alloc.c \
                      src/realloc.c \
                      src/free.c \
                      src/string.c

MEP_TESTS_N         = mep_test1 mep_test2 mep_test3
TESTS               = $(MEP_TESTS_N)
check_PROGRAMS      = $(MEP_TESTS_N)

MEP_TEST_COMMON     = tests/test.c

mep_test1_SOURCES   = $(MEP_TEST_COMMON) tests/test1.c
mep_test1_LDADD     = libmep.la

mep_test2_SOURCES   = $(MEP_TEST_COMMON) tests/test2.c
mep_test2_LDADD     = libmep.la

mep_test3_SOURCES   = $(MEP_TEST_COMMON) tests/test3.c
mep_test3_LDADD     = libmep.la

if MEP_BENCHMARK
noinst_PROGRAMS                = mep_benchmark_alloc
mep_benchmark_alloc_SOURCES    = benchmark/benchmark.h benchmark/benchmark.c benchmark/alloc.c
mep_benchmark_alloc_LDADD      = libmep.la
endif
