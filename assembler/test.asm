LDA #$A0 
ADC #31
STA $AB
ADC #%00000001
LDX $AB
INX
LDA #2
STA $F0C0
ADC $F000,X
JMP ($FFFE)
BRK
LDY $AB
