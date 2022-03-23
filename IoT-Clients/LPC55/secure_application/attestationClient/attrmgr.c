

#include "attrmgr.h"
#include "string.h"
#include "evidence.pb-c.h"
#include "trust_query.pb-c.h"
#include "check_request.pb-c.h"
#include "memory.h"

#include "fsl_debug_console.h"

uint8_t private_key[] = { 0xba, 0x8b, 0xa7, 0x1f, 0x6c, 0x76, 0xda, 0x0c, 0xf3, 0x24, 0xd6, 0x66, 0x3d, 0xc4, 0x80, 0x20, 0x47, 0x19, 0xf3, 0x75, 0xdf, 0xfe, 0xb2, 0x1f, 0xae, 0x76, 0xa7, 0x90, 0x20, 0xa4, 0x43, 0xf1 };
struct AttestationManagerClient client;
int isInitialized = 0;

// ID of the device //
char *prvID = "073B";




void submit_evidence(char *blockID, uint8_t *output, int len) {
	uint8_t *privkeyfile = private_key;

	if(isInitialized == 0) {

		initClient(&client,  privkeyfile);
		isInitialized = 1;
		}
	int size = 0;
	uint8_t *encodedEvidence = buildEvidencePayload(blockID, &size);
	output = submitEvidence(client, encodedEvidence, prvID, size);
/*	for (int i = 0 ; i < len; ++i) {
			output[i]= response[i];
		}*/
	//free(response);
	free(encodedEvidence);
}



void trustQueryDirect( char *trustee, uint8_t *output, int len) {
	uint8_t *privkeyfile = private_key;
	if(isInitialized == 0) {
			initClient(&client,  privkeyfile);
			isInitialized = 1;
			}
	int size = 0;
	uint8_t *queryBytes = buildTrustQueryPayload(trustee, &size);
	const char *response = submitTrustQuery(client, queryBytes, size);
	for (int i = 0 ; i < len; ++i) {
		output[i]= response[i];
	}

	printf("Trust Query Result: %s\n", response);

}


uint8_t* buildEvidencePayload(uint8_t *blockID, int *size) {
	Evidence evidence = EVIDENCE__INIT;
	uint8_t *buffer;
	size_t length;
	//uint8_t* hash[SHA256_BLOCK_SIZE];
	uint8_t* hash;
	attest(&hash);
	evidence.blockid = blockID;
	evidence.proveridentity = prvID;
	evidence.proveridentity = "measurement";
	//evidence.measurement = (char *)hash;
	length = evidence__get_packed_size(&evidence);

	buffer = malloc(length);

	evidence__pack(&evidence, buffer);

	*size = length;

	return buffer;
}

unsigned char* buildTrustQueryPayload(char *trustee, int *size) {
	TrustQuery trustQuery = TRUST_QUERY__INIT;
	uint8_t *buffer;
	size_t length;

	trustQuery.trustor = prvID;
	trustQuery.trustee = trustee;


	length = trust_query__get_packed_size(&trustQuery);

	buffer = malloc(length);

	trust_query__pack(&trustQuery, buffer);

	*size = length;

	return buffer;
}

void checkRequests(uint8_t *output, int len) {
	uint8_t *privkeyfile = private_key;
	if(isInitialized == 0) {

			initClient(&client,  privkeyfile);
			isInitialized = 1;
			}
	int size = 0;
	uint8_t *queryBytes = buildCheckRequestPayload( &size);
	printf("Size Query: %d\n", size);
	println_hex(queryBytes, size);
	uint8_t *response = submitCheckRequest(client, queryBytes, size);
	for (int i = 0 ; i < len; ++i) {
			output[i]= response[i];
		}
}

void attest(uint8_t * hash){

	SHA256_CTX sha256;
	sha256_init(&sha256);
	uint8_t buf[512];
	memory_init();
	for(int i = 0x40000; i < 0x9FFFF; i+=0x200) {
		memory_read(i, buf, sizeof(buf)); //read whole unsecure flash memory starting from 0x40000
		sha256_update(&sha256, buf, 512);
	}
	sha256_final(&sha256, hash);

}

unsigned char* buildCheckRequestPayload(int *size) {
	Checkrequest checkrequests = CHECKREQUEST__INIT;
	uint8_t *buffer;
	size_t length;

	checkrequests.deviceid = prvID;

	length = checkrequest__get_packed_size(&checkrequests);

	buffer = malloc(length);

	checkrequest__pack(&checkrequests, buffer);

	*size = length;

	return buffer;
}
