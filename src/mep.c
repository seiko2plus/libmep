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
 * That's how pool looks like
 * default line
 * +------+-------+------------+--------+--------------------------+------------+
 * | pool | line  | prev chunk |  chunk >  unuse  or    block      | next chunk |
 * +------+-------+------------+--------+--------------------------+------------+
 * new line if we need more chunks
 * +-------+------------+--------+--------------------------+------------+
 * | line  | prev chunk |  chunk >  unuse  or    block      | next chunk |
 * +-------+------------+--------+--------------------------+------------+
*/

mep_t *mep_new(mep_t *parent, size_t line_size)
{
    mep_t *mp;

    assert(line_size > 1 && line_size <= MEP_MAX_LINE_SIZE);
    line_size = MEP_ALIGN(line_size);

    if (parent)
        mp = mep_alloc(parent, line_size + MEP_SIZE + LINE_SIZE + CHUNK_SIZE);
    else
        mp = mep_align_alloc(line_size + MEP_SIZE + LINE_SIZE + CHUNK_SIZE);

    if (likely(mp)) {
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
        DL_FOREACH_SAFE(mp->ln_h->next, ln, tmp)
            mep_free(mp->parent, ln);
    } else {
        DL_FOREACH_SAFE(mp->ln_h->next, ln, tmp)
            mep_align_free(ln);
    }

    mep_init(mp, mp->ln_h->size);
}

void mep_destroy(mep_t *mp)
{
    mep_line_t *ln, *tmp;
    assert(mp != NULL);

    if (mp->parent) {
        DL_FOREACH_SAFE(mp->ln_h->next, ln, tmp)
            mep_free(mp->parent, ln);
        mep_free(mp->parent, mp);
    } else {
        DL_FOREACH_SAFE(mp->ln_h->next, ln, tmp)
            mep_align_free(ln);
        mep_align_free(mp);
    }
}

void mep_stats(mep_t *mp, mep_stats_t *stat)
{
    mep_line_t   *ln;
    mep_chunk_t  *ck;
    assert(mp != NULL && stat != NULL);

    memset(stat, 0, sizeof(mep_stats_t));

    DL_FOREACH(mp->ln_h, ln) {
        stat->total += ln->size;
        stat->lines++;

        ck = CHUNK_FROM_LINE(ln);

        for(;;) {
            if (CHUNK_IS_UNUSED(ck)) {
                stat->unuse_count++;
                stat->available += ck->size;
            } else {
                stat->use_count++;
                stat->left += ck->left;
            }

            if (!CHUNK_HAVE_NEXT(ck))
                break;

            ck = NEXT_CHUNK(ck);
        }
    }
}


size_t mep_sizeof(const void *ptr)
{
    mep_chunk_t *ck;
    assert(ptr != NULL);

    ck  = CHUNK_FROM_PTR(ptr);
    return ck->size;
}

size_t mep_len(const void *ptr)
{
    mep_chunk_t *ck;
    assert(ptr != NULL);

    ck  = CHUNK_FROM_PTR(ptr);
    return ck->size - ck->left;
}

void mep_init(mep_t *mp, size_t line_size)
{
    mep_line_t   *ln;
    mep_chunk_t  *ck;

    mp->ln_h = NULL;
    mp->un_h = NULL;

    ln = (mep_line_t*) MEP_PTR(mp + MEP_SIZE);
    ln->size = line_size;
    DL_APPEND(mp->ln_h, ln);

    ck        = CHUNK_FROM_LINE(ln);
    ck->size  = line_size;
    ck->prev  = NULL;
    ck->flags = MEP_FLAG_UNUSED;
    DL_APPEND(mp->un_h, UNUSE_FROM_CHUNK(ck));
}
