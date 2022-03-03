#include "attestation_main.h"


char *FAMILY_NAME = "attestation";

// define the header file  
// Hashing helper method
void hash512(uint8_t *data, size_t len, uint8_t *toString) {
	uint8_t hash[SHA512_DIGEST_LENGTH];
	//SHA512(data, len, hash);
	sha512(hash,data, len);
	memcpy(toString, hash, SHA512_DIGEST_LENGTH);
}
// Hashing helper method
// MAKE SURE LEN IS NOT SIZE OF CHAR BUT STRLEN OF CHAR

void hash256(uint8_t *data, size_t len, uint8_t *toString) {
	//uint8_t hash[SHA512_DIGEST_LENGTH/2];
	//sha256(hash, data, len);
	sha256_hash_t hash;
	sha256(&hash, data, len);
	memcpy(toString, hash, SHA512_DIGEST_LENGTH/2);
}

void hash_to_string(uint8_t *string, size_t len) {

    size_t i;

    for (i = 0; i < len; ++i) {
        printf("%02x", string[i]);
    }
    printf("\n");
}
// Helper method for address assembly with current transaction family name


const char* assembleAddress(uint8_t *public_key, size_t size) {
	char *assembledAddress = (char*) malloc(sizeof(char) * 71);

	uint8_t hashoffamilyname[SHA512_DIGEST_LENGTH];
	hash512(FAMILY_NAME, strlen(FAMILY_NAME), hashoffamilyname);

	char string_of_hashoffamilyname[SHA512_DIGEST_LENGTH * 2 + 1];
	int8_to_char(hashoffamilyname, SHA512_DIGEST_LENGTH, string_of_hashoffamilyname);

	uint8_t hashpublic_key[SHA512_DIGEST_LENGTH];
	hash512(public_key, size, hashpublic_key);

	char string_of_hashpublic_key[SHA512_DIGEST_LENGTH * 2 + 1];
	int8_to_char(hashpublic_key, SHA512_DIGEST_LENGTH, string_of_hashpublic_key);

	strncpy(assembledAddress, string_of_hashoffamilyname, 6);
	strncat(assembledAddress, string_of_hashpublic_key, 64);

	assembledAddress[70] = '\0';

	return assembledAddress;
}

//Initialize the client class. Mainly getting the key pair and computing the address.
void initClient(struct AttestationManagerClient *client, const char *base_url, uint8_t *key_file) {

	int result = 0;

	client->base_url = base_url;
	client->key_file = key_file;

	uint8_t private_key[PRIVATE_KEY_SIZE];
	strncpy((char*) private_key, (char*) key_file, PRIVATE_KEY_SIZE);

	uint8_t public_key[64];
	result = uECC_compute_public_key(private_key, public_key, uECC_secp256k1());

	//println_hex(key_file, 64);
	//println_hex(public_key, 64);

	uint8_t public_key_serialized[PUBLIC_KEY_SIZE];
	size_t length = sizeof(public_key);

	uECC_compress(public_key, public_key_serialized, uECC_secp256k1());

	//println_hex(public_key_serialized, 33);

	client->public_key = malloc(PUBLIC_KEY_SIZE);

	strncpy((char*) client->public_key, (char*) public_key_serialized, 33);

	result = uECC_valid_public_key(client->public_key, uECC_secp256k1());

	client->address = assembleAddress(client->public_key, PUBLIC_KEY_SIZE);

}

bool encode_string(pb_ostream_t* stream, const pb_field_t* field, void* const* arg)
{
    const char* str = (const char*)(*arg);

    if (!pb_encode_tag_for_field(stream, field))
        return false;

    return pb_encode_string(stream, (uint8_t*)str, strlen(str));
}

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

unsigned char* buildEvidencePayload(char *provID, char *attestationResult, int timestamp, size_t *size) {
	
	Evidence evidence = Evidence_init_zero;
	size_t length;
	pb_ostream_t stream;
	//unsigned char *buffer;

	evidence.ProverIdentity.arg = provID;
	evidence.ProverIdentity.funcs.encode = &encode_string;
	evidence.AttestationResult.arg = attestationResult;
	evidence.AttestationResult.funcs.encode = &encode_string;
	evidence.Timestamp = timestamp;

	pb_get_encoded_size(&length, Evidence_fields , &evidence);
//	printf("Message size (before encode) is %lu\n", length);

	//buffer = malloc(length);
	uint8_t buffer[length];
	unsigned char *test = malloc(sizeof(char) * length);
	stream = pb_ostream_from_buffer(buffer, length);
    pb_encode(&stream, Evidence_fields, &evidence);
 //   printf("Message size is %lu\n", stream.bytes_written);
	for(int i = 0; i < length; ++i) {
		test[i] = buffer[i];
	}
	//todo return size of buffer here
	*size = length;
	return test;
}

unsigned char* buildTrustQueryPayload(char *trustor, char *trustee,float minReliability, size_t *size) {
	
	TrustQuery trustQuery = TrustQuery_init_zero;
	size_t length;
	pb_ostream_t stream;
	//unsigned char *buffer;

	trustQuery.Trustor.arg = trustor;
	trustQuery.Trustor.funcs.encode = &encode_string;
	trustQuery.Trustee.arg = trustee;
	trustQuery.Trustee.funcs.encode = &encode_string;
	trustQuery.MinReliability = minReliability;


	pb_get_encoded_size(&length, TrustQuery_fields , &trustQuery);
	//printf("Message size (before encode) is %lu\n", length);

	//buffer = malloc(length);
	uint8_t buffer[length];
	unsigned char *test = malloc(sizeof(char) * length);
	stream = pb_ostream_from_buffer(buffer, length);
    pb_encode(&stream, Evidence_fields, &trustQuery);
    //printf("Message size is %lu\n", stream.bytes_written);
	for(int i = 0; i < length; ++i) {
		test[i] = buffer[i];
	}
	//todo return size of buffer here
	*size = length;
	return test;
}

unsigned char* buildcheckRequestPayload(char *deviceID, size_t *size) {
	
	checkRequests checkRequest = checkRequests_init_zero;
	size_t length;
	pb_ostream_t stream;
	//unsigned char *buffer;

	checkRequest.DeviceIdentity.arg = deviceID;
	checkRequest.DeviceIdentity.funcs.encode = &encode_string;
	
	pb_get_encoded_size(&length, checkRequests_fields , &checkRequest);
	//printf("Message size (before encode) is %lu\n", length);

	//buffer = malloc(length);
	uint8_t buffer[length];
	unsigned char *test = malloc(sizeof(char) * length);
	stream = pb_ostream_from_buffer(buffer, length);
    pb_encode(&stream, checkRequests_fields, &checkRequest);
    //printf("Message size is %lu\n", stream.bytes_written);
	for(int i = 0; i < length; ++i) {
		test[i] = buffer[i];
	}
	//todo return size of buffer here
	*size = length;
	return test;
}
//Create a transaction, then wrap it in a batch. Even single transactions must be wrapped into a batch. Called by submitEvidence and submitTrustQuery.
char* _wrap_and_send(struct AttestationManagerClient client, const char *action, int size, uint8_t *data, int n_input, const char *input_address_list[], int n_output, const char *output_address_list[], int wait) {

	// +++CBOR of payload and actions
	size_t cborBuffer_size = 6 + strnlen(action, 14) + 7 + size + 5;
	uint8_t cborPayload[cborBuffer_size];

	CborEncoder encoder, mapEncoder;
	cbor_encoder_init(&encoder, cborPayload, cborBuffer_size, 2);
	cbor_encoder_create_map(&encoder, &mapEncoder, 2);

	cbor_encode_text_stringz(&mapEncoder, "Action");
	cbor_encode_text_stringz(&mapEncoder, action);

	cbor_encode_text_stringz(&mapEncoder, "Payload");
	cbor_encode_byte_string(&mapEncoder, data, size);
	cbor_encoder_close_container(&encoder, &mapEncoder);

	size_t cborLength = cbor_encoder_get_extra_bytes_needed(&encoder);
	// ===CBOR Done

	// +++Create a TransactionHeader.
	TransactionHeader transaction_header = TransactionHeader_init_zero;
	pb_ostream_t stream;

	size_t transaction_header_length;

	char public_key_as_String[PUBLIC_KEY_SIZE * 2 + 1];
	int8_to_char(client.public_key, PUBLIC_KEY_SIZE, public_key_as_String);

	transaction_header.signer_public_key.arg = public_key_as_String;
	transaction_header.signer_public_key.funcs.encode = &encode_string;

	transaction_header.family_name.arg = FAMILY_NAME;
	transaction_header.family_name.funcs.encode = &encode_string;

	transaction_header.family_version.arg = "1.0";
	transaction_header.family_version.funcs.encode = &encode_string;

	transaction_header.inputs.arg = input_address_list;
	transaction_header.inputs.funcs.encode = &encode_string;

	transaction_header.outputs.arg = output_address_list;
	transaction_header.outputs.funcs.encode = &encode_string;

	char *dependencies[] = { };
	transaction_header.dependencies.arg = dependencies;
	transaction_header.dependencies.funcs.encode = &encode_string;

	char hash_of_transaction_header_payload[SHA512_DIGEST_LENGTH];
	hash512(cborPayload, cborLength, hash_of_transaction_header_payload);

	char string_of_hash_of_transaction_header_payload[SHA512_DIGEST_LENGTH * 2 + 1];
	int8_to_char(hash_of_transaction_header_payload, SHA512_DIGEST_LENGTH, string_of_hash_of_transaction_header_payload);

	transaction_header.payload_sha512.arg = string_of_hash_of_transaction_header_payload;
	transaction_header.payload_sha512.funcs.encode = &encode_string;

	transaction_header.batcher_public_key.arg = public_key_as_String;
	transaction_header.batcher_public_key.funcs.encode = &encode_string;

//	srand(time(NULL));
//	double nonce = (double) rand() / (double) RAND_MAX;
//	char sNonce[21];
	char *sss = "0x1.ec152cbfa7704p-2";
//	snprintf(sNonce, 21, "%.18f", nonce);
//	printf("Snonce: %s\n", sNonce);

	transaction_header.nonce.arg = sss;
	transaction_header.nonce.funcs.encode = &encode_string;

	pb_get_encoded_size(&transaction_header_length, TransactionHeader_fields , &transaction_header);
	//printf("Message size (before encode) is %lu\n", transaction_header_length);

	char header_buffer[transaction_header_length];
	stream = pb_ostream_from_buffer(header_buffer, sizeof(header_buffer));
    pb_encode(&stream, TransactionHeader_fields, &transaction_header);
   // printf("Message size is %lu\n", stream.bytes_written);
	

	//transaction_header_length = transaction_header__get_packed_size(&transaction_header);
	//uint8_t transaction_header_buffer[transaction_header_length];
	//transaction_header__pack(&transaction_header, transaction_header_buffer);
	// ===Transaction Header Done

	char *transaction_header_signature;

// +++Create a Transaction from the header and payload above.
	Transaction transaction = Transaction_init_zero;
//	uint8_t *transaction_buffer;
	size_t transaction_length;

	transaction.header.arg = header_buffer;
	transaction.header.funcs.encode = &encode_string;
	
	transaction.payload.arg = cborPayload;
	transaction.payload.funcs.encode = &encode_string;
	/*ProtobufCBinaryData transaction_cborPayload;
	transaction_cborPayload.data = cborPayload;
	transaction_cborPayload.len = cborLength;

	transaction.header = transaction_transaction_header;
	transaction.payload = transaction_cborPayload;
	*/

	uint8_t der_of_transaction_transaction_header_signature[DER_DIGEST_LENGTH];
	//uint8_t *hash_of_transaction_header = malloc(SHA256_BLOCK_SIZE);
	uint8_t hash_of_transaction_header[SHA256_BLOCK_SIZE];
	hash256(header_buffer, transaction_header_length, hash_of_transaction_header);
	uECC_sign(client.key_file, hash_of_transaction_header, SHA256_BLOCK_SIZE, der_of_transaction_transaction_header_signature,uECC_secp256k1());

	char string_of_transaction_header_der_signature[DER_DIGEST_LENGTH * 2 + 1];
	int8_to_char(der_of_transaction_transaction_header_signature,
	DER_DIGEST_LENGTH, string_of_transaction_header_der_signature);
	transaction.header_signature.arg = string_of_transaction_header_der_signature;
	transaction.header_signature.funcs.encode = &encode_string;

	//transaction_header_signature = malloc(DER_DIGEST_LENGTH * 2 + 1);
	//strncpy(transaction_header_signature, string_of_transaction_header_der_signature,
	//DER_DIGEST_LENGTH * 2 + 1);

	//transaction_length = transaction__get_packed_size(&transaction);
	pb_get_encoded_size(&transaction_length, Transaction_fields , &transaction);
	//printf("Transaction Message size (before encode) is %lu\n", transaction_length);

	uint8_t transaction_buffer[transaction_length];
	stream = pb_ostream_from_buffer(transaction_buffer, sizeof(transaction_buffer));
    pb_encode(&stream, Transaction_fields, &transaction);
  //  printf("Message size is %lu\n", stream.bytes_written);
//	transaction_buffer = malloc(transaction_length);
//	transaction__pack(&transaction, transaction_buffer);
	// ===Transaction Done

	// +++Create a BatchHeader from transaction_list above.
	BatchHeader batch_header = BatchHeader_init_default;
	size_t batch_header_length;

	batch_header.signer_public_key.arg = public_key_as_String;
	batch_header.signer_public_key.funcs.encode = &encode_string;

	/*
	Transaction *transactions;
	Transaction **transaction_list;
	transactions = &transaction;
	transaction_list = &transactions;
	char **transaction_ids_list;
	*/
	//transaction_ids_list = &transaction_header_signature;

	//batch_header.n_transaction_ids = 1;
	batch_header.transaction_ids.arg = string_of_transaction_header_der_signature;
	batch_header.transaction_ids.funcs.encode = &encode_string;

	pb_get_encoded_size(&batch_header_length, BatchHeader_fields , &batch_header);
	//printf("Transaction Message size (before encode) is %lu\n", batch_header_length);
	//batch_header_length = batch_header__get_packed_size(&batch_header);
	//batch_header_buffer = malloc(batch_header_length + 200);
//	uint8_t batch_header_buffer[batch_header_length + 200];
	//batch_header__pack(&batch_header, batch_header_buffer);

	uint8_t batch_header_buffer[batch_header_length];
	stream = pb_ostream_from_buffer(batch_header_buffer, sizeof(batch_header_buffer));
        pb_encode(&stream, BatchHeader_fields, &batch_header);
  //  printf("Message size is %lu\n", stream.bytes_written);
	// ===Batch Header Done

	char *batch_header_signature;

	// +++Create Batch using the BatchHeader and transaction_list above.
	Batch batch = Batch_init_zero;
//	uint8_t *batch_buffer;
	size_t batch_length;
/*
	ProtobufCBinaryData batch_batch_header;
	batch_batch_header.data = batch_header_buffer;
	batch_batch_header.len = batch_header_length;
*/
	batch.header.arg = batch_header_buffer;
	batch.header.funcs.encode = &encode_string;
	uint8_t der_of_batch_batch_header_signature[DER_DIGEST_LENGTH];
	uint8_t *hash_of_batch_header = malloc(SHA256_BLOCK_SIZE);
	hash256(batch_header_buffer, batch_header_length, hash_of_batch_header);
	uECC_sign(client.key_file, hash_of_batch_header, SHA256_BLOCK_SIZE, der_of_batch_batch_header_signature,uECC_secp256k1());

	//batch.header = batch_batch_header;
	char string_of_batch_header_der_signature[DER_DIGEST_LENGTH * 2 + 1];
	int8_to_char(der_of_batch_batch_header_signature, DER_DIGEST_LENGTH, string_of_batch_header_der_signature);
	batch.header_signature.arg = string_of_batch_header_der_signature;
	batch.header_signature.funcs.encode = &encode_string;

	//batch.n_transactions = 1;
	batch.transactions.arg = transaction_buffer;
	batch.transactions.funcs.encode = &encode_string;

	batch_header_signature = malloc(DER_DIGEST_LENGTH * 2 + 1);
	strncpy(batch_header_signature, string_of_batch_header_der_signature,
	DER_DIGEST_LENGTH * 2 + 1);

	//batch_length = batch__get_packed_size(&batch);
//	batch_buffer = malloc(batch_length);
//	uint8_t batch_buffer[batch_length];
//	batch__pack(&batch, batch_buffer);
	// ===Batch Done

	// +++Create a Batch List from Batch above

	BatchList batch_list = BatchList_init_default;
	size_t batch_list_length;
/*
	Batch *batche;
	Batch **batches;
	batche = &batch;
	batches = &batche;

	batch_list.n_batches = 1;
	batch_list.batches = batches;
*/
	batch_list.batches.arg = transaction_buffer;
	batch_list.batches.funcs.encode = &encode_string;	
	//batch_list_length = batch_list__get_packed_size(&batch_list);
	//batch_list_buffer = malloc(batch_list_length);
	//batch_list__pack(&batch_list, batch_list_buffer);
	pb_get_encoded_size(&batch_list_length, BatchList_fields , &batch_list);
	//printf("Transaction Message size (before encode) is %lu\n", batch_list_length);
	uint8_t batch_list_buffer[batch_list_length];
	stream = pb_ostream_from_buffer(batch_list_buffer, sizeof(batch_list_buffer));
        pb_encode(&stream, BatchList_fields, &batch_list);
   // printf("Message size is %lu\n", stream.bytes_written);
	// ===Batch List Done

	//printf("BatchList HEX: ");
//	println_hex(batch_list_buffer, batch_list_length);

	//printf("BatchList: %s\n", batch_list_buffer);

		char *string_of_batch_list = malloc(batch_list_length);
		int8_to_char(batch_list_buffer, batch_list_length, string_of_batch_list);

	char *batch_id = batch_header_signature;

//	uint8_t * test = "{\"batch_list\": \"\\n\\u00d8\\t\\n\\u00c7\\u0001\\nB0308b98fa4a188f6401c0b2fd77f59308e4e7193bff28eb3471d24099f5a033b50\\u0012\\u0080\\u000142dc2930052bd8a30a65f57e10d842196c3849bda5b32a9e95734cb95d63e4222e7c12e1f78649261c5c91e11384385ffed741f21de911b341668bdc4cf10bae\\u0012\\u0080\\u0001f061e62fb869df519ead553d0fc810ec93b584dfd184d3192f59b5254d0dfbd97c5faf24c33d4a63056e8fa0cd164f6ea518da6981ddcc2ddb76556277a8691c\\u001a\\u0088\\u0007\\n\\u00d3\\u0005\\nB0308b98fa4a188f6401c0b2fd77f59308e4e7193bff28eb3471d24099f5a033b50\\u001a\\u000battestation\"\\u00031.0*\\b00b10c00*\\b00b10c01*\\u0006fadc96*F5a752685e4842d73555848afa198ee40c32e19a400d2fd1a59fdad8c7b57d25b78757c*F5a7526b8d9d9581e82c7c8ec2cb2614bd8da7334cc1335838dd7ad275b9093dbb0a122*F5a7526f43437fca1d5f3d0381073ed3eec9ae42bf86988559e98009795a969919cbeca*F5a75264f03016f8dfef256580a4c6fdeeb5aa0ca8b4068e816a677e908c95b3bdd2150*F5a752639c6f558e7151b5f83e4c1763d427cd0fef5192d2c86ea3db7c5bc1f1546f9ba2\\u00140x1.dc9c27abe3870p-4:\\b00b10c00:\\b00b10c01:\\u0006fadc96J\\u0080\\u0001c809bc33e4465e72fcc665d67dcc30bd15a8babb0c29e711d64fa33b443149b5f1ac940149e19f20c5dcbf870911faf0add62863ef32a571d7c5dd6674205927RB0308b98fa4a188f6401c0b2fd77f59308e4e7193bff28eb3471d24099f5a033b50\\u0012\\u0080\\u000142dc2930052bd8a30a65f57e10d842196c3849bda5b32a9e95734cb95d63e4222e7c12e1f78649261c5c91e11384385ffed741f21de911b341668bdc4cf10bae\\u001a-\\u00a2fActionjtrustQuerygPayloadQ\\n\\u00040794\\u0012\\u0004073B\\u001d\\u0000\\u0000\\u0000?\", \"batch_id\": \"f061e62fb869df519ead553d0fc810ec93b584dfd184d3192f59b5254d0dfbd97c5faf24c33d4a63056e8fa0cd164f6ea518da6981ddcc2ddb76556277a8691c\", \"device_id\": \"admin\"}";

//	printf("test %s\n", test);

	return string_of_batch_list;
        free(hash_of_batch_header);
	free(batch_header_signature);
}

