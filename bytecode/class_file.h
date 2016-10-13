// -*- c++ -*-

#ifndef _BYTECODE_CLASS_FILE_H
#define _BYTECODE_CLASS_FILE_H

#include <cstddef>
#include <vector>

#define packed __attribute__((packed))

namespace bytecode {
  struct header_t {
    uint32_t magic;
    uint16_t minor_version;
    uint16_t major_version;
  } packed;

  enum class cp_tag : uint8_t {
    CP_UTF8                 = 1,
    CP_INTEGER              = 3,
    CP_FLOAT                = 4,
    CP_LONG                 = 5,
    CP_DOUBLE               = 6,
    CP_CLASS                = 7,
    CP_STRING               = 8,
    CP_FIELDREF             = 9,
    CP_METHODREF            = 10,
    CP_INTERFACE_METHOD_REF = 11,
    CP_NAME_AND_TYPE        = 12,
    CP_METHOD_HANDLE        = 15,
    CP_METHOD_TYPE          = 16,
    CP_INVOKE_DYNAMIC       = 18
  };

  struct cp_info_t {
    cp_tag tag;
  } packed;

  struct cp_utf8_info_t {
    cp_tag tag;
    uint16_t length;
    const char *bytes;
  } packed;

  struct field_info_t {

  };

  struct method_info_t {

  };

  struct attribute_info_t {

  };

  class class_file {
  public:
    header_t header;

    std::vector<cp_info_t *> constant_pool;
  };
}

#endif // !_BYTECODE_CLASS_FILE_H
