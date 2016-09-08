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

typedef struct mep_s mep_t;
typedef struct mep_stats_s {
    size_t lines,
           total,
           available,
           left,
           unuse_count,
           use_count;
} mep_stats_t;

MEP_EXTERN mep_t *mep_new        (mep_t *parent, size_t line_size);
MEP_EXTERN void   mep_reset      (mep_t *mp);
MEP_EXTERN void   mep_destroy    (mep_t *mp);
MEP_EXTERN void   mep_stats      (mep_t *mp, mep_stats_t *stat);

MEP_EXTERN void  *mep_alloc      (mep_t *mp, size_t size);
MEP_EXTERN void  *mep_calloc     (mep_t *mp, size_t count, size_t size);
MEP_EXTERN void  *mep_realloc    (mep_t *mp, void *ptr, size_t size);
MEP_EXTERN void   mep_free       (mep_t *mp, void *ptr);

MEP_EXTERN size_t mep_sizeof     (void *ptr);

MEP_EXTERN size_t mep_max_alloc  (void);
MEP_EXTERN size_t mep_max_line   (void);
MEP_EXTERN size_t mep_align_size (void);

MEP_EXTERN char  *mep_strdup     (mep_t *mp, const char *s);
MEP_EXTERN char  *mep_strndup    (mep_t *mp, const char *s, size_t n);


# if defined __cplusplus
}
# endif /* __cplusplus */

# endif /* MEP_H */