// Submit Attestation Evidence to validator.
char* submitEvidence(struct AttestationManagerClient client, uint8_t *evidence, const char *storageKey, int size) {

	// Access to administrative databases must be defined
	const char *administrationAddresses[] = { "5a752685e4842d73555848afa198ee40c32e19a400d2fd1a59fdad8c7b57d25b78757c", "5a7526b8d9d9581e82c7c8ec2cb2614bd8da7334cc1335838dd7ad275b9093dbb0a122", "5a7526f43437fca1d5f3d0381073ed3eec9ae42bf86988559e98009795a969919cbeca", "5a75264f03016f8dfef256580a4c6fdeeb5aa0ca8b4068e816a677e908c95b3bdd2150", "5a752639c6f558e7151b5f83e4c1763d427cd0fef5192d2c86ea3db7c5bc1f1546f9ba" };

	const char *storageAddress = assembleAddress(storageKey, strlen(storageKey));

	// Allow access to block-info data and the administration transaction family namespace
	const char *input_address_list[] = { "00b10c00", "00b10c01", storageAddress };

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

	const char *output_address_list[] = { "00b10c00", "00b10c01", storageAddress };

	int size_of_output_address_list = sizeof(output_address_list) / sizeof(output_address_list[0]);

	char *result = _wrap_and_send(client, "submitEvidence", size, evidence, size_of_combined_list, combined_list, size_of_output_address_list, output_address_list, 10);

	return result;
}

