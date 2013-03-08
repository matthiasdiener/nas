#!/bin/bash

if [ $# -lt 3 ]; then
	echo "Usage: $0 <binary> <#threads> <class>"
	exit 1
fi

LIST=$(nm $1 | awk '{print $3}' | grep "_$" | grep -v __ | grep -v  "^_" | grep -v timer_ | grep -v print_results_ | sort | uniq | sed s/.$//)

PROG=$(basename $1 | sed s/\\..*//)
DIR=$(basename $1)
NTHREADS=$2
CLASS=$3


for i in $(seq 0 $NTHREADS); do
	echo ${i}
	DIRID=../${DIR}_$i
	rm -rf $DIRID
	mkdir $DIRID
	cp * $DIRID 2>/dev/null
	for sym in $LIST; do
		echo -e \\t $sym;
		(cd $DIRID; find . -name "*.[hf]" -or -name "*.incl" | xargs sed -i s/\\b$sym\\b/${sym}_${i}/g;)
		(cd $DIRID; find . -name "*.f" | xargs sed -i s/program\\\s.*/subroutine\ ${PROG}$i/;)
	done
	(cd $DIRID; make CLASS=$CLASS)
done

echo "#define BENCHMARK $PROG" > ../app.h
echo "#define NTHREADS $NTHREADS" >> ../app.h

gcc -O2 -fopenmp -o ../${PROG}_${CLASS}_$NTHREADS ../${DIR}_*/*.o ../common/print_results.o ../common/timers.o ../common/wtime.o ../starter_pt.c -lgfortran -Wall -pthread


for i in $(seq 0 $NTHREADS); do
	DIRID=../${DIR}_$i
	rm -rf $DIRID
done

rm ../app.h