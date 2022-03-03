#include <string.h>
#include "utils.h"

void int8_to_char(const uint8_t *buffer, size_t buff_len, char *out) {
    const char hex[] = "0123456789abcdef";
    int i = 0;
    int j = 0;
    while (j < buff_len) {
        out[i++] = hex[(buffer[j] >> 4) & 0xF];
        out[i++] = hex[buffer[j] & 0xF];
        j++;
    }
    out[i] = '\0';
}

void check_print(const char *expected, char *actual, const char *suffix) {
    if (suffix)
        printf("%s\n", suffix);

    if (strcmp(expected, actual) != 0) {
        printf("!!! MISMATCH !!!\nExpected: %s\nActual  : %s\n", expected, actual);
    } else {
        printf("OK: %s\n", actual);
    }

    printf("\n");
}