// Submit Attestation Evidence to validator.
char* submitTrustQuery(struct AttestationManagerClient client, uint8_t *trustQuery, const char *storageKey, int size) {

	// Access to administrative databases must be defined
	const char *administrationAddresses[] = { "5a752685e4842d73555848afa198ee40c32e19a400d2fd1a59fdad8c7b57d25b78757c", "5a7526b8d9d9581e82c7c8ec2cb2614bd8da7334cc1335838dd7ad275b9093dbb0a122", "5a7526f43437fca1d5f3d0381073ed3eec9ae42bf86988559e98009795a969919cbeca", "5a75264f03016f8dfef256580a4c6fdeeb5aa0ca8b4068e816a677e908c95b3bdd2150", "5a752639c6f558e7151b5f83e4c1763d427cd0fef5192d2c86ea3db7c5bc1f1546f9ba" };

	const char *storageAddress = assembleAddress(storageKey, strlen(storageKey));

	// Allow access to block-info data and the administration transaction family namespace
	const char *input_address_list[] = { "00b10c00", "00b10c01", storageAddress };

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

	const char *output_address_list[] = { "00b10c00", "00b10c01", storageAddress };

	int size_of_output_address_list = sizeof(output_address_list) / sizeof(output_address_list[0]);

	char *result = _wrap_and_send(client, "submitTrustQuery", size, trustQuery, size_of_combined_list, combined_list, size_of_output_address_list, output_address_list, 10);

	return result;
}

