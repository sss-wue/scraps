#include <avr/eeprom.h>
#include "randomBA.h"

static uint16_t random_number = 0;

static uint16_t lfsr16_next(uint16_t n) {
    return (n >> 0x01U) ^ (-(n & 0x01U) & 0xB400U);    
}

void random_initBA(uint16_t seed) {
#ifdef USE_RANDOM_SEED
    random_number = lfsr16_next(eeprom_read_word((uint16_t *)RANDOM_SEED_ADDRESS) ^ seed);
    eeprom_write_word((uint16_t *)0, random_number);
#else
    random_number = seed;
#endif
}

uint16_t randomBA(void) {
    return (random_number = lfsr16_next(random_number)); 
}

// Fills the first *size* positions of dest with random bytes.
// Since random() returns an uint16_t, we take the top or the
// bottom part with 50% probability (random() & 0x01).
int uECC_RNG(uint8_t *dest, unsigned size){
    uint8_t i;
    uint16_t rnd;
    uint8_t take_top;
    for(i=0; i<size; i++) {
        rnd = randomBA();
        take_top = randomBA() & 0x01;
        if(take_top) {
            dest[i] = (uint8_t)(rnd >> 8);
        } else {
            dest[i] = (uint8_t)rnd;
        }
    }
    return 1;
}
