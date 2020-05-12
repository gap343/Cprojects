#include "MSP.h"
void ADCInit()
{
	  ADC14->CTL0 =  0x00000010;    // power on and disable during configuration
    ADC14->CTL0 |= 0x04080300;    // sample and hold mode, sysclk, 32 sample clocks, software trigger
    ADC14->CTL1 =  0x00000030;    // 14-bit resolution 
    ADC14->MCTL[2] = 1;           // A1 input, single-ended, Vref=AVCC 
    P5->SEL1 |= 0x10;             // Configure P5.4 for A1
    P5->SEL0 |= 0x10;
    ADC14->CTL1 |= 0x00020000;    // convert for memory reg. 2
    ADC14->CTL0 |= 2;             // enable ADC
	return;
}

int ADCRead()
{
	int z = 0;
	ADC14->CTL0 |= 1;         // start a conversion
	while (!ADC14->IFGR0);    // wait till conversion complete
	z = ADC14->MEM[2];       // read conversion result
	return z;
}
