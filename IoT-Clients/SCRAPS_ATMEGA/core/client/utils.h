#ifndef TINY_CBOR_UTILS_H
#define TINY_CBOR_UTILS_H

#include <stdint.h>
#include <stdio.h>

void int8_to_char(const uint8_t *buffer, size_t buff_len, char *out);
void check_print(const char *expected, char *actual, const char *suffix);

#endif //TINY_CBOR_UTILS_H
