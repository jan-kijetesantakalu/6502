start:
LDA #$01
JMP jump
middle:
LDA #$02
JMP end
jump:
ADC #15
STA $2000
JMP middle
end:
STA $2001
