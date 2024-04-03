; Find prime numbers and put them into memory
; Find 8 bit prime numbers
; Write them in order in location $2000 up to $20FF
; Simplest solution first
; Zero out memeroy at $2000 to $20FF

;LDX #0
;LDA #0
;clear:
;    STA $2000,X
;    INX
;    CPX #$FF
;    BNE clear

; Index in the ACCUMULATOR
LDA #01             ; First prime
STA $0500            ; Put A in location 500

next_value:
    LDA $0500
    CLC
    ADC #01
    CMP #255   ; Check whether we are at maxium value of A
    BEQ end       ; We've finished

check_prime:
    STA $0500    ; STORE A at location $500
    LDX #01
    loop:
        INX
        STX $1000   ; put value of X in location 1000
        CMP $1000   ; really we only need to go up to the sqrt of A
        BEQ is_prime
        ; check whether X is a multiple of A
        is_multiple:
            SEC
            SBC $1000    ; subtract X from A
            BEQ next_value   ; if A is zero then not prime so continue
            BPL is_multiple   ; keep subtracting if we haven't underflowed
            LDA $0500
            JMP loop  


is_prime:
    STA $2000,Y
    INY ; increase the Y register with number of primes we have found
    JMP next_value

end:
    BRK    ; Finished testing primes and all prime numbers should now be located between $2000 and $20FF
