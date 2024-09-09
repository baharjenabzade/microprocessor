#include <avr/io.h>
#include <lcd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char temp[8];

int main() {
    // initialiing SPI
    // Enable
    SPCR |= (1<<SPE); 
    // Low Clock Polarity
    SPCR |= (0<<CPOL); 
    // Clock Phase => Leading edge sample // Trailing Edge setup
    SPCR |= (0<<CPHA); 
    //Clock Rate: f/128 = 62.5 KHz
    SPCR |= (1<<SPR1) | (1<<SPR0); 
    SPSR |= (0<<SPI2X);
    //Data Order => MSB First
    SPCR |= (0<<DORD); 
    //Slave
    SPCR |= (0<<MSTR); 

    // initialiing LCD
    DDRC = 0XFF;
    LCD_Init();
    int i ;

   ///////////////////////////////////////////////////////////////////////////////////////////////////////////

	//HEATER

    // OCR2(PORTD.7) PORTD.7 => 1
    
    DDRD  = (1<<DDD7)   | (0<<DDD6)   | (0<<DDD5)   | (0<<DDD4)   | (0<<DDD3)   | (0<<DDD2)   | (1<<DDD1)   | (1<<DDD0);
    // PORTD.0 & PORTD.1 are output for controlling motor direction
    PORTD = (0<<PORTD7) | (0<<PORTD6) | (0<<PORTD5) | (0<<PORTD4) | (0<<PORTD3) | (0<<PORTD2) | (0<<PORTD1) | (0<<PORTD0);
    // TIMER COUNTER 2
    // Clock Timer2 = CLK/64                          => CS22 = 0 , CS21  = 1 , CS20  = 1
    // Fast PWM                                => WGM20 = 1 , WGM21 = 1
    // compare match => Clear OC0 , set OC0 at BOTTOM  => COM21 = 1 , COM20 = 0
    TCCR2 = (0<<FOC2)  | (1<<WGM20)  | (1<<COM21)  | (0<<COM20)  | (1<<WGM21)  | (0<<CS22)  | (1<<CS21)  | (1<<CS20);
    TCNT2 = 0;
    OCR2  = 0;


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   //COOLER

     //OCR0(PORTB.3) , PORTB.3 => 1
    DDRB  = (0<<DDB7)   | (0<<DDB6)   | (0<<DDB5)   | (0<<DDB4)   | (1<<DDB3)   | (0<<DDB2)   | (1<<DDB1)   | (1<<DDB0);
    PORTB = (0<<PORTB7) | (0<<PORTB6) | (0<<PORTB5) | (0<<PORTB4) | (0<<PORTB3) | (0<<PORTB2) | (0<<PORTB1) | (0<<PORTB0);
    // TIMER COUNTER 0
    // Clock Timer Counter 0 = CLK/64      prescale = 64                    => CS02 = 0 , CS01  = 1 , CS00  = 1 
    // Fast PWM                                => WGM00 = 1, WGM01 = 1
    // compare match => Clear OC0 ,  set OC0 at BOTTOM  =>  COM01 = 1 , COM00 = 0
    TCCR0 = (0<<FOC0)  | (1<<WGM00) | (1<<COM01)  | (0<<COM00)  | (1<<WGM01)  | (0<<CS02)  | (1<<CS01)  | (1<<CS00);
    TCNT0 = 0;
    OCR0  = 0;
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////







    
    while(1) {
        LCD_Clear();
        SPDR = '0'; // master doesn't care about slave's data
        while (((SPSR >> SPIF) & 1) == 0); // waiting for master's data
        itoa(SPDR, temp, 10); // convert temp to ascii code
        
        for( i = 0; i < strlen(temp); i++) {
            LCD_Char(temp[i]);  //show on LCD
        }

        if(SPDR <25 && SPDR > 20){
                OCR2 = 0 ;
                OCR0 = 0 ;        }

        if(SPDR >= 25 && SPDR <= 40){ // OCR0 for COOLER
            if(SPDR >= 25 && SPDR < 30){  //speed = 1

                OCR0 = 75;
            }
            if(SPDR >= 30 && SPDR < 35){ // speed = 2

                OCR0 = 150;
            }
            if(SPDR >= 35 && SPDR <= 40){ //speed = 3
                OCR0 = 255 ;
            }
        }
        if(SPDR <= 20 && SPDR >= 5){  //OCR2 for HEATER
            if(SPDR <=20 && SPDR > 15){ //speed = 1
                OCR2 = 75;   
            }
            if(SPDR <=15 && SPDR >10){ //speed = 2
                OCR2 = 150;
            }
            if(SPDR <= 10 && SPDR >= 5){ //speed = 3
                OCR2 = 255;   
            }

        }
        _delay_ms(500); // wait 500 ms to see data
    }
}