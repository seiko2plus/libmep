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

void *mep_alloc(mep_t *mp, size_t size)
{
    mep_line_t   *ln;
    mep_chunk_t  *ck,  *nck;
    mep_unuse_t  *tmp, *unck;
    mep_size_t    diff, a_size, line_size;

    assert(mp != NULL);

    if (size > MEP_MAX_ALLOC)
        return NULL;

    if (size < MEP_UNUSE_SIZE)
        a_size = MEP_ALIGN(MEP_UNUSE_SIZE);    
    else
        a_size = MEP_ALIGN(size);

    /* looking inside unse chunks */
    DL_FOREACH_SAFE(mp->unuses, unck, tmp) {
        ck = MEP_CHUNK_UNUSE(unck);
        /* do we have the size? */
        if (ck->size >= a_size ) {
            /* remove from unuse list since we got size what we want */
            ck->flags &= ~MEP_FLAG_UNUSE;
            DL_DELETE(mp->unuses, unck);
            goto split;
        }
    }

    /*
     * we can't find specfic size in unuse list so we create a new line
    */
    line_size = mp->lines->size; /* first line size */
    if (line_size < a_size)
        line_size = a_size;

    if (mp->parent)
        ln = mep_alloc(mp->parent, line_size + MEP_LINE_SIZE + MEP_CHUNK_SIZE);
    else
        ln = mep_align_alloc(line_size + MEP_LINE_SIZE + MEP_CHUNK_SIZE);

    if (ln == NULL)
        return NULL;

    ln->size = line_size;
    DL_APPEND(mp->lines, ln);

    ck = MEP_CHUNK_LN(ln);
    ck->size  = line_size;
    ck->flags = 0;
    ck->prev  = 0;

split:
    diff = ck->size - a_size;
    if (diff < MEP_SPLIT_SIZE)
        goto down;
    /*
     * we going to split chunk
     * from the right
    */
    ck->size   = a_size; /* set a new size of first chunk, so we can use MEP_NEXT_CHUNK macro*/

    nck = MEP_NEXT_CHUNK(ck);
    nck->flags = MEP_FLAG_UNUSE;
    nck->size  = diff - MEP_CHUNK_SIZE;
    nck->prev  = a_size  + MEP_CHUNK_SIZE;

    /*
     * check if the first chunk has next chunk so we pass it
     * and fix prev size
    */
    if (MEP_HAVE_NEXT(ck)) {
         nck->flags |= MEP_FLAG_NEXT;
         MEP_NEXT_CHUNK(nck)->prev = diff;
    }
    /*
     * add our new chunk to the new unuse list
    */
    tmp = MEP_UNUSE(nck);
    DL_APPEND(mp->unuses, tmp);
    /*
     * return chunk ptr
     * and pass only next flag
     * next one is the new splited chunk
    */
    ck->flags = MEP_FLAG_NEXT;

down:
    assert((ck->size - size) <= UINT8_MAX);
    ck->left = ck->size - size;
    return MEP_CHUNK_PTR(ck);
}

void *mep_calloc(mep_t *mp, size_t count, size_t size)
{
    void *ptr;
    if (size > (size_t)-1 / count)
        return NULL; /* overflow */
    if ( NULL == (ptr = mep_alloc(mp, size * count)) )
        return NULL;
    memset(ptr, 0, size * count); /* todo we need memset depend on memory alignment to gunrentee speed */
    return ptr;
}
