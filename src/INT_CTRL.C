#include "PRJ_REF.H"
#include "IO08_REF.H"
#include "PHYS_REF.H"

// Vector table for interrupts
void (* const _vectab[])(void) = {
    0,                // Reserved
    0,                // Reserved
    0,                // Reserved
    0,                // Reserved
    0,                // Reserved
    0,                // Reserved
    0,                // Reserved
    0,                // Reserved
    tim_A_ch0_int,    // Timer A channel 0 (OCS)
    0,                // Timer A channel 1
    tim_A_ch2_int,    // Timer A channel 2 (IC - tub level sensor)
    0,                // Timer A channel 3
    0,                // Timer A channel 4
    tim_A_ch5_int,    // Timer A channel 5 (ZC)
    0,                // Timer A channel 6
    0                 // Timer A channel 7
};

// Global variables
volatile uint16_t zc_period = 0;
volatile uint8_t mains_frequency = 0;  // 0=unknown, 50=50Hz, 60=60Hz
volatile uint16_t last_zc_time = 0;
volatile uint8_t zc_phase = 0;  // Current phase (0-11)
volatile uint16_t tank_level_calibration = 0;

// Initialize interrupts and timers
void int_startup(void) {
    // Configure Timer A
    TSCR1 = 0x80;  // Enable timer
    TSCR2 = (TIM_A_DIV_VAL & 0x07);  // Set prescaler
    
    // Configure Timer A channel 0 (OCS) as Output Compare
    TIOS |= (1 << OCS);  // Set as output compare
    TCTL2 &= ~(0x03 << (OCS * 2));  // Disconnect from output pin
    TIE |= (1 << OCS);  // Enable interrupt
    
    // Configure Timer A channel 5 (ZC) as Input Capture
    TIOS &= ~(1 << ZC);  // Set as input capture
    TCTL4 |= (0x01 << (ZC * 2));  // Capture on rising edge
    TIE |= (1 << ZC);  // Enable interrupt
    
    // Configure Timer A channel 2 (IC) as Input Capture
    TIOS &= ~(1 << IC);  // Set as input capture
    TCTL4 |= (0x02 << (IC * 2));  // Capture on falling edge
    TIE |= (1 << IC);  // Enable interrupt
    
    // Configure Timer A channel 3 (PWM_ANA_OUT) for debug
    TIOS |= (1 << PWM_ANA_OUT);  // Set as output compare
    TCTL2 |= (0x01 << (PWM_ANA_OUT * 2));  // Toggle output on compare
    
    // Configure Timer B
    TSCR1B = 0x80;  // Enable timer
    TSCR2B = (TIM_B_DIV_VAL & 0x07);  // Set prescaler
    
    // Configure Timer B channel 0 (SERIAL_TIMER) for serial port
    TIOSB |= (1 << SERIAL_TIMER);  // Set as output compare
    
    // Start timers
    _START_TIM();
}

// Initialize timer-related functionality
void timer_init(void) {
    // Read calibration value for tub level sensor
    read_tank_level_calibration();
}

// Read tank level calibration from EEPROM
void read_tank_level_calibration(void) {
    // In a real implementation, this would read from EEPROM
    // For this example, we'll use a fixed value
    tank_level_calibration = 500;
}

// Process tank level sensor reading
void process_tank_level(void) {
    // In a real implementation, this would process the sensor reading
    // For this example, we'll just toggle a debug pin
    PORTF ^= (1 << PTF1);
}

// Initialize serial port
void SER_Init(void) {
    // Configure Timer B channel 0 for software UART
    TC0B = TCNTB + 1667;  // Set for 9600 baud (4.9152MHz / 9600 / 3)
    TIEB |= (1 << SERIAL_TIMER);  // Enable interrupt
    
    // Configure TX/RX pins
    DDRF |= (1 << PTF0);  // TX pin as output
    DDRF &= ~(1 << PTF1);  // RX pin as input
    
    // Initialize TX pin to idle state (high)
    PORTF |= (1 << PTF0);
}

// Timer A Channel 2 Interrupt Service Routine (tub level sensor)
void tim_A_ch2_int(void) {
    // Clear interrupt flag
    TFLG1 = (1 << IC);
    
    // Process tub level sensor reading
    process_tank_level();
}

// Timer A Channel 5 Interrupt Service Routine (Zero Crossing)
void tim_A_ch5_int(void) {
    // Clear interrupt flag
    TFLG1 = (1 << ZC);
    
    // Call ZC interrupt handler
    zc_int();
}

// Zero Crossing interrupt handler
void zc_int(void) {
    uint16_t current_time;
    
    // Toggle watchdog pin
    PORTE ^= (1 << WATCHDOG_PIN);
    
    // Get current timer value
    current_time = TC5;
    
    // Calculate ZC period if not first capture
    if (last_zc_time > 0) {
        zc_period = current_time - last_zc_time;
        
        // Determine mains frequency
        if (zc_period > ZC_THRESHOLD) {
            mains_frequency = 50;
        } else {
            mains_frequency = 60;
        }
        
        // Set up first OCS event at 1/12 of period
        DISABLE_TINT();
        SET_OC(current_time + (zc_period / 12));
        ENABLE_TINT();
        
        // Reset phase counter
        zc_phase = 0;
    }
    
    last_zc_time = current_time;
}

// Timer A Channel 0 Interrupt Service Routine (Output Compare)
void tim_A_ch0_int(void) {
    // Clear interrupt flag
    TFLG1 = (1 << OCS);
    
    // Call OCS interrupt handler
    ocs_int();
}

// Output Compare interrupt handler
void ocs_int(void) {
    // Toggle watchdog pin
    PORTE ^= (1 << WATCHDOG_PIN);
    
    // Schedule next event at 1/12 of period
    DISABLE_TINT();
    ADD_OC(zc_period / 12);
    ENABLE_TINT();
    
    // Increment phase counter (0-11)
    zc_phase = (zc_phase + 1) % 12;
    
    // Execute phase-specific activities
    switch (zc_phase) {
        case 0:  // Zero of phase 1
        case 6:  // Zero of phase 2
            // Read digital inputs
            din_acq();
            // Read analog inputs
            ain_acq();
            break;
            
        case 3:  // Peak of phase 1
        case 9:  // Peak of phase 2
            // Drive outputs
            drive_outputs();
            // Activate optotriac if needed
            optotriac_activation();
            break;
            
        default:
            // Other time reference management
            time_ref_mng();
            break;
    }
}

// Time reference management
void time_ref_mng(void) {
    // Update system time counters
    static uint16_t ms_counter = 0;
    static uint8_t second_counter = 0;
    
    ms_counter++;
    
    // 1 second tick (approximately)
    if (ms_counter >= (mains_frequency == 50 ? 50 : 60)) {
        ms_counter = 0;
        second_counter++;
        
        // Toggle debug LED every second
        PORTF ^= (1 << PTF2);
    }
}