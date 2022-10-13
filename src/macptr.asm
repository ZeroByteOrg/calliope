MACPTR := $FF44

.import popa
.export _macptr

;---------------------------------------------------[ _macptr ]---------------
; C wrapper for the Kernal MACPTR routine.
;
; int __fastcall__ macptr(char nbytes, void* addr);

; MACPTR is a block-read routine that reads "up to 512 bytes" and stores them
; into memory at the specified location. I am dubious of this claim, as the
; underlying code seems to return only up to 256 bytes, and the "n-bytes"
; parameter is the .A register - so how could it give more?
;
; MACPTR: A  = n-bytes (0 = MACPTR decides how many to load)
;         XY = destination memory address.
;         If the load is going into Hi RAM, the active bank is used and
;         incremented by MACPTR as bank wraps occur.
;
;         VERIFY THIS:
;         Returns XY = the number of bytes transferred, Carry flag clear
;            or   XY = 0, Carry set if block xfer unsupported by device.
;         Messages such as EOI, File not open, etc are returned in the
;         status byte (load it yourself to check it, apparently)
;
; This C wrapper returns 0 as bytes read if error, or if that was returned by
; MACPTR. It is up to the caller to check the status byte thereafter.
;

.proc _macptr: near
  ; convert .AX (addr) into .XY for Kernal Call, and pop nbytes from C stack
	phx
	tax
	jsr popa
	ply
	clc
	jsr MACPTR
	bcs macptr_unsupported
macptr_success:
  ; convert MACPTR return .XY into C return .AX
	txa
	phy
	plx
	rts
macptr_unsupported: ; return -1 if MACPTR returns C set.
	ldx #$FF
	lda #$FF
	rts
.endproc
