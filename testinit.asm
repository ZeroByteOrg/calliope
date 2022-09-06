
.export _init

YMreg = $9f40
YMval = $9f41

.proc _init: near
  LDA #$0F
  LDX #$E0
NEXT_RR:
  JSR YMWRITE
  INX
  BNE NEXT_RR
  LDA #7
  LDX #8
  NOP
  NOP
  NOP
  NOP
NEXT_KEYUP:
  JSR YMWRITE
  DEC
  BPL NEXT_KEYUP
  LDA #$80
  LDX #$19
  JSR YMWRITE  ; AMS is a special case
  LDA #02
  LDX #01
  JSR YMWRITE  ; reset LFO
  LDA #0
  LDX #$0F
FULL_CLEAR:
  JSR YMWRITE
  INX
  CMP #2
  BNE FULL_CLEAR
  RTS
.endproc

YMWRITE:
  BIT YMval
  BMI YMWRITE
  STX YMreg
  NOP
  NOP
  NOP
  NOP
  NOP
  STA YMval
  RTS
