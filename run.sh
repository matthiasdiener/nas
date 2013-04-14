#!/bin/bash

usage() {
	echo "Usage: $0 <version> <#threads> <#runs> <size> <benchmarks> <mapping>"
	echo "Available Mappings: round-robin (RR), random static (RS), operating system (OS)"
	exit $1
}

if [ $# -ne 6 ]; then
	usage 1
fi

trap 'exit 3' SIGINT

VER=${1^^}
THREADS=$2
RUNS=$3
SIZE=${4^^}
BM=${5,,}
MAP_ALGO=${6^^}

PUS=$(cat /proc/cpuinfo | grep processor | tail -1 | awk '{print $3}')
if [ $(($THREADS-1)) -gt $PUS ]; then
	echo "Number of threads larger than number of processors, cannot continue"
	usage 4
fi

do_map() {
	MAP="-binding user:"
	case ${MAP_ALGO} in
		"RR") # round robin
			for i in $(seq 0 $(($THREADS-1))); do
				MAP="$MAP$i,"
			done
			MAP=${MAP:0:${#MAP} - 1} ;;

		"RS") # random static
			unset mapped
			mapped=()
			RANDOM=$1
			for j in $(seq 0 $(($THREADS-1))); do
				while [ 1 ]; do
					cpu=$(($RANDOM%$(($PUS+1))))
					if [ "${mapped[$cpu]}" != "x" ]; then break; fi
				done
				mapped[$cpu]="x"
				MAP="$MAP$cpu,"
			done
			MAP=${MAP:0:${#MAP} - 1} ;;
		"OS") # OS mapping
			MAP=""
			;;
		*)
			echo "Illegal Mapping '${MAP_ALGO}'"
			usage 2
	esac
}

DIR="NPB3.3-$VER/bin"

OUTPATH=results/$VER/$THREADS/$SIZE

for bm in $BM; do
	mkdir -p $OUTPATH/$bm
	for i in $(seq 1 $RUNS); do
		do_map $i
		name=$OUTPATH/$bm/$(date|tr ' ' '-').txt
		# echo $i
		echo mpirun $MAP -np $THREADS $DIR/$bm.$SIZE.$THREADS | tee $name
		sleep 1
	done
done
