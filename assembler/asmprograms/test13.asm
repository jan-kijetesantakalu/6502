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
