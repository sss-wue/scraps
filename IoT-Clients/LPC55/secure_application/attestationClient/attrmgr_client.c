 //
// Created by danny on 3/10/20.
//

#include "attrmgr_client.h"
#include "fsl_debug_console.h"

int s_exit_flag = 0;
int s_show_headers = 0;
char *s_show_headers_opt = "--show-headers";

// The Transaction Family Name (e.g. TF Prefix is first 6 characters of SHA-512("attestation"), FADC96)
char *FAMILY_NAME = "attestation";

char *device_id = "073B";
//char *trustmngt_topic = "trust-admin";
int toStop = 0;

// Hashing helper method
void hash512(uint8_t *data, size_t len, uint8_t *toString) {
	unsigned char hash[SHA512_DIGEST_LENGTH];
	SHA512(data, len, hash);
	memcpy(toString, hash, SHA512_DIGEST_LENGTH);
}

// Hashing helper method
// MAKE SURE LEN IS NOT SIZE OF CHAR BUT STRLEN OF CHAR
void hash256(uint8_t *data, size_t len, uint8_t *toString) {
	SHA256_CTX sha256;
	unsigned char hash[SHA256_BLOCK_SIZE];
	sha256_init(&sha256);
	sha256_update(&sha256, data, len);
	sha256_final(&sha256, hash);
	memcpy(toString, hash, SHA256_BLOCK_SIZE);
}

// Helper method for address assembly with current transaction family name
char* assembleAddress(uint8_t *public_key, size_t size) {
	char *assembledAddress = malloc(sizeof(char) * 71);

	char hashoffamilyname[SHA512_DIGEST_LENGTH];
	hash512(FAMILY_NAME, strlen(FAMILY_NAME), hashoffamilyname);

	char *string_of_hashoffamilyname[SHA512_DIGEST_LENGTH * 2 + 1];
	int8_to_char(hashoffamilyname, SHA512_DIGEST_LENGTH, string_of_hashoffamilyname);

	char hashpublic_key[SHA512_DIGEST_LENGTH];
	hash512(public_key, size, hashpublic_key);

	char *string_of_hashpublic_key[SHA512_DIGEST_LENGTH * 2 + 1];
	int8_to_char(hashpublic_key, SHA512_DIGEST_LENGTH, string_of_hashpublic_key);

	strncpy(assembledAddress, string_of_hashoffamilyname, 6);
	strncat(assembledAddress, string_of_hashpublic_key, 64);

	assembledAddress[70] = '\0';

	return assembledAddress;
}

//Initialize the client class. Mainly getting the key pair and computing the address.
void initClient(struct AttestationManagerClient *client, uint8_t *key_file) {

	//int result = 0;
//	int ctx_preallocated_size = 0;

	client->key_file = key_file;

	uint8_t private_key[PRIVATE_KEY_SIZE];
	strncpy((char*) private_key, (char*) key_file, PRIVATE_KEY_SIZE);

	secp256k1_context *signer = NULL;
	//int ctx_preallocated_size = secp256k1_context_preallocated_size(SECP256K1_CONTEXT_VERIFY | SECP256K1_CONTEXT_SIGN);
	signer = secp256k1_context_create(SECP256K1_CONTEXT_VERIFY | SECP256K1_CONTEXT_SIGN);

	int result = secp256k1_ec_seckey_verify(signer, private_key);

	printf("%d", result);

	secp256k1_pubkey public_key;
	result = secp256k1_ec_pubkey_create(signer, &public_key, private_key);


	uint8_t public_key_serialized[SECP256K1_PUBLIC_KEY_INITIAL_SERIALIZATION_SIZE];
	size_t length = sizeof(public_key);

	result = secp256k1_ec_pubkey_serialize(signer, public_key_serialized, &length, &public_key, SECP256K1_EC_COMPRESSED);

	client->public_key = malloc(PUBLIC_KEY_SIZE);

	strncpy((char*) client->public_key, (char*) public_key_serialized, length);

	result = secp256k1_ec_pubkey_parse(client->signer, &public_key, client->public_key, length);

	client->signer = signer;

	client->address = assembleAddress(client->public_key, PUBLIC_KEY_SIZE);

}

uint8_t* getPublicKey(struct AttestationManagerClient client) {
	return client.public_key;
}



