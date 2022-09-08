.export _cx16_k_kbdbuf_put

; Kernal API:
; JMP to $FEC3
; -- Append char in .A to the keyboard queue
; -- Returns: none
;
; C Prototype: void __fastcall__ cx16_k_kbdbuf_put(const char key);
;

.code

.proc _cx16_k_kbdbuf_put: near
  jsr $FF68
  lda #0
  ldx #0
  rts

.endproc
