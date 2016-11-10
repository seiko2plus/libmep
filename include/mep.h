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

# ifndef MEP_H
# define MEP_H

# if defined __cplusplus
extern "C" {
# endif

# include <stddef.h>

# ifdef _WIN32
#   if defined(MEP_SHARED)
#       define MEP_EXTERN __declspec(dllexport) /* Building shared */
#   elif defined(MEP_USING_SHARED)
#       define MEP_EXTERN __declspec(dllimport) /* Using our shared */
#   else
#       define MEP_EXTERN /* Nothing, building static */
#   endif
# elif __GNUC__ >= 4
#   define MEP_EXTERN __attribute__((visibility("default")))
# else
#   define MEP_EXTERN /* Nothing */
# endif /* MEP_EXTERN */

/**
 * @brief mep_t
 * Memory pool type
 */

typedef struct mep_s mep_t;

/**
 * @brief mep_stats_t
 * need it by mep_stats() to perform statistics
 */
typedef struct mep_stats_s
{
    /** count of allocated lines */
    size_t lines;
    /** total allocated bytes */
    size_t total;
    /** available bytes to use */
    size_t available;
    /**
     * total of wasted bytes.
     * wasted bytes can be :
     * - different of requested size and aligned size
     * - small bytes left without splited or added to unsed list, follow MEP_SPLIT_SIZE for more understanding
     */
    size_t left;
    /** count of unsed chunks */
    size_t unuse_count;
    /** count of used chunks (count of allocated ptr) */
    size_t use_count;
} mep_stats_t;

/**
 * @brief mep_new
 * Allocate a new memory pool and the first line *
 * Memory pool line is used to storing used or unused chunks
 * If there's no space enough for requsted size memory pool gonna allocate a new line
 * Any new line gonna automatically freed when freeing last used chunk on it
 *
 * @param parent
 * memory pool pointer or pass NULL for non-parent
 *
 * @param line_size
 * The size of the first memory pool line, it also the default size for the next lines
 * I don't recommended a specific size in userspace world.
 * Note: if alloc, realloc requested size > line size mep gonna allocate a new line with the requested size and ignore default size.
 *
 * @return
 * memory pool pointer or NULL if insufficient memory was available
 */
MEP_EXTERN mep_t *mep_new (mep_t *parent, size_t line_size);

/**
 * @brief mep_reset
 * Free all childs, extra lines and reinit the pool
 *
 * @param mp
 * memory pool pointer
 */
MEP_EXTERN void mep_reset (mep_t *mp);

/**
 * @brief mep_destroy
 * Destroy memory pool and free all memory usage
 *
 * @param mp
 * memory pool pointer
 */
MEP_EXTERN void mep_destroy (mep_t *mp);

/**
 * @brief mep_stats
 * Memory pool statistics
 *
 * @param mp
 * memory pool pointer
 *
 * @param stat
 * mep stats ptr
 */
MEP_EXTERN void mep_stats (mep_t *mp, mep_stats_t *stat);

/**
 * @brief mep_alloc
 * Allocate size bytes of memory same as malloc() work
 *
 * @param mp
 * memory pool pointer
 *
 * @param size
 * Allocate size in bytes
 *
 * @return
 * allocated pointer or NULL if insufficient memory was available
 */
MEP_EXTERN void *mep_alloc (mep_t *mp, size_t size);

/**
 * @brief mep_calloc
 * Allocates a block of memory for an array of num elements and initialize it to zero , same as calloc work
 *
 * @param mp
 * memory pool pointer
 *
 * @param count
 * count of array
 *
 * @param size
 * Array element size in bytes
 *
 * @return
 * array pointer or NULL if insufficient memory was available
 */
MEP_EXTERN void *mep_calloc (mep_t *mp, size_t count, size_t size);

/**
 * @brief mep_realloc
 * Our swiss army knife
 * Reallocates the given area of memory. It must be previously allocated by mep_malloc(), mep_calloc() or mep_realloc(), same as realloc()
 * Redirect rules depend on libev allocator
 * if @param ptr is null it gonna redirect to mep_alloc()
 * if @param ptr is not null and @param size is zero it gonna redirect to mep_free()
 *
 * @param mp
 * memory pool pointer
 *
 * @param ptr
 * reallocte pointer
 *
 * @param size
 * reallocte size in bytes
 *
 * @return
 * allocated pointer or NULL if insufficient memory was available
 */
MEP_EXTERN void *mep_realloc (mep_t *mp, void *ptr, size_t size);

/**
 * @brief mep_free
 * Deallocates the memory previously allocated by a call to mep_calloc, mep_malloc, or mep_realloc.
 *
 * @param mp
 * memory pool pointer
 *
 * @param ptr
 * Allocated ptr
 */
MEP_EXTERN void mep_free (mep_t *mp, void *ptr);

/**
 * @brief mep_sizeof
 * @param ptr
 * Allocated pool ptr
 * @return
 * size of Allocated ptr in bytes
 */
MEP_EXTERN size_t mep_sizeof (void *ptr);

/**
 * @brief mep_max_alloc
 *
 * @return
 * the maximum size that memory pool can allocate
 */
MEP_EXTERN size_t mep_max_alloc  (void);

/**
 * @brief mep_max_line
 * @return
 * the maximum size of memory pool line can be
 */
MEP_EXTERN size_t mep_max_line   (void);

/**
 * @brief mep_align_size
 * @return
 * current memory pool alignment size default size is 16 unless u changed in compiling time
 */
MEP_EXTERN size_t mep_align_size (void);

/**
 * @brief mep_strdup
 * duplicate a string like strdup()
 *
 * @param mp
 * memory pool pointer
 *
 * @param s
 * string pointer
 *
 * @return
 * returned pointer can be passed to mep_free() or null if fail
 */
MEP_EXTERN char *mep_strdup (mep_t *mp, const char *s);

/**
 * @brief mep_strndup
 * duplicate a string like mep_strdup but only copies at most n bytes
 *
 * @param mp
 * memory pool pointer
 *
 * @param s
 * string pointer
 *
 * @param n
 * number of bytes
 *
 * @return
 * returned pointer can be passed to mep_free() or null if insufficient memory was available.
 */
MEP_EXTERN char *mep_strndup (mep_t *mp, const char *s, size_t n);

# if defined __cplusplus
}
# endif /* __cplusplus */

# endif /* MEP_H */
