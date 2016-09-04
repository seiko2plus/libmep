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

test_h_t test_h = {NULL, 0, 0};

int main(void) {
    test_h.mp = TEST_NEW_POOL(NULL, TEST_LINE_SIZE);
    TEST_ASSERT(test_h.mp != NULL);

    test_h.source = (uint) time(NULL);

    srand(test_h.source);

    test_main(test_h.mp);

    TEST_FREE_POOL(test_h.mp);
    return 0;
}

void test_fill(void *ptr, size_t size)
{
    size_t i;
    uint8_t *u = ptr;

    for (i = 0; i < size; i++)
        u[i] = i % 255;
}

int test_check(void *ptr, size_t size)
{
    size_t i;
    uint8_t *u = ptr;

    for (i = 0; i < size; i++) {
        if (u[i] != i % 255)
            return -1;
    }
    return 0;
}

int test_rand(void)
{
    if (0 == (++test_h.tik % TEST_SEED_EVERY) ) {
        test_h.source += (uint) time(NULL) + test_h.tik;
        srand(test_h.source);
    }
    return (rand() + 1) % TEST_MAX_RAND;
}
