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
    mep_piece_t  *pc,  *npc;
    mep_unuse_t  *tmp, *unpc;
    mep_size_t    diff, a_size;
    size_t        line_size;

    assert(mp != NULL && size <= MEP_MAX_ALLOC);

    if (size < MEP_UNUSE_SIZE)
        a_size = MEP_ALIGN(MEP_UNUSE_SIZE);
    else
        a_size = MEP_ALIGN(size);

    /* looking inside unsed pieces */
    DL_FOREACH_SAFE(mp->unuses, unpc, tmp) {
        pc = MEP_PIECE(unpc);
        /* do we have the size? */
        if (a_size > pc->size)
            continue;

        /* remove from unuse list since we got size what we want */        
        DL_DELETE(mp->unuses, unpc);

split:
        diff = pc->size - a_size;
        if (diff < MEP_SPLIT_SIZE) {
            pc->flags &= ~MEP_FLAG_UNUSE;
            return MEP_PIECE_PTR(pc);
        }        
        /*
         * we going to split piece
         * from the right
        */
        pc->size   = a_size; /* set a new size of first piece, so we can use MEP_NEXT_PIECE macro*/
        pc->left   = a_size - size;

        npc = MEP_NEXT_PIECE(pc);
        npc->flags = MEP_FLAG_UNUSE;
        npc->size  = diff - MEP_PIECE_SIZE;
        npc->prev  = a_size  + MEP_PIECE_SIZE;

        /*
         * check if the first piece has next piece so we pass it
         * and fix prev size
        */
        if (MEP_HAS_NEXT(pc)) {
             npc->flags |= MEP_FLAG_NEXT;
             MEP_NEXT_PIECE(npc)->prev = diff;
        }
        /*
         * add our new piece to the new unuse list
        */
        tmp = MEP_UNUSE(npc);
        DL_APPEND(mp->unuses, tmp);
        /*
         * return piece ptr
         * and pass only hasNext flag
         * next one is the new splited piece
        */
        pc->flags = MEP_FLAG_NEXT;
        return MEP_PIECE_PTR(pc);
    }
    /*
     * since we reach here thats mean we don't find enough unsed spaces
     * that's mean we gonna allocate a new line
    */
    line_size = mp->lines->size; /* first line size */
    if (line_size < a_size)
        line_size = a_size;

    if (mp->parent)
        ln = mep_alloc(mp->parent, line_size + MEP_LINE_SIZE + MEP_PIECE_SIZE);
    else
        ln = mep_align_alloc(line_size + MEP_LINE_SIZE + MEP_PIECE_SIZE);

    if (ln) {
        ln->size = line_size;
        DL_APPEND(mp->lines, ln);

        pc = MEP_PIECE_LN(ln);
        pc->flags = MEP_FLAG_UNUSE;
        pc->size  = line_size;
        pc->prev  = 0;
        goto split;
    }
    return NULL;
}

void *mep_calloc(mep_t *mp, size_t count, size_t size)
{
    void *ptr;
    if ( NULL == (ptr = mep_alloc(mp, size * count)) )
        return NULL;
    memset(ptr, 0, size * count); /* todo we need memset depend on memory alignment to gunrentee speed */
    return ptr;
}
