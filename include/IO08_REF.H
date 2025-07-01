#ifndef IO08_REF_H
#define IO08_REF_H

#include "PRJ_REF.H"

// Timer registers
#define TSCR1   (*(volatile uint8_t*)0x0046)
#define TSCR2   (*(volatile uint8_t*)0x004D)
#define TFLG1   (*(volatile uint8_t*)0x004E)
#define TFLG2   (*(volatile uint8_t*)0x004F)
#define TCTL1   (*(volatile uint8_t*)0x0048)
#define TCTL2   (*(volatile uint8_t*)0x0049)
#define TCTL3   (*(volatile uint8_t*)0x004A)
#define TCTL4   (*(volatile uint8_t*)0x004B)
#define TIE     (*(volatile uint8_t*)0x004C)
#define TCNT    (*(volatile uint16_t*)0x0044)
#define TC0     (*(volatile uint16_t*)0x0050)
#define TC1     (*(volatile uint16_t*)0x0052)
#define TC2     (*(volatile uint16_t*)0x0054)
#define TC3     (*(volatile uint16_t*)0x0056)
#define TC4     (*(volatile uint16_t*)0x0058)
#define TC5     (*(volatile uint16_t*)0x005A)
#define TC6     (*(volatile uint16_t*)0x005C)
#define TC7     (*(volatile uint16_t*)0x005E)
#define TIOS    (*(volatile uint8_t*)0x0040)

// Timer B registers
#define TSCR1B  (*(volatile uint8_t*)0x0086)
#define TSCR2B  (*(volatile uint8_t*)0x008D)
#define TFLG1B  (*(volatile uint8_t*)0x008E)
#define TFLG2B  (*(volatile uint8_t*)0x008F)
#define TCTL1B  (*(volatile uint8_t*)0x0088)
#define TCTL2B  (*(volatile uint8_t*)0x0089)
#define TCTL3B  (*(volatile uint8_t*)0x008A)
#define TCTL4B  (*(volatile uint8_t*)0x008B)
#define TIEB    (*(volatile uint8_t*)0x008C)
#define TCNTB   (*(volatile uint16_t*)0x0084)
#define TC0B    (*(volatile uint16_t*)0x0090)
#define TIOSB   (*(volatile uint8_t*)0x0080)

// Timer control macros
#define _STOP_RESET_TIM()   (TSCR1 &= ~0x80)
#define RESET_TIM()         (TCNT = 0)
#define _START_TIM()        (TSCR1 |= 0x80)

// Timer channel control macros
#define DISABLE_TINT()      (TIE &= ~(1 << OCS))
#define ENABLE_TINT()       (TIE |= (1 << OCS))
#define SET_OC(val)         (TC0 = (val))
#define ADD_OC(val)         (TC0 += (val))

// Timer register bit masks
#define TSCR1_TEN_MASK      0x80

// Zero crossing detection macros
#define ZC_PERIOD_50HZ      24576  // Timer ticks for 50Hz (1.2288MHz / 50Hz)
#define ZC_PERIOD_60HZ      20480  // Timer ticks for 60Hz (1.2288MHz / 60Hz)
#define ZC_THRESHOLD        22000  // Threshold to distinguish between 50Hz and 60Hz

#endif /* IO08_REF_H */