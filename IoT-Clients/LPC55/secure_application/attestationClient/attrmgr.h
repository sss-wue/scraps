
#ifndef IOT_ATTRMGR_H
#define IOT_ATTRMGR_H

#endif //IOT_ATTRMGR_H

#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "attrmgr_client.h"
#include "evidence.pb-c.h"
#include "trust_query.pb-c.h"

/*struct trustArgs {
	char *command;
	char *trustor;
	char *trustee;

};*/

/*struct evidenceArgs {
	char *command;
	char *blockID;
	char *prvID;
	char *measurement;

};*/

//void submit_evidence(struct evidenceArgs args);
void submit_evidence(char *blockID, uint8_t *output, int len);
void trustQuery(struct trustArgs args);
void trustQueryDirect( char *trustee,  uint8_t  *output, int len);
unsigned char* buildEvidencePayload(uint8_t  *blockID, int *size);
unsigned char* buildTrustQueryPayload( char *trustee, int *size);
void attest(uint8_t *hash);
void checkRequests( uint8_t *output, int len);
unsigned char* buildCheckRequestPayload( int *size);
