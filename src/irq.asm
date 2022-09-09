.include "x16.inc"

.export _install_irq
.export _remove_irq

.import playmusic_IRQ

.import _print_addresses

.code
.proc irqhandler: near
  lda VERA_ctrl  ; both branches need the ctrl value backed up, so load it first
  tay
  lda #$02 ; check for VERA Line IRQ
  bit VERA_isr
  beq VBLANK
RASTERLINE:
  ; this is a VERA line IRQ. ACK and process before RTI
  sta VERA_isr
  phy ; save the VERA_ctrl value for after Zsound finishes playing...
  jsr playmusic_IRQ
  lda #2
  sta VERA_ctrl
  lda #(640>>2) ; restore screen width to 640 so the "not-lit" part of CPU is BG bitmap.
  sta VERA_dc_hstop
  ply
  sty VERA_ctrl ; restore VERA_ctrl to previous state
  ply           ; restore CPU registers and return from IRQ
  plx
  pla
  rti
VBLANK:
  lda #2
  sta VERA_ctrl
  lda #(632>>2)  ; set H_STOP to x=632 to show CPU raster bar
  sta VERA_dc_hstop
  sty VERA_ctrl
  ;jsr _print_addresses
  jmp $ffff
.endproc

system_irq := (*-2) ; self-mods the jmp $ffff at DONE: in irqhandler

.proc _install_irq: near
  lda IRQVec
  ldx IRQVec+1
  cmp #<irqhandler
  bne INSTALL
  cpx #>irqhandler
  beq EXIT
INSTALL:
  sta system_irq
  stx system_irq+1
  lda #<irqhandler
  ldx #>irqhandler
  sei
  sta IRQVec
  stx IRQVec+1
  cli
EXIT:
  rts
.endproc

.proc _remove_irq: near
  lda IRQVec
  ldx IRQVec+1
  cmp #<irqhandler
  bne EXIT
  cpx #>irqhandler
  bne EXIT
  lda system_irq
  ldx system_irq+1
  sei
  sta IRQVec
  stx IRQVec+1
  cli
EXIT:
  rts
.endproc
