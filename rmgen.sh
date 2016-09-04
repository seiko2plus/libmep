#!/bin/bash
cd `dirname "$0"`
make clean
rm -rf src/.deps src/.dirstamp tests/.deps tests/.dirstamp test-driver mep_test* m4/* Makefile config.log configure config.log config.sub config.guess Makefile.in depcomp libtool missing aclocal.m4 autom4te.cache compile config.status install-sh ltmain.sh

