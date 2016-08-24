#!/bin/bash

set -o errexit -o nounset -o posix -o pipefail

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $DIR

bm="bt cg ep ft is lu mg sp ua"

OUTDIR=~/Dropbox/results/$(hostname)/$(basename $DIR)/
mkdir -p $OUTDIR

exe() { echo "++ $@" |& tee -a $OUTDIR/$b.txt ;  if [[ "$1" == "export" ]]; then $@ ; fi ;  $@ |& tee -a $OUTDIR/$b.txt ; }

for b in $bm; do
	echo -n > $OUTDIR / $b.txt #clean output file
	exe echo "running $b"
	exe date
	exe uname - a
	[[ -x /usr/bin/hwloc-ls ]] && exe hwloc-ls

	exe export GOMP_CPU_AFFINITY = 0 - 1024 KMP_AFFINITY=explicit,verbose,proclist=[0 - 1024] 
	exe pwd exe git log --oneline --no-color -1 
	exe sudo -E perf stat -A -a -e instructions,cache-misses,cache-references,cycles bin/$b.B
done

