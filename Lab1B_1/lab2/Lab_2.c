//*****************************************************************************
// Lab 1B: GPIO & SysTick
// Johnny Nguyen
// March 25th, 2022
// ECGR 3101
//*****************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_nvic.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "buttons.h"
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
    uint8_t button_val;
    unsigned char Delta;
    unsigned char Rawstate;

    // Sets the necessary clock sources to 80 MHz.
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    // Enable the GPIO port that is used for the on-board LED.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    // Check if the peripheral access is enabled.
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)){}

    // Enable the GPIO pin for the Red and Green LED (PF1 & PF3).
    // Set the direction as output.
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);

    // Initialize the buttons
    ButtonsInit();

    while(1)
    {
        // Polling Check for the Buttons.
        button_val = ButtonsPoll(&Delta, &Rawstate);

        // Stays Red (PF1) for the duration of the delay.
        // myDelay() is 20 loops as SysTick delay every 0.2 second to be 4 seconds.
        while(BUTTON_PRESSED(LEFT_BUTTON, button_val, Delta)){
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);
            myDelay(20);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x0);
            myDelay(20);
            button_val = ButtonsPoll(&Delta, &Rawstate);
        }
        // Stays Green (PF3) for the duration of the delay.
        while(BUTTON_PRESSED(RIGHT_BUTTON, button_val, Delta)){
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);
            myDelay(20);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x0);
            myDelay(20);
            button_val = ButtonsPoll(&Delta, &Rawstate);
        }
    }
}