char* submitCheckRequest(struct AttestationManagerClient client, uint8_t *checkRequest, const char *storageKey, int size) {

	// Access to administrative databases must be defined
	const char *administrationAddresses[] = { "5a752685e4842d73555848afa198ee40c32e19a400d2fd1a59fdad8c7b57d25b78757c", "5a7526b8d9d9581e82c7c8ec2cb2614bd8da7334cc1335838dd7ad275b9093dbb0a122", "5a7526f43437fca1d5f3d0381073ed3eec9ae42bf86988559e98009795a969919cbeca", "5a75264f03016f8dfef256580a4c6fdeeb5aa0ca8b4068e816a677e908c95b3bdd2150", "5a752639c6f558e7151b5f83e4c1763d427cd0fef5192d2c86ea3db7c5bc1f1546f9ba" };

	const char *storageAddress = assembleAddress(storageKey, strlen(storageKey));

	// Allow access to block-info data and the administration transaction family namespace
	const char *input_address_list[] = { "00b10c00", "00b10c01", storageAddress };

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

	const char *output_address_list[] = { "00b10c00", "00b10c01", storageAddress };

	int size_of_output_address_list = sizeof(output_address_list) / sizeof(output_address_list[0]);

	char *result = _wrap_and_send(client, "submitCheckRequest", size, checkRequest, size_of_combined_list, combined_list, size_of_output_address_list, output_address_list, 10);

	return result;
}

