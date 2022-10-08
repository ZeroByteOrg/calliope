DIR_ZSOUND ?= ../zsound
DIR_BUILD ?= ./root
DIR_SKINS ?= ./skins
PROGNAME ?= CALLIOPE.PRG

# Change this to be the name of the skin to use as the default skin.
#SKINNAME ?= BASIC.SK
SKINNAME ?= STEAMPUNK.SK

MDRIVE ?= g:
MDIR ?= .
MPATH = $(MDRIVE)/$(MDIR)

#VGM = $(notdir $(wildcard res/vgm/*.VGM))
#ZSM = $(VGM:%.VGM=%.ZSM)

#ZSMFILES = $(addprefix $(DIR_BUILD)/ZSM/,$(ZSM))
#MEDIA    = $(ZSMFILES)

ZSOUND_INCLIST = $(wildcard $(DIR_ZSOUND)/inc/*.inc)
ZSOUND_HLIST = $(wildcard $(DIR_ZSOUND)/include/*.h)
ZSOUND_INCFILES = $(notdir $(ZSOUND_INCLIST)) $(notdir $(ZSOUND_HLIST))

ZSOUND_LIB = $(DIR_ZSOUND)/lib/zsound.lib
ZSOUND_INCS = $(addprefix zsound/,$(ZSOUND_INCFILES))
LIBZSOUND = zsound/zsound.lib
FLAGS_ZSOUND = -I./zsound -L./zsound --asm-include-dir ./zsound

TARGET ?= $(DIR_BUILD)/$(PROGNAME)
SYM ?= $(notdir $(TARGET:%.PRG=%.sym))
FLAGS ?= -t cx16 -g -Ln $(SYM)

SRCS = $(wildcard src/*.c) $(wildcard src/*.asm)
INCS = $(wildcard src/*.h) $(wildcard src/*.inc)

SKIN = $(DIR_BUILD)/DEFAULT.SK
DITTY = $(DIR_BUILD)/OPENING.BIN


# Main target to build the program and its skin data.
.phony: calliope
calliope: $(TARGET) skin $(DITTY)

.PHONY: help
help:
	@echo "Build Targets:"
	@echo
	@echo "calliope:"
	@echo "  Builds the Calliope program and the default skin file"
	@echo
	@echo "allskins:"
	@echo "  Bulds all skins. (not done except here or when making sdcard)"
	@echo
	@echo "sdcard:"
	@echo "  Installs Calliope and all skins onto SD card image using"
	@echo "  mcopy. Current SD path is $(MPATH) Edit Makefile to alter this"
	@echo
	@echo "autoboot:"
	@echo "  Same as sdcard, but additionally makes a copy of the program with"
	@echo "  the filneame AUTOBOOT.X16 so the X16 will automatically run Calliope"
	@echo "  when booted with this SD image/card inserted."
	@echo
	@echo "distclean:"
	@echo "  in addition to the usual make clean behavior, the local copy of zsound"
	@echo "  is removed."


# Build Calliope and all skins
all: calliope allskins

.PHONY: skin
skin: $(SKIN)
#	cp $(DIR_SKINS)/$(SKINNAME) $(SKIN)

skins: allskins
allskins:
	@cd $(DIR_SKINS) && $(MAKE) all
	@mkdir -p $(DIR_BUILD)
	cp $(DIR_SKINS)/*.SK $(DIR_BUILD)/

sdcard: calliope
	mcopy -o $(TARGET) $(MPATH)/$(PROGNAME)
	mcopy -o $(DITTY) $(MPATH)/
	mcopy -o $(SKIN) $(MPATH)/

autoboot: sdcard
	mcopy -o $(TARGET) $(MPATH)/AUTOBOOT.X16

sdskins: skins $(SKIN)
	mcopy -o $(DIR_SKINS)/*.SK $(MPATH)/

# Build CALLIOPE.PRG
$(TARGET): $(SRCS) $(INCS) $(LIBZSOUND) $(ZSOUND_INCS)
	@mkdir -p $(DIR_BUILD)
	cl65 $(FLAGS) $(FLAGS_ZSOUND) -O -o $@ $(SRCS) $(LIBZSOUND)

# Build DEFAULT.SK from the skin named in $SKINNAME
#$(SKIN):
skin:
	@cd $(DIR_SKINS) && make $(SKINNAME)
	@mkdir -p $(DIR_BUILD)
	cp $(DIR_SKINS)/$(SKINNAME) $(SKIN)

$(SKIN): $(DIR_SKINS)/$(SKINNAME)
	@mkdir -p $(DIR_BUILD)
	cp $< $@

# Copy intro.zsm from the default skin
$(DITTY): $(DIR_SKINS)/$(SKINNAME:%.SK=%)/intro.zsm
	@mkdir -p $(DIR_BUILD)
	cp $< $@

# # Buld a specific skin by name
$(DIR_SKINS)/$(SKINNAME):
	@cd $(DIR_SKINS) && make $(SKINNAME)

# Don't require building a .zsm file
.PHONY $(DIR_SKINS)/$(SKINNAME:%.SK=%)/intro.zsm:

clean:
	rm -f $(DIR_BUILD)/*
	rm -f $(SYM)
	rm -f src/*.o src/*.s
	@cd skins && $(MAKE) clean

libclean:
	rm -f zsound/*

distclean: clean libclean

#zsm: $(ZSMFILES)

zsound: $(LIBZSOUND) $(ZSOUND_INCS)

$(DIR_BUILD)/ZSM/%.ZSM: res/vgm/%.VGM
	@mkdir -p $(dir $@)
	vgm2zsm $< $@

$(LIBZSOUND): $(ZSOUND_LIB)
	@mkdir -p zsound
	cp $< $@

zsound/%.h:	$(DIR_ZSOUND)/include/%.h
	@mkdir -p zsound
	cp $< $@

zsound/%.inc:	$(DIR_ZSOUND)/inc/%.inc
	@mkdir -p zsound
	cp $< $@

$(ZSOUND_LIB):
	@cd $(DIR_ZSOUND) ; make lib

.PHONY: $(DIR_ZSOUND)/inc/%.inc

.PHONY: $(DIR_ZSOUND)/include/%.h

.PHONY: $(INCS)
