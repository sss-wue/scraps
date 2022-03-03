#ifndef _RANDOM_H_
#define _RANDOM_H_

#ifdef    USE_RANDOM_SEED
#define    RANDOM_SEED_ADDRESS    0x00
#endif    /* !USE_RANDOM_SEED */

void random_initBA(uint16_t seed);

uint16_t randomBA(void);

int uECC_RNG(uint8_t *dest, unsigned size);

#endif    /* !_RANDOM_H_ */
