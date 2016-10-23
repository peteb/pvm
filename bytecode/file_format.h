// -*- c++ -*-

#ifndef _BYTECODE_FILE_FORMAT_H
#define _BYTECODE_FILE_FORMAT_H

#define packed __attribute__((packed))

namespace bytecode {
  struct header_t {
    uint32_t magic;
    uint16_t minor_version;
    uint16_t major_version;
  } packed;

  enum class cp_tag : uint8_t {
    UTF8                 = 1,
    INTEGER              = 3,
    FLOAT                = 4,
    LONG                 = 5,
    DOUBLE               = 6,
    CLASS                = 7,
    STRING               = 8,
    FIELDREF             = 9,
    METHODREF            = 10,
    INTERFACE_METHOD_REF = 11,
    NAME_AND_TYPE        = 12,
    METHOD_HANDLE        = 15,
    METHOD_TYPE          = 16,
    INVOKE_DYNAMIC       = 18
  };

  enum cp_access_flags {
    PUBLIC     = 0x0001,
    PRIVATE    = 0x0002,
    PROTECTED  = 0x0004,
    STATIC     = 0x0008,
    FINAL      = 0x0010,
    SUPER      = 0x0020,
    VOLATILE   = 0x0040,
    TRANSIENT  = 0x0080,
    INTERFACE  = 0x0200,
    ABSTRACT   = 0x0400,
    SYNTHETIC  = 0x1000,
    ANNOTATION = 0x2000,
    ENUM       = 0x4000
  };

  struct cp_info_t {
    cp_tag tag;
  } packed;

  struct cp_utf8_info_t {
    cp_tag tag;
    uint16_t length;
    char bytes[0];
  } packed;

  struct cp_integer_info_t {
    cp_tag tag;
    uint32_t value;
  } packed;

  struct cp_float_info_t {
    cp_tag tag;
    uint32_t value;
  } packed;

  struct cp_string_info_t {
    cp_tag tag;
    uint16_t string_idx;
  } packed;

  struct cp_methodref_info_t {
    cp_tag tag;
    uint16_t class_idx;
    uint16_t name_and_type_idx;
  } packed;

  struct cp_fieldref_info_t : public cp_info_t {
    uint16_t class_idx;
    uint16_t name_and_type_idx;
  } packed;

  struct cp_class_info_t {
    cp_tag tag;
    uint16_t name_idx;
  } packed;

  struct cp_name_and_type_info_t {
    cp_tag tag;
    uint16_t name_idx;
    uint16_t descriptor_idx;
  } packed;

  struct attribute_info_t {
    uint16_t name_idx;
    uint32_t length;
    uint8_t info[0];
  } packed;

  struct field_info_t {
    uint16_t access_flags;
    uint16_t name_idx;
    uint16_t descriptor_idx;
    uint16_t attributes_count;
    attribute_info_t attributes[0];
  } packed;

  using method_info_t = field_info_t;

  struct attribute_source_file_t : public attribute_info_t {
    uint16_t sourcefile_idx;
  } packed;
}

#endif // !_BYTECODE_FILE_FORMAT
