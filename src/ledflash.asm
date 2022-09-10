.include "x16.inc"

STEP4 = 3
STEP8 = 4

.export ledflash

.code
ledflash:
  ; backup both data ports
  lda VERA_addr_low
  pha
  lda VERA_addr_high
  pha
  lda VERA_addr_bank
  pha
  lda VERA_ctrl
  pha
  eor #1
  sta VERA_ctrl
  lda VERA_addr_low
  pha
  lda VERA_addr_high
  pha
  lda VERA_addr_bank
  pha
  lda VERA_ctrl
  pha

  ; set data0 to point at PSG voices' volume reg (ofst 2) step3
  stz VERA_ctrl
  VERA_SET_ADDR (VRAM_psg+2) , STEP4

  ; set data1 to point at Sprite Attr, starting with sprite 8 (*8 bytes per sprite)
  inc VERA_ctrl
  VERA_SET_ADDR (VRAM_sprattr + (8*8)), STEP8

  ldx #16
@next:
  lda VERA_data0
  lsr
  lsr
  and #$0f
  sta VERA_data1
  dex
  bne @next

.repeat 2
  pla
  sta VERA_ctrl
  pla
  sta VERA_addr_bank
  pla
  sta VERA_addr_high
  pla
  sta VERA_addr_low
.endrepeat
  rts
