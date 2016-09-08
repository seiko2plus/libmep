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

#include "mep.h"
#include <string.h>

char *mep_strdup(mep_t *mp, const char *s)
{
    char  *dst;
    size_t len = strlen(s) + 1;

    if (NULL == (dst = mep_alloc(mp, len)))
        return NULL;

    return memcpy(dst, s, len);
}

char *mep_strndup(mep_t *mp, const char *s, size_t n)
{
    char  *dst;
    size_t len = strlen(s);

    if (n < len)
        len = n;

    if (NULL == (dst = mep_alloc(mp, len + 1)))
        return NULL;

    dst[len] = '\0';
    return memcpy(dst, s, len);
}
