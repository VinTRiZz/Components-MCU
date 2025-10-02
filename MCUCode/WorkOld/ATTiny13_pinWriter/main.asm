; init portB
sbi DDRB, 1
sbi DDRB, 0

sbi DDRB, 2
sbi DDRB, 3

; Set 1 to all 2 buttons
ldi r24, 0x03
out PORTB, r24

main:
	; if (PINB.2 != 1)
	in r16, PINB ; Get current port data
	andi r16, 2
	cpi r16, 0 ; Check if button pressed (button on PB2)
	breq true
	rjmp main ; if false, re-check

	true:
		; Set buffer things
		ldi r16, 4     ; mins
		ldi r17, 45    ; sec (in 3 min N sec)
		ldi r18, 70    ; sec (in 1 minute)
		ldi r19, 4	   ; 4 times
		ldi r20, 250   ; M
		ldi r21, 250   ; us

		ldi r24, 7
		out PORTB, r24

		waitMin:
			dec r16
			cpi r16, 1 ; If seconds ended and 3 minutes elapsed
			brsh wait60sec
	
		waitSec:
			dec r17
			cpi r17, 1
			brsh wait1sec
	
			; if ends waiting 

			ldi r24, 3
			out PORTB, r24
		


rjmp main ; restart program


wait60sec:	
	dec r18
	cpi r18, 1
	brsh waitOneSec ; repeat
	ldi r18, 70 ; at the end
	rjmp waitMin ; return

waitOneSec:
	dec r20 ; 250 -> 0
	cpi r20, 1
	brsh waitOnems
	dec r19
	cpi r19, 1
	brsh waitOneSec
	ldi r19, 4
	ldi r20, 250
	rjmp wait60sec

waitOnems: ; ~1002 us is one cycle
	dec r21 ; 1 tact
	cpi r21, 1 ; 1 tact
	brsh waitOnems ; 2 tact
	ldi r21, 250 ; 1 tact
	rjmp waitOneSec

wait1sec:
	dec r20 ; 250 -> 0
	cpi r20, 1
	brsh wait1ms
	dec r19
	cpi r19, 1
	brsh wait1sec
	ldi r20, 250
	ldi r19, 4
	rjmp waitSec

wait1ms: ; ~1002 us is one cycle
	dec r21 ; 1 tact
	cpi r21, 1 ; 1 tact
	brsh wait1ms ; 2 tact
	ldi r21, 250 ; 1 tact
	rjmp wait1sec
