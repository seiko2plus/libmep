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


/*
******** ********** ******** ********** ********
* USED *** UNUSE *** FREE *** UNUSE *** USED *
******** ********** ******** ********** ********
*/
void mep_free(mep_t *mp, void *ptr)
{
    mep_chunk_t  *ck;

    assert(mp != NULL && ptr != NULL);
    ck  = CHUNK_FROM_PTR(ptr);
    assert(!CHUNK_IS_UNUSED(ck));

    mep_free_chunk(mp, ck);
}

void mep_free_chunk(mep_t *mp, mep_chunk_t *ck)
{
    mep_chunk_t *tck;
    mep_line_t   *ln;

    if (CHUNK_HAVE_PREV(ck)) {
        tck = PREV_CHUNK(ck);
        if (CHUNK_IS_UNUSED(tck)) {
            CHUNK_MERGE(tck, ck);
            ck = tck;
            goto next;
        }
    }

    ADD_UNUSED(mp, ck);

next:
    if (CHUNK_HAVE_NEXT(ck)) {
        tck = NEXT_CHUNK(ck);
        if (CHUNK_IS_UNUSED(tck)) {
            REMOVE_UNUSED(mp, tck);
            CHUNK_MERGE(ck, tck);
        }
    }

    /* Shrink memory pool and free unused lines */
    if (!CHUNK_HAVE_PREV(ck) && !CHUNK_HAVE_NEXT(ck)) {
        ln = (mep_line_t*) MEP_PTR(ck  - LINE_SIZE);

        /* is not the first line */
        if (ln != (mep_line_t*) MEP_PTR(mp + MEP_SIZE)) {
            REMOVE_UNUSED(mp, ck);
            DL_DELETE(mp->ln_h, ln);

            if (mp->parent)
                mep_free(mp->parent, ln);
            else
                mep_align_free(ln);
        }
    }
}

void mep_free_over(mep_t *mp, mep_chunk_t *ck, uint32_t size)
{
    mep_chunk_t *nck;
    uint32_t     dif;

    dif = ck->size - size;

    if (dif < MEP_SPLIT_SIZE)
        return;

    ck->size = size;
    nck = NEXT_CHUNK(ck);
    nck->prev = ck;
    nck->size = dif - CHUNK_SIZE;

    DL_APPEND(mp->un_h, UNUSE_FROM_CHUNK(nck));

    if (!CHUNK_HAVE_NEXT(ck)) {
        nck->flags = MEP_FLAG_UNUSED;
        ck->flags |= MEP_FLAG_NEXT;
        return;
    }

    nck->flags = MEP_FLAG_NEXT | MEP_FLAG_UNUSED;
    ck = NEXT_CHUNK(nck);
    ck->prev = nck;

    if (CHUNK_IS_UNUSED(ck)) {
        REMOVE_UNUSED(mp, ck);
        CHUNK_MERGE(nck, ck);
    }
}