// Submit Attestation Evidence to validator.
uint8_t* submitEvidence(struct AttestationManagerClient client, uint8_t *evidence, char *storageKey, int size) {

	// Access to administrative databases must be defined
	char *administrationAddresses[] = { "5a752685e4842d73555848afa198ee40c32e19a400d2fd1a59fdad8c7b57d25b78757c", "5a7526b8d9d9581e82c7c8ec2cb2614bd8da7334cc1335838dd7ad275b9093dbb0a122", "5a7526f43437fca1d5f3d0381073ed3eec9ae42bf86988559e98009795a969919cbeca", "5a75264f03016f8dfef256580a4c6fdeeb5aa0ca8b4068e816a677e908c95b3bdd2150", "5a752639c6f558e7151b5f83e4c1763d427cd0fef5192d2c86ea3db7c5bc1f1546f9ba" };

	char *storageAddress = assembleAddress(storageKey, strlen(storageKey));

	// Allow access to block-info data and the administration transaction family namespace
	char *input_address_list[] = { "00b10c00", "00b10c01", storageAddress };

	int size_of_administrationAddresses = sizeof(administrationAddresses) / sizeof(administrationAddresses[0]);
	int size_of_input_address_list = sizeof(input_address_list) / sizeof(input_address_list[0]);
	int size_of_combined_list = size_of_input_address_list + size_of_administrationAddresses;
	char *combined_list[size_of_combined_list];

	for (int i = 0; i < size_of_combined_list; ++i) {
		if (i < size_of_input_address_list) {
			combined_list[i] = input_address_list[i];
		} else {
			combined_list[i] = administrationAddresses[i - size_of_input_address_list];
		}
	}

	char *output_address_list[] = { "00b10c00", "00b10c01", storageAddress };

	int size_of_output_address_list = sizeof(output_address_list) / sizeof(output_address_list[0]);

	uint8_t *result = _wrap_and_send(client, "submitEvidence", size, evidence, size_of_combined_list, combined_list, size_of_output_address_list, output_address_list);

	return result;
}

//Submit a Trust Query to validator.
uint8_t* submitTrustQuery(struct AttestationManagerClient client, uint8_t *payload, int size) {

	// Access to administrative databases must be defined
	char *administrationAddresses[] = { "5a752685e4842d73555848afa198ee40c32e19a400d2fd1a59fdad8c7b57d25b78757c", "5a7526b8d9d9581e82c7c8ec2cb2614bd8da7334cc1335838dd7ad275b9093dbb0a122", "5a7526f43437fca1d5f3d0381073ed3eec9ae42bf86988559e98009795a969919cbeca", "5a75264f03016f8dfef256580a4c6fdeeb5aa0ca8b4068e816a677e908c95b3bdd2150", "5a752639c6f558e7151b5f83e4c1763d427cd0fef5192d2c86ea3db7c5bc1f1546f9ba" };
	// Allow access to block-info data and the administration transaction family namespace
	char *input_address_list[] = { "00b10c00", "00b10c01", "fadc96" };

	int size_of_administrationAddresses = sizeof(administrationAddresses) / sizeof(administrationAddresses[0]);
	int size_of_input_address_list = sizeof(input_address_list) / sizeof(input_address_list[0]);
	int size_of_combined_list = size_of_input_address_list + size_of_administrationAddresses;
	char *combined_list[size_of_combined_list];
	for (int i = 0; i < size_of_combined_list; ++i) {
		if (i < size_of_input_address_list) {
			combined_list[i] = input_address_list[i];
		} else {
			combined_list[i] = administrationAddresses[i - size_of_input_address_list];
		}
	}

	char *output_address_list[] = { "00b10c00", "00b10c01", "fadc96" };

	int size_of_output_address_list = sizeof(output_address_list) / sizeof(output_address_list[0]);

	uint8_t *result = _wrap_and_send(client, "trustQuery", size, payload, size_of_combined_list, combined_list, size_of_output_address_list, output_address_list);

	return result;
}


