#include "PRJ_REF.H"
#include "PHYS_REF.H"

// SPI registers
#define SPICR1  (*(volatile uint8_t*)0x00A8)
#define SPICR2  (*(volatile uint8_t*)0x00A9)
#define SPIBR   (*(volatile uint8_t*)0x00AA)
#define SPISR   (*(volatile uint8_t*)0x00AB)
#define SPIDR   (*(volatile uint8_t*)0x00AC)

// SPI pins
#define SPI_MISO_PIN    PTC1
#define SPI_MOSI_PIN    PTC2
#define SPI_SPSCK_PIN   PTC4

// SPI commands
#define CMD_DISPLAY     0x46
#define CMD_LED         0x7E
#define CMD_KEY_PRESSED 0x4E

// SPI message structure
typedef struct {
    uint8_t header;
    uint8_t length;
    uint8_t cmd_code;
    uint8_t cmd_data[16];
    uint8_t checksum;
    uint8_t dummy;
    uint8_t ack_nack;
} spi_message_t;

// Initialize SPI
void spi_init(void) {
    // Configure SPI pins
    DDRC |= (1 << SPI_MOSI_PIN) | (1 << SPI_SPSCK_PIN);  // MOSI and SPSCK as outputs
    DDRC &= ~(1 << SPI_MISO_PIN);  // MISO as input
    
    // Configure SPI
    SPIBR = 0x03;  // Set baud rate divider to 128 (4.9152MHz / 128 = 38.4kHz)
    SPICR1 = 0x50;  // Enable SPI, master mode, CPOL=0, CPHA=0
}

// Calculate checksum
uint8_t spi_calculate_checksum(uint8_t *data, uint8_t length) {
    uint8_t checksum = 0;
    for (uint8_t i = 0; i < length; i++) {
        checksum ^= data[i];
    }
    return checksum;
}

// Send a message via SPI
void spi_send_message(spi_message_t *msg) {
    // Calculate checksum
    uint8_t *data = (uint8_t*)msg;
    msg->checksum = spi_calculate_checksum(data + 2, msg->length);
    
    // Send header
    SPIDR = msg->header;
    while (!(SPISR & 0x80));  // Wait for transfer complete
    
    // Send length
    SPIDR = msg->length;
    while (!(SPISR & 0x80));  // Wait for transfer complete
    
    // Send command and data
    for (uint8_t i = 0; i < msg->length; i++) {
        SPIDR = data[i + 2];  // cmd_code and cmd_data
        while (!(SPISR & 0x80));  // Wait for transfer complete
    }
    
    // Send checksum
    SPIDR = msg->checksum;
    while (!(SPISR & 0x80));  // Wait for transfer complete
    
    // Send dummy
    SPIDR = msg->dummy;
    while (!(SPISR & 0x80));  // Wait for transfer complete
    
    // Receive ACK/NACK
    SPIDR = 0x00;  // Send dummy byte to receive response
    while (!(SPISR & 0x80));  // Wait for transfer complete
    msg->ack_nack = SPIDR;
}

// Example: Send display message
void spi_send_display(char *text, uint8_t length) {
    spi_message_t msg;
    
    msg.header = 0xC6;
    msg.length = length + 1;  // cmd_code + cmd_data
    msg.cmd_code = CMD_DISPLAY;
    
    // Copy text to cmd_data
    for (uint8_t i = 0; i < length && i < 16; i++) {
        msg.cmd_data[i] = text[i];
    }
    
    msg.dummy = 0x00;
    
    spi_send_message(&msg);
}

// Example: Send LED command
void spi_send_led(uint8_t led_mask) {
    spi_message_t msg;
    
    msg.header = 0xC6;
    msg.length = 5;  // cmd_code + 4 bytes of cmd_data
    msg.cmd_code = CMD_LED;
    msg.cmd_data[0] = led_mask;  // LSB(SPI_led1)
    msg.cmd_data[1] = 0x00;      // MSB(SPI_led1)
    msg.cmd_data[2] = 0x00;      // LSB(SPI_led2)
    msg.cmd_data[3] = 0x00;      // MSB(SPI_led2)
    msg.dummy = 0x00;
    
    spi_send_message(&msg);
}

// Process received key press
void spi_process_key_press(uint8_t key_code) {
    // Handle key press based on key_code
    switch (key_code) {
        case 0x01:
            // Key 1 pressed
            relay_states[0] = !relay_states[0];
            break;
            
        case 0x02:
            // Key 2 pressed
            relay_states[1] = !relay_states[1];
            break;
            
        case 0x04:
            // Key 3 pressed
            relay_states[2] = !relay_states[2];
            break;
            
        case 0x08:
            // Key 4 pressed
            relay_states[3] = !relay_states[3];
            break;
    }
}