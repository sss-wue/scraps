#include <avr/io.h>

#define BAUD 57600
#include <util/setbaud.h>

#include "microvisor.h"

//custom includes-----------------------
#include "libs/list/list.h"
//#include "sha1.h"
#include <stdio.h>
#include <string.h>

//end of custom includes----------------

void uart_init(void) {
  // Make sure I/O clock to USART1 is enabled
  	PRR0 &= ~(1 << PRUSART1);

  	// Set baud rate to 57.6k at fOSC = 11.0592 MHz
  	UBRR1 = 0x0B;

  	// Clear USART Transmit complete flag, normal USART transmission speed
  	UCSR1A = (1 << TXC1) | (0 << U2X1);

  	// Enable receiver and transmitter
  	UCSR1B = (1 << RXEN1) | (1 << TXEN1);

  	// Asynchronous mode, no parity, 1 stop bit, character size = 8-bit
  	UCSR1C = (1 << UCSZ11) | (1 << UCSZ10) | (0 << UCPOL1);
}

char uart_getchar() {
  char c;
  loop_until_bit_is_set(UCSR1A, RXC1);
  c = UDR1;
  return c;
}

void uart_putchar(char c) {
  if (c == '\n') {
    uart_putchar('\r');
  }
  loop_until_bit_is_set(UCSR1A, UDRE1);
  UDR1 = c;
}

void uart_puts(char *c) {
  while(*c) {
    uart_putchar(*c++);
  }
}

//BOOTLOADER_SECTION void
/*
void aggregate_nonces(SHA1Context *ctx, void *src, void *dest) {

  // Initialise (fill in) the SHA-1 context
  SHA1Reset(ctx);

  // Process all values in the list by adding them to
  // the SHA-1
  node_t * curr = src;
  while (curr->next != NULL) {
    memcpy(dest, curr->val, NONCELEN);
    SHA1Input(ctx, dest, NONCELEN);
    curr = curr->next;
  }
  memcpy(dest, curr->val, NONCELEN);
  SHA1Input(ctx, dest, NONCELEN);

  // Get the hash from the context and write it in the destination
  SHA1Result(ctx, dest);
}
*/
int main(void) {
   uint8_t last_char;
   uint8_t buf[20];  // nonces, sha-1 and everything else that is 20 bytes long
   uint8_t buf2[64]; // secp256k1 signature (2*private_key = 64 bytes)
   uint8_t i;  // valid range: 0..255, for more make it uint16_t
   
   uart_init();

   /* MOSI and MISO pin */
   DDRB |= ( _BV(PB5) | _BV(PB6) );

   while(1) {
       node_t * list = NULL;
       //SHA1Context context;
       i = 0;
       last_char = 'x';
       
       while (last_char != '.') {
           last_char = uart_getchar();
           
           if (last_char == ',' || last_char == '.') {
               if (list == NULL) {
      	           list = l_create(buf);
      	       } else {
      	           l_append(list, buf);
 	       }
 	       i = 0;
           } else {
               buf[i] = last_char;
               i++;
           }
       }

      
       // Do remote attest
       remote_attestation(buf, buf2);
      // char *msg = submit_request_new();
	//char *msg = submit_trust_query();
	uart_putchar(msg[0]);
	uart_putchar(msg[1]);
	uart_putchar(msg[2]);
	uart_putchar(msg[3]);
       // Write signature
       for(i=0; i<64; i++) {
           uart_putchar(buf2[i]);
       }
       
       uart_putchar(';');
       uart_putchar(';');
       
       // Write memory/nonce hash
       for(i=0; i<20; i++){
           uart_putchar(buf[i]);
       }
       
       uart_putchar(';');
       uart_putchar(';');
       
       // Write nonces one by one as the final argument. Separate them with ','

       node_t *curr = list;
       while(curr->next != NULL) {
           for(i=0; i<NONCELEN; i++) {
               uart_putchar(curr->val[i]);
           }
           
           uart_putchar(',');
           
           curr = curr->next;
       }
       for(i=0; i<NONCELEN; i++) {
           uart_putchar(curr->val[i]);
       }
       
       uart_putchar('.');
       uart_putchar('.');
   }
}
