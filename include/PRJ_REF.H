#ifndef PRJ_REF_H
#define PRJ_REF_H

// Clock and timer definitions
#define BUS_FREQ        4915200UL  // 4.9152 MHz
#define TIM_A_DIV_VAL   4
#define TIM_B_DIV_VAL   1

// Timer channel assignments
#define SERIAL_TIMER    0  // Timer B channel 0
#define OCS             0  // Timer A channel 0
#define ZC              5  // Timer A channel 5
#define IC              2  // Timer A channel 2
#define PWM_ANA_OUT     3  // Timer A channel 3

// Function prototypes
void int_startup(void);
void timer_init(void);
void SER_Init(void);
void tim_A_ch0_int(void);
void tim_A_ch2_int(void);
void tim_A_ch5_int(void);
void zc_int(void);
void ocs_int(void);
void din_acq(void);
void ain_acq(void);
void drive_outputs(void);
void optotriac_activation(void);
void zc_check(void);
void time_ref_mng(void);
void read_tank_level_calibration(void);
void process_tank_level(void);

// Type definitions
typedef unsigned char uint8_t;
typedef unsigned int uint16_t;
typedef unsigned long uint32_t;

#endif /* PRJ_REF_H */