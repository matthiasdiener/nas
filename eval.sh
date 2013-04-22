#!/bin/bash

if [ $# -ne 5 ]; then
	echo "Usage: $0 <version> <#procs> <#threads> <size> <mapping>"
	exit 1
fi

VER=${1^^}
PROCS=$2
THREADS=$3
SIZE=${4^^}
MAP_ALGO=${5^^}

OUTPATH=results/$VER/P$PROCS-T$THREADS/$SIZE/$MAP_ALGO

cd $OUTPATH

for bm in *; do
	echo "$bm TIME (seconds)"
	grep  "Time in seconds" $bm/*.txt | awk '{print $6}'
	echo
done

for bm in *; do
	echo "$bm PACKAGE ENERGY (J)"
	grep  "package energy" $bm/*.txt | awk '{print $3}'
	echo
done

for bm in *; do
	echo "$bm CORE ENERGY (J)"
	grep  "core energy" $bm/*.txt | awk '{print $3}'
	echo
done

for bm in *; do
	echo "$bm DRAM ENERGY (J)"
	grep  "dram energy" $bm/*.txt | awk '{print $3}'
	echo
done

for bm in *; do
	echo "$bm INSTRUCTIONS"
	grep  " insns per cycle " $bm/*.txt | awk '{print $2}'
	echo
done

for bm in *; do
	echo "$bm L2 CACHE MISSES"
	grep  "raw 0x224" $bm/*.txt | awk '{print $2}'
	echo
done

for bm in *; do
	echo "$bm L3 CACHE MISSES"
	grep  "raw 0x412e" $bm/*.txt | awk '{print $2}'
	echo
done
