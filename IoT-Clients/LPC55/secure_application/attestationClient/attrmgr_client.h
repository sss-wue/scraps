//
// Created by danny on 3/10/20.
//

#ifndef IOT_ATTRMGR_CLIENT_H
#define IOT_ATTRMGR_CLIENT_H

#define PRIVATE_KEY_SIZE 32
#define PUBLIC_KEY_SIZE 33
#define SECP256K1_PUBLIC_KEY_INITIAL_SERIALIZATION_SIZE 64
#define DER_DIGEST_LENGTH 64

#endif //IOT_ATTRMGR_CLIENT_H

#include "protobuf-c.h"
#include "secp256k1_preallocated.h"
#include "secp256k1.h"
#include "batch.pb-c.h"
#include "time.h"
#include "transaction.pb-c.h"
#include "sha256.h"
#include "sha512.h"
#include "cbor.h"
#include "utils.h"
#include "string.h"
#include "tiny-json.h"


struct AttestationManagerClient {
	//priv key for now
	uint8_t *key_file;
	uint8_t *public_key;
	secp256k1_context *signer;
	char *address;

};

void hash512(uint8_t *data, size_t len, uint8_t *toString);
void hash256(uint8_t *data, size_t len, uint8_t *toString);
char* assembleAddress(uint8_t *public_key, size_t size);
void initClient(struct AttestationManagerClient *client, uint8_t *key_file);
uint8_t* getPublicKey(struct AttestationManagerClient client);
uint8_t* submitEvidence(struct AttestationManagerClient client, uint8_t *evidence, char *storageKey, int size);
uint8_t* submitTrustQuery(struct AttestationManagerClient client, uint8_t *payload, int size);
uint8_t * _wrap_and_send(struct AttestationManagerClient client, char *action, int n_data, uint8_t *data, int n_input, char *input_address_list[], int n_output, char *output_address_list[]);
void print_hex(const uint8_t *data, size_t data_len);
void println_hex(const uint8_t *data, size_t data_len);
uint8_t* submitCheckRequest(struct AttestationManagerClient client, uint8_t *payload, int size);
