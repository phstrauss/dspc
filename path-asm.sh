#!/bin/sh
## -ffast-math
## graphite :
CFLAGS="-O3 -march=core2 -mssse3 \
-funroll-loops \
-floop-block \
-freschedule-modulo-scheduled-loops \
-fselective-scheduling2 \
-fsel-sched-pipelining \
-fsel-sched-pipelining-outer-loops \
-ftree-vectorize \
-fpredictive-commoning \
-fprefetch-loop-arrays \
-floop-nest-optimize \
-ftree-loop-if-convert \
-ftree-loop-if-convert-stores"

PHILOU=/home/philou

. $PHILOU/bin/devtools-select.sh gcc

$CC -E path_filtering_c.h > _path_filtering_c.c
cat _path_filtering_c.c | sed -e "s/extern\ inline\ __attribute__\ ((__gnu_inline__))\ //g" > tmp.c
mv tmp.c _path_filtering_c.c
$CC $CFLAGS -o _path_filtering_c.S -S _path_filtering_c.c
