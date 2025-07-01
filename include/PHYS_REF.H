#ifndef PHYS_REF_H
#define PHYS_REF_H

#include "PRJ_REF.H"

// Port definitions
#define PORTA   (*(volatile uint8_t*)0x0000)
#define PORTB   (*(volatile uint8_t*)0x0001)
#define PORTC   (*(volatile uint8_t*)0x0002)
#define PORTD   (*(volatile uint8_t*)0x0003)
#define PORTE   (*(volatile uint8_t*)0x0004)
#define PORTF   (*(volatile uint8_t*)0x0005)

// Data direction registers
#define DDRA    (*(volatile uint8_t*)0x0010)
#define DDRB    (*(volatile uint8_t*)0x0011)
#define DDRC    (*(volatile uint8_t*)0x0012)
#define DDRD    (*(volatile uint8_t*)0x0013)
#define DDRE    (*(volatile uint8_t*)0x0014)
#define DDRF    (*(volatile uint8_t*)0x0015)

// Pin definitions (bit positions)
#define PTA0    0
#define PTA1    1
#define PTA2    2
#define PTA3    3
#define PTA4    4
#define PTA5    5
#define PTA6    6
#define PTA7    7

#define PTB0    0
#define PTB1    1
#define PTB2    2
#define PTB3    3
#define PTB4    4
#define PTB5    5
#define PTB6    6
#define PTB7    7

#define PTC0    0
#define PTC1    1
#define PTC2    2
#define PTC3    3
#define PTC4    4
#define PTC5    5
#define PTC6    6
#define PTC7    7

#define PTD0    0
#define PTD1    1
#define PTD2    2
#define PTD3    3
#define PTD4    4
#define PTD5    5
#define PTD6    6
#define PTD7    7

#define PTE0    0
#define PTE1    1
#define PTE2    2
#define PTE3    3
#define PTE4    4
#define PTE5    5
#define PTE6    6
#define PTE7    7

#define PTF0    0
#define PTF1    1
#define PTF2    2
#define PTF3    3
#define PTF4    4
#define PTF5    5
#define PTF6    6
#define PTF7    7

// Pin assignments
#define OPTOTRIAC_PIN   PTE2  // Pin for optotriac control
#define WATCHDOG_PIN    PTE3  // Pin for watchdog

// Relay pins
#define RELAY1_PIN      PTD0
#define RELAY2_PIN      PTD1
#define RELAY3_PIN      PTD2
#define RELAY4_PIN      PTD3

// Input pins
#define ZC_PIN          PTC5  // Zero crossing input pin
#define LEVEL_SENSOR    PTC2  // Tub level sensor input

// ADC registers
#define ADSCR   (*(volatile uint8_t*)0x0030)
#define ADR     (*(volatile uint8_t*)0x0031)

#endif /* PHYS_REF_H */