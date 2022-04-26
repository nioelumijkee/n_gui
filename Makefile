################################################################################
lib.name = n_gui
cflags = 
class.sources = \
n_knob.c 
sources = \
./include/*
datafiles = \
n_gui-meta.pd \
README.md \
LICENSE.txt

################################################################################
PDLIBBUILDER_DIR=pd-lib-builder/
include $(PDLIBBUILDER_DIR)/Makefile.pdlibbuilder
