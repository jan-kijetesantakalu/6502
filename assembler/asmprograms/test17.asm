; XOR a string at $2000 with a key at $1000, Output to $3000
write_test_string:
	;Hello World! key = 0x03
	LDA #72
	STA $2000
	LDA #101
	STA $2001
	LDA #108
	STA $2002
	LDA #108
	STA $2003
	LDA #111
	STA $2004
	LDA #44
	STA $2005
	LDA #32
	STA $2006
	LDA #87
	STA $2007
	LDA #111
	STA $2008
	LDA #114
	STA $2009
	LDA #108
	STA $2010
	LDA #100
	STA $2011
	LDA #33
	STA $2012
	LDA #3
	STA $1000
loop:
	LDA  $2000,X
	CMP  #0
	BEQ  end
	LDA  $1000
	EOR  $2000,X
	STA  $3000,X
	INX
	JMP loop	
end:
	BRK
