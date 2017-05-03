/*
 * config.h
 *
 *  Created on: Mar 27, 2017
 *      Author: jdknoll
 */

#ifndef CONFIG_H_
#define CONFIG_H_

//#define _MATLAB_OUT
#define _DEBUG_MODE


//lower priority levels are higher priority - top 3 bits determine priority
#define INT_PRIORITY_LEVEL_0 0x00	// 0b0000_0000
#define INT_PRIORITY_LEVEL_1 0x20	// 0b0010_0000
#define INT_PRIORITY_LEVEL_2 0x40	// 0b0100_0000
#define INT_PRIORITY_LEVEL_3 0x60	// 0b0110_0000
#define INT_PRIORITY_LEVEL_4 0x80	// 0b1000_0000
#define INT_PRIORITY_LEVEL_5 0xA0	// 0b1010_0000
#define INT_PRIORITY_LEVEL_6 0xC0	// 0b1100_0000
#define INT_PRIORITY_LEVEL_7 0xF0	// 0b1110_0000

#endif /* CONFIG_H_ */
