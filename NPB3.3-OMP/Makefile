SHELL=/bin/sh
CLASS=W
VERSION=
SFILE=config/suite.def

LIBMAPPING_PRE_FILTER=2
LIBMAPPING_POS_FILTER=1
LIBMAPPING_MAX_OVERHEAD=0
LIBMAPPING_DEBUG=1

MAPFLAGS=
MAPFLAGS += -DLIBMAPPING_REMAP_SIMICS_COMM_PATTERN_REALMACHINESIDE
#MAPFLAGS += -DLIBMAPPING_REMAP_SIMICS_COMM_PATTERN_SIMSIDE
#MAPFLAGS += -DLIBMAPPING_REAL_REMAP_SIMICS

MAPFLAGS += -DPRE_FILTER=$(LIBMAPPING_PRE_FILTER) -DPOS_FILTER=$(LIBMAPPING_POS_FILTER) -DMAX_OVERHEAD=$(LIBMAPPING_MAX_OVERHEAD)
MAPFLAGS += -DDEBUG=$(LIBMAPPING_DEBUG)
#MAPFLAGS += -DLIBMAPPING_WITH_PAPI

default: header
	@ sys/print_instructions

BT: bt
bt: header
	cd BT; $(MAKE) CLASS=$(CLASS) VERSION=$(VERSION)

SP: sp		       
sp: header	       
	cd SP; $(MAKE) CLASS=$(CLASS)

LU: lu		       
lu: header	       
	cd LU; $(MAKE) CLASS=$(CLASS) VERSION=$(VERSION)

MG: mg		       
mg: header	       
	cd MG; $(MAKE) CLASS=$(CLASS)

FT: ft		       
ft: header	       
	cd FT; $(MAKE) CLASS=$(CLASS)

IS: is		       
is: header	       
	cd IS; $(MAKE) CLASS=$(CLASS)

CG: cg		       
cg: header	       
	cd CG; $(MAKE) CLASS=$(CLASS)

EP: ep		       
ep: header	       
	cd EP; $(MAKE) CLASS=$(CLASS)

UA: ua
ua: header	       
	cd UA; $(MAKE) CLASS=$(CLASS)

DC: dc
dc: header	       
	cd DC; $(MAKE) CLASS=$(CLASS)

# Awk script courtesy cmg@cray.com, modified by Haoqiang Jin
suite: mappinglib
	@ awk -f sys/suite.awk SMAKE=$(MAKE) $(SFILE) | $(SHELL)

mappinglib: bin/mapping-lib.o bin/remap.o bin/map_algorithm.o bin/libremap.o

bin/libremap.o:
	gcc -o bin/libremap.o -c ../../libmapping/libremap.c -O2 -fopenmp -DENABLE_OPENMP $(MAPFLAGS)

bin/mapping-lib.o:
	gcc -o bin/mapping-lib.o -c ../../libmapping/libmapping.c -O2 -fopenmp -DENABLE_OPENMP $(MAPFLAGS)

bin/remap.o:
	gcc -o bin/remap.o -c remap.c -O2 -fopenmp -DENABLE_OPENMP -I../../libmapping $(MAPFLAGS)

bin/map_algorithm.o:
	g++ -o bin/map_algorithm.o -c ../../libmapping/map_algorithm.cpp -O2 -fopenmp $(MAPFLAGS)

simics_test:
	gcc -o bin/simics_test simics_test.c -O2 -DENABLE_OPENMP -I../../libmapping -DLIBMAPPING_REAL_REMAP_SIMICS

# It would be nice to make clean in each subdirectory (the targets
# are defined) but on a really clean system this will won't work
# because those makefiles need config/make.def
clean:
	- rm bin/*
	- rm -f core 
	- rm -f *~ */core */*~ */*.o */npbparams.h */*.obj */*.exe
	- rm -f sys/setparams sys/makesuite sys/setparams.h
	- rm -rf */rii_files

veryclean: clean

header: mappinglib
	@ sys/print_header



