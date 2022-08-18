.include "x16.inc"

.export _install_irq
.export _remove_irq

.import playmusic_IRQ

.code
.proc irqhandler: near
  lda #$01
  and VERA_isr
  beq DONE
  jsr playmusic_IRQ
DONE:
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
