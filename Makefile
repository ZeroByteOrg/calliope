DIR_ZSOUND ?= ../zsound
DIR_BIN ?= ./root
PROGNAME ?= CALLIOPE.PRG

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

$(TARGET): $(SRCS) $(INCS) $(LIBZSOUND) $(ZSOUND_INCS)
	@mkdir -p $(DIR_BIN)
	cl65 $(FLAGS) $(FLAGS_ZSOUND) -O -o $@ $(SRCS) $(LIBZSOUND)

all: $(TARGET) $(MEDIA) skins

.PHONY: skins
skins:
	@cd skins && $(MAKE)

media: $(MEDIA)

sdcard: $(TARGET)
#	mcopy -s -o $(DIR_BIN)/* $(MDRIVE)
	mcopy -o $(TARGET) $(MPATH)/$(PROGNAME)
	mcopy -o res/BG.BIN $(MPATH)/
	mcopy -o res/2BPPALTFONT.BIN $(MPATH)/
	mcopy -o res/OPENING.BIN $(MPATH)/
	mcopy -o res/PAL.BIN $(MPATH)/
	mcopy -o res/LED.BIN $(MPATH)/
	mcopy -o skins/*.SK $(MPATH)/

clean:
	rm -f $(TARGET) $(SYM)
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
