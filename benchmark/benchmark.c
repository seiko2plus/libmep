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

# include "benchmark.h"

static void print_helper(void)
{
    printf(
        "Libmep Benchmark: \n"
        "   -s, --line-size    memory pool line size in bytes\n"
    );
    fflush(stdout);
}


int main(int argc, char **argv)
{
    mep_t *mp;
    size_t ln_size;
    uint64_t started;

    if (argc < 3) {
        print_helper();
        return 0;
    }

    if (0 != strcmp(argv[1], "-s") && 0 != strcmp(argv[1], "--line-size")) {
        TEST_ERR("Invalid arg");
        print_helper();
        return 0;
    }

    if (1 > (ln_size = atol(argv[2]))) {
        TEST_ERR("Invalid arg, line size in bytes");
        print_helper();
        return 0;
    }


    printf("Benchmark started....\n");
    {
        started = now();
    }

    mp = mep_new(NULL, ln_size);
    if (mp == NULL) {
        TEST_ERR("Memory pool fail to allocate %u", ln_size);
        return 0;
    }

    bench_main(mp);
    mep_destroy(mp);

    printf("Benchmark stopped, it takes %u \n", now() - started);

    return 0;
}


uint64_t now(void)
{
    struct timeval t;
    struct timezone tzp;
    gettimeofday(&t, &tzp);
    return t.tv_sec + t.tv_usec;
}

/*double get_time()
{
    LARGE_INTEGER t, f;
    QueryPerformanceCounter(&t);
    QueryPerformanceFrequency(&f);
    return (double)t.QuadPart/(double)f.QuadPart;
}
*/
