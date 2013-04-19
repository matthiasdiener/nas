#!/bin/bash

trap 'exit 3' SIGINT

ORACLE_FILE=oracle_$(hostname).sh
source $ORACLE_FILE

usage() {
	echo "Usage: $0 <version> <#procs> <#threads> <#runs> <size> <benchmarks> <mapping>"
	echo "Available Mappings: round-robin (RR), random static (RS), operating system (OS), Oracle (ORACLE)"
	exit $1
}

[ $# -eq 7 ] || usage 1

VER=${1^^}
PROCS=$2
THREADS=$3
RUNS=$4
SIZE=${5^^}
BM=${6,,}
MAP_ALGO=${7^^}

PUS=$(cat /proc/cpuinfo | grep processor | tail -1 | awk '{print $3}')


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

		"ORACLE") #oracle mapping from machine file "oracle_$(hostname).sh"
			varname=MAP_$bm$THREADS
			MAP+=${!varname}
			;;

		*)
			echo "Illegal Mapping '${MAP_ALGO}'"
			usage 2
			;;
	esac
}

DIR="NPB3.3-$VER/bin"

OUTPATH=results/$VER/P$PROCS-T$THREADS/$SIZE/$MAP_ALGO

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
		do_map $run $bm# calculate new mapping for this run
		name=$OUTPATH/$bm/$(date|tr ' ' '-').txt # name of output file
		cmd="OMP_NUM_THREADS=$THREADS mpirun $MAP -np $PROCS $DIR/$bm.$SIZE.$PROCS"
		echo "Run $run - '$cmd'" | tee $name
		energy=($(/home/mdiener/rapl_msr2))
		start_package=(${energy[0]})
		start_core=(${energy[1]})
		start_dram=(${energy[2]})
		sh -c "$cmd" | tee -a $name
		energy=($(/home/mdiener/rapl_msr2))
		end_package=(${energy[0]})
		end_core=(${energy[1]})
		end_dram=(${energy[2]})

		echo "package energy: $(float_eval $end_package-$start_package)" | tee -a $name
		echo "core energy: $(float_eval $end_core-$start_core)" | tee -a $name
		echo "dram energy: $(float_eval $end_dram-$start_dram)" | tee -a $name

		sleep 1
	done
done
