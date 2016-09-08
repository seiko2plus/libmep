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
    mep_chunk_t  *chunk, *tmp;
    mep_line_t   *ln;
    assert(mp != NULL && ptr != NULL);

    chunk  = MEP_CHUNK(ptr);
    assert(!MEP_IS_UNUSE(chunk));

    if (MEP_HAVE_PREV(chunk)) {
        tmp = MEP_PREV_CHUNK(chunk);

        if (MEP_IS_UNUSE(tmp)) {
            MEP_MERGE(tmp, chunk);
            chunk = tmp;
            goto next; /* escape adding to unuse since tmp already there */
        }
    }

    MEP_ADD_UNUSE(mp, chunk);

next:
    if (MEP_HAVE_NEXT(chunk)) {
        tmp = MEP_NEXT_CHUNK(chunk);

        if (MEP_IS_UNUSE(tmp)) {
            MEP_REMOVE_UNUSE(mp, tmp);
            MEP_MERGE(chunk, tmp);
        }
    }

    /* Shrink memory pool and free unused lines */
    if (!MEP_HAVE_PREV(chunk) && !MEP_HAVE_NEXT(chunk)) {
        ln = (mep_line_t*) MEP_PTR(chunk  - MEP_LINE_SIZE);

        /* is not the first line */
        if (ln != (mep_line_t*) MEP_PTR(mp + MEP_SIZE)) {
            MEP_REMOVE_UNUSE(mp, chunk);
            DL_DELETE(mp->lines, ln);

            if (mp->parent)
                mep_free(mp->parent, ln);
            else
                mep_align_free(ln);
        }
    }
}
