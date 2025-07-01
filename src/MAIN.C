#include "PRJ_REF.H"
#include "IO08_REF.H"
#include "PHYS_REF.H"

// Main function
int main(void) {
    // Initialize GPIO
    DDRD |= (1 << RELAY1_PIN) | (1 << RELAY2_PIN) | (1 << RELAY3_PIN) | (1 << RELAY4_PIN);  // Relays as outputs
    DDRE |= (1 << OPTOTRIAC_PIN) | (1 << WATCHDOG_PIN);  // Optotriac and watchdog as outputs
    DDRF |= (1 << PTF2);  // Debug LED as output
    
    // Initialize timers
    int_startup();
    
    // Initialize serial port
    SER_Init();
    
    // Check for Zero Crossing signal
    zc_check();
    
    // Main loop
    while (1) {
        // Process any pending tasks
        
        // Example: Toggle relay 1 based on analog input 0
        if (ain_values[0] > 128) {
            relay_states[0] = 1;
        } else {
            relay_states[0] = 0;
        }
        
        // Example: Set optotriac value based on analog input 1
        optotriac_value = ain_values[1];
        
        // Example: Send debug data via UART
        if (uart_available()) {
            uint8_t cmd = uart_read_byte();
            
            // Process command
            switch (cmd) {
                case 'A':
                    // Send analog values
                    uart_send_byte(ain_values[0]);
                    uart_send_byte(ain_values[1]);
                    uart_send_byte(ain_values[2]);
                    uart_send_byte(ain_values[3]);
                    break;
                    
                case 'D':
                    // Send digital values
                    uart_send_byte(din_debounced[0]);
                    uart_send_byte(din_debounced[1]);
                    uart_send_byte(din_debounced[2]);
                    uart_send_byte(din_debounced[3]);
                    break;
                    
                case 'R':
                    // Send relay states
                    uart_send_byte(relay_states[0]);
                    uart_send_byte(relay_states[1]);
                    uart_send_byte(relay_states[2]);
                    uart_send_byte(relay_states[3]);
                    break;
                    
                case 'F':
                    // Send mains frequency
                    uart_send_byte(mains_frequency);
                    break;
                    
                case 'Z':
                    // Send ZC period
                    uart_send_byte((uint8_t)(zc_period >> 8));  // High byte
                    uart_send_byte((uint8_t)(zc_period & 0xFF));  // Low byte
                    break;
                    
                case '1':
                    // Toggle relay 1
                    relay_states[0] = !relay_states[0];
                    uart_send_byte(relay_states[0]);
                    break;
                    
                case '2':
                    // Toggle relay 2
                    relay_states[1] = !relay_states[1];
                    uart_send_byte(relay_states[1]);
                    break;
                    
                case '3':
                    // Toggle relay 3
                    relay_states[2] = !relay_states[2];
                    uart_send_byte(relay_states[2]);
                    break;
                    
                case '4':
                    // Toggle relay 4
                    relay_states[3] = !relay_states[3];
                    uart_send_byte(relay_states[3]);
                    break;
                    
                case 'O':
                    // Set optotriac value
                    if (uart_available()) {
                        optotriac_value = uart_read_byte();
                        uart_send_byte(optotriac_value);
                    }
                    break;
                    
                default:
                    // Unknown command
                    uart_send_byte('?');
                    break;
            }
        }
    }
    
    // Never reached
    return 0;
}