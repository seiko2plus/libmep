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

static mep_chunk_t *__chunk_from_unuse (mep_t *mp, uint32_t size);
static mep_chunk_t *__alloc_new        (mep_t *mp, uint32_t size);

/***********************************************************************/

void *mep_alloc(mep_t *mp, size_t size)
{
    mep_chunk_t  *ck;
    uint32_t      a_size;

    assert(mp != NULL);
    assert(size > 0);

    if (unlikely(size > MEP_MAX_ALLOC))
        return NULL;

    a_size = MEP_ALIGN(size);

    if (NULL == (ck = __chunk_from_unuse(mp, a_size))) {
        if (unlikely(NULL == (ck = __alloc_new(mp, a_size))))
            return NULL;
    }

    assert((ck->size - size) <= UINT16_MAX);
    ck->left = ck->size - size;
    return PTR_CHUNK(ck);
}


void *mep_calloc(mep_t *mp, size_t count, size_t size)
{
    void *ptr;
    if (size > (MEP_MAX_ALLOC / count))
        return NULL; /* overflow */
    if ( NULL == (ptr = mep_alloc(mp, size * count)) )
        return NULL;
    memset(ptr, 0, size * count);
    return ptr;
}

/************************ Private Defines  ******************************/

mep_chunk_t *__chunk_from_unuse(mep_t *mp, uint32_t size)
{
    mep_chunk_t  *ck;
    mep_unused_t *un, *un_tmp;

    DL_FOREACH_SAFE(mp->un_h, un, un_tmp) {
        ck = CHUNK_FROM_UN(un);
        /* do we have the size? */
        if (ck->size < size )
            continue;

        DL_DELETE(mp->un_h, un);
        ck->flags &= ~MEP_FLAG_UNUSED;
        mep_free_over(mp, ck, size);
        return ck;
    }
    return NULL;
}

static mep_chunk_t *__alloc_new(mep_t *mp, uint32_t size)
{
    mep_line_t  *line;
    mep_chunk_t *ck;
    uint32_t    line_size;

    line_size = mp->ln_h->size; /* first line size */
    if (line_size < size)
        line_size = size;

    if (mp->parent)
        line = mep_alloc(mp->parent, line_size + LINE_SIZE + CHUNK_SIZE);
    else
        line = mep_align_alloc(line_size + LINE_SIZE + CHUNK_SIZE);

    if (line == NULL)
        return NULL;

    line->size = line_size;
    DL_APPEND(mp->ln_h, line);

    ck = CHUNK_FROM_LINE(line);
    ck->size  = line_size;
    ck->prev  = NULL;
    ck->flags = 0;

    mep_free_over(mp, ck, size);
    return ck;
}