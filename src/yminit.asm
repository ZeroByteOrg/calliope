
.export _ym_init
.export _psg_init

YMreg = $9f40
YMval = $9f41

.proc _ym_init: near
  LDA #$0F
  LDX #$E0
NEXT_RR:         ; Set Release Rate = max
  JSR YMWRITE
  INX
  BNE NEXT_RR
  LDA #7
  LDX #8
NEXT_KEYUP:      ; Release all 8 voices
  JSR YMWRITE
  DEC
  BPL NEXT_KEYUP
  ; AMS is a special case of writing $80 into AMS/PMS register
  ; Write $80 here, and let the FULL_CLEAR loop write the PMS=0 later
  LDA #$80
  LDX #$19
  JSR YMWRITE
  LDA #02
  LDX #01
  JSR YMWRITE  ; reset LFO
  ; special-cases handled. Now write 0 into every register.
  LDA #0
  LDX #$0F     ; loop through regs $0F -> $FF -> $00 -> $01.
FULL_CLEAR:
  JSR YMWRITE
  INX
  CPX #2       ;
  BNE FULL_CLEAR
  RTS
.endproc

.proc YMWRITE: near
  ; YM Status busy flag check (bit7 set = YM is busy.)
  BIT YMval
  BMI YMWRITE
  ; pre-write delay (currently 0)
  STX YMreg
  ; reg/val delay (minimum 5 NOPS)
  NOP
  NOP
  NOP
  NOP
  NOP
  STA YMval
  RTS
.endproc

.proc _psg_init: near
  STZ VERA_ctrl
  LDA #$C0
  STA VERA_addr_low
  LDA #$F9
  STA VERA_addr_high
  LDA #$11
  STA VERA_addr_bank
  LDX #$40
next:
  STZ VERA_data0
  DEX
  BPL next
  RTS
.endproc
