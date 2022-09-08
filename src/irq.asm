.include "x16.inc"

.export _install_irq
.export _remove_irq

.import playmusic_IRQ

.import print_addresses

.code
.proc irqhandler: near
  lda #$02 ; check for VERA Line IRQ
  bit VERA_isr
  beq SHOW_RAM_INDICATORS
  ; this is a VERA line IRQ. ACK and process before RTI
  sta VERA_isr
  lda #5 ; green
  sta VERA_dc_border
  jsr playmusic_IRQ
  stz VERA_dc_border
  ply
  plx
  pla
  rti
SHOW_RAM_INDICATORS:
  ;jsr print_addresses
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
