DIR_ZSOUND ?= ../zsound
DIR_BIN ?= ./root
DIR_SKINS ?= ./skins
PROGNAME ?= CALLIOPE.PRG

# Change this to be the name of the skin to use as the default skin.
#SKINNAME ?= BASIC.SK
SKINNAME ?= STEAMPUNK.SK

MDRIVE ?= g:
MDIR ?= .
MPATH = $(MDRIVE)/$(MDIR)

VGM = $(notdir $(wildcard res/vgm/*.VGM))
ZSM = $(VGM:%.VGM=%.ZSM)

ZSMFILES = $(addprefix $(DIR_BIN)/ZSM/,$(ZSM))
MEDIA    = $(ZSMFILES)

ZSOUND_INCLIST = $(wildcard $(DIR_ZSOUND)/inc/*.inc)
ZSOUND_HLIST = $(wildcard $(DIR_ZSOUND)/include/*.h)
ZSOUND_INCFILES = $(notdir $(ZSOUND_INCLIST)) $(notdir $(ZSOUND_HLIST))

ZSOUND_LIB = $(DIR_ZSOUND)/lib/zsound.lib
ZSOUND_INCS = $(addprefix zsound/,$(ZSOUND_INCFILES))
LIBZSOUND = zsound/zsound.lib
FLAGS_ZSOUND = -I./zsound -L./zsound --asm-include-dir ./zsound

TARGET ?= $(DIR_BIN)/$(PROGNAME)
SYM ?= $(notdir $(TARGET:%.PRG=%.sym))
FLAGS ?= -t cx16 -g -Ln $(SYM)

SRCS = $(wildcard src/*.c) $(wildcard src/*.asm)
INCS = $(wildcard src/*.h) $(wildcard src/*.inc)

SKIN = $(DIR_BIN)/DEFAULT.SK
DITTY = $(DIR_BIN)/OPENING.BIN

.PHONY: skins

.phony: calliope

# Main target to build the program and its skin data.
calliope: $(TARGET) $(SKIN) $(DITTY)

# Build CALLIOPE.PRG
$(TARGET): $(SRCS) $(INCS) $(LIBZSOUND) $(ZSOUND_INCS)
	@mkdir -p $(DIR_BIN)
	cl65 $(FLAGS) $(FLAGS_ZSOUND) -O -o $@ $(SRCS) $(LIBZSOUND)

# Build DEFAULT.SK from the skin named in $SKINNAME
$(SKIN): $(DIR_SKINS)/$(SKINNAME)
	@mkdir -p $(DIR_BIN)
	cp $< $@

$(DIR_SKINS)/$(SKINNAME):
	@cd $(DIR_SKINS) && $(MAKE) $(SKINNAME)

$(DITTY): $(DIR_SKINS)/$(SKINNAME:%.SK=%)/intro.zsm
	@mkdir -p $(DIR_BIN)
	cp $< $@

.PHONY $(DIR_SKINS)/$(SKINNAME:%.SK=%)/intro.zsm:


all: $(TARGET) $(MEDIA) skins

skins: $(SKIN)
	@cd skins && $(MAKE)
	cp $(DIR_SKINS)/$(SKINNAME) $(SKIN)

media: $(MEDIA)

sdcard: calliope
	mcopy -o $(TARGET) $(MPATH)/$(PROGNAME)
	mcopy -o $(DITTY) $(MPATH)/
	mcopy -o $(SKIN) $(MPATH)/

autoboot: sdcard
	mcopy -o $(TARGET) $(MPATH)/AUTOBOOT.X16

sdskins: skins
	mcopy -o $(DIR_SKINS)/*.SK $(MPATH)/

clean:
	@rm -f $(DIR_BIN)/*
	@rm -f $(SYM)
	@rm -f src/*.o src/*.s
	@cd skins && $(MAKE) clean

mediaclean:
	rm -rf $(DIR_BIN)*

libclean:
	rm -f zsound/*

distclean: clean mediaclean libclean

zsm: $(ZSMFILES)

zsound: $(LIBZSOUND) $(ZSOUND_INCS)

$(DIR_BIN)/ZSM/%.ZSM: res/vgm/%.VGM
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