void submit_evidence(int blockID, char *prvID, char *measurement, uint8_t * private_key, char *DEFAULT_URL) {
	struct AttestationManagerClient client;
	initClient(&client, DEFAULT_URL, private_key);
	size_t size = 0;
	char *evidence = buildEvidencePayload(prvID, measurement, blockID, &size);
	submitEvidence(client,evidence, prvID, size);
	free(evidence);

}
void submit_trustQuery(char *trustor, char* trustee, float minReliability, uint8_t * private_key, char *DEFAULT_URL) {
	struct AttestationManagerClient client;
	initClient(&client, DEFAULT_URL, private_key);
	size_t size = 0;
	char *trustQuery = buildTrustQueryPayload(trustor, trustee, minReliability, &size);
	submitTrustQuery(client, trustQuery, trustor, size);
	free(trustQuery);
        free(client.address);
        free(client.public_key);
}

char * submit_checkRequest(char *deviceID, uint8_t *private_key, char *DEFAULT_URL) {
	struct AttestationManagerClient client;
	initClient(&client, DEFAULT_URL, private_key);
	size_t size = 0;
	char *checkRequest = buildcheckRequestPayload(deviceID, &size);
	char * msg = submitCheckRequest(client, checkRequest, deviceID, size);
	free(checkRequest);
	free(client.address);
        free(client.public_key);
	return msg;
}





