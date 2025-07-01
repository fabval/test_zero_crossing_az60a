#include "PRJ_REF.H"
#include "IO08_REF.H"
#include "PHYS_REF.H"

// Analog input acquisition table
const uint8_t AIN_ACQ_TAB[] = {
    0,  // Channel 0
    1,  // Channel 1
    2,  // Channel 2
    3,  // Channel 3
    0xFF  // End of table marker
};

// Global variables
volatile uint8_t ain_values[8];  // Storage for analog input values
volatile uint8_t din_values[8];  // Storage for digital input values
volatile uint8_t din_debounced[8];  // Debounced digital inputs
volatile uint8_t relay_states[4];  // Relay output states
volatile uint8_t optotriac_value;  // Optotriac control value

// Acquire analog inputs
void ain_acq(void) {
    static uint8_t channel_index = 0;
    uint8_t current_channel;
    
    // Read converted value from previous channel
    if (channel_index > 0 && AIN_ACQ_TAB[channel_index-1] != 0xFF) {
        ain_values[AIN_ACQ_TAB[channel_index-1]] = ADR;  // Read ADC result
    }
    
    // Get next channel to convert
    current_channel = AIN_ACQ_TAB[channel_index];
    
    // If end of table, reset to beginning
    if (current_channel == 0xFF) {
        channel_index = 0;
        current_channel = AIN_ACQ_TAB[0];
    } else {
        channel_index++;
    }
    
    // Start conversion of next channel
    ADSCR = 0x80 | current_channel;  // Enable ADC and set channel
}

// Acquire and debounce digital inputs
void din_acq(void) {
    // Read raw digital inputs
    din_values[0] = (PORTD & (1 << PTD4)) ? 1 : 0;
    din_values[1] = (PORTD & (1 << PTD5)) ? 1 : 0;
    din_values[2] = (PORTD & (1 << PTD6)) ? 1 : 0;
    din_values[3] = (PORTD & (1 << PTD7)) ? 1 : 0;
    
    // Simple debouncing
    static uint8_t debounce_counters[8] = {0};
    
    for (int i = 0; i < 4; i++) {
        if (din_values[i]) {
            if (debounce_counters[i] < 10) debounce_counters[i]++;
        } else {
            if (debounce_counters[i] > 0) debounce_counters[i]--;
        }
        
        // Update debounced value
        if (debounce_counters[i] > 7) {
            din_debounced[i] = 1;
        } else if (debounce_counters[i] < 3) {
            din_debounced[i] = 0;
        }
    }
}

// Drive relay outputs
void drive_outputs(void) {
    // Update physical outputs based on relay_states
    if (relay_states[0]) PORTD |= (1 << RELAY1_PIN); else PORTD &= ~(1 << RELAY1_PIN);
    if (relay_states[1]) PORTD |= (1 << RELAY2_PIN); else PORTD &= ~(1 << RELAY2_PIN);
    if (relay_states[2]) PORTD |= (1 << RELAY3_PIN); else PORTD &= ~(1 << RELAY3_PIN);
    if (relay_states[3]) PORTD |= (1 << RELAY4_PIN); else PORTD &= ~(1 << RELAY4_PIN);
}

// Control optotriac for soft start
void optotriac_activation(void) {
    // Update optotriac output based on optotriac_value
    if (optotriac_value > 0) {
        PORTE |= (1 << OPTOTRIAC_PIN);
    } else {
        PORTE &= ~(1 << OPTOTRIAC_PIN);
    }
}

// Check for Zero Crossing signal
void zc_check(void) {
    uint16_t timeout_counter = 0;
    
    // Configure ZC pin as input
    DDRC &= ~(1 << ZC_PIN);
    
    // Configure Timer A for ZC detection
    TSCR1 = 0x80;  // Enable timer
    TSCR2 = (TIM_A_DIV_VAL & 0x07);  // Set prescaler
    
    // Configure Timer A channel 5 (ZC) as Input Capture
    TIOS &= ~(1 << ZC);  // Set as input capture
    TCTL4 |= (0x01 << (ZC * 2));  // Capture on rising edge
    TFLG1 = (1 << ZC);  // Clear flag
    
    // Wait for ZC signal
    while (!(TFLG1 & (1 << ZC))) {
        timeout_counter++;
        if (timeout_counter > 50000) {
            // ZC signal not detected, handle error
            break;
        }
    }
    
    // If ZC detected, measure period
    if (TFLG1 & (1 << ZC)) {
        uint16_t first_zc = TC5;
        TFLG1 = (1 << ZC);  // Clear flag
        
        // Wait for next ZC
        timeout_counter = 0;
        while (!(TFLG1 & (1 << ZC))) {
            timeout_counter++;
            if (timeout_counter > 50000) {
                // Second ZC not detected, handle error
                break;
            }
        }
        
        // Calculate period
        if (TFLG1 & (1 << ZC)) {
            uint16_t second_zc = TC5;
            zc_period = second_zc - first_zc;
            
            // Determine mains frequency
            if (zc_period > ZC_THRESHOLD) {
                mains_frequency = 50;
            } else {
                mains_frequency = 60;
            }
        }
    }
}