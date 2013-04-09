SHELL=/bin/sh
CLASS=U
NPROCS=1
SUBTYPE=
VERSION=
SFILE=config/suite.def

default: header
	@ sys/print_instructions

BT: bt
bt: header
	cd BT; $(MAKE) -j8 NPROCS=$(NPROCS) CLASS=$(CLASS) SUBTYPE=$(SUBTYPE) VERSION=$(VERSION)

SP: sp
sp: header
	cd SP; $(MAKE) -j8 NPROCS=$(NPROCS) CLASS=$(CLASS)

LU: lu
lu: header
	cd LU; $(MAKE) -j8 NPROCS=$(NPROCS) CLASS=$(CLASS) VERSION=$(VERSION)

MG: mg
mg: header
	cd MG; $(MAKE) -j8 NPROCS=$(NPROCS) CLASS=$(CLASS)

FT: ft
ft: header
	cd FT; $(MAKE) -j8 NPROCS=$(NPROCS) CLASS=$(CLASS)

IS: is
is: header
	cd IS; $(MAKE) -j8 NPROCS=$(NPROCS) CLASS=$(CLASS)

CG: cg
cg: header
	cd CG; $(MAKE) -j8 NPROCS=$(NPROCS) CLASS=$(CLASS)

EP: ep
ep: header
	cd EP; $(MAKE) -j8 NPROCS=$(NPROCS) CLASS=$(CLASS)

DT: dt
dt: header
	cd DT; $(MAKE) -j8 CLASS=$(CLASS)

# Awk script courtesy cmg@cray.com, modified by Haoqiang Jin
suite:
	@ mkdir -p bin
	@ awk -f sys/suite.awk SMAKE=$(MAKE) $(SFILE) | $(SHELL)


# It would be nice to make clean in each subdirectory (the targets
# are defined) but on a really clean system this will won't work
# because those makefiles need config/make.def
clean:
	- rm -f core 
	- rm -f *~ */core */*~ */*.o */npbparams.h */*.obj */*.exe
	- rm -f MPI_dummy/test MPI_dummy/libmpi.a
	- rm -f sys/setparams sys/makesuite sys/setparams.h
	- rm -f btio.*.out*

veryclean: clean
	- rm -f config/make.def config/suite.def 
	- rm -f bin/sp.* bin/lu.* bin/mg.* bin/ft.* bin/bt.* bin/is.* 
	- rm -f bin/ep.* bin/cg.* bin/dt.*

header:
	@ mkdir -p bin
	@ sys/print_header

