#!/bin/sh

IT=$1
BM=$2
DIR=$(dirname $0)


if [ $# -ne 2 ]; then
  echo "Usage: $(basename $0) <iterations> <benchmarks>"
  exit 1
fi

for bm in $BM; do
	for i in $(seq 1 $IT); do
		sleep 2
		echo "### Run: $i $(date)" | tee -a $bm.out
		$DIR/bin/$bm.A.x | tee -a $bm.out
	done;
done;
