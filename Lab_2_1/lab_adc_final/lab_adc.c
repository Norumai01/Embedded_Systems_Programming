//*****************************************************************************
// Lab 2: Battery Level Indicator
// Johnny Nguyen & Nathan Jackson
// April 8th, 2022
// ECGR 3101
//*****************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_nvic.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/adc.h"
#include "inc/tm4c123gh6pm.h"

// The error routine that is called if the driver library encounters an error.
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
    while(1);
}
#endif

//*****************************************************************************
//
// ADC Project: Translate range of voltage level to LEDs. SysTick for Delay.
//
//*****************************************************************************
void DelaySetup(void)
{
    // Disable Systick
    NVIC_ST_CTRL_R = 0;
    NVIC_ST_RELOAD_R = 15999999;
    NVIC_ST_CURRENT_R = 0;
    // Enable SysTick
    NVIC_ST_CTRL_R |= 0x5;
    // Trigger Systick timer until reload value reaches 0.
    while( (NVIC_ST_CTRL_R & (1<<16) ) == 0){}
    NVIC_ST_CTRL_R = 0;
};
void myDelay(unsigned int loop)
{
    unsigned int counter;

    // Calculated loops based on 0.2s delay at DelaySetup().
    for(counter = 0; counter < loop; counter++) {
        DelaySetup();
    }
};
int
main(void)
{
    uint32_t ADCvalue[1];
    volatile float voltageV;

    // Sets the necessary clock sources to 80 MHz.
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    // Enabling the GPIO port.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)){}

    // Red LED's pin.
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_3);

    // Enabling the Analog to Digital Converter port.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0)){}

    // Configuring and enabling ADC sequence.
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 3);
    // Reset ADC register counter to 0, in any case it is not reseted.
    ADCIntClear(ADC0_BASE, 3);

    while(1)
    {
        // Reset LEDs' pins.
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_3, 0x0);

        // Triggers ADC interrupt
        ADCProcessorTrigger(ADC0_BASE, 3);
        while(!ADCIntStatus(ADC0_BASE, 3, 0)){}
        ADCIntClear(ADC0_BASE, 3);
        ADCSequenceDataGet(ADC0_BASE, 3, ADCvalue);

        // Converts digital value to output voltage level.
        voltageV = (float)ADCvalue[0] * (3.3 / 4096.0);

        // Depending on the voltage triggers these different states.
        if (voltageV > 2.8)
        {
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);
        }
        if ( (voltageV > 2.5) && (voltageV < 2.8) )
        {
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);
        }
        if (voltageV < 2.5)
        {
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);
            myDelay(5);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x0);
            myDelay(5);
        }
    }
}
