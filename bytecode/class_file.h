// -*- c++ -*-

#ifndef _BYTECODE_CLASS_FILE_H
#define _BYTECODE_CLASS_FILE_H

#include <cstddef>
#include <vector>
#include <sstream>
#include <string>

#include "file_format.h"

namespace bytecode {
  class field {
  public:
    uint16_t access_flags;
    uint16_t name_idx;
    uint16_t descriptor_idx;

    std::vector<attribute_info_t *> attributes;
  };

  using method = field;

  class class_file {
  public:
    std::string str() const {
      std::stringstream ss;
      ss << "constant_pool: " << constant_pool.size() << "\n";
      ss << "fields: " << fields.size() << "\n";
      ss << "methods: " << methods.size() << "\n";
      ss << "attributes: " << attributes.size() << "\n";
      ss << "interfaces: " << num_interfaces << "\n";
      return ss.str();
    }

    uint32_t magic;
    uint16_t minor_version;
    uint16_t major_version;
    uint16_t access_flags;
    uint16_t this_class;
    uint16_t super_class;

    std::vector<cp_info_t *> constant_pool;
    std::vector<field> fields;
    std::vector<method> methods;
    std::vector<attribute_info_t *> attributes;

    uint16_t num_interfaces;
    uint16_t *interfaces;

  private:
    friend class parser;

    std::vector<char> buffer;
  };
}

#endif // !_BYTECODE_CLASS_FILE_H
