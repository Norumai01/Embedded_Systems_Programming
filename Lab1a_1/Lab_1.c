#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

// Johnny Nguyen & Sean McLellan
// February 13th, 2022
// Lab - 1A

// The error routine that is called if the driver library encounters an error.
#ifdef DEBUG
void
__error__( char *pcFilename, uint32_t ui32Line )
{
    while(1);
}
#endif


void blink(uint32_t pinval, int blink_rate, int loop) {

    volatile uint32_t ui32Loop;
    int i;

    for(i = 0; i < loop; i++) {
        // Turn on the LED.
        GPIOPinWrite(GPIO_PORTF_BASE, pinval, pinval);

        // Delay for 1/4 second.
        for(ui32Loop = 0; ui32Loop < blink_rate; ui32Loop++){}

        // Turn off the LED.
        GPIOPinWrite(GPIO_PORTF_BASE, pinval, 0x0);

        // Delay for 3/4 second.
        for(ui32Loop = 0; ui32Loop < (blink_rate*3); ui32Loop++ ){}
    }
};

void delay() {
    volatile uint32_t ui32Loop;

    // Delay for 3 seconds after blinking 7 times.
    for(ui32Loop = 0; ui32Loop < 3500000; ui32Loop++ ){}
};


int
main(void)
{
    volatile uint32_t ui32Loop;

    // Enable the GPIO port that is used for the on-board LED.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    // Check if the peripheral access is enabled.
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)){}

    // Enable the GPIO pin for the LED (PF1 and PF2).  Set the direction as output.
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);

    while(1)
    {
        blink(GPIO_PIN_1, 500000, 7);
        delay();
        blink(GPIO_PIN_2, 500000, 7);
        delay();
    }
}
