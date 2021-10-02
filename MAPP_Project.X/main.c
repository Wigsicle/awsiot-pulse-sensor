/*
 * File:   main.c
 * Author: thega
 *
 * Created on 16 August 2021, 23:24
 */


#include <xc.h>
#include "delays.h"
#include "lcd.h"

void Rotation180(){
    unsigned int i;
    for(i=0;i<12;i++){
        PORTCbits.RC2 =1;
        delay_ms(1.25);
        PORTCbits.RC2=0;
        delay_ms(18.3);
    }
}

void returnServo(){
    unsigned int i;
    for(i=12;i>0;i--){
        PORTCbits.RC2 =1;
        delay_ms(2.25);
        PORTCbits.RC2=0;
        delay_ms(18.3);
    }
}


void onetone(void)
{
    unsigned int k;
    
    for(k=0;k<100;k++){
        delay_us(3000);
        PORTBbits.RB0 = !PORTBbits.RB0;
    }
}

void secondtone(void)
{
    unsigned int k;
    PORTAbits.RA0 = 0;
    for(k=0;k<20;k++){
        delay_us(5000);
        PORTBbits.RB0 = !PORTBbits.RB0;
    }
    PORTAbits.RA0 = 1;
    delay_ms(500);
    for(k=0;k<20;k++){
        delay_us(5000);
        PORTBbits.RB0 = !PORTBbits.RB0;
    }
    PORTAbits.RA0 = 0;
    delay_ms(500);
    for(k=0;k<20;k++){
        delay_us(5000);
        PORTBbits.RB0 = !PORTBbits.RB0;
    }
    PORTAbits.RA0 = 1;
}

unsigned char outchar;
char MessageP[]="Permission Granted";
char MessageD[]="Permission Denied";
char MessageR[]="Closing Door";
void main(void) {
    ADCON1 = 0x0F; // Make Port A Digital
    TRISA = 0x00;
    PORTAbits.RA0 = 0;
    PORTAbits.RA1 = 0;
    TRISC = 0xFF;
    TRISBbits.TRISB0 = 0;
    TRISCbits.RC2 = 0;
    int i;
    int j;
    lcd_init();
    
   
         //Permission Granted
    while(PORTCbits.RC0 == 1) {
        PORTAbits.RA0 = 1;
        PORTAbits.RA1 = 0;
        onetone();
        lcd_write_cmd(0x01);
        lcd_write_cmd(0x80);    
        for(i=0; i<18; i++){
            outchar=MessageP[i];
            lcd_write_data(outchar);
        }
        Rotation180();
        delay_ms(5000);
        lcd_write_cmd(0x01);
        lcd_write_cmd(0x80);    
        for(i=0; i<12; i++){
            outchar=MessageR[i];
            lcd_write_data(outchar);
        }
        secondtone();
        returnServo();
        lcd_write_cmd(0x01);
        lcd_write_cmd(0x80); 
        PORTAbits.RA0 = 0;
        while(PORTCbits.RC0 == 1);
        
    }
    
    //Permission Denied
    while(PORTCbits.RC1 == 1) {
         PORTAbits.RA1 = 1;
         PORTAbits.RA0 = 0;
         onetone();
         delay_us(3000);
         onetone();
         lcd_write_cmd(0x01);
         lcd_write_cmd(0x80);
         for(j=0; j<17; j++){
            outchar=MessageD[j];
            lcd_write_data(outchar);
         }
         PORTAbits.RA1 = 0;
         while(PORTCbits.RC1 == 1);
    }
    
  
}