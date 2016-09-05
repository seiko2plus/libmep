#!/bin/bash

cd `dirname "$0"`

mkdir m4 2>/dev/null

if [ "$(uname)" == "Darwin" ]; then
    glibtoolize --copy
else
    libtoolize --copy
fi

aclocal -I m4
autoconf
automake --add-missing --copy
