################################################################################
lib.name = n_gui
cflags = 
class.sources = \
n_canvas.c \
n_knob.c
sources = \
./include/*
datafiles = \
n_gui-meta.pd \
n_canvas-help.pd \
n_knob-help.pd \
README.md \
LICENSE.txt

################################################################################
PDLIBBUILDER_DIR=pd-lib-builder/
include $(PDLIBBUILDER_DIR)/Makefile.pdlibbuilder
