// CAN.c contains CAN functions

#include <hidef.h>                //common defines and macros  
#include "derivative.h"           // derivative-specific definitions
#include <stdlib.h>
#include "CAN.h"



#define RINGBUFFSIZE 32U
// Ring Buffer for Incoming messages

volatile unsigned char CanRingBuffRX[RINGBUFFSIZE];
volatile unsigned char RingBuffHead = 0;
volatile unsigned char RingBuffTail = 0;




// CanRingCheck
// returns 1 if something is in ring buff, 0 if ring buff empty

unsigned char CanRingCheck(void) {
  if (RingBuffHead == RingBuffTail)
    return 0;
  else
    return 1;
}

unsigned char CanPullRing(void) {
  unsigned char returnval = 0;
  if (RingBuffHead != RingBuffTail) {
    returnval = CanRingBuffRX[RingBuffHead];
    RingBuffHead = (RingBuffHead + 1) % RINGBUFFSIZE;
  }
  return returnval;
}
  


// Code adapted from AN3034

// ******************************************************************
//                        CANInit()
//        Configures and starts the CAN controller
// ******************************************************************

void CANInit(void) {
  // Enter Initialization mode : CANCTL0 Register
    CANCTL0 = INIT_MODE;                   
    while (! (CANCTL1 & INIT_MODE)) {};   // Wait for acknowledgement that initialization mode has started
  
  // Set the CAN mode : CANCTL1 Register 
    CANCTL1 = CAN_ENABLED_MODE_BUS_CLOCK; // Enable normal mode with bus clock
    //CANCTL1 = LOOPBACK_MODE_BUS_CLOCK;       // Enable Loopback mode with bus clock
    
  // Set the baud rate to 125k : CANBTR0 and CANBTR1 Register
    CANBTR0 = BIT_0_125K; 
    CANBTR1 = BIT_1_125K;
    
  // Set filters
    CANIDAC = FOUR_16BIT_FILTERS;              // Define four 16-bit filters
    
    // Define 16 bit filters that accept all messages
    
       // Define a 16 bit filter for ID 0x100
       CANIDAR0 = ACC_CODE_ID101_HIGH;        // High order bits of ACC_CODE_ID go into the first 8 bit register
       CANIDMR0 = MASK_CODE_ST_ID_HIGH; 
       CANIDAR1 = ACC_CODE_ID101_LOW;         // Low order bits of ACC_CODE_ID go into the second 8 bit register
       CANIDMR1 = MASK_CODE_ST_ID_LOW;     
       
    
  // Exit initialization mode and enter normal mode 
    CANCTL0 = NORMAL_MODE;                    // Send request
    while ((CANCTL1 & 0x01)) {};  // Wait for normal mode acknowledgement (was 0x00)
}


// ******************************************************************
//                        CANTx(id, priority, length, *txdata)
//        Sends a CAN frame via polling 
// ******************************************************************

unsigned char CANTx(unsigned long id, unsigned char priority, unsigned char length, unsigned char *txdata) {
  
  unsigned char txbuffer;  // Stores the selected buffer for transmtting
  unsigned char index;     // Index into the data array
  
  // Check the transmit buffer to see if it is full 
  if (!CANTFLG){
       return CAN_ERR_BUFFER_FULL;
  }
  CANTBSEL = CANTFLG;      // This selects the lowest empty transmit buffer
  txbuffer = CANTBSEL;     // Keeps a backup of the selected transmit buffer
  
  // Fill in the different parts of the CAN frame
    // Load the ID to the appropriate ID register (e.g. IDR0)   
    *((unsigned long *) ((unsigned long)(&CANTXIDR0))) = id; 
  
    // Load the message data to the Tx buffer data segment registers 
    for (index = 0; index < length; index++) {
      *(&CANTXDSR0 + index) = txdata[index]; 
    }
    
    // Set the data length code (DLC)
    CANTXDLR = length;
    
    // Set priority
    CANTXTBPR = priority; 
    
    // Start transmission
    CANTFLG = txbuffer;       // Setting the flag bit starts the transmission
    
    // Wait for transmission to complete
    while ((CANTFLG & txbuffer) != txbuffer){};  
    
    return CAN_NO_ERROR;                 // No error
}


// ******************************************************************
//                        CANRxISR()
//        Interrupt handler for CAN Rx 
// Function reads the data buffer and clears the reception flag to keep
// the module available to receive further messages 
// ******************************************************************

 interrupt VectorNumber_Vcanrx void CANRxISR(void) {
  unsigned char length, index; 
  
  length = (CANRXDLR & 0x0F); 
  for (index=0; index < length; index++) {
    CanRingBuffRX[RingBuffTail] = *(&CANRXDSR0 + index);   // Get received data
    RingBuffTail = (RingBuffTail + 1) % RINGBUFFSIZE;  
  }
  
  CANRFLG |= 0x01;                            // Clear the received flag RXF and check for NEW messages - resets the interrupt
  
}




