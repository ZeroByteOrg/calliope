.include "cx16.inc"

.importzp tmp1, ptr1, sreg
.import popa, popax

.export _vram_copy

; C prototype:
; void __fastcall__ vram_copy(
;   const char dbank,
;   char* data,
;   const char vbank,
;   const char* vaddr,
;   unsigned long bytes
; )

; make some significant names for the tmp ZP variables (for readability)
data  = ptr1
bytes = tmp1 ; 32bit value... also uses tmp2

.proc _vram_copy: near

  ; rightmost arg is bytes - save to ZP for counter
  sta bytes
  stx bytes+1
  lda sreg
  sta bytes+2
  lda sreg+1
  sta bytes+3

  ; pop vaddr and vbank from the stack and set VERA data0 pointer
  jsr popax
  sta VERA::ADDR
  stx VERA::ADDR + 1
  jsr popa
  ora #VERA::INC1
  sta VERA::ADDR+2

  ; pop addr and use as ZP pointer
  jsr popax
  sta data
  stx data+1

  ; pop dbank (data bank) and use it to set RAM_BANK
  jsr popa
  sta RAM_BANK

  ldy data
  stz data      ; page-align the pointer for efficiency
  ldx bytes     ; .X = low byte of bytes counter
  bra checkdone
nextpage:
  inc A
  sta data+1
checkdone:
  dex
  cpx #$ff
  bne copyit
  lda bytes+1
  dec
  sta bytes+1
  cmp #$ff
  bne copyit
  lda bytes+2
  dec
  sta bytes+2
  cmp #$ff
  beq copyit
  lda bytes+3
  dec
  sta bytes+3
  cmp #$ff
  beq done
copyit:
  lda (data),Y
  sta VERA::DATA0
  iny
  bne checkdone
  lda data+1
  cmp #$bf
  bne nextpage
  lda #$a0
  sta data+1
  inc BANK::RAM
  bra checkdone
done:
  lda #0
  ldx #0
  rts
.endproc