uint8_t* submitCheckRequest(struct AttestationManagerClient client, uint8_t *payload, int size) {
	// Access to administrative databases must be defined
	char *administrationAddresses[] = { "5a752685e4842d73555848afa198ee40c32e19a400d2fd1a59fdad8c7b57d25b78757c", "5a7526b8d9d9581e82c7c8ec2cb2614bd8da7334cc1335838dd7ad275b9093dbb0a122", "5a7526f43437fca1d5f3d0381073ed3eec9ae42bf86988559e98009795a969919cbeca", "5a75264f03016f8dfef256580a4c6fdeeb5aa0ca8b4068e816a677e908c95b3bdd2150", "5a752639c6f558e7151b5f83e4c1763d427cd0fef5192d2c86ea3db7c5bc1f1546f9ba" };
	// Allow access to block-info data and the administration transaction family namespace
	char *input_address_list[] = { "00b10c00", "00b10c01", "fadc96" };

	int size_of_administrationAddresses = sizeof(administrationAddresses) / sizeof(administrationAddresses[0]);
	int size_of_input_address_list = sizeof(input_address_list) / sizeof(input_address_list[0]);
	int size_of_combined_list = size_of_input_address_list + size_of_administrationAddresses;
	char *combined_list[size_of_combined_list];
	for (int i = 0; i < size_of_combined_list; ++i) {
		if (i < size_of_input_address_list) {
			combined_list[i] = input_address_list[i];
		} else {
			combined_list[i] = administrationAddresses[i - size_of_input_address_list];
		}
	}

	char *output_address_list[] = { "00b10c00", "00b10c01", "fadc96" };

	int size_of_output_address_list = sizeof(output_address_list) / sizeof(output_address_list[0]);

	uint8_t *result = _wrap_and_send(client, "checkRequest", size, payload, size_of_combined_list, combined_list, size_of_output_address_list, output_address_list);

	return result;
}


int showtopics = 1;
int nodelimiter = 1;


