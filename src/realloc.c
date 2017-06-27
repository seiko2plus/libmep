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

# include "mep_p.h"

/************************ Private Declare  ******************************/

static int  __take_next (mep_t *mp, mep_chunk_t *ck, uint32_t size);

/***********************************************************************/

void *mep_realloc(mep_t *mp, void *ptr, size_t size)
{
    mep_chunk_t *ck;
    void        *nptr;
    uint32_t    a_size;

    assert(mp != NULL);

    if (ptr == NULL) {
        if (unlikely(size == 0))
            return NULL;
        return mep_alloc(mp, size);
    }

    if (size == 0) {
        mep_free(mp, ptr);
        return NULL;
    }

    if (unlikely(size > MEP_MAX_ALLOC))
        return NULL;

    a_size = MEP_ALIGN(size);
    ck  = CHUNK_FROM_PTR(ptr);

    if (ck->size < a_size) {
        if (!__take_next(mp, ck, a_size)) {
            nptr = mep_alloc(mp, size);
            if (nptr) {
                memcpy(nptr, ptr, ck->size - ck->left);
                mep_free_chunk(mp, ck);
            }
            return nptr;
        }
    }

    mep_free_over(mp, ck, a_size);
    assert((ck->size - size) <= UINT16_MAX);
    ck->left = ck->size - size;
    return ptr;

}

/************************ Private Defines  ******************************/

static int __take_next(mep_t *mp, mep_chunk_t *ck, uint32_t size)
{
    mep_chunk_t *nck;
    if (CHUNK_HAVE_NEXT(ck))
        return 0;

    nck = NEXT_CHUNK(ck);
    if (!CHUNK_IS_UNUSED(nck))
        return 0;

    if (size < CHUNK_FSIZE(nck))
        return 0;

    REMOVE_UNUSED(mp, nck);
    CHUNK_MERGE(ck, nck);
    return 1;
}
