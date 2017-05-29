/* DriverLib Includes */
#include "driverlib.h"

extern bool buttonPressedFlag;
extern bool port1Pin1FallingEdgeFlag;

/*port1Pin1Debounce-----------------------------------------*/

void port1Pin1Debounce(){
  
  static enum {start,wait,check} state=start;
  
  #define debounce_time 5;

  static uint32_t count=debounce_time;
  
  //static bool buttonPressedFlag=false;
  
  switch( state ) {
    case start:
      if (port1Pin1FallingEdgeFlag==true){
        state=wait;
      } 
      break;
     case wait: 
       count--;
       if (count == 0){
        port1Pin1FallingEdgeFlag=false; 
        state=check;      
       }
       break; 
     case check:  
       if(GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1)==GPIO_INPUT_PIN_LOW){  
        buttonPressedFlag=true;
       }else{
        buttonPressedFlag=false;
       }
       GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
       count=debounce_time;
       state=start;
       break;
  }
  
}
