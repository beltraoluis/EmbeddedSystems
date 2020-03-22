#include <stdint.h>
#include <stdbool.h>
// includes da biblioteca driverlib
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

/* 
 * Embedded Systems
 * Team 01
 * Luís Henrique Beltrão Santana
 * 10/03/2020
 */

void setup();
void buttonCheck();
void calc();

//#define CLOCK 24000000
#define CLOCK 120000000
#if CLOCK == 120000000
#define PERIOD 8750
#define CORRECTION 1.002004
#else
#define PERIOD 2500
#define CORRECTION 1.055743
#endif
uint32_t high_pulse = (uint32_t)((PERIOD/2)*CORRECTION);
uint32_t low_pulse = PERIOD - (uint32_t)((PERIOD/2)*CORRECTION);
float duty = 0.5;
bool calc_flag = false;

void main()
{
  setup();
 
  while(true){
    GPIOPinWrite(GPIO_PORTH_BASE, GPIO_PIN_0, 1);
    for(uint32_t high_counter = 0; high_counter < high_pulse; high_counter++){}
    GPIOPinWrite(GPIO_PORTH_BASE, GPIO_PIN_0, 0);
    for(uint32_t low_counter = 0; low_counter < low_pulse; low_counter++){}
    buttonCheck();
    if(calc_flag){
      calc();
    }
  }
}

void setup(){
  // clock setup
  SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), CLOCK);
  // enable GPIO PORT H
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOH);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOH));
  // enable GPIO PORT K
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOK));
  // set aligator pad
  GPIOPinTypeGPIOOutput(GPIO_PORTH_BASE, GPIO_PIN_0);
  GPIOPinWrite(GPIO_PORTH_BASE, GPIO_PIN_0, 0);
  GPIOPadConfigSet(GPIO_PORTH_BASE, GPIO_PIN_0 , GPIO_STRENGTH_12MA, GPIO_PIN_TYPE_STD);
  //set S1
  GPIOPinTypeGPIOInput(GPIO_PORTH_BASE, GPIO_PIN_1);
  GPIOPadConfigSet(GPIO_PORTH_BASE, GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
  //set S2
  GPIOPinTypeGPIOInput(GPIO_PORTK_BASE, GPIO_PIN_6);
  GPIOPadConfigSet(GPIO_PORTK_BASE, GPIO_PIN_6, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
}

void buttonCheck(){
  if(GPIOPinRead(GPIO_PORTH_BASE, GPIO_PIN_1) == 0){
    while(GPIOPinRead(GPIO_PORTH_BASE, GPIO_PIN_1) == 0);
    if(duty < 0.9){
      duty += 0.1;
      calc_flag = true;
    }
  }
  else if(GPIOPinRead(GPIO_PORTK_BASE, GPIO_PIN_6) == 0){
    while(GPIOPinRead(GPIO_PORTK_BASE, GPIO_PIN_6) == 0);
    if(duty > 0.2){ // there is a very little (up) error at float operation here
      duty -= 0.1;
      calc_flag = true;
    }
  }
}

void calc(){
  high_pulse = (uint32_t)((PERIOD*duty)*CORRECTION);
  low_pulse = PERIOD - (uint32_t)((PERIOD*duty)*CORRECTION);
}
