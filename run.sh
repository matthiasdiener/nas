#!/bin/bash

trap 'exit 3' SIGINT

ORACLE_FILE=oracle_$(hostname).sh
source $ORACLE_FILE

usage() {
	echo "Usage: $0 <version> <#processes> <#threads> <#runs> <size> <benchmarks> <mapping>"
	echo "Available Mappings: round-robin (RR), random static (RS), operating system (OS), Oracle (ORACLE)"
	exit $1
}

[ $# -eq 7 ] || usage 1

VER=${1^^} # NAS Version, omp, mpi, ...
PROCESSES=$2 # number of processes
THREADS=$3 # number of threads per process
RUNS=$4 # number of executions
SIZE=${5^^} # input size: A, B, ...
BM=${6,,} # name of benchmarks
MAP_ALGO=${7^^} # mapping algorithm to use

PUS=$(cat /proc/cpuinfo | grep processor | tail -1 | awk '{print $3}') # get number of processing units for random static mapping


do_map() {
	MAP="-binding user:"
	case ${MAP_ALGO} in
		"RR") # round robin
			for j in $(seq 0 $(($THREADS-1))); do
				MAP+="$j,"
			done
			MAP=${MAP:0:${#MAP} - 1}
			;;

		"RS") # random static
			unset mapped
			mapped=()
			if [ $(($THREADS-1)) -gt $PUS ]; then
				echo "Number of threads larger than number of processors, cannot continue"
				exit 4
			fi
			RANDOM=$1
			for j in $(seq 0 $(($THREADS-1))); do
				while [ 1 ]; do # find unused processing unit
					cpu=$(($RANDOM%$(($PUS+1))))
					if [ "${mapped[$cpu]}" != "x" ]; then break; fi
				done
				mapped[$cpu]="x"
				MAP+="$cpu,"
			done
			MAP=${MAP:0:${#MAP} - 1}
			;;

		"OS") # OS mapping, do nothing
			MAP=""
			;;

		"SPCD") # SPCD handles mapping, do nothing
			MAP=""
			;;

		"ORACLE") # oracle mapping from machine file "oracle_$(hostname).sh"
			varname=MAP_$bm$PROCESSES
			MAP+=${!varname}
			;;

		*)
			echo "Illegal Mapping '${MAP_ALGO}'"
			usage 2
			;;
	esac
}

DIR="NPB3.3-$VER/bin"

OUTPATH=results/$VER/P$PROCESSES-T$THREADS/$SIZE/$MAP_ALGO # where to save the log files

function float_eval()
{
    local stat=0
    local result=0.0
    if [[ $# -gt 0 ]]; then
        result=$(echo "scale=2; $*" | bc -q 2>/dev/null)
        stat=$?
        if [[ $stat -eq 0  &&  -z "$result" ]]; then stat=1; fi
    fi
    echo $result
    return $stat
}


for bm in $BM; do
	mkdir -p $OUTPATH/$bm
	for run in $(seq 1 $RUNS); do

		do_map $run $bm  # calculate new mapping for this run

		name=$OUTPATH/$bm/$(date|tr ' ' '-').txt # name of output file

		cmd="OMP_NUM_THREADS=$THREADS perf stat -a --log-fd 1 -e instructions -e r412e -e r0224 $DIR/$bm.$SIZE.x" # r412e: LLC misses; r0224: l2 data misses
		echo "Run $run - '$cmd'" | tee $name

		energy=($(rapl_msr2)) # get energy before start
		start_package=(${energy[0]})
		start_core=(${energy[1]})
		start_dram=(${energy[2]})

		sh -c "$cmd" | tee -a $name # execute benchmark

		energy=($(rapl_msr2)) # get energy after start
		end_package=(${energy[0]})
		end_core=(${energy[1]})
		end_dram=(${energy[2]})

		echo "package energy: $(float_eval $end_package-$start_package)" | tee -a $name
		echo "core energy: $(float_eval $end_core-$start_core)" | tee -a $name
		echo "dram energy: $(float_eval $end_dram-$start_dram)" | tee -a $name

		sleep 1
	done
done
