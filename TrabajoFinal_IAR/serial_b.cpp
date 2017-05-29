//trbaja moviendo un cuadrado de izquierda a derecha y viceversa en processing:
//ej_adc_processing

/* DriverLib Includes */
#include "driverlib.h"

/* Standard Includes */
#include <cstdint>
#include <cmath>
#include <cstring>


/* Global flags */
//interrupt global flags
bool OneMsFlag=true;
bool port1Pin1FallingEdgeFlag=false;
bool euscia0DataReceivedFlag=false;
bool adcConvertionReadyFlag=false;

//tasks global flags
bool buttonPressedFlag=false;
bool transmitStartFlag=false;
bool transmitDoneFlag=false;

/*global variables*/
#define buffer 3                //numero de datos a ser recibidos en puerto serial
uint8_t serialBuffer=buffer;    //
int16_t dato1,dato2,dato3,drx[buffer];       //transmitir: dato, recibir: drx[]
uint16_t resultsBuffer[4];      //resultado conversiones ADC

/* Function prototypes */
//task functions
void processing();
void transmit();
void na();

//config functions
extern void config_systimer();
extern void config_p11();
extern void config_eUSCI_A0_UART_Interrupt(void);
extern void configADC(void);

//ISR functions
extern void Port1_Isr(void);
extern void SysTick_Handler(void);
extern void EUSCIA0_IRQHandler(void);


int main(void)
{
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer();
    /* Set Flash read wait states*/
    FlashCtl_setWaitState(FLASH_BANK0,3);
    FlashCtl_setWaitState(FLASH_BANK1,3); 
    
    /* Zero-filling buffer */
    memset(resultsBuffer, 0x00, 4);
    
    /* Task timing variables */
    uint32_t
    transmitTaskCounter=1, 
    processingTaskCounter=1;
    
    /* Config interrupts */
    config_systimer();
    config_eUSCI_A0_UART_Interrupt();
    configADC();
    
    /* Configuring P1.0 as an output with value '0' */
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
        
    /* Enabling MASTER interrupts */
    Interrupt_enableMaster();   
   
    while(true)
    {
      if (OneMsFlag==true){
        OneMsFlag=false;
        
        //processing task
        processingTaskCounter--;
        if (processingTaskCounter==0){ 
          processing();                         //called every 5 ms
          processingTaskCounter=5;
        } 

        //transmit task
        transmitTaskCounter--;
        if (transmitTaskCounter==0){ 
          transmit();                         //called every 1 ms
          transmitTaskCounter=1;
        }         
        
      
      }      
    }
}

///* processing ------------------------------------------*/
void processing(){
  
  static enum {s0,s1} state=s0;

  switch(state){
    case s0:
      if (adcConvertionReadyFlag==true){
        adcConvertionReadyFlag=false;
        dato1 = MAP_ADC14_getResult(ADC_MEM0);
        transmitStartFlag=true;
        dato2 = MAP_ADC14_getResult(ADC_MEM1);
        transmitStartFlag=true;
        dato3 = MAP_ADC14_getResult(ADC_MEM3);
        transmitStartFlag=true;
        state=s1;
      }
      break;        
        
    case s1:
      if (transmitDoneFlag==true){
        transmitDoneFlag=false;
        ADC14_toggleConversionTrigger();  //start new convertion
        state=s0;
      }
      break;
  }
  
}
//--------------------------------------------------

/*transmit -----------------------------------------*/
void transmit(){
  static enum {s0,s1,s2,s3,s4} state=s0;

  switch(state){
    case s0:
      if (transmitStartFlag==true){
        transmitStartFlag=false;
        state=s1;
      }
      break;
    case s1:
      UART_transmitData(EUSCI_A0_BASE, dato1);
      state=s2;
      break;
    case s2:
      UART_transmitData(EUSCI_A0_BASE, dato2);
      state=s3;
      break;
    
    case s3:
      UART_transmitData(EUSCI_A0_BASE, dato3);
      state=s4;
      break;
    
    case s4:  
      transmitDoneFlag=true;
      state=s0;
      break;
  }
}
//--------------------------------------------------