// -*- c++ -*-

#ifndef _BYTECODE_FIELD_H
#define _BYTECODE_FIELD_H

#include <cstdint>
#include <vector>

#include "file_format.h"

namespace bytecode {
  class class_file;

  class field {
  public:
    field(class_file *cf);

    uint16_t access_flags;
    uint16_t name_idx;
    uint16_t descriptor_idx;

    std::vector<attribute_info_t *> attributes;

    const attribute_info_t *find_attribute(const char *name) const;

  protected:
    class_file *class_data;
  };

}

#endif
