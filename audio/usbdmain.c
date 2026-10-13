/*----------------------------------------------------------------------------
 * Name:    usbmain.c
 * Purpose: USB Audio Class Demo
 * Version: V1.20
 *----------------------------------------------------------------------------
 *      This software is supplied "AS IS" without any warranties, express,
 *      implied or statutory, including but not limited to the implied
 *      warranties of fitness for purpose, satisfactory quality and
 *      noninfringement. Keil extends you a royalty-free right to reproduce
 *      and distribute executable files created using this software for use
 *      on NXP Semiconductors LPC microcontroller devices only. Nothing else 
 *      gives you the right to use this software.
 *
 * Copyright (c) 2009 Keil - An ARM Company. All rights reserved.
 *
 * Adapted from the Keil USB Audio Class example (usbmain.c V1.20):
 * entry point turned into begin_audio(), on-screen title, potentiometer
 * read refactor, and a joystick-LEFT exit that resets the board.
 *---------------------------------------------------------------------------*/

#include "LPC17xx.h"                        /* LPC17xx definitions */
#include "type.h"

#include "usb.h"
#include "usbcfg.h"
#include "usbhw.h"
#include "usbcore.h"
#include "usbaudio.h"
#include "usbdmain.h"
#include "kbd.h"
#include "LED.h"
#include "GLCD.h"


extern  void SystemClockUpdate(void);
extern uint32_t SystemFrequency;  
extern unsigned char GIMP_IMAGE_pixel_data[];

uint8_t  Mute;                                 /* Mute State */
uint32_t Volume;                               /* Volume Level */
//int rep;

#if USB_DMA
uint32_t *InfoBuf = (uint32_t *)(DMA_BUF_ADR);
short *DataBuf = (short *)(DMA_BUF_ADR + 4*P_C);
#else
uint32_t InfoBuf[P_C];
short DataBuf[B_S];                         /* Data Buffer */
#endif

uint16_t  DataOut;                              /* Data Out Index */
uint16_t  DataIn;                               /* Data In Index */

uint8_t   DataRun;                              /* Data Stream Run State */
uint16_t  value_Potentiometer;                               /* Potenciometer Value */
uint32_t  VUM;                                  /* VU Meter */
uint32_t  Tick;                                 /* Time Tick */

void read_potentiometer(void){
  uint32_t raw;
  LED_Init();

  LPC_ADC->CR |= 0x01000000;              /* Start A/D Conversion */

  do{ raw = LPC_ADC->GDR; }               //read ad reg
  while ((raw & 0x80000000) == 0);      /* Wait for end of A/D Conversion */

  LPC_ADC->CR &= ~0x01000000;             /* Stop A/D Conversion */

  value_Potentiometer = ((raw >> 8) & 0xF8) + ((raw >> 7) & 0x08); /* Extract Potenciometer Value */
}


/*
 * Timer Counter 0 Interrupt Service Routine
 *   executed each 31.25us (32kHz frequency)
 */

void TIMER0_IRQHandler(void){
    int value_Joystick;
    long  value;
    uint32_t count_Data;

	value_Joystick = get_button();
	switch(value_Joystick){												
		case KBD_LEFT: NVIC_SystemReset(); break;
		default:
			if(DataRun){                            /* Data Stream is running */
				value = DataBuf[DataOut];                 /* Get Audio Sample */
				count_Data = (DataIn - DataOut) & (B_S - 1);   /* Buffer Data Count */
				if (count_Data == (B_S - P_C*P_S)) {           /* Too much Data in Buffer */
					DataOut++;                            /* Skip one Sample */
				}
				if (count_Data > (P_C*P_S)) {                  /* Still enough Data in Buffer */
					DataOut++;                            /* Update Data Out Index */
				}
				DataOut &= B_S - 1;                     /* Adjust Buffer Out Index */
				if (value < 0) VUM -= value;                /* Accumulate Neg Value */
				else         VUM += value;                /* Accumulate Pos Value */
				value  *= Volume;                         /* Apply Volume Level */
				value >>= 16;                             /* Adjust Value */
				value  += 0x8000;                         /* Add Bias */
				value  &= 0xFFFF;                         /* Mask Value */
			} else {
				value = 0x8000;                           /* DAC Middle Point */
			}

			if (Mute) {
				value = 0x8000;                           /* DAC Middle Point */
			}

			LPC_DAC->CR = value & 0xFFC0;               /* Set Speaker Output */

			if ((Tick++ & 0x03FF) == 0) {             /* On every 1024th Tick */
				read_potentiometer();                           /* Get Potenciometer Value */
				if (VolCur == 0x8000) {                 /* Check for Minimum Level */
					Volume = 0;                           /* No Sound */
				} else {
					Volume = VolCur * value_Potentiometer;             /* Chained Volume Level */
				}
				value = VUM >> 20;                        /* Scale Accumulated Value */
				VUM = 0;                                /* Clear VUM */
				if (value > 7) value = 7;                   /* Limit Value */
			}
			
			LPC_TIM0->IR = 1;                         /* Clear Interrupt Flag */		
			break;
	}
}


int begin_audio(void) {
    volatile uint32_t pclkdiv, pclk;

    SystemClockUpdate(); // Update system clock frequency

    // Configure pins for ADC (P0.25) and DAC (P0.26)
    LPC_PINCON->PINSEL1 &= ~((0x03 << 18) | (0x03 << 20));
    LPC_PINCON->PINSEL1 |= ((0x01 << 18) | (0x02 << 20));

    LPC_SC->PCONP |= (1 << 12);       // Enable ADC clock

    LPC_ADC->CR = 0x00200E04;         // ADC: 10-bit AIN2 @ 4MHz
    LPC_DAC->CR = 0x00008000;         // DAC output at midpoint

    // Determine peripheral clock for TIMER0
    pclkdiv = (LPC_SC->PCLKSEL0 >> 2) & 0x03;
    switch (pclkdiv) {
        case 0x01: pclk = SystemFrequency; break;
        case 0x02: pclk = SystemFrequency / 2; break;
        case 0x03: pclk = SystemFrequency / 8; break;
        case 0x00:
        default:   pclk = SystemFrequency / 4; break;
    }

    LPC_TIM0->MR0 = pclk / DATA_FREQ - 1; // Set timer match value
    LPC_TIM0->MCR = 3;                     // Interrupt & reset on match
    LPC_TIM0->TCR = 1;                     // Enable timer

    GLCD_Clear(White);

    // Display title
    GLCD_SetBackColor(White);
    GLCD_SetTextColor(Black);
    GLCD_DisplayString(0, 0, 1, " AUDIO CENTER        ");

    // Display subtitle
    GLCD_SetBackColor(White);
    GLCD_SetTextColor(Black);
    GLCD_DisplayString(2, 5, 1, "Playing:");

    NVIC_EnableIRQ(TIMER0_IRQn); // Enable timer interrupt

    USB_Init();                   // Initialize USB
    NVIC_EnableIRQ(USB_IRQn);     // Enable USB interrupt
    USB_Reset();
    USB_SetAddress(0);
    USB_Connect(TRUE);

    GLCD_Clear(Black); // Clear screen after USB init
    return 0;
}



