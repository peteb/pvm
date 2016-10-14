// -*- c++ -*-

#ifndef _BYTECODE_CLASS_FILE_H
#define _BYTECODE_CLASS_FILE_H

#include <cstddef>
#include <vector>
#include <sstream>
#include <string>

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

  struct cp_info_t {
    cp_tag tag;
  } packed;

  struct cp_utf8_info_t {
    cp_tag tag;
    uint16_t length;
    const char *bytes;
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

  struct cp_fieldref_info_t {
    cp_tag tag;
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

  struct method_info_t {
    uint16_t access_flags;
    uint16_t name_idx;
    uint16_t descriptor_idx;
    uint16_t attributes_count;
    attribute_info_t attributes[0];
  } packed;

  class field_info {
  public:
    uint16_t access_flags;
    uint16_t name_idx;
    uint16_t descriptor_idx;

    std::vector<attribute_info_t *> attributes;
  };

  class method_info {
  public:
    uint16_t access_flags;
    uint16_t name_idx;
    uint16_t descriptor_idx;

    std::vector<attribute_info_t *> attributes;
  };

  class class_file {
  public:
    std::string str() const {
      std::stringstream ss;
      ss << "constant_pool: " << constant_pool.size() << "\n";
      ss << "fields: " << fields.size() << "\n";
      ss << "methods: " << methods.size() << "\n";
      ss << "attributes: " << attributes.size() << "\n";
      return ss.str();
    }

    header_t *header;
    uint16_t access_flags;
    uint16_t this_class;
    uint16_t super_class;

    std::vector<cp_info_t *> constant_pool;
    std::vector<field_info> fields;
    std::vector<method_info> methods;
    std::vector<attribute_info_t *> attributes;

    uint16_t num_interfaces;
    uint16_t *interfaces;

  private:
    friend class parser;

    std::vector<char> buffer;
  };
}

#endif // !_BYTECODE_CLASS_FILE_H
