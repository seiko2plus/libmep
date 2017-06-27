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

# if defined __GNUC__ || defined __llvm__
#   ifndef likely
#       define likely(x) __builtin_expect(!!(x), 1)
#   endif
#   ifndef unlikely
#       define unlikely(x) __builtin_expect(!!(x), 0)
#   endif
# else
#   define likely(x) (x)
#   define unlikely(x) (x)
# endif

# if __STDC_VERSION__ >= 199901L || __GNUC__ >= 3
#   define MEP_INLINE static inline
# else
#   define MEP_INLINE static
# endif

/* fixed align size , 16 is good enough*/
# define MEP_ALIGN_SIZE 16

/*
 * MEP_SPLIT_SIZE
 * Must be higher than MEP_CHUNK_SIZE + MEP_UNUSE_SIZE
 * Leave margin for align lose < MEP_ALIGN_SIZE
 * mep_chunk_s::left can rich the max, assert cause
*/
# ifndef MEP_SPLIT_SIZE
#   define MEP_SPLIT_SIZE    (UINT8_MAX /2)
# endif

# define MEP_SIZE_MAX        UINT32_MAX
# define MEP_MAX_LINE_SIZE   (MEP_ALIGN(MEP_SIZE_MAX - MEP_ALIGN_SIZE) - (MEP_SIZE + LINE_SIZE))

# ifndef MEP_MAX_ALLOC
#   define MEP_MAX_ALLOC     (MEP_MAX_LINE_SIZE - CHUNK_SIZE)
# endif

# define MEP_ALIGN(X)        (((X) + MEP_ALIGN_SIZE - 1) & ~(MEP_ALIGN_SIZE - 1))
# define MEP_ALIGN_OF(X)     MEP_ALIGN(sizeof(X))
# define MEP_PTR(X)          ((uintptr_t) X)

# define LINE_SIZE           MEP_ALIGN_OF(mep_line_t)
# define MEP_SIZE            MEP_ALIGN_OF(mep_t)
# define CHUNK_SIZE          MEP_ALIGN_OF(mep_chunk_t)
# define UNUSED_SIZE         MEP_ALIGN_OF(mep_unused_t)

# define CHUNK_FSIZE(C)      (C->size + CHUNK_SIZE)
# define CHUNK_HAVE_PREV(C)  (C->prev)
# define CHUNK_HAVE_NEXT(C)  (C->flags & MEP_FLAG_NEXT)
# define CHUNK_IS_UNUSED(C)  (C->flags & MEP_FLAG_UNUSED)
# define CHUNK_FROM_PTR(PTR) ((mep_chunk_t*)  MEP_PTR(PTR - CHUNK_SIZE))
# define CHUNK_FROM_LINE(LN) ((mep_chunk_t*)  MEP_PTR(LN + LINE_SIZE))
# define CHUNK_FROM_UN(UN)   ((mep_chunk_t*)  MEP_PTR(UN - UNUSED_SIZE))

# define NEXT_CHUNK(C)       ((mep_chunk_t*)  MEP_PTR(C + (C->size + CHUNK_SIZE)))
# define PREV_CHUNK(C)       (C->prev)

# define UNUSE_FROM_CHUNK(C) ((mep_unused_t*) MEP_PTR(C + CHUNK_SIZE))
# define PTR_CHUNK(C)        (((void*) MEP_PTR(C + CHUNK_SIZE)))

# define CHUNK_MERGE(C1, C2)            \
do {                                    \
    (C1)->size += CHUNK_FSIZE(C2);      \
    if (CHUNK_HAVE_NEXT(C2)) {          \
        (C1)->flags |= MEP_FLAG_NEXT;   \
        NEXT_CHUNK(C2)->prev = (C1);    \
    } else {                            \
        (C1)->flags &= ~MEP_FLAG_NEXT;  \
    }                                   \
} while(0)

# define REMOVE_UNUSED(MP, C)                   \
do {                                            \
    (C)->flags &= ~MEP_FLAG_UNUSED;             \
    DL_DELETE((MP)->un_h, UNUSE_FROM_CHUNK(C)); \
} while(0)

# define ADD_UNUSED(MP, C)                      \
do {                                            \
    (C)->flags |= MEP_FLAG_UNUSED;              \
    DL_APPEND((MP)->un_h, UNUSE_FROM_CHUNK(C)); \
} while(0)



/* types */
typedef struct mep_chunk_s mep_chunk_t;
typedef struct mep_line_s  mep_line_t;
typedef struct mep_unused_s mep_unused_t;

/**
 * @brief The mep_chunk_s struct
 */
struct mep_chunk_s
{
    uint32_t size;
    uint16_t left;
    uint16_t flags;
    mep_chunk_t *prev;
};

struct mep_unused_s
{
    mep_unused_t *next,
                 *prev;
};

struct mep_line_s
{
    mep_line_t *next,
               *prev;
    uint32_t    size;
};

struct mep_s
{
    mep_line_t   *ln_h;
    mep_unused_t *un_h;
    mep_t        *parent;
};

enum
{
    MEP_FLAG_NEXT   = 0x02,
    MEP_FLAG_UNUSED = 0x04
};

void mep_init(mep_t *mp, size_t line_size);
void mep_free_chunk(mep_t *mp, mep_chunk_t *ck);
void mep_free_over(mep_t *mp, mep_chunk_t *ck, uint32_t size);

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
