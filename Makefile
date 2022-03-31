################################################################################
lib.name = n_gui
cflags = 
class.sources = \
n_canvas.c \
n_knob.c 
sources = \
./include/*
datafiles = \
README.md \
LICENSE.txt

################################################################################
PDLIBBUILDER_DIR=pd-lib-builder/
include $(PDLIBBUILDER_DIR)/Makefile.pdlibbuilder
