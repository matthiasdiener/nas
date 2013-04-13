#!/bin/bash


VER="MPI"
DIR="NPB3.3-$VER/bin"
THREADS="16"
RUNS=10
BM="bt cg ep ft is lu mg sp"
SIZE="B"


OUTPATH=results/$VER/$THREADS/$SIZE

mkdir -p $OUTPATH


for bm in $BM; do
	mkdir -p $OUTPATH/$bm
	for i in $(seq 1 $RUNS); do
		name=$OUTPATH/$bm/$(date|tr ' ' '-').txt
		echo $i
		mpirun -np $THREADS $DIR/$bm.$SIZE.$THREADS | tee $name
		exit
	done
done