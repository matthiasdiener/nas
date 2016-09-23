#!/bin/bash

set -o errexit -o nounset -o posix -o pipefail

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $DIR

bm="bt cg ep ft is lu mg sp ua"
input=B

OUTDIR=~/Dropbox/results/$(hostname)/$(basename $DIR)/$input
mkdir -p $OUTDIR

# does not work with quotation marks:
exe() { echo "++ $@" |& tee -a $OUTDIR/$b.txt ;  if [[ "$1" == "export" ]]; then $@ ; fi ;  $@ |& tee -a $OUTDIR/$b.txt ; }

for b in $bm; do
	echo -n > $OUTDIR/$b.txt #clean output file
	exe echo "running $b"
	exe date
	exe uname -a
	[[ -x /usr/bin/hwloc-ls ]] && exe hwloc-ls --of console

	exe export GOMP_CPU_AFFINITY=0-227 KMP_AFFINITY=explicit,verbose,proclist=[0-227]
	exe pwd
	exe git log -1 --oneline
	exe sudo -E perf stat -A -a -e instructions,cache-misses,cache-references,cycles bin/$b.$input
done

