#include "PRJ_REF.H"
#include "IO08_REF.H"
#include "PHYS_REF.H"

// UART buffer
#define UART_BUFFER_SIZE 64
volatile uint8_t uart_tx_buffer[UART_BUFFER_SIZE];
volatile uint8_t uart_rx_buffer[UART_BUFFER_SIZE];
volatile uint8_t uart_tx_head = 0;
volatile uint8_t uart_tx_tail = 0;
volatile uint8_t uart_rx_head = 0;
volatile uint8_t uart_rx_tail = 0;

// UART state
volatile uint8_t uart_tx_state = 0;  // 0=idle, 1=sending
volatile uint8_t uart_tx_data = 0;
volatile uint8_t uart_tx_bits = 0;

// UART Timer B Channel 0 interrupt handler
void tim_B_ch0_int(void) {
    // Clear interrupt flag
    TFLG1B = (1 << SERIAL_TIMER);
    
    // Schedule next bit time
    TC0B += 1667;  // 4.9152MHz / 9600 / 3
    
    // Handle TX
    if (uart_tx_state == 1) {
        if (uart_tx_bits == 0) {
            // Start bit
            PORTF &= ~(1 << PTF0);  // TX pin low
            uart_tx_bits++;
        } else if (uart_tx_bits >= 1 && uart_tx_bits <= 8) {
            // Data bits (LSB first)
            if (uart_tx_data & 0x01) {
                PORTF |= (1 << PTF0);  // TX pin high
            } else {
                PORTF &= ~(1 << PTF0);  // TX pin low
            }
            uart_tx_data >>= 1;
            uart_tx_bits++;
        } else if (uart_tx_bits == 9) {
            // Stop bit
            PORTF |= (1 << PTF0);  // TX pin high
            uart_tx_bits++;
        } else {
            // Transmission complete
            uart_tx_state = 0;
            
            // Check if more data to send
            if (uart_tx_head != uart_tx_tail) {
                // Start next byte
                uart_tx_data = uart_tx_buffer[uart_tx_tail];
                uart_tx_tail = (uart_tx_tail + 1) % UART_BUFFER_SIZE;
                uart_tx_bits = 0;
                uart_tx_state = 1;
            }
        }
    }
    
    // Handle RX (simplified, would need edge detection in real implementation)
    static uint8_t rx_bits = 0;
    static uint8_t rx_data = 0;
    static uint8_t rx_sample = 0;
    
    // Check RX pin
    uint8_t rx_pin = (PORTF & (1 << PTF1)) ? 1 : 0;
    
    // Sample RX pin
    if (rx_bits == 0 && rx_pin == 0) {
        // Start bit detected
        rx_bits = 1;
        rx_data = 0;
        rx_sample = 0;
    } else if (rx_bits >= 1 && rx_bits <= 8) {
        // Data bits
        if (rx_sample == 1) {  // Sample in middle of bit
            if (rx_pin) {
                rx_data |= (1 << (rx_bits - 1));
            }
            rx_bits++;
            rx_sample = 0;
        } else {
            rx_sample++;
        }
    } else if (rx_bits == 9) {
        // Stop bit
        if (rx_sample == 1) {
            if (rx_pin) {
                // Valid stop bit, store data
                uart_rx_buffer[uart_rx_head] = rx_data;
                uart_rx_head = (uart_rx_head + 1) % UART_BUFFER_SIZE;
            }
            rx_bits = 0;
        } else {
            rx_sample++;
        }
    }
}

// Send a byte via UART
void uart_send_byte(uint8_t data) {
    // Wait if buffer full
    while ((uart_tx_head + 1) % UART_BUFFER_SIZE == uart_tx_tail);
    
    // Add to buffer
    uart_tx_buffer[uart_tx_head] = data;
    uart_tx_head = (uart_tx_head + 1) % UART_BUFFER_SIZE;
    
    // Start transmission if idle
    if (uart_tx_state == 0) {
        uart_tx_data = uart_tx_buffer[uart_tx_tail];
        uart_tx_tail = (uart_tx_tail + 1) % UART_BUFFER_SIZE;
        uart_tx_bits = 0;
        uart_tx_state = 1;
    }
}

// Check if byte available
uint8_t uart_available(void) {
    return uart_rx_head != uart_rx_tail;
}

// Read a byte from UART
uint8_t uart_read_byte(void) {
    // Wait if buffer empty
    while (uart_rx_head == uart_rx_tail);
    
    // Get byte from buffer
    uint8_t data = uart_rx_buffer[uart_rx_tail];
    uart_rx_tail = (uart_rx_tail + 1) % UART_BUFFER_SIZE;
    
    return data;
}