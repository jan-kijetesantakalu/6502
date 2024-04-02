;https://skilldrick.github.io/easy6502/
;  ___           _        __ ___  __ ___
; / __|_ _  __ _| |_____ / /| __|/  \_  )
; \__ \ ' \/ _` | / / -_) _ \__ \ () / /
; |___/_||_\__,_|_\_\___\___/___/\__/___|

; Change direction: W A S D


  jsr init
  jsr loop

init:
  jsr initSnake
  jsr generateApplePosition
  rts


initSnake:
  lda #2  ;start direction
  sta $02

  lda #4  ;start length (2 segments)
  sta $03
  
  lda #$11
  sta $10
  
  lda #$10
  sta $12
  
  lda #$0f
  sta $14 ; body segment 1
  
  lda #$04
  sta $11
  sta $13 ; body segment 1
  sta $15 ; body segment 2
  rts


generateApplePosition:
  ;load a new random byte into $00
  lda $fe
  sta $00

  ;load a new random number from 2 to 5 into $01
  lda $fe
  and #$03 ;mask out lowest 2 bits
  clc
  adc #2
  sta $01

  rts

loop:
  jsr readKeys
readKeys:
  lda $ff
  cmp #$77
  beq upKey
  

upKey:
  lda #4
  bit $02
  bne illegalMove
  lda #1
  sta $02
  rts

illegalMove:
  rts