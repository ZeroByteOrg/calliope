SKINS ?= $(patsubst %/,%,$(wildcard */))

MKPAL ?= ../tools/img2pal.py
MKBIN ?= ../tools/img2tileset.py

BG   := bg.png
FONT := font.png
LED  := leds.png
SONG := intro.zsm

GRAPHICS := $(BG) $(LED) $(FONT)
GFXBIN   := $(GRAPHICS:%.png=%.bin)
PALETTE  := $(GRAPHICS:%.png=%.pal)

.PHONY: all
all: $(addsuffix .SK,$(SKINS))
	@echo All skins complete.
	@rm -f $(GFXBIN) $(PALETTE)

clean:
	@rm -f *.SK
	@rm -f $(GFXBIN)
	@rm -f $(PALETTE)

%.SK:
	$(eval DIR   := $(patsubst %.SK,%/,$@))
	$(eval _BG   := $(addprefix $(DIR),$(BG)))
	$(eval _FONT := $(addprefix $(DIR),$(FONT)))
	$(eval _LED  := $(addprefix $(DIR),$(LED)))
	$(eval GRAPHICS := $(BG) $(LED) $(FONT))
	$(eval GFXBIN   := $(GRAPHICS:%.png=%.bin))
	$(eval PALETTE  := $(GRAPHICS:%.png=%.pal))
	@[ -x $(MKPAL) ]
	@[ -x $(MKBIN) ]
	$(MKBIN) -s 640x240 -b 4 -n $(_BG) $(BG:%.png=%.bin)
	$(MKBIN) -s 8x8     -b 2 -n $(_FONT) $(FONT:%.png=%.bin)
	$(MKBIN) -s 8x8     -b 4 -n $(_LED) $(LED:%.png=%.bin)
	$(MKPAL) $(_BG) $(BG:%.png=%.pal)
	$(MKPAL) $(_FONT) $(FONT:%.png=%.pal)
	$(MKPAL) $(_LED) $(LED:%.png=%.pal)
	@echo -n Concatenating $@ . . .
	@cat $(GFXBIN) $(PALETTE) > $@
	@echo done
	@rm -f $(GFXBIN) $(PALETTE)
