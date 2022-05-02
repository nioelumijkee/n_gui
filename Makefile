################################################################################
lib.name = n_canvas
cflags = 
class.sources = \
n_canvas.c 
sources = \
./include/*
datafiles = \
n_canvas-meta.pd \
README.md \
LICENSE.txt

################################################################################
PDLIBBUILDER_DIR=pd-lib-builder/
include $(PDLIBBUILDER_DIR)/Makefile.pdlibbuilder
