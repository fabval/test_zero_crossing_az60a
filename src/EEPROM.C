#include "PRJ_REF.H"

// EEPROM memory map (simulated in RAM for this example)
#define EEPROM_BASE_ADDR 0x0600
#define EEPROM_SIZE      1024

// Simulated EEPROM in RAM
static uint8_t eeprom_data[EEPROM_SIZE];

// EEPROM parameter addresses
#define PARAM_TANK_LEVEL_CAL  0x0010
#define PARAM_RELAY_DEFAULT   0x0020
#define PARAM_OPTO_DEFAULT    0x0030
#define PARAM_MAINS_FREQ      0x0040

// Initialize EEPROM with default values
void eeprom_init(void) {
    // Set default values
    eeprom_write_word(PARAM_TANK_LEVEL_CAL, 500);
    eeprom_write_byte(PARAM_RELAY_DEFAULT, 0);
    eeprom_write_byte(PARAM_OPTO_DEFAULT, 0);
    eeprom_write_byte(PARAM_MAINS_FREQ, 50);
}

// Read a byte from EEPROM
uint8_t eeprom_read_byte(uint16_t addr) {
    if (addr < EEPROM_SIZE) {
        return eeprom_data[addr];
    }
    return 0;
}

// Write a byte to EEPROM
void eeprom_write_byte(uint16_t addr, uint8_t data) {
    if (addr < EEPROM_SIZE) {
        eeprom_data[addr] = data;
    }
}

// Read a word (16-bit) from EEPROM
uint16_t eeprom_read_word(uint16_t addr) {
    if (addr < EEPROM_SIZE - 1) {
        return (uint16_t)eeprom_data[addr] | ((uint16_t)eeprom_data[addr + 1] << 8);
    }
    return 0;
}

// Write a word (16-bit) to EEPROM
void eeprom_write_word(uint16_t addr, uint16_t data) {
    if (addr < EEPROM_SIZE - 1) {
        eeprom_data[addr] = (uint8_t)(data & 0xFF);
        eeprom_data[addr + 1] = (uint8_t)(data >> 8);
    }
}