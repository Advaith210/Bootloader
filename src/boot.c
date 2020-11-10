/*
 * boot.c
 *
 *  Created on: 08-Nov-2020
 *      Author: advaith
 */

#include "boot.h"
#include <string.h>
#include "stm32f7xx.h"
#include "stm32f7xx_hal.h"

extern uint32_t g_pfnVectors[];

static uint32_t vectorTable_RAM[256] __attribute__(( aligned(0x200ul) ));
static uint8_t enable = 0;

void init_boot_process()
{
	__disable_irq();
	memmove(vectorTable_RAM,g_pfnVectors,256*sizeof(uint32_t));
	SCB->VTOR = 0x8020000;
	__DSB();
	__ISB();
}

void enable_boot()
{
	enable = 1;
}

void boot_to_app(uint32_t app_addr)
{
	if (enable != 1)
		return;

	__disable_irq();
	for(int i = 0;i < 8;i++) NVIC->ICER[i] = 0xFFFFFFFF;
	for(int i = 0;i < 8;i++) NVIC->ICPR[i] = 0xFFFFFFFF;
	__set_CONTROL(0);
	__set_MSP(*(__IO uint32_t*)app_addr);
	uint32_t JumpAddress = *((volatile uint32_t*) (app_addr + 4));
	__ISB();
	__DSB();
	SysTick->CTRL &= ~(SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk);
	void (*reset_handler)(void) = (void*)JumpAddress;
	while(1)
	   reset_handler();
}






