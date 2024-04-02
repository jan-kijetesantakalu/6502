; Calculates the 8 bit $F6 and 9 bit remainder of a 16 bit unsigned integer in
; $F0/$F1. The result is always in the range 0 to 255 and is held in
; $F6, the remainder is in the range 0 to 511 and is held in $F2/$F3
;
; partial results are held in $F4/$F5
;
; This routine is the complement to the integer square program.
;
; Destroys A, X registers.

; variables - must be in RAM

SqRoot:
	LDA	#$00		; clear A
	STA	$F2		; clear remainder low byte
	STA	$F3		; clear remainder high byte
	STA	$F6		; clear $F6
	LDX	#$08		; 8 pairs of bits to do
Loop:
	ASL	$F6		; $F6 = $F6 * 2

	ASL	$F0		; shift highest bit of number ..
	ROL	$F1		;
	ROL	$F2		; .. into remainder
	ROL	$F3		;

	ASL	$F0		; shift highest bit of number ..
	ROL	$F1		;
	ROL	$F2		; .. into remainder
	ROL	$F3		;

	LDA	$F6		; copy $F6 ..
	STA	$F4		; .. to $F4
	LDA	#$00		; clear byte
	STA	$F5		; clear temp high byte

	SEC			; +1
	ROL	$F4		; temp = temp * 2 + 1
	ROL	$F5		;

	LDA	$F3		; get remainder high byte
	CMP	$F5		; comapre with partial high byte
	BCC	Next		; skip sub if remainder high byte smaller

	BNE	Subtr		; do sub if <> (must be remainder>partial !)

	LDA	$F2		; get remainder low byte
	CMP	$F4		; comapre with partial low byte
	BCC	Next		; skip sub if remainder low byte smaller

				; else remainder>=partial so subtract then
				; and add 1 to $F6. carry is always set here
Subtr:
	LDA	$F2		; get remainder low byte
	SBC	$F4		; subtract partial low byte
	STA	$F2		; save remainder low byte
	LDA	$F3		; get remainder high byte
	SBC	$F5		; subtract partial high byte
	STA	$F3		; save remainder high byte

	INC	$F6		; increment $F6
Next
	DEX			; decrement bit pair count
	BNE	Loop		; loop if not all done

	RTS