//floorIO.c

#include <hidef.h>                //common defines and macros
#include "derivative.h"           // derivative-specific definitions
#include <stdlib.h>
#include "floorIO.h"


#define SSEG_1  0b00010100
#define SSEG_2  0b10111010
#define SSEG_3  0b10011110
#define SSEG_E  0b11101010




void floorIOinit(void){

   //7 seg display port A bits 2, 1, 0 (OUTPUT)
   DDRA |= 0xFF;

   //Pushbutton port J 7, 6 (INPUT)
   DDRJ &= 0x3F;
   DDRB &= 0x00;


   //LED port Port S 2, 3 (OUTPUT)
   DDRS |= 0x0E;
   PTS &= 0xF1;
  
}




//PA0 is serial input
//PA1 is serial clock
//PA2 resets display
void SegDisplay(unsigned char Num){

//  unsigned char i;
  unsigned char bitPattern;
  switch(Num){
    case 0x01 :
      bitPattern = SSEG_1;
      break;
    case 0x02 :
      bitPattern = SSEG_2;
      break;
    case 0x03 :
      bitPattern = SSEG_3;
      break;
    default:
      bitPattern = SSEG_E;
  }
  /* //for shift register code
  for (i = 0; i < 8; i++){
    PORTA &= 0xFE; //clears last bit of port T
    PORTA |= ((bitPattern >> i) & 0x01);
    PORTA ^= 0x02;//combined with next line, pulses sclk
    PORTA ^= 0x02;
  }

  PORTA ^= 0x04;//combined with next line, pulses display refresh
  PORTA ^= 0x04;
  */

  //Hardcode
  PORTA = bitPattern;
}



void LED1on(void){
  PTS |= 0x04;
}

void LED1off(void){
  PTS &= 0xFB;
}
void LED2on(void){
  PTS |= 0x08;
}

void LED2off(void){
  PTS &= 0xF7;
}
void LED3on(void){
  PTS |= 0x02;
}

void LED3off(void){
  PTS &= 0xFD;
}
