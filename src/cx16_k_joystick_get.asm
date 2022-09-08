; Kernal API:
; JMP to $FF56
; -- Args:
;   .A = joystick ID number (0=virtual kbd joystick, 1-4 = SNES joysticks)
; -- Returns:
;   .A: controller byte 0 bits: (B Y SELECT START UP DN LFT RT)
;   .X: controller byte 1 bits: (A X L R 1 1 1 1)
;   .Y: $00 = joystick present, $FF = joystick not present
;
; C Prototype: char __fastcall__ cx16_k_joystick_get(char id, unsigned int* state);
;
; Args:
;   id:    joystick index number.
;   state: Pointer to u16 to the value .AX from Kernal
;
; Returns:
;   If .Y=$FF then state is not modified and the function returns 0 (fals)
;   If .Y=$00 then .AX are stored in state and function returns 1 (true)
;

      .importzp       tmp1
      .import         popa

      .export         _cx16_k_joystick_get


.code

.proc _cx16_k_joystick_get: near
  sta tmp1
  stx tmp1+1
  jsr popa
  jsr $FF56        ; X16 Kernal API address for joystick_get
  cpy #0
  bne no_joystick  ; carry flag guaranteed set from cpy #0 as well....
  sta (tmp1),y
  txa
  iny
  sta (tmp1),y
  clc
no_joystick:       ; if here from branch, C=1  else C=0
  lda #0
  rol
  eor #$01         ; .A now holds 1 if joystick present, 0 if not.
  ldx #0
  rts
.endproc
