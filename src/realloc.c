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

void *mep_realloc(mep_t *mp, void *ptr, size_t size)
{
    mep_piece_t  *pc, *nxpc;
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

    pc  = MEP_PIECE(ptr);

    if (a_size == pc->size)
        return ptr;

    if (pc->size > a_size) {
        diff  = pc->size - a_size;
        pc->left = a_size - size;

        if (diff >= MEP_SPLIT_SIZE) {
            pc->size  = a_size;

            /* new piece */
            nxpc = MEP_NEXT_PIECE(pc);
            nxpc->size  = diff - MEP_PIECE_SIZE;
            nxpc->prev  = a_size + MEP_PIECE_SIZE;

            if (MEP_HAVE_NEXT(pc)) {
                /* going to merge with next if is unuse */
                pc = MEP_NEXT_PIECE(nxpc);

                if (MEP_IS_UNUSE(pc)) {
                    MEP_REMOVE_UNUSE(mp, pc);
                    MEP_MERGE(nxpc, pc);
                } else {
                    pc->prev    = nxpc->size + MEP_PIECE_SIZE;
                    nxpc->flags = MEP_FLAG_NEXT;
                }
            } else {
                nxpc->flags = 0;
                pc->flags  |= MEP_FLAG_NEXT;
            }
            MEP_ADD_UNUSE(mp, nxpc);
        }
        return ptr;
    }

    if (MEP_HAVE_NEXT(pc)) {
        nxpc   = MEP_NEXT_PIECE(pc);

        if (MEP_IS_UNUSE(nxpc)) {
            need  = a_size - pc->size;
            diff  = nxpc->size  - need;

            if (diff < 0 )
                goto mmcp;

            pc->left = a_size - size;

            MEP_REMOVE_UNUSE(mp, nxpc);

            if (diff >= MEP_SPLIT_SIZE) {
                /*
                 * steal what we need from next piece and set new size
                 * so MEP_NEXT_PIECE will bring to us a new piece
                */
                pc->size += need;

                if (MEP_HAVE_NEXT(nxpc)) {
                    nxpc = MEP_NEXT_PIECE(pc);
                    nxpc->flags = MEP_FLAG_NEXT;
                    nxpc->size  = diff;
                    MEP_NEXT_PIECE(nxpc)->prev = diff + MEP_PIECE_SIZE;
                } else {
                    nxpc = MEP_NEXT_PIECE(pc);
                    nxpc->flags = 0;
                    nxpc->size  = diff;  /* diff already without MEP_PIECE_SIZE */
                }

                nxpc->prev = pc->size + MEP_PIECE_SIZE;
                MEP_ADD_UNUSE(mp, nxpc);

            } else {
                pc->size += nxpc->size + MEP_PIECE_SIZE; /* steal all next piece size */

                if (MEP_HAVE_NEXT(nxpc))
                    MEP_NEXT_PIECE(pc)->prev = pc->size + MEP_PIECE_SIZE;
                else
                    pc->flags &= ~MEP_FLAG_NEXT;
            }
            return ptr;
        }
    }

mmcp:
    nptr = mep_alloc(mp, size);
    if (nptr) {
        memcpy(nptr, ptr, pc->size - pc->left); /* todo we need memcpy depend on memory alignment to gunrentee speed */
        mep_free(mp, ptr);
    }
    return nptr;
}
