/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: check_request.proto */

/* Do not generate deprecated warnings for self */
#ifndef PROTOBUF_C__NO_DEPRECATED
#define PROTOBUF_C__NO_DEPRECATED
#endif

#include "check_request.pb-c.h"
void   checkrequest__init
                     (Checkrequest         *message)
{
  static const Checkrequest init_value = CHECKREQUEST__INIT;
  *message = init_value;
}
size_t checkrequest__get_packed_size
                     (const Checkrequest *message)
{
  assert(message->base.descriptor == &checkrequest__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t checkrequest__pack
                     (const Checkrequest *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &checkrequest__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t checkrequest__pack_to_buffer
                     (const Checkrequest *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &checkrequest__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
Checkrequest *
       checkrequest__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (Checkrequest *)
     protobuf_c_message_unpack (&checkrequest__descriptor,
                                allocator, len, data);
}
void   checkrequest__free_unpacked
                     (Checkrequest *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &checkrequest__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
static const ProtobufCFieldDescriptor checkrequest__field_descriptors[1] =
{
  {
    "DeviceID",
    1,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    offsetof(Checkrequest, deviceid),
    NULL,
    &protobuf_c_empty_string,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned checkrequest__field_indices_by_name[] = {
  0,   /* field[0] = DeviceID */
};
static const ProtobufCIntRange checkrequest__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 1 }
};
const ProtobufCMessageDescriptor checkrequest__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "Checkrequest",
  "Checkrequest",
  "Checkrequest",
  "",
  sizeof(Checkrequest),
  1,
  checkrequest__field_descriptors,
  checkrequest__field_indices_by_name,
  1,  checkrequest__number_ranges,
  (ProtobufCMessageInit) checkrequest__init,
  NULL,NULL,NULL    /* reserved[123] */
};
