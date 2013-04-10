SHELL=/bin/sh
CLASS=W
VERSION=
SFILE=config/suite.def

default: header
	@ sys/print_instructions

BT: bt
bt: header
	cd BT; $(MAKE) -j8 CLASS=$(CLASS) VERSION=$(VERSION)

SP: sp
sp: header
	cd SP; $(MAKE) -j8 CLASS=$(CLASS)

LU: lu
lu: header
	cd LU; $(MAKE) -j8 CLASS=$(CLASS) VERSION=$(VERSION)

MG: mg
mg: header
	cd MG; $(MAKE) -j8 -j8 CLASS=$(CLASS)

FT: ft
ft: header
	cd FT; $(MAKE) -j8 CLASS=$(CLASS)

IS: is
is: header
	cd IS; $(MAKE) -j8 CLASS=$(CLASS)

CG: cg
cg: header
	cd CG; $(MAKE) -j8 CLASS=$(CLASS)

EP: ep
ep: header
	cd EP; $(MAKE) -j8 CLASS=$(CLASS)

UA: ua
ua: header	       
	cd UA; $(MAKE) -j8 CLASS=$(CLASS)

DC: dc
dc: header	       
	cd DC; $(MAKE) -j8 CLASS=$(CLASS)

# Awk script courtesy cmg@cray.com, modified by Haoqiang Jin
suite:
	@ mkdir -p bin
	@ awk -f sys/suite.awk SMAKE=$(MAKE) $(SFILE) | $(SHELL)


# It would be nice to make clean in each subdirectory (the targets
# are defined) but on a really clean system this won't work
# because those makefiles need config/make.def
clean:
	- rm -f core 
	- rm -f *~ */core */*~ */*.o */npbparams.h */*.obj */*.exe
	- rm -f sys/setparams sys/makesuite sys/setparams.h
	- rm -f {DC/,}ADC.{logf,view,dat,viewsz,groupby,chunks}.* 

veryclean: clean
	- rm -f config/make.def config/suite.def 
	- rm -f bin/sp.* bin/lu.* bin/mg.* bin/ft.* bin/bt.* bin/is.*
	- rm -f bin/ep.* bin/cg.* bin/ua.* bin/dc.*

header:
	@ mkdir -p bin
	@ sys/print_header



