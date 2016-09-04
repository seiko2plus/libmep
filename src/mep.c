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

#include "mep_p.h"

mep_t *mep_new(mep_t *parent, size_t line_size)
{
    mep_t *mp;

    assert(line_size > 1 && line_size <= MEP_MAX_LINE_SIZE);
    line_size = MEP_ALIGN(line_size);

    if (parent)
        mp = mep_alloc(parent, line_size + MEP_SIZE + MEP_LINE_SIZE + MEP_PIECE_SIZE);
    else
        mp = mep_align_alloc(line_size + MEP_SIZE + MEP_LINE_SIZE + MEP_PIECE_SIZE);

    if (mp) {
        mp->parent = parent;
        mep_init(mp, line_size);
    }

   return mp;
}

void mep_reset(mep_t *mp)
{
    mep_line_t *ln, *tmp;
    assert(mp != NULL);

    if (mp->parent) {
        DL_FOREACH_SAFE(mp->lines->next, ln, tmp)
            mep_free(mp->parent, ln);
    } else {
        DL_FOREACH_SAFE(mp->lines->next, ln, tmp)
            mep_align_free(ln);
    }

    mep_init(mp, mp->lines->size);
}


void mep_destroy(mep_t *mp)
{
    mep_line_t *ln, *tmp;
    assert(mp != NULL);

    if (mp->parent) {
        DL_FOREACH_SAFE(mp->lines->next, ln, tmp)
            mep_free(mp->parent, ln);
        mep_free(mp->parent, mp);
    } else {
        DL_FOREACH_SAFE(mp->lines->next, ln, tmp)
            mep_align_free(ln);
        mep_align_free(mp);
    }
}

void mep_stat(mep_t *mp, mep_stat_t *stat)
{
    mep_line_t   *ln;
    mep_piece_t  *pc;
    assert(mp != NULL && stat != NULL);

    memset(stat, 0, sizeof(mep_stat_t));

    DL_FOREACH(mp->lines, ln) {
        stat->total += ln->size;
        stat->lines++;

        pc = MEP_PIECE_LN(ln);

        for(;;) {
            if (pc->flags & MEP_FLAG_UNUSE) {
                stat->unuse_count++;
                stat->free += pc->size;
            } else {
                stat->use_count++;
                stat->left += pc->left;
            }

            if (!(pc->flags & MEP_FLAG_NEXT))
                break;

            pc = MEP_NEXT_PIECE(pc);
        }
    }
}


size_t mep_size_of(void *ptr)
{
    mep_piece_t *pc;
    assert(ptr != NULL);

    pc  = MEP_PIECE(ptr);
    return pc->size;
}

size_t mep_max_line(void)
{
    return MEP_MAX_LINE_SIZE;
}

size_t mep_max_alloc(void)
{
    return MEP_MAX_ALLOC;
}

size_t mep_align_size(void)
{
    return MEP_ALIGN_SIZE;
}

void mep_init(mep_t *mp, size_t line_size)
{
    mep_line_t   *ln;
    mep_piece_t  *pc;
    mep_unuse_t *unpc;

    mp->lines    = NULL;
    mp->unuses  = NULL;

    ln = MEP_PTR(mp + MEP_SIZE);
    ln->size = line_size;
    DL_APPEND(mp->lines, ln);

    pc            = MEP_PIECE_LN(ln);
    pc->size      = line_size;
    pc->prev      = 0;
    pc->flags     = MEP_FLAG_UNUSE;

    unpc = MEP_UNUSE(pc);
    DL_APPEND(mp->unuses, unpc);
}
