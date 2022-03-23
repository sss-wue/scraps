#include<stdio.h> 
#include<stdlib.h>
#include "uECC.h"
#include "batch.pb.h"
#include "transaction.pb.h"
//#include "shae256.h"
#include "sha2_large_common.h"
#include "sha512.h"
//#include "sha512mbedTLS.h"
#include "shae256.h"
#include "cbor/cbor.h"
#include "utils.h"
#include "string.h"
#include "pb_encode.h"
#include "pb.h"
#include "evidence_lukas.pb.h"
#include "trust_query.pb.h"
#include "deviceRequests.pb.h"

#define PRIVATE_KEY_SIZE 32
#define PUBLIC_KEY_SIZE 33
#define SECP256K1_PUBLIC_KEY_INITIAL_SERIALIZATION_SIZE 64
#define DER_DIGEST_LENGTH 64
#define SHA512_DIGEST_LENGTH 64
#define SHA256_BLOCK_SIZE SHA512_DIGEST_LENGTH/2

struct AttestationManagerClient {
	char *base_url;
	uint8_t *key_file;
	uint8_t *public_key;
	char *address;

};

void hash512(uint8_t *data, size_t len, uint8_t *toString);
void hash256(uint8_t *data, size_t len, uint8_t *toString);
void hash_to_string(uint8_t *string, size_t len);
const char* assembleAddress(uint8_t *public_key, size_t size);
void initClient(struct AttestationManagerClient *client, const char *base_url, uint8_t *key_file);
bool encode_string(pb_ostream_t* stream, const pb_field_t* field, void* const* arg);
void print_hex(const uint8_t *data, size_t data_len);
void println_hex(const uint8_t *data, size_t data_len);
unsigned char* buildEvidencePayload(char *provID, char *attestationResult, int timestamp, size_t *size);
char* _wrap_and_send(struct AttestationManagerClient client, const char *action, int size, uint8_t *data, int n_input, const char *input_address_list[], int n_output, const char *output_address_list[], int wait);
char* submitEvidence(struct AttestationManagerClient client, uint8_t *evidence, const char *storageKey, int size);
void submit_evidence(int blockID, char *prvID, char *measurement, uint8_t * private_key, char *DEFAULT_URL);
unsigned char* buildTrustQueryPayload(char *trustor, char *trustee,float minReliability, size_t *size);
char* submitTrustQuery(struct AttestationManagerClient client, uint8_t *trustQuery, const char *storageKey, int size);
void submit_trustQuery(char *trustor, char* trustee, float minReliability, uint8_t * private_key, char *DEFAULT_URL);
char * submit_checkRequest(char *deviceID, uint8_t *private_key, char *DEFAULT_URL);
unsigned char* buildcheckRequestPayload(char *deviceID, size_t *size);
char* submitCheckRequest(struct AttestationManagerClient client, uint8_t *checkRequest, const char *storageKey, int size);
