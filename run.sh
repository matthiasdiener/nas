#!/bin/bash

if [ $# -ne 5 ]; then
	echo "Usage: $0 <version> <#threads> <#runs> <size> <benchmarks>"
	exit 1
fi


VER=$1
THREADS=$2
RUNS=$3
SIZE=$4
BM=$5


DIR="NPB3.3-$VER/bin"

OUTPATH=results/$VER/$THREADS/$SIZE

for bm in $BM; do
	mkdir -p $OUTPATH/$bm
	for i in $(seq 1 $RUNS); do
		name=$OUTPATH/$bm/$(date|tr ' ' '-').txt
		echo $i
		mpirun -np $THREADS $DIR/$bm.$SIZE.$THREADS | tee $name
	done
done
