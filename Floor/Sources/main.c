#include <hidef.h>            /* common defines and macros */
#include "derivative.h"       /* derivative-specific definitions */
#include "CAN.h"
#include <stdio.h>
#include <stdlib.h>
#include "delay.h"
#include "floorIO.h"









void main(void) {
  
  unsigned char errorflag = CAN_NO_ERROR;
  unsigned char txBuffer[1]; //single bit transmission, for using with supplied canTX function
  unsigned char buttonStatus = 0; //button status of 0 is unpressed, 1 is pressed
  unsigned char rxByte;
  
  txBuffer[0] = 0x01;
  
  CANInit();
  floorIOinit();
   
  while (!(CANCTL0 & CAN_SYNC));       // Wait for MSCAN to synchronize with the CAN bus
  
  CANRFLG = 0xC3;                      // Enable CAN Rx interrupts
  CANRIER = 0x01;                      // Clear CAN Rx flag
  EnableInterrupts;

  
  msDelay(250);
  
  
  for(;;) {
      if ((buttonStatus == 0) && (PTJ & 0xC0) != 0xC0) {
      buttonStatus = 1;
      LEDon();
      errorflag = CANTx(FLOOR_TX_ID, 0x00, 1, txBuffer);  
      }
      
      if (CanRingCheck()) {
        rxByte = CanPullRing();
        if ((rxByte - FLOOR_NUM_OFFSET) == FLOOR_NUM) {
          buttonStatus = 0;
          LEDoff();
        }
        
        //SegDisplay(rxByte - 0x04);
        SegDisplay(rxByte - FLOOR_NUM_OFFSET);      
        
      }
      
      
    
     
  
    _FEED_COP(); /* feeds the dog */
  } /* loop forever */
  /* please make sure that you never leave main */
}
