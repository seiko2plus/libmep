# Libmep

## Overview
  libmep is dynamic memory pool for real-time usage

## Licensing
  libmep is licensed under the MIT license.

## Build
    $ sh autogen.sh
    $ ./configure
    $ make
    $ make check
    $ make install

## API

#####  mep_t *mep_new (mep_t *parent, size_t line_size);

#####  void mep_reset (mep_t *mp);

#####  void mep_destroy (mep_t *mp);

#####  void mep_stat (mep_t *mp, mep_stat_t *stat);

#####  void *mep_alloc (mep_t *mp, size_t size);

#####  void *mep_calloc (mep_t *mp, size_t count, size_t size);

#####  void *mep_realloc (mep_t *mp, void *ptr, size_t size);

#####  void mep_free (mep_t *mp, void *ptr);

#####  size_t mep_sizeof (void *ptr);

#####  size_t mep_max_alloc (void);

#####  size_t mep_max_line (void);

#####  size_t mep_align_size (void);

#####  char *mep_strdup (mep_t *mp, const char *s);
