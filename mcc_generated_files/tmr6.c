/**
  TMR6 Generated Driver File

  @Company
    Microchip Technology Inc.

  @File Name
    tmr6.c

  @Summary
    This is the generated driver implementation file for the TMR6 driver using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description
    This source file provides APIs for TMR6.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.77
        Device            :  PIC18F26K22
        Driver Version    :  2.01
    The generated drivers are tested against the following:
        Compiler          :  XC8 2.05 and above
        MPLAB 	          :  MPLAB X 5.20
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

/**
  Section: Included Files
*/

#include <xc.h>
#include "tmr6.h"

/**
  Section: Global Variables Definitions
*/

void (*TMR6_InterruptHandler)(void);

/**
  Section: TMR6 APIs
*/

void TMR6_Initialize(void)
{
    // Set TMR6 to the options selected in the User Interface

    // PR6 62; 
    PR6 = 0x3E;

    // TMR6 0; 
    TMR6 = 0x00;

    // Clearing IF flag before enabling the interrupt.
    PIR5bits.TMR6IF = 0;

    // Enabling TMR6 interrupt.
    PIE5bits.TMR6IE = 1;

    // Set Default Interrupt Handler
    TMR6_SetInterruptHandler(TMR6_DefaultInterruptHandler);

    // T6CKPS 1:16; T6OUTPS 1:16; TMR6ON on; 
    T6CON = 0x7E;
}

void TMR6_StartTimer(void)
{
    // Start the Timer by writing to TMRxON bit
    T6CONbits.TMR6ON = 1;
}

void TMR6_StopTimer(void)
{
    // Stop the Timer by writing to TMRxON bit
    T6CONbits.TMR6ON = 0;
}

uint8_t TMR6_ReadTimer(void)
{
    uint8_t readVal;

    readVal = TMR6;

    return readVal;
}

void TMR6_WriteTimer(uint8_t timerVal)
{
    // Write to the Timer6 register
    TMR6 = timerVal;
}

void TMR6_LoadPeriodRegister(uint8_t periodVal)
{
   PR6 = periodVal;
}

void TMR6_ISR(void)
{
    static volatile unsigned int CountCallBack = 0;

    // clear the TMR6 interrupt flag
    PIR5bits.TMR6IF = 0;

    // callback function - called every 3839th pass
    if (++CountCallBack >= TMR6_INTERRUPT_TICKER_FACTOR)
    {
        // ticker function call
        TMR6_CallBack();

        // reset ticker counter
        CountCallBack = 0;
    }
}

void TMR6_CallBack(void)
{
    // Add your custom callback code here
    // this code executes every TMR6_INTERRUPT_TICKER_FACTOR periods of TMR6
    if(TMR6_InterruptHandler)
    {
        TMR6_InterruptHandler();
    }
}

void TMR6_SetInterruptHandler(void (* InterruptHandler)(void)){
    TMR6_InterruptHandler = InterruptHandler;
}

void TMR6_DefaultInterruptHandler(void){
    // add your TMR6 interrupt custom code
    // or set custom function using TMR6_SetInterruptHandler()
}

/**
  End of File
*/