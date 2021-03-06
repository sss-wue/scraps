/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: transaction.proto */

/* Do not generate deprecated warnings for self */
#ifndef PROTOBUF_C__NO_DEPRECATED
#define PROTOBUF_C__NO_DEPRECATED
#endif

#include "transaction.pb-c.h"
void   transaction_header__init
                     (TransactionHeader         *message)
{
  static const TransactionHeader init_value = TRANSACTION_HEADER__INIT;
  *message = init_value;
}
size_t transaction_header__get_packed_size
                     (const TransactionHeader *message)
{
  assert(message->base.descriptor == &transaction_header__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t transaction_header__pack
                     (const TransactionHeader *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &transaction_header__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t transaction_header__pack_to_buffer
                     (const TransactionHeader *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &transaction_header__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
TransactionHeader *
       transaction_header__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (TransactionHeader *)
     protobuf_c_message_unpack (&transaction_header__descriptor,
                                allocator, len, data);
}
void   transaction_header__free_unpacked
                     (TransactionHeader *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &transaction_header__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
void   transaction__init
                     (Transaction         *message)
{
  static const Transaction init_value = TRANSACTION__INIT;
  *message = init_value;
}
size_t transaction__get_packed_size
                     (const Transaction *message)
{
  assert(message->base.descriptor == &transaction__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t transaction__pack
                     (const Transaction *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &transaction__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t transaction__pack_to_buffer
                     (const Transaction *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &transaction__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
Transaction *
       transaction__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (Transaction *)
     protobuf_c_message_unpack (&transaction__descriptor,
                                allocator, len, data);
}
void   transaction__free_unpacked
                     (Transaction *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &transaction__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
void   transaction_list__init
                     (TransactionList         *message)
{
  static const TransactionList init_value = TRANSACTION_LIST__INIT;
  *message = init_value;
}
size_t transaction_list__get_packed_size
                     (const TransactionList *message)
{
  assert(message->base.descriptor == &transaction_list__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t transaction_list__pack
                     (const TransactionList *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &transaction_list__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t transaction_list__pack_to_buffer
                     (const TransactionList *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &transaction_list__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
TransactionList *
       transaction_list__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (TransactionList *)
     protobuf_c_message_unpack (&transaction_list__descriptor,
                                allocator, len, data);
}
void   transaction_list__free_unpacked
                     (TransactionList *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &transaction_list__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
static const ProtobufCFieldDescriptor transaction_header__field_descriptors[9] =
{
  {
    "batcher_public_key",
    1,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    offsetof(TransactionHeader, batcher_public_key),
    NULL,
    &protobuf_c_empty_string,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "dependencies",
    2,
    PROTOBUF_C_LABEL_REPEATED,
    PROTOBUF_C_TYPE_STRING,
    offsetof(TransactionHeader, n_dependencies),
    offsetof(TransactionHeader, dependencies),
    NULL,
    &protobuf_c_empty_string,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "family_name",
    3,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    offsetof(TransactionHeader, family_name),
    NULL,
    &protobuf_c_empty_string,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "family_version",
    4,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    offsetof(TransactionHeader, family_version),
    NULL,
    &protobuf_c_empty_string,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "inputs",
    5,
    PROTOBUF_C_LABEL_REPEATED,
    PROTOBUF_C_TYPE_STRING,
    offsetof(TransactionHeader, n_inputs),
    offsetof(TransactionHeader, inputs),
    NULL,
    &protobuf_c_empty_string,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "nonce",
    6,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    offsetof(TransactionHeader, nonce),
    NULL,
    &protobuf_c_empty_string,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "outputs",
    7,
    PROTOBUF_C_LABEL_REPEATED,
    PROTOBUF_C_TYPE_STRING,
    offsetof(TransactionHeader, n_outputs),
    offsetof(TransactionHeader, outputs),
    NULL,
    &protobuf_c_empty_string,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "payload_sha512",
    9,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    offsetof(TransactionHeader, payload_sha512),
    NULL,
    &protobuf_c_empty_string,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "signer_public_key",
    10,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    offsetof(TransactionHeader, signer_public_key),
    NULL,
    &protobuf_c_empty_string,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned transaction_header__field_indices_by_name[] = {
  0,   /* field[0] = batcher_public_key */
  1,   /* field[1] = dependencies */
  2,   /* field[2] = family_name */
  3,   /* field[3] = family_version */
  4,   /* field[4] = inputs */
  5,   /* field[5] = nonce */
  6,   /* field[6] = outputs */
  7,   /* field[7] = payload_sha512 */
  8,   /* field[8] = signer_public_key */
};
static const ProtobufCIntRange transaction_header__number_ranges[2 + 1] =
{
  { 1, 0 },
  { 9, 7 },
  { 0, 9 }
};
const ProtobufCMessageDescriptor transaction_header__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "TransactionHeader",
  "TransactionHeader",
  "TransactionHeader",
  "",
  sizeof(TransactionHeader),
  9,
  transaction_header__field_descriptors,
  transaction_header__field_indices_by_name,
  2,  transaction_header__number_ranges,
  (ProtobufCMessageInit) transaction_header__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCFieldDescriptor transaction__field_descriptors[3] =
{
  {
    "header",
    1,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_BYTES,
    0,   /* quantifier_offset */
    offsetof(Transaction, header),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "header_signature",
    2,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    offsetof(Transaction, header_signature),
    NULL,
    &protobuf_c_empty_string,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "payload",
    3,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_BYTES,
    0,   /* quantifier_offset */
    offsetof(Transaction, payload),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned transaction__field_indices_by_name[] = {
  0,   /* field[0] = header */
  1,   /* field[1] = header_signature */
  2,   /* field[2] = payload */
};
static const ProtobufCIntRange transaction__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 3 }
};
const ProtobufCMessageDescriptor transaction__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "Transaction",
  "Transaction",
  "Transaction",
  "",
  sizeof(Transaction),
  3,
  transaction__field_descriptors,
  transaction__field_indices_by_name,
  1,  transaction__number_ranges,
  (ProtobufCMessageInit) transaction__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCFieldDescriptor transaction_list__field_descriptors[1] =
{
  {
    "transactions",
    1,
    PROTOBUF_C_LABEL_REPEATED,
    PROTOBUF_C_TYPE_MESSAGE,
    offsetof(TransactionList, n_transactions),
    offsetof(TransactionList, transactions),
    &transaction__descriptor,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned transaction_list__field_indices_by_name[] = {
  0,   /* field[0] = transactions */
};
static const ProtobufCIntRange transaction_list__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 1 }
};
const ProtobufCMessageDescriptor transaction_list__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "TransactionList",
  "TransactionList",
  "TransactionList",
  "",
  sizeof(TransactionList),
  1,
  transaction_list__field_descriptors,
  transaction_list__field_indices_by_name,
  1,  transaction_list__number_ranges,
  (ProtobufCMessageInit) transaction_list__init,
  NULL,NULL,NULL    /* reserved[123] */
};
