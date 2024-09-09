#include <avr/io.h>
#include <lcd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>



void init_SPI(){
     // SPI initialization
    SPCR |= (1<<SPE); // Enable
    SPCR |= (0<<CPOL); //  Clock Polarity: Low
    SPCR |= (0<<CPHA); //  Clock Phase: Leading edge sample / Trailing Edge setup
    SPCR |= (1<<SPR1) | (1<<SPR0); //  Clock Rate: f/128 = 62.5 KHz
    SPSR |= (0<<SPI2X);
    SPCR |= (0<<DORD); //  Data Order: MSB First
    SPCR |= (0<<MSTR); //  Type: Slave
}
char lv[8];

int main() {
   
    init_SPI();
    // LCD initialization
    DDRC = 0XFF;
    DDRD=0XFF;
    LCD_Init();

    // Timer/Counter 1 initialization
    // Clock source: System Clock _ f/256
    // Clear OCR1A/PCR1B on compare match,Set OCR1A/PCR1B at TOP
    // FAST PWM _ TOP=OCR1A
    TCCR2 = (0<<FOC2)  | (1<<WGM20)  | (1<<COM21)  | (0<<COM20)  | (1<<WGM21)  | (0<<CS22)  | (1<<CS21)  | (1<<CS20);
    TCNT2 = 0;
    OCR2  = 0;

    while(1) {
        LCD_Clear();
        SPDR = '0'; // Slave data doesn't matter for master
        while (((SPSR >> SPIF) & 1) == 0); // waiting for data

        itoa(SPDR, lv, 10); // convert v to ascii
        
        for(int i = 0; i < strlen(lv); i++) {
            LCD_Char(lv[i]);
        }

        if(SPDR == 0){
            OCR2 = 0;
        }
        if( SPDR == 1){

            OCR2 = 75;
        }
        if(SPDR == 2){

            OCR2 = 150;
        }
        if(SPDR == 3){
            OCR2 = 255 ;
        }  
        _delay_ms(1000); // wait 1s to see data
    }
}