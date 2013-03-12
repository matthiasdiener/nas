#!/bin/bash

if [ $# -ne 1 ]; then
	echo "Usage: $0 <class>"
	exit 1
fi

echo "### Compiling original source code and finding symbols"

make clean
make CLASS=A

BLACKLIST="print_results|timer|storage|c_print_results" # Can not be empty
LIST=$(nm *.o | awk '{print $3}' | grep "_$" | grep -v __ | grep -v  "^_" | sort | uniq | sed s/.$// | egrep -v "\b($BLACKLIST)\b" )

if [[ -z $LIST ]]; then
	LIST=$(nm *.o | grep -v "\bU\b" | awk '{print $3}' | grep -v "^\." |grep -v "_omp_" | egrep -v "\b($BLACKLIST)\b" | grep -v "\bmain\b")
fi

DIR=$(basename $PWD | tr '[A-Z]' '[a-z]')

NTHREADS=15
CLASS=$1

echo "### Converting source code and compiling it"

for i in $(seq 0 $NTHREADS); do
	echo Compiler iteration: ${i}
	DIRID=../${DIR}_$i
	rm -rf $DIRID
	mkdir $DIRID
	cp * $DIRID 2>/dev/null
	(cd $DIRID; make clean; make CLASS=$CLASS config)
	for sym in $LIST; do
		echo -e \\t $sym;
		(cd $DIRID; find . -name "*.[hfc]" -or -name "*.incl" | xargs sed -i "/include/!s/\b$sym\b/${sym}x${i}/g";)
	done
	(cd $DIRID; find . -name "*.f" | xargs sed -i s/program\\\s.*/subroutine\ ${DIR}$i/;)
	(cd $DIRID; find . -name "*.c" | xargs sed -i "s/\bmain\b/${DIR}${i}_/";)
	(cd $DIRID; make CLASS=$CLASS link)
done

echo "#define BENCHMARK $DIR" > ../app.h


echo "### Linking Final Binary"
CMD="gcc -o ../${DIR}_${CLASS}_$NTHREADS -O2 -fopenmp ../${DIR}_*/*.o ../common/print_results.o ../common/c_print_results.o ../common/timers.o ../common/c_timers.o ../common/wtime.o ../common/randi8.o ../starter_pt.c -lgfortran -Wall -pthread"

echo $CMD

$CMD

for i in $(seq 0 $NTHREADS); do
	DIRID=../${DIR}_$i
	rm -rf $DIRID
done

rm ../app.h


echo "### Done"