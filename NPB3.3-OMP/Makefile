SHELL=/bin/sh
CLASS=W
VERSION=
SFILE=config/suite.def

default: header
	@ sys/print_instructions

BT: bt
bt: header
	cd BT; $(MAKE) -j CLASS=$(CLASS) VERSION=$(VERSION)

SP: sp
sp: header
	cd SP; $(MAKE) -j CLASS=$(CLASS)

LU: lu
lu: header
	cd LU; $(MAKE) -j CLASS=$(CLASS) VERSION=$(VERSION)

MG: mg
mg: header
	cd MG; $(MAKE) -j CLASS=$(CLASS)

FT: ft
ft: header
	cd FT; $(MAKE) -j CLASS=$(CLASS)

IS: is
is: header
	cd IS; $(MAKE) -j CLASS=$(CLASS)

CG: cg
cg: header
	cd CG; $(MAKE) -j CLASS=$(CLASS)

EP: ep
ep: header
	cd EP; $(MAKE) -j CLASS=$(CLASS)

UA: ua
ua: header
	cd UA; $(MAKE) -j CLASS=$(CLASS)

DC: dc
dc: header
	cd DC; $(MAKE) -j CLASS=$(CLASS)

# Awk script courtesy cmg@cray.com, modified by Haoqiang Jin
suite: header
	@ awk -f sys/suite.awk SMAKE=$(MAKE) $(SFILE) | $(SHELL)


# It would be nice to make clean in each subdirectory (the targets
# are defined) but on a really clean system this will won't work
# because those makefiles need config/make.def
clean:
	- rm -f bin/*
	- rm -f core
	- rm -f *~ */core */*~ */*.o */npbparams.h */*.obj */*.exe
	- rm -f sys/setparams sys/makesuite sys/setparams.h
	- rm -rf */rii_files

veryclean: clean

header:
	cd sys; ${MAKE} all
	@ mkdir -p bin
	@ sys/print_header
