//*****************************************************************************
// Lab 3: Simon Says' Game
// Johnny Nguyen & Nathan Jackson
// April 22th, 2022
// ECGR 3101
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
//*****************************************************************************
// The error routine that is called if the driver library encounters an error.
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
    while(1);
}
#endif

//*****************************************************************************
// Creating Simon Says' Game.
//*****************************************************************************
const int max_level = 20;
int level = 0;
unsigned int random_sequence[20];
unsigned int user_sequence[20];

void DelaySetup(void)
{
    // Delay setup.
    NVIC_ST_CTRL_R = 0;
    NVIC_ST_RELOAD_R = 15999999;
    NVIC_ST_CURRENT_R = 0;
    NVIC_ST_CTRL_R |= 0x5;
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

// Turns off all LEDs.
void LEDs_Off()
{
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0x0);
};

void LEDs_On(uint32_t GPIO_Pin)
{
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_Pin, GPIO_Pin);
    myDelay(2);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_Pin, 0x0);
    myDelay(2);
};

// Randomize numbers from 0 to 2 to individual LEDs.
void Randomizer(int number)
{
    if(number == 0)
    {
        LEDs_On(GPIO_PIN_1);
    }
    if(number == 1)
    {
        LEDs_On(GPIO_PIN_2);
    }
    if(number == 2)
    {
        LEDs_On(GPIO_PIN_3);
    }
};

// User guess correct, next level.
void Correct_Input()
{
    int loop_1;
    TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet()*15);
    for(loop_1 = 0; loop_1 < 3; loop_1++)
    {
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);
        myDelay(1);
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x0);
        myDelay(1);
    }

    if(level < max_level){
        level++;
    }
    if(level == max_level){
        level = 0;
    }

};

// Randomize patterns every new game.
void Build_Sequence()
{
    unsigned int random_gen = 0;
    int i;
    //TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet()*15);
    for(i = 0; i < max_level; i++)
    {
        random_gen = rand() % 3;
        random_sequence[i] = random_gen;
    }
};

// Show colors that user need to press.
void Show_Sequence()
{
    int i;
    TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet()*15);
    for(i = 0; i <= level; i++)
    {
        Randomizer(random_sequence[i]);
    }
};

// Display incorrect input from user, restarting game.
void Incorrect_Input()
{
    int loop_2;
    TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet()*15);
    for(loop_2 = 0; loop_2 < 3; loop_2++)
    {
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);
        myDelay(1);
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x0);
        myDelay(1);
    }

    level = 0;
};

// Setting up Timer Interrupt.
void
Timer0IntHandler(void)
{
    int i;

    // All leds off by default.
    LEDs_Off();

    // Clear the timer.
    TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet()*15);

    // Red blink 5 times
    for(i = 0; i < 5; i++)
    {
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);
        myDelay(1);
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x0);
        myDelay(1);
    }

    Build_Sequence();

    TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet()*15);
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
}

// User Input Button
void Input_Sequence()
{
    volatile uint32_t pinVal;
    volatile uint32_t pinVal_2;
    volatile uint32_t pinVal_3;
    unsigned int flag = 0;
    int i = 0;

    for(i = 0; i <= level; i++)
    {
        // Go out of while loop, when each button is pressed.
        flag = 0;
        while(flag == 0)
        {
            pinVal = GPIOPinRead(GPIO_PORTA_BASE,GPIO_PIN_2);
            pinVal_2 = GPIOPinRead(GPIO_PORTA_BASE,GPIO_PIN_3);
            pinVal_3 = GPIOPinRead(GPIO_PORTA_BASE,GPIO_PIN_4);

            // If button 1 is pressed, shows light and move out of while loop
            // when it is correct. Wrong input reset.
            if(pinVal != GPIO_PIN_2)
            {
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);
                user_sequence[i] = 0;
                flag = 1;
                myDelay(1);
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x0);
                myDelay(1);
                TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet()*15);
                if(user_sequence[i] != random_sequence[i])
                {
                    Incorrect_Input();
                    return;
                }
            }
            if(pinVal_2 != GPIO_PIN_3)
            {
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
                user_sequence[i] = 1;
                flag = 1;
                myDelay(1);
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x0);
                myDelay(1);
                TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet()*15);
                if(user_sequence[i] != random_sequence[i])
                {
                    Incorrect_Input();
                    return;
                }
            }
            if(pinVal_3 != GPIO_PIN_4)
            {

                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);
                user_sequence[i] = 2;
                flag = 1;
                myDelay(1);
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x0);
                myDelay(1);
                TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet()*15);
                if(user_sequence[i] != random_sequence[i])
                {
                    Incorrect_Input();
                    return;
                }
            }
        }
        Correct_Input();
    }
};

int
main(void)
{
    volatile uint32_t pinVal;
    volatile uint32_t pinVal_2;
    volatile uint32_t pinVal_3;

    // Start psuedo-randomizing as time passes.
    srand(time(NULL));

    // Sets the necessary clock sources to 80 MHz.
    TimerClockSourceGet(TIMER_CLOCK_SYSTEM);
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);


    // Enable the GPIO port that is used for the on-board LED.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA)){}
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)){}

    // Enable the Timer Peripheral.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    // Clear the timer interrupt.
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    // Register interrupt for Timer 0A interrupt.
    TimerIntRegister(TIMER0_BASE, TIMER_A, Timer0IntHandler);
    // Sets timer reload value.
    TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet()*15);
    // Enable timer interrupt.
    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    // Enables the timer.
    TimerEnable(TIMER0_BASE, TIMER_A);


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
        // Resets the sequence every new game.
        if(level == 0)
        {
            Build_Sequence();
        }
        // Gameplay
        if(level <= max_level)
        {
            Show_Sequence();
            Input_Sequence();
        }
    }
}
