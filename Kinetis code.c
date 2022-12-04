#include "fsl_device_registers.h"
#include <stdint.h>
#include <stdio.h>

unsigned int nr_overflows = 0;
const unsigned int decoder[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67};
void FTM3_IRQHandler(void) {
	nr_overflows++;
	uint32_t SC_VAL = FTM3_SC;
	if (SC_VAL & 0x80) FTM3_SC &= 0x7F; // clear TOF
}

void ssd_function(unsigned int dutycycle){
	unsigned int counter = (unsigned int)dutycycle;

	GPIOC_PCOR = 0xBF;
	GPIOD_PCOR = 0x7F;

	GPIOC_PSOR = ((decoder[counter % 10] & 0x3F) | ((decoder[counter % 10] & 0x40) <<1)) & 0x1BF;
	GPIOD_PSOR = decoder[counter / 10] & 0xFF;


}
int main(void) {
	//uint32_t inputRegister = GPIOB_PDIR & 0x0C;

	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK; // Port B clock enable
	SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK; // Port C clock enable
	SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK; // Port D clock enable
	SIM_SCGC3 |= SIM_SCGC3_FTM3_MASK; // FTM3 clock enable

	PORTC_GPCLR = 0x01BF0100;
	PORTD_GPCLR = 0x00FF0100;
	PORTB_GPCLR = 0x000C0100; // Configure Port B Pins 2-3 for GPIO;

	GPIOB_PDDR = 0x00000000; // Configure Port A Pins 0-1 for INPUT;

	PORTC_PCR10 = 0x300; // Port C Pin 10 as FTM3_CH6 (ALT3)
	//PORTC_PCR9 = 0x300; // Port C Pin 11 as FTM3_CH6 (ALT3)

	GPIOD_PDDR |= 0xFF;
	GPIOC_PDDR |= 0x1BF;


	FTM3_MODE = 0x5; // Enable FTM3
	FTM3_MOD = 0xFFFF;
	FTM3_SC = 0x0D; // System clock / 32

	NVIC_EnableIRQ(FTM3_IRQn); // Enable FTM3 interrupts

	FTM3_SC |= 0x40; // Enable TOF

	unsigned int t1=0, t2=0, t3=0, pulse_width=0, periode=0, duty_cycle=0;

	while (1) {

		FTM3_CNT = 0; nr_overflows = 0; // initialize counters
		FTM3_C6SC = 0x4; // rising edge
		while(!(FTM3_C6SC & 0x80)); // wait for CHF
		FTM3_C6SC &= ~(1 << 7);
		t1 = FTM3_C6V; // first edge

		FTM3_C6SC = 0x8; // falling edge
		while(!(FTM3_C6SC & 0x80)); // wait for CHF
		FTM3_C6SC &= ~(1 << 7);
		t2 = FTM3_C6V; // second edge

		if (t2 >= t1)
			pulse_width = (nr_overflows << 16) + (t2 - t1);
		else
			pulse_width = ((nr_overflows-1) << 16) + (t2 - t1);

		FTM3_C6SC = 0x4; // rising edge
		while(!(FTM3_C6SC & 0x80)); // wait for CHF
		FTM3_C6SC = 0; // stop C6
		t3 = FTM3_C6V; // third edge

		if (t3 >= t1)
			periode = (nr_overflows << 16) + (t3 - t1);
		else
			periode = ((nr_overflows-1) << 16) + (t3 - t1);

		duty_cycle = pulse_width*100/periode;

		ssd_function(duty_cycle);

	}
	return 0;

}
