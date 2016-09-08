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

# ifndef MEP_P_H
# define MEP_P_H

# include "mep.h"

# if defined __cplusplus
extern "C" {
# endif

# include <stddef.h>
# include <stdlib.h>
# include <stdio.h>
# include <string.h>

# include "utlist.h"

# ifdef _WIN32
#   include <windows.h>
# endif

# if defined(_MSC_VER) && _MSC_VER < 1600
#   include "stdint-msvc2008.h"
# else
#   include <stdint.h>
# endif

# if __STDC_VERSION__ >= 199901L || __GNUC__ >= 3
#   define MEP_INLINE static inline
# else
#   define MEP_INLINE static
# endif

# ifndef MEP_ALIGN_SIZE
#   define MEP_ALIGN_SIZE 16
# endif

/*
 * MEP_SPLIT_SIZE
 * Must be higher than MEP_CHUNK_SIZE + MEP_UNUSE_SIZE
 * Leave margin for align lose < MEP_ALIGN_SIZE
 * mep_chunk_s::left can rich the max, assert cause
*/

# ifndef MEP_SPLIT_SIZE
#   define MEP_SPLIT_SIZE (UINT8_MAX /2)
# endif

# if MEP_ALIGN_SIZE > 16
#   define MEP_MAX_SIZE
# endif

# ifdef MEP_MAX_SIZE
    typedef size_t mep_size_t;
#   define MEP_SIZE_MAX SIZE_MAX
# else
#   if MEP_ALIGN_SIZE <= 8
        typedef uint16_t mep_size_t;
#       define MEP_SIZE_MAX UINT16_MAX
#   elif MEP_ALIGN_SIZE <= 16
        typedef uint32_t mep_size_t;
#       define MEP_SIZE_MAX UINT32_MAX
#   endif
# endif

# define MEP_MAX_LINE_SIZE  (MEP_ALIGN(MEP_SIZE_MAX - MEP_ALIGN_SIZE) - (MEP_SIZE + MEP_LINE_SIZE))

# ifndef MEP_MAX_ALLOC
#   define MEP_MAX_ALLOC    (MEP_MAX_LINE_SIZE - MEP_CHUNK_SIZE)
# endif

# define MEP_ALIGN(X)       (((X) + MEP_ALIGN_SIZE - 1) & ~(MEP_ALIGN_SIZE - 1))
# define MEP_ALIGN_OF(X)    MEP_ALIGN(sizeof(X))

# define MEP_CHUNK_SIZE     MEP_ALIGN_OF(mep_chunk_t)
# define MEP_LINE_SIZE      MEP_ALIGN_OF(mep_line_t)
# define MEP_SIZE           MEP_ALIGN_OF(mep_t)
# define MEP_UNUSE_SIZE     (sizeof(mep_unuse_t))

# define MEP_PTR(X)         ((uintptr_t) X)

# define MEP_CHUNK(PTR)     ((mep_chunk_t*)  MEP_PTR(PTR - MEP_CHUNK_SIZE))
# define MEP_CHUNK_PTR(CK)  ((void*)         MEP_PTR(CK  + MEP_CHUNK_SIZE))
# define MEP_CHUNK_LN(LN)   ((mep_chunk_t*)  MEP_PTR(LN  + MEP_LINE_SIZE))
# define MEP_CHUNK_UNUSE(U) ((mep_chunk_t*)  MEP_PTR(U   - MEP_UNUSE_SIZE))

# define MEP_HAVE_PREV(CK)  (CK->prev > 0)
# define MEP_HAVE_NEXT(CK)  (CK->flags & MEP_FLAG_NEXT)
# define MEP_NEXT_CHUNK(CK) ((mep_chunk_t*)  MEP_PTR(CK  + (CK->size + MEP_CHUNK_SIZE)))
# define MEP_PREV_CHUNK(CK) ((mep_chunk_t*)  MEP_PTR(CK  - CK->prev))

# define MEP_IS_UNUSE(CK)   (CK->flags & MEP_FLAG_UNUSE)
# define MEP_UNUSE(CK)      ((mep_unuse_t*)  MEP_PTR(CK  + MEP_CHUNK_SIZE))


# define MEP_REMOVE_UNUSE(MP, CK) \
do { \
    (CK)->flags &= ~MEP_FLAG_UNUSE; \
    DL_DELETE((MP)->unuses, MEP_UNUSE(CK)); \
} while(0)  \

# define MEP_ADD_UNUSE(MP, CK) \
do { \
    (CK)->flags |= MEP_FLAG_UNUSE; \
    DL_APPEND((MP)->unuses, MEP_UNUSE(CK)); \
} while(0)

# define MEP_MERGE(CK1, CK2) \
do { \
    (CK1)->size += (CK2)->size + MEP_CHUNK_SIZE; \
    if (MEP_HAVE_NEXT(CK2)) { \
        (CK1)->flags |= MEP_FLAG_NEXT; \
        MEP_NEXT_CHUNK(CK2)->prev = (CK1)->size + MEP_CHUNK_SIZE; \
    } else { \
        (CK1)->flags &= ~MEP_FLAG_NEXT; \
    } \
} while(0)


typedef struct mep_unuse_s
{
    struct mep_unuse_s *next,
                       *prev;
} mep_unuse_t;

typedef struct mep_chunk_s
{
    mep_size_t size,
               prev;
    uint8_t    flags,
               left;
} mep_chunk_t;

typedef struct mep_line_s
{
    struct mep_line_s *next,
                      *prev;
    mep_size_t         size;
} mep_line_t;

struct mep_s
{
    mep_line_t   *lines;
    mep_unuse_t  *unuses;
    mep_t        *parent;
};

enum
{
    MEP_FLAG_NEXT   = 0x02,
    MEP_FLAG_UNUSE  = 0x04
};

void mep_init(mep_t *mp, size_t line_size);


MEP_INLINE
void *mep_align_alloc(size_t size)
{
    void *ptr;
# if !defined(MEP_HAVE_POSIX_ALIGN) && !defined(MEP_HAVE_MEMALIGN)
    uintptr_t diff;
# endif

# ifdef MEP_HAVE_POSIX_ALIGN
    if (posix_memalign(&ptr, MEP_ALIGN_SIZE, size))
        ptr = NULL;
# elif defined(MEP_HAVE_MEMALIGN)
        ptr = memalign(MEP_ALIGN_SIZE, size);
# else
    if ((ptr = malloc(size + MEP_ALIGN_SIZE))) {
        diff = ((~(uintptr_t)ptr) & (MEP_ALIGN_SIZE - 1)) + 1;
        ptr  = (void*) MEP_PTR(ptr + diff);
    }
# endif
    return ptr;
}

MEP_INLINE
void mep_align_free(void *ptr)
{
# if !defined(MEP_HAVE_POSIX_ALIGN) && !defined(MEP_HAVE_MEMALIGN)
    uintptr_t diff;
    diff = ((~(uintptr_t)ptr) & (MEP_ALIGN_SIZE - 1)) + 1;
    ptr  = (void*) MEP_PTR(ptr - diff);
# endif
    free(ptr);
}


# if defined __cplusplus
}
# endif /* __cplusplus */

# endif /* MEP_P_H */
