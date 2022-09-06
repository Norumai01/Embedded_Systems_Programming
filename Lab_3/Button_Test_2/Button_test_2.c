//*****************************************************************************
// Lab 1B: GPIO & SysTick
// Johnny Nguyen
// March 25th, 2022
// ECGR 3101
//
//*****************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_nvic.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
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
// Blink the on-board LED.
//
//*****************************************************************************
const int max_level = 5;
unsigned int user_sequence[5];

void DelaySetup(void)
{
    // Disable SysTick during setup
    NVIC_ST_CTRL_R = 0;
    NVIC_ST_RELOAD_R = 15999999;
    // Clear values written to CURRENT.
    NVIC_ST_CURRENT_R = 0;
    // Enable SysTick
    NVIC_ST_CTRL_R |= 0x5;
    // Trigger Systick timer until reload value reaches 0.
    while( (NVIC_ST_CTRL_R & (1<<16) ) == 0){}
    NVIC_ST_CTRL_R = 0;
};

void myDelay(unsigned int loop) {

    unsigned int counter;

    // Calculated loops based on 0.2s delay at DelaySetup().
    for(counter = 0; counter < loop; counter++) {
        DelaySetup();
    }
};

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

    // Configures the button to the following GPIO ports and pins
    GPIOPadConfigSet(GPIO_PORTA_BASE,GPIO_PIN_2,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
    GPIOPadConfigSet(GPIO_PORTA_BASE,GPIO_PIN_3,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
    GPIOPadConfigSet(GPIO_PORTA_BASE,GPIO_PIN_4,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);

    while(1)
    {
        pinVal = GPIOPinRead(GPIO_PORTA_BASE,GPIO_PIN_2);
        pinVal_2 = GPIOPinRead(GPIO_PORTA_BASE,GPIO_PIN_3);
        pinVal_3 = GPIOPinRead(GPIO_PORTA_BASE,GPIO_PIN_4);

        int i;
        for(i = 0; i < max_level; i++)
        {
            if(pinVal != GPIO_PIN_2){
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);
                user_sequence[i] = 0;
                myDelay(1);
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x0);
            }

            if(pinVal_2 != GPIO_PIN_3){
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
                user_sequence[i] = 1;
                myDelay(1);
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x0);
            }

            if(pinVal_3 != GPIO_PIN_4){
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);
                user_sequence[i] = 2;
                myDelay(1);
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x0);
            }
        }
    }
}
