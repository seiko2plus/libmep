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
    mep_stat_t stat;
#   endif

    size = TEST_LINE_SIZE;

    TEST_PRINT("Allocating %lu", size);
    ptr = TEST_ALLOC(mp, size);
    TEST_ASSERT(ptr != NULL);
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

#   ifdef TEST_CHECK_STATS
    TEST_PRINT("Stats");
    mep_stat(mp, &stat);
    TEST_ASSERT(stat.lines == 1);
    TEST_ASSERT(stat.use_count   == 1);
    TEST_ASSERT(stat.unuse_count == 1);
    TEST_SUCS("[PASSED]");
#   endif

    TEST_PRINT("Free");
    TEST_FREE(mp, rptr);
    TEST_SUCS("[PASSED]");

#   ifdef TEST_CHECK_STATS
    TEST_PRINT("Stats");
    mep_stat(mp, &stat);
    TEST_ASSERT(stat.lines == 1);
    TEST_ASSERT(stat.use_count   == 0);
    TEST_ASSERT(stat.unuse_count == 1);
    TEST_ASSERT(stat.available == TEST_LINE_SIZE);
    TEST_ASSERT(stat.total == TEST_LINE_SIZE);
    TEST_SUCS("[PASSED]");
#   endif
}
