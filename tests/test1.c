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

#include "test.h"

void test_main (TEST_POOL_T *mp)
{
    void   *ptr, *rptr;
    size_t  size;
#   ifdef TEST_CHECK_STATS
    mep_stats_t stats;
#   endif

    size = TEST_LINE_SIZE;

    TEST_PRINT("Allocating %lu", size);
    ptr = TEST_ALLOC(mp, size);
    TEST_ASSERT(ptr != NULL);
    TEST_SUCS("[PASSED]");

    TEST_PRINT("Length");
    TEST_ASSERT(size == TEST_LEN(ptr));
    TEST_SUCS("[PASSED]");

    TEST_PRINT("Sizeof");
    TEST_ASSERT(MEP_ALIGN(size) == TEST_SIZEOF(ptr));
    TEST_SUCS("[PASSED]");

    TEST_PRINT("Fill");
    test_fill(ptr, size);
    TEST_SUCS("[PASSED]");

    TEST_PRINT("Check");
    TEST_ASSERT(0 == test_check(ptr, size));
    TEST_SUCS("[PASSED]");

    size /= 3;
    TEST_PRINT("Reallocating %lu", size);
    rptr = TEST_REALLOC(mp, ptr, size);
    TEST_ASSERT(rptr == ptr);
    TEST_SUCS("[PASSED]");

    TEST_PRINT("Check");
    TEST_ASSERT(0 == test_check(rptr, size));
    TEST_SUCS("[PASSED]");

    TEST_PRINT("Length");
    TEST_ASSERT(size == TEST_LEN(ptr));
    TEST_SUCS("[PASSED]");

    TEST_PRINT("Sizeof");
    TEST_ASSERT(MEP_ALIGN(size) == TEST_SIZEOF(rptr));
    TEST_SUCS("[PASSED]");

#   ifdef TEST_CHECK_STATS
    TEST_PRINT("Stats");
    mep_stats(mp, &stats);
    TEST_ASSERT(stats.lines == 1);
    TEST_ASSERT(stats.use_count   == 1);
    TEST_ASSERT(stats.unuse_count == 1);
    TEST_SUCS("[PASSED]");
#   endif

    TEST_PRINT("Free");
    TEST_FREE(mp, rptr);
    TEST_SUCS("[PASSED]");

#   ifdef TEST_CHECK_STATS
    TEST_PRINT("Stats");
    mep_stats(mp, &stats);
    TEST_ASSERT(stats.lines == 1);
    TEST_ASSERT(stats.use_count   == 0);
    TEST_ASSERT(stats.unuse_count == 1);
    TEST_ASSERT(stats.available == TEST_LINE_SIZE);
    TEST_ASSERT(stats.total == TEST_LINE_SIZE);
    TEST_SUCS("[PASSED]");
#   endif

    TEST_PRINT("Overflow detection");
    ptr = TEST_CALLOC(mp, -1, -1);
    TEST_ASSERT(ptr == NULL);
    TEST_SUCS("[PASSED]");
}
