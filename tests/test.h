/* Copyright Sayed Adel. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
*/

# ifndef TEST_H
# define TEST_H

# include "../src/mep_p.h"
# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <time.h>

# if defined(_MSC_VER) && _MSC_VER < 1600
#   include "../src/stdint-msvc2008.h"
# else
#   include <stdint.h>
# endif

# define TEST_ASSERT_ARG (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__), __LINE__,__func__

# define TEST_SUCS(STR, ...)  fprintf(stdout, "\x1B[32m" STR "\x1B[0m \n", ##__VA_ARGS__)
# define TEST_PRINT(STR, ...) fprintf(stdout, STR "\n", ##__VA_ARGS__)
# define TEST_ERR(STR, ...)   fprintf(stderr, STR "\n", ##__VA_ARGS__)

# define TEST_ASSERT(X) \
do { \
    if (!(X)) {  \
        TEST_ERR("Assert %s:%d (%s) -> %s ", TEST_ASSERT_ARG, #X); \
        abort(); \
    } \
} while(0)

# define TEST_NEW_POOL(PARENT, SIZE)    mep_new(PARENT, SIZE)
# define TEST_FREE_POOL(POOL)           mep_destroy(POOL)
# define TEST_ALLOC(POOL, SIZE)         mep_alloc(POOL, SIZE)
# define TEST_CALLOC(POOL, COUNT, SIZE) mep_calloc(POOL, COUNT, SIZE)
# define TEST_REALLOC(POOL, PTR, SIZE)  mep_realloc(POOL, PTR, SIZE)
# define TEST_FREE(POOL, PTR)           mep_free(POOL, PTR)
# define TEST_SIZEOF(PTR)               mep_sizeof(PTR)
# define TEST_LEN(PTR)                  mep_len(PTR)
# define TEST_ALIGN(X)                  MEP_ALIGN(X)

# define TEST_POOL_T       mep_t
# define TEST_LINE_SIZE    (1024 * 1024 * 10)
# define TEST_CHECK_STATS  1
# define TEST_SEED_EVERY   10
# define TEST_MAX_RAND     (1024 * 100)

typedef struct {
    TEST_POOL_T *mp;
    uint  tik;
    uint  source;
} test_h_t;

extern test_h_t test_h;

void   test_main       (TEST_POOL_T *mp);
void   test_fill       (void *ptr, size_t size);
int    test_check      (void *ptr, size_t size);
int    test_rand       (void);
# endif /* TEST_H */
