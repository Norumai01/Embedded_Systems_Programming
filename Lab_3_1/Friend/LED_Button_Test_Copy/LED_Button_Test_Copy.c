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

//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>Blinky (blinky)</h1>
//!
//! A very simple example that blinks the on-board LED using direct register
//! access.
//
//*****************************************************************************

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
    while(1);
}
#endif

//*****************************************************************************
//
// Blink the on-board LED.
//
//*****************************************************************************
int
main(void)
{
    volatile uint32_t pinVal;
    volatile uint32_t pinVal_2;
    volatile uint32_t pinVal_3;

    // Sets the necessary clock sources to 80 MHz.
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);


    // Enable the GPIO port that is used for the on-board LED.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA)){}
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)){}

    // Set the direction as output, and enable the GPIO pin for digital function.
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);
    GPIOPadConfigSet(GPIO_PORTA_BASE,GPIO_PIN_2,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
    GPIOPadConfigSet(GPIO_PORTA_BASE,GPIO_PIN_3,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
    GPIOPadConfigSet(GPIO_PORTA_BASE,GPIO_PIN_4,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);


    //
    // Loop forever.
    //
    while(1)
    {
        pinVal = GPIOPinRead(GPIO_PORTA_BASE,GPIO_PIN_2);
        pinVal_2 = GPIOPinRead(GPIO_PORTA_BASE,GPIO_PIN_3);
        pinVal_3 = GPIOPinRead(GPIO_PORTA_BASE,GPIO_PIN_4);

        //
        // Turn on the LED.
        //
        if(pinVal != GPIO_PIN_2){
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);
        }

        if(pinVal == GPIO_PIN_2){
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x0);
        }

        if(pinVal_2 != GPIO_PIN_3){
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
        }

        if(pinVal_2 == GPIO_PIN_3){
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x0);
        }

        if(pinVal_3 != GPIO_PIN_4){
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);
        }

        if(pinVal_3 == GPIO_PIN_4){
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x0);
        }

    }
}
