/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: evidence.proto */

/* Do not generate deprecated warnings for self */
#ifndef PROTOBUF_C__NO_DEPRECATED
#define PROTOBUF_C__NO_DEPRECATED
#endif

#include "evidence.pb-c.h"
void   evidence__init
                     (Evidence         *message)
{
  static const Evidence init_value = EVIDENCE__INIT;
  *message = init_value;
}
size_t evidence__get_packed_size
                     (const Evidence *message)
{
  assert(message->base.descriptor == &evidence__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t evidence__pack
                     (const Evidence *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &evidence__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t evidence__pack_to_buffer
                     (const Evidence *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &evidence__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
Evidence *
       evidence__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (Evidence *)
     protobuf_c_message_unpack (&evidence__descriptor,
                                allocator, len, data);
}
void   evidence__free_unpacked
                     (Evidence *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &evidence__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
static const ProtobufCFieldDescriptor evidence__field_descriptors[3] =
{
  {
    "ProverIdentity",
    1,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    offsetof(Evidence, proveridentity),
    NULL,
    &protobuf_c_empty_string,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "BlockID",
    2,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    offsetof(Evidence, blockid),
    NULL,
    &protobuf_c_empty_string,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "Measurement",
    3,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    offsetof(Evidence, measurement),
    NULL,
    &protobuf_c_empty_string,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned evidence__field_indices_by_name[] = {
  1,   /* field[1] = BlockID */
  2,   /* field[2] = Measurement */
  0,   /* field[0] = ProverIdentity */
};
static const ProtobufCIntRange evidence__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 3 }
};
const ProtobufCMessageDescriptor evidence__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "Evidence",
  "Evidence",
  "Evidence",
  "",
  sizeof(Evidence),
  3,
  evidence__field_descriptors,
  evidence__field_indices_by_name,
  1,  evidence__number_ranges,
  (ProtobufCMessageInit) evidence__init,
  NULL,NULL,NULL    /* reserved[123] */
};
