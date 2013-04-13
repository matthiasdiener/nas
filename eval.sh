#!/bin/bash

if [ $# -ne 3 ]; then
	echo "Usage: $0 <version> <#threads> <size>"
	exit 1
fi

VER=$1
THREADS=$2
SIZE=$3

OUTPATH=results/$VER/$THREADS/$SIZE

cd $OUTPATH

for bm in *; do
	echo $bm
	grep  "Time in seconds" $bm/*.txt | awk '{print $6}'
	echo
done
