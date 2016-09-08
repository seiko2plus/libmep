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

# define left_return \
do { \
    assert((a_size - size) <= UINT8_MAX); \
    chunk->left = a_size - size; \
    return ptr; \
} while(0)

void *mep_realloc(mep_t *mp, void *ptr, size_t size)
{
    mep_chunk_t  *chunk, *nxchunk, *nxnxchunk;
    void         *nptr;
    mep_size_t    need, a_size;
    int64_t       diff;

    assert(mp != NULL && size <= MEP_MAX_ALLOC);

    if (ptr == NULL) {
        if (size == 0)
            return NULL;
        return mep_alloc(mp, size);
    }

    if (size == 0) {
        mep_free(mp, ptr);
        return NULL;
    }

    if (size < MEP_UNUSE_SIZE)
        a_size = MEP_ALIGN(MEP_UNUSE_SIZE);
    else
        a_size = MEP_ALIGN(size);

    chunk  = MEP_CHUNK(ptr);
    if (a_size == chunk->size)
        left_return;

    if (chunk->size > a_size) {
        diff  = chunk->size - a_size;

        if (diff >= MEP_SPLIT_SIZE) {
            chunk->size  = a_size;

            /* new chunk */
            nxchunk = MEP_NEXT_CHUNK(chunk);
            nxchunk->size  = diff - MEP_CHUNK_SIZE;
            nxchunk->prev  = a_size + MEP_CHUNK_SIZE;

            if (MEP_HAVE_NEXT(chunk)) {
                /* going to merge with next if is unuse */
                nxnxchunk = MEP_NEXT_CHUNK(nxchunk);

                if (MEP_IS_UNUSE(nxnxchunk)) {
                    MEP_REMOVE_UNUSE(mp, nxnxchunk);
                    MEP_MERGE(nxchunk, nxnxchunk);
                } else {
                    nxnxchunk->prev = nxchunk->size + MEP_CHUNK_SIZE;
                    nxchunk->flags  = MEP_FLAG_NEXT;
                }
            } else {
                nxchunk->flags = 0;
                chunk->flags  |= MEP_FLAG_NEXT;
            }
            MEP_ADD_UNUSE(mp, nxchunk);
        }

        left_return;
    }

    if (MEP_HAVE_NEXT(chunk)) {
        nxchunk   = MEP_NEXT_CHUNK(chunk);

        if (MEP_IS_UNUSE(nxchunk)) {
            need  = a_size - chunk->size;
            diff  = nxchunk->size  - need;

            if (diff < 0 )
                goto mmcp;

            MEP_REMOVE_UNUSE(mp, nxchunk);

            if (diff >= MEP_SPLIT_SIZE) {
                /*
                 * steal what we need from next chunk and set new size
                 * so MEP_NEXT_CHUNK will bring to us a new chunk
                */
                chunk->size += need;

                if (MEP_HAVE_NEXT(nxchunk)) {
                    nxchunk = MEP_NEXT_CHUNK(chunk);
                    nxchunk->flags = MEP_FLAG_NEXT;
                    nxchunk->size  = diff;
                    MEP_NEXT_CHUNK(nxchunk)->prev = diff + MEP_CHUNK_SIZE;
                } else {
                    nxchunk = MEP_NEXT_CHUNK(chunk);
                    nxchunk->flags = 0;
                    nxchunk->size  = diff;  /* diff already without MEP_CHUNK_SIZE */
                }

                nxchunk->prev = chunk->size + MEP_CHUNK_SIZE;
                MEP_ADD_UNUSE(mp, nxchunk);

            } else {
                chunk->size += nxchunk->size + MEP_CHUNK_SIZE; /* steal all next chunk size */

                if (MEP_HAVE_NEXT(nxchunk))
                    MEP_NEXT_CHUNK(chunk)->prev = chunk->size + MEP_CHUNK_SIZE;
                else
                    chunk->flags &= ~MEP_FLAG_NEXT;
            }

            left_return;
        }
    }

mmcp:
    nptr = mep_alloc(mp, size);
    if (nptr) {
        memcpy(nptr, ptr, chunk->size - chunk->left); /* todo we need memcpy depend on memory alignment to gunrentee speed */
        mep_free(mp, ptr);
    }
    return nptr;
}
