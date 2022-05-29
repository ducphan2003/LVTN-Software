/*
 * File: KEYPAD_cfg.c
 * Driver Name: [[ 4x4 KEYPAD ]]
 * SW Layer:   ECUAL
 * Created on: Jun 28, 2020
 * Author:     Khaled Magdy
 * -------------------------------------------
 * For More Information, Tutorials, etc.
 * Visit Website: www.DeepBlueMbedded.com
 *
 */

#include "KEYPAD.h"

const KEYPAD_CfgType KEYPAD_CfgParam[KEYPAD_UNITS] =
{
	// KeyPAD Unit 1 Configurations
    {
    	/* ROWs Pins Info */
	  {GPIOB, GPIOB, GPIOB, GPIOB},
		{GPIO_PIN_9, GPIO_PIN_8, GPIO_PIN_7, GPIO_PIN_6},
		/* COLs Pins */
		{GPIOB, GPIOB, GPIOB, GPIOA},
		{GPIO_PIN_5, GPIO_PIN_4, GPIO_PIN_3, GPIO_PIN_15}
	}
};