//Create a transaction, then wrap it in a batch. Even single transactions must be wrapped into a batch. Called by submitEvidence and submitTrustQuery.
uint8_t * _wrap_and_send(struct AttestationManagerClient client, char *action, int size, uint8_t *data, int n_input, char *input_address_list[], int n_output, char *output_address_list[]) {

	// +++CBOR of payload and actions
	cbor_item_t *root = cbor_new_definite_map(2);
	cbor_map_add(root, (struct cbor_pair ) { .key = cbor_move(cbor_build_string("Action")), .value = cbor_move(cbor_build_string(action)) });
	cbor_map_add(root, (struct cbor_pair ) { .key = cbor_move(cbor_build_string("Payload")), .value = cbor_move(cbor_build_bytestring(data, size)) });

	uint8_t *cborPayload;
	size_t cborBuffer_size;
	size_t cborLength = cbor_serialize_alloc(root, &cborPayload, &cborBuffer_size);
	// ===CBOR Done

	// +++Create a TransactionHeader.
	TransactionHeader transaction_header = TRANSACTION_HEADER__INIT;
	uint8_t *transaction_header_buffer;
	size_t transaction_header_length;

	char public_key_as_String[PUBLIC_KEY_SIZE * 2 + 1];
	int8_to_char(client.public_key, PUBLIC_KEY_SIZE, public_key_as_String);

	transaction_header.signer_public_key = public_key_as_String;
	transaction_header.family_name = FAMILY_NAME;
	transaction_header.family_version = "1.0";

	transaction_header.n_inputs = n_input;
	transaction_header.inputs = input_address_list;

	transaction_header.n_outputs = n_output;
	transaction_header.outputs = output_address_list;

	char *dependencies[] = { };
	transaction_header.n_dependencies = 0;
	transaction_header.dependencies = dependencies;

	char hash_of_transaction_header_payload[SHA512_DIGEST_LENGTH];
	hash512(cborPayload, cborLength, hash_of_transaction_header_payload);

	char string_of_hash_of_transaction_header_payload[SHA512_DIGEST_LENGTH * 2 + 1];
	int8_to_char(hash_of_transaction_header_payload, SHA512_DIGEST_LENGTH, string_of_hash_of_transaction_header_payload);

	transaction_header.payload_sha512 = string_of_hash_of_transaction_header_payload;

	transaction_header.batcher_public_key = public_key_as_String;

	srand(time(NULL));
	double nonce = (double) rand() / (double) RAND_MAX;
	char sNonce[21];
	snprintf(sNonce, 21, "%.18f", nonce);
	printf("Snonce: %s\n", sNonce);

	transaction_header.nonce = sNonce;

	transaction_header_length = transaction_header__get_packed_size(&transaction_header);
	transaction_header_buffer = malloc(transaction_header_length);
	transaction_header__pack(&transaction_header, transaction_header_buffer);

	// ===Transaction Header Done

	char *transaction_header_signature;

	// +++Create a Transaction from the header and payload above.
	Transaction transaction = TRANSACTION__INIT;
	uint8_t *transaction_buffer;
	size_t transaction_length;

	ProtobufCBinaryData transaction_transaction_header;
	transaction_transaction_header.data = transaction_header_buffer;
	transaction_transaction_header.len = transaction_header_length;

	ProtobufCBinaryData transaction_cborPayload;
	transaction_cborPayload.data = cborPayload;
	transaction_cborPayload.len = cborLength;

	transaction.header = transaction_transaction_header;
	transaction.payload = transaction_cborPayload;

	secp256k1_ecdsa_signature transaction_transaction_header_signature;
	uint8_t *hash_of_transaction_header = malloc(SHA256_BLOCK_SIZE);
	hash256(transaction_header_buffer, transaction_header_length, hash_of_transaction_header);
	secp256k1_ecdsa_sign(client.signer, &transaction_transaction_header_signature, hash_of_transaction_header, client.key_file, NULL, NULL);

	uint8_t der_of_transaction_transaction_header_signature[DER_DIGEST_LENGTH];
	secp256k1_ecdsa_signature_serialize_compact(&client.signer, der_of_transaction_transaction_header_signature, &transaction_transaction_header_signature);

	char string_of_transaction_header_der_signature[DER_DIGEST_LENGTH * 2 + 1];
	int8_to_char(der_of_transaction_transaction_header_signature, DER_DIGEST_LENGTH, string_of_transaction_header_der_signature);
	transaction.header_signature = string_of_transaction_header_der_signature;

	transaction_header_signature = malloc(DER_DIGEST_LENGTH * 2 + 1);
	strncpy(transaction_header_signature, string_of_transaction_header_der_signature, DER_DIGEST_LENGTH * 2 + 1);

	transaction_length = transaction__get_packed_size(&transaction);
	transaction_buffer = malloc(transaction_length);
	transaction__pack(&transaction, transaction_buffer);
	// ===Transaction Done

	// +++Create a BatchHeader from transaction_list above.
	BatchHeader batch_header = BATCH_HEADER__INIT;
	uint8_t *batch_header_buffer;
	size_t batch_header_length;

	batch_header.signer_public_key = public_key_as_String;

	Transaction *transactions;
	Transaction **transaction_list;
	transactions = &transaction;
	transaction_list = &transactions;
	char **transaction_ids_list;
	transaction_ids_list = &transaction_header_signature;

	batch_header.n_transaction_ids = 1;
	batch_header.transaction_ids = transaction_ids_list;

	batch_header_length = batch_header__get_packed_size(&batch_header);
	batch_header_buffer = malloc(batch_header_length + 200);
	batch_header__pack(&batch_header, batch_header_buffer);
	// ===Batch Header Done

	char *batch_header_signature;

	// +++Create Batch using the BatchHeader and transaction_list above.
	Batch batch = BATCH__INIT;
	uint8_t *batch_buffer;
	size_t batch_length;

	ProtobufCBinaryData batch_batch_header;
	batch_batch_header.data = batch_header_buffer;
	batch_batch_header.len = batch_header_length;

	secp256k1_ecdsa_signature batch_batch_header_signature;
	uint8_t *hash_of_batch_header = malloc(SHA256_BLOCK_SIZE);
	hash256(batch_header_buffer, batch_header_length, hash_of_batch_header);
	secp256k1_ecdsa_sign(client.signer, &batch_batch_header_signature, hash_of_batch_header, client.key_file, NULL, NULL);

	uint8_t der_of_batch_batch_header_signature[DER_DIGEST_LENGTH];
	secp256k1_ecdsa_signature_serialize_compact(&client.signer, der_of_batch_batch_header_signature, &batch_batch_header_signature);

	batch.header = batch_batch_header;
	char string_of_batch_header_der_signature[DER_DIGEST_LENGTH * 2 + 1];
	int8_to_char(der_of_batch_batch_header_signature, DER_DIGEST_LENGTH, string_of_batch_header_der_signature);
	batch.header_signature = string_of_batch_header_der_signature;

	batch.n_transactions = 1;
	batch.transactions = transaction_list;

	batch_header_signature = malloc(DER_DIGEST_LENGTH * 2 + 1);
	strncpy(batch_header_signature, string_of_batch_header_der_signature, DER_DIGEST_LENGTH * 2 + 1);

	batch_length = batch__get_packed_size(&batch);
	batch_buffer = malloc(batch_length);
	batch__pack(&batch, batch_buffer);
	// ===Batch Done

	// +++Create a Batch List from Batch above

	BatchList batch_list = BATCH_LIST__INIT;
	uint8_t *batch_list_buffer;
	//uint8_t *batch_list_buffer;
	size_t batch_list_length;

	Batch *batche;
	Batch **batches;
	batche = &batch;
	batches = &batche;

	batch_list.n_batches = 1;
	batch_list.batches = batches;

	batch_list_length = batch_list__get_packed_size(&batch_list);
	batch_list_buffer = malloc(batch_list_length);
	batch_list__pack(&batch_list, batch_list_buffer);
	// ===Batch List Done

	printf("BatchList HEX: ");
	println_hex(batch_list_buffer, batch_list_length);

	printf("BatchList: %s\n", batch_list_buffer);

	uint8_t *batch_id = batch_header_signature;
    //char *json_data = buildJSON(batch_list_buffer, batch_list_length, batch_id, device_id);
	//char str[2800];
	//= "{ \"batch_list\":  batch_list_buffer , \"batch_id\": 32, \"transaction_id\": \"peter\", \"device_id\": 32 }";

//	strcpy(str, "{ \"batch_list\":");
//	strcat(str, &batch_list_buffer);
//	strcpy(str, " , \"batch_id\": ");
//	strcat(str, &batch_id);
//	strcpy(str, ", \"transaction_id\": ");
//	strcat(str, &transaction_header_signature);
//	strcpy(str, ", \"device_id\":");
//	strcat(str, device_id);
//	strcpy(str, " }");
//    char * list = (char*) batch_list_buffer;
//	strcpy(str, "{\n"
//			"\t\"batch_list\": \"");
//	strcat(str, list);
//	strcat(str, "\",\n"
//			"\t\"batch_id\": \"");
//	strcat(str, batch_id);
//	strcat(str, "\",\n"
//			"\t\"transaction_id\": \"");
//	strcat(str, transaction_header_signature);
//	strcat(str, "\",\n"
//			"\t\"device_id\": \"");
//	strcat(str, device_id);
//	strcat(str, " \" }\n");
//	strcat(str,"\0");

//	strcpy(str, " }");
//	strcat(str, &device_id);
//	strcpy(str, ", \"device_id\":");
//	strcat(str, &transaction_header_signature);
//	strcpy(str, ", \"transaction_id\": ");
//	strcat(str, &batch_id);
//	strcpy(str, " , \"batch_id\": ");
//	strcat(str, &batch_list_buffer);
//	strcpy(str, "{ \"batch_list\":");

	//memcpy(batch_list_buffer, str, sizeof(str) );

//	strcpy(batch_list_buffer, str);


	free(hash_of_batch_header);
	free(hash_of_transaction_header);
	free(transaction_header_buffer);
	free(transaction_header_signature);
	free(transaction_buffer);
	free(batch_header_buffer);
	free(batch_header_signature);
	free(batch_buffer);
   // free(batch_list_buffer);
	cbor_decref(&root);
	//pb_ostream_t stream = pb_ostream_from_buffer(batch_list_buffer, sizeof(batch_list_buffer));
	//pb_encode(&stream, &batch_list__descriptor, &batch_list);
	printf("Stopping\n");
	return batch_list_buffer;

}

// small helper functions that prints data in hex to the serial port (e.g. 1 byte (ff) = 2 chars (FF))
void print_hex(const uint8_t *data, size_t data_len) {
	for (int i = 0; i < data_len; i++) {
		printf("%02x", data[i]);
	}
}

// just adds a new line to the end of the data
void println_hex(const uint8_t *data, size_t data_len) {
	print_hex(data, data_len);
	printf("\n");
}
