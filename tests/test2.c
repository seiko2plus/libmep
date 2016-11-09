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

# include "test.h"

# define ALLOC_NUM 1000

void test_main (TEST_POOL_T *mp)
{
    void **ptr_arr;
    uint  *size_arr;
    uint   i;
    size_t total_alloc = 0;
#   ifdef TEST_CHECK_STATS
    mep_stats_t stats;
#   endif

    TEST_PRINT("Allocate ptr array for %u", ALLOC_NUM);
    ptr_arr = TEST_ALLOC(mp, sizeof(void*) * ALLOC_NUM);
    TEST_ASSERT(ptr_arr != NULL);
    size_arr   = TEST_ALLOC(mp, sizeof(uint) * ALLOC_NUM);
    TEST_ASSERT(size_arr != NULL);
    TEST_SUCS("[PASSED]");

    TEST_PRINT("Fill %u random sizes", ALLOC_NUM);
    for (i = 0; i < ALLOC_NUM; i++) {
        size_arr[i]  = test_rand();
        total_alloc += size_arr[i];
        ptr_arr[i]   = TEST_ALLOC(mp, size_arr[i]);
        TEST_ASSERT(ptr_arr[i] != NULL);
        test_fill(ptr_arr[i], size_arr[i]);
    }
    TEST_SUCS("[PASSED]");

    TEST_PRINT("Check and free %u random sizes with size %lu", ALLOC_NUM, total_alloc);
    for (i = 0; i < ALLOC_NUM; i++) {
        TEST_ASSERT(0 == test_check(ptr_arr[i], size_arr[i]));
        TEST_FREE(mp, ptr_arr[i]);
    }
    TEST_SUCS("[PASSED]");

    TEST_FREE(mp, ptr_arr);
    TEST_FREE(mp, size_arr);


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
}
