/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.6-dev */

#ifndef PB_EVIDENCE_LUKAS_PB_H_INCLUDED
#define PB_EVIDENCE_LUKAS_PB_H_INCLUDED
#include "pb.h" //was <ph.bh>

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Struct definitions */
typedef struct _EvidenceList { 
    pb_callback_t Evidences; 
} EvidenceList;

typedef struct _Evidence { 
    pb_callback_t ProverIdentity; 
    pb_callback_t AttestationResult; 
    int32_t Timestamp; 
} Evidence;


#ifdef __cplusplus
extern "C" {
#endif

/* Initializer values for message structs */
#define Evidence_init_default                    {{{NULL}, NULL}, {{NULL}, NULL}, 0}
#define EvidenceList_init_default                {{{NULL}, NULL}}
#define Evidence_init_zero                       {{{NULL}, NULL}, {{NULL}, NULL}, 0}
#define EvidenceList_init_zero                   {{{NULL}, NULL}}

/* Field tags (for use in manual encoding/decoding) */
#define EvidenceList_Evidences_tag               1
#define Evidence_ProverIdentity_tag              1
#define Evidence_AttestationResult_tag           2
#define Evidence_Timestamp_tag                   3

/* Struct field encoding specification for nanopb */
#define Evidence_FIELDLIST(X, a) \
X(a, CALLBACK, SINGULAR, STRING,   ProverIdentity,    1) \
X(a, CALLBACK, SINGULAR, STRING,   AttestationResult,   2) \
X(a, STATIC,   SINGULAR, INT32,    Timestamp,         3)
#define Evidence_CALLBACK pb_default_field_callback
#define Evidence_DEFAULT NULL

#define EvidenceList_FIELDLIST(X, a) \
X(a, CALLBACK, REPEATED, MESSAGE,  Evidences,         1)
#define EvidenceList_CALLBACK pb_default_field_callback
#define EvidenceList_DEFAULT NULL
#define EvidenceList_Evidences_MSGTYPE Evidence

extern const pb_msgdesc_t Evidence_msg;
extern const pb_msgdesc_t EvidenceList_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define Evidence_fields &Evidence_msg
#define EvidenceList_fields &EvidenceList_msg

/* Maximum encoded size of messages (where known) */
/* Evidence_size depends on runtime parameters */
/* EvidenceList_size depends on runtime parameters */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif