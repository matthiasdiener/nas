#!/bin/bash

if [ $# -ne 2 ]; then
	echo "Usage: $0 <binary> <class>"
	exit 1
fi

echo "### Converting source code and compiling it"

LIST=$(nm $1 | awk '{print $3}' | grep "_$" | grep -v __ | grep -v  "^_" | grep -v timer_ | grep -v print_results_ | sort | uniq | sed s/.$//)

PROG=$(basename $1 | sed s/\\..*//)
DIR=$(basename $1)
NTHREADS=15
CLASS=$2

for i in $(seq 0 $NTHREADS); do
	echo ${i}
	DIRID=../${DIR}_$i
	rm -rf $DIRID
	mkdir $DIRID
	cp * $DIRID 2>/dev/null
	for sym in $LIST; do
		echo -e \\t $sym;
		(cd $DIRID; find . -name "*.[hf]" -or -name "*.incl" | xargs sed -i "/include/!s/\b$sym\b/${sym}_${i}/g";)
		(cd $DIRID; find . -name "*.f" | xargs sed -i s/program\\\s.*/subroutine\ ${PROG}$i/;)
	done
	(cd $DIRID; make clean; make CLASS=$CLASS link)
done

echo "#define BENCHMARK $PROG" > ../app.h


echo "### Linking Final Binary"
CMD="gcc -o ../${PROG}_${CLASS}_$NTHREADS -O2 -fopenmp ../${DIR}_*/*.o ../common/print_results.o ../common/timers.o ../common/wtime.o ../starter_pt.c -lgfortran -Wall -pthread"

echo $CMD

$CMD

for i in $(seq 0 $NTHREADS); do
	DIRID=../${DIR}_$i
	rm -rf $DIRID
done

rm ../app.h


echo "### Done"