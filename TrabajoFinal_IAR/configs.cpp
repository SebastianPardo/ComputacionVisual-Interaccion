/* DriverLib Includes */
#include "driverlib.h"

extern bool OneMsFlag;
extern bool port1Pin1FallingEdgeFlag;
extern bool euscia0DataReceivedFlag;
extern bool adcConvertionReadyFlag;
extern int16_t drx[3];
extern uint8_t serialBuffer;
extern uint16_t resultsBuffer[4];

/*--eUSCI_A0_UART_Interrupt-------------------------------------------*/
/* UART Configuration Parameter. These are the configuration parameters to
 * make the eUSCI A UART module to operate with a 9600 baud rate.
 * SMCLK = 3 Mhz*/
const eUSCI_UART_Config uartConfig =
{
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        19,                                      // BRDIV = 19
        8,                                       // UCxBRF = 8
        0,                                       // UCxBRS = 0
        EUSCI_A_UART_NO_PARITY,                  // No Parity
        EUSCI_A_UART_LSB_FIRST,                  // LSB First
        EUSCI_A_UART_ONE_STOP_BIT,               // One stop bit
        EUSCI_A_UART_MODE,                       // UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION  // Oversampling
};
//config UART interrupt
void config_eUSCI_A0_UART_Interrupt(void){

    /* Selecting P1.2 and P1.3 in UART mode */
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
            GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Configuring UART Module */
    UART_initModule(EUSCI_A0_BASE, &uartConfig);

    /* Enable UART module */
    UART_enableModule(EUSCI_A0_BASE);

    /* Enabling interrupts */
    UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    Interrupt_enableInterrupt(INT_EUSCIA0);
}

//-----------------------------------------------------*/

/*--systimer-------------------------------------------*/
void config_systimer(){

    /* Configuring SysTick to interrupt every 1 ms */
    SysTick_enableModule();
    SysTick_setPeriod(3000);
    SysTick_enableInterrupt();


}
//---------------------------------------------------*/

/*--p11----------------------------------------------*/
void config_p11(){  
  
    /* Configuring P1.1 as an input and enabling interrupts */
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
    GPIO_interruptEdgeSelect (GPIO_PORT_P1,GPIO_PIN1,GPIO_HIGH_TO_LOW_TRANSITION);
    GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
    GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);
    Interrupt_enableInterrupt(INT_PORT1);

}
//---------------------------------------------------*/

/* EUSCI A0 UART ISR ------------------------------- */
extern "C" void EUSCIA0_IRQHandler(void)
{
    static uint8_t countrx=0;
  
    uint32_t status = MAP_UART_getEnabledInterruptStatus(EUSCI_A0_BASE);

    MAP_UART_clearInterruptFlag(EUSCI_A0_BASE, status);

    if(status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
    {
      if (countrx<(serialBuffer-1)){
          drx[countrx]=UART_receiveData(EUSCI_A0_BASE);
          countrx++;
      }else{
        drx[countrx]=UART_receiveData(EUSCI_A0_BASE);
        euscia0DataReceivedFlag=true;
        countrx=0;
      }
    }

}
//---------------------------------------------------*/
/* SysTick ISR --------------------------------------*/
extern "C" void SysTick_Handler(void)
{
   OneMsFlag=true;   
}
//-----------------------------------------------------

/* Port1_Isr ----------------------------------------------*/
extern "C" void Port1_Isr(void)
{
    uint32_t status;

    status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    GPIO_clearInterruptFlag(GPIO_PORT_P1, status);

    /* Toggling the output on the LED */
    if(status & GPIO_PIN1)
    {
       port1Pin1FallingEdgeFlag=true;
    }
}

//--config ADC ------------------------------------------------------
void configADC(void){
    /* Initializing ADC (MCLK/1/1) */
    MAP_ADC14_enableModule();
    MAP_ADC14_initModule(ADC_CLOCKSOURCE_MCLK, ADC_PREDIVIDER_1, ADC_DIVIDER_1,
            0);
    
    ADC14_setResolution(ADC_8BIT);

    /* Configuring GPIOs for Analog In */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P5,
            GPIO_PIN5 | GPIO_PIN4 | GPIO_PIN2 | GPIO_PIN1, GPIO_TERTIARY_MODULE_FUNCTION);

    /* Configuring ADC Memory (ADC_MEM0 - ADC_MEM3) (A0 - A3)  with no repeat)*/
    MAP_ADC14_configureMultiSequenceMode(ADC_MEM0, ADC_MEM3, false);
    
    MAP_ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_AVCC_VREFNEG_VSS,
    ADC_INPUT_A0, false);
    MAP_ADC14_configureConversionMemory(ADC_MEM1, ADC_VREFPOS_AVCC_VREFNEG_VSS,
    ADC_INPUT_A1, false);
    MAP_ADC14_configureConversionMemory(ADC_MEM2, ADC_VREFPOS_AVCC_VREFNEG_VSS,
    ADC_INPUT_A2, false);
    MAP_ADC14_configureConversionMemory(ADC_MEM3, ADC_VREFPOS_AVCC_VREFNEG_VSS,
    ADC_INPUT_A3, false);
 
    /* Enabling the interrupt when a conversion on channel 3 (end of sequence)
     is complete and enabling conversions */
    MAP_ADC14_enableInterrupt(ADC_INT3);

    /* Enabling Interrupts */
    MAP_Interrupt_enableInterrupt(INT_ADC14);
    //MAP_Interrupt_enableMaster();

    /* Setting up the sample timer to automatically step through the sequence
     * convert.
     */
    MAP_ADC14_enableSampleTimer(ADC_AUTOMATIC_ITERATION);

    /* Triggering the start of the sample */
    MAP_ADC14_enableConversion();
    MAP_ADC14_toggleConversionTrigger();
}
//-------------------------------------------------------------------
    
/* ADC Interrupt Handler. This handler is called whenever there is a conversion
 * that is finished for ADC_MEM0.
 */
extern "C" void ADC14_IRQHandler(void)
{
    uint64_t status;

    status = MAP_ADC14_getEnabledInterruptStatus();
    MAP_ADC14_clearInterruptFlag(status);

    if(status & ADC_INT3)
    {
        MAP_ADC14_getMultiSequenceResult(resultsBuffer);
        adcConvertionReadyFlag=true;
    }
}