#!/bin/bash

if [ $# -ne 4 ]; then
	echo "Usage: $0 <version> <#threads> <size> <mapping>"
	exit 1
fi

VER=${1^^}
THREADS=$2
SIZE=${3^^}
MAP_ALGO=${4^^}

OUTPATH=results/$VER/$THREADS/$SIZE/$MAP_ALGO

cd $OUTPATH

for bm in *; do
	echo $bm
	grep  "Time in seconds" $bm/*.txt | awk '{print $6}'
	echo
done
