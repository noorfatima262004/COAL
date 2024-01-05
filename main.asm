.include "m328pdef.inc"
.include "delay.inc"
.include "UART.inc"

.equ PIR_PIN = 2
.equ LDR_PIN = 0   ; A0 is mapped to pin 0 in ATmega328P
.equ BUZZER_PIN = 3
.equ DARK_THRESHOLD = 200

.def A = r16
.def AH = r17
 
.org 0x0000
Serial_begin

; Initialize the PIR sensor pin as input
ldi r18, (0 << PIR_PIN)
out DDRD, r18 ; Use DDRD for setting the direction of port D

; Initialize the buzzer pin as output
ldi r19, (1 << BUZZER_PIN)
out DDRD, r19 ; Use DDRD for setting the direction of port D

LDI A,0b11000111 ; [ADEN ADSC ADATE ADIF ADIE ADIE ADPS2 ADPS1 ADPS0]
STS ADCSRA,A
LDI A,0b01100000 ; [REFS1 REFS0 ADLAR – MUX3 MUX2 MUX1 MUX0]
STS ADMUX,A ; Select ADC0 (PC0) pin

loop:

	LDS A,ADCSRA ; Start Analog to Digital Conversion
	ORI A,(1<<ADSC)
	STS ADCSRA,A

wait:
	LDS A,ADCSRA ; wait for conversion to complete
	sbrc A,ADSC
	rjmp wait

	LDS A,ADCL ; Must Read ADCL before ADCH
	LDS AH,ADCH
	delay 100

; Loop to check the LDR value and PIR sensor value and turn on the buzzer if it is dark and motion is detected
loop2:
	
	Serial_read
	cpi r22, '1' 
	breq turn_on_buzzer

	cpi AH, DARK_THRESHOLD
    brcs turn_off_buzzer

    sbis PIND, PIR_PIN ; Skip if PIR_PIN is set (motion detected)
    rjmp turn_off_buzzer

    out PORTD, r19 ; Turn on the buzzer
  
	rjmp loop2

; Turn off the buzzer
turn_off_buzzer:
    ldi r20, (0 << BUZZER_PIN)
    out PORTD, r20
    rjmp loop

turn_on_buzzer:
	out PORTD, r19 ; Turn on the buzzer

	Serial_read
	cpi r22, '0'
	breq turn_off_buzzer

	rjmp turn_on_buzzer