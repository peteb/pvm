// -*- c++ -*-

#ifndef _BYTECODE_CLASS_FILE_H
#define _BYTECODE_CLASS_FILE_H

#include <cstddef>
#include <vector>
#include <sstream>
#include <string>
#include <stdexcept>

#include "file_format.h"

namespace bytecode {
  template<typename T> struct cp_info_traits {};
  template<> struct cp_info_traits<cp_class_info_t> {cp_tag tag = cp_tag::CLASS; };
  template<> struct cp_info_traits<cp_utf8_info_t> {cp_tag tag = cp_tag::UTF8; };
  template<> struct cp_info_traits<cp_name_and_type_info_t> {cp_tag tag = cp_tag::NAME_AND_TYPE; };
  template<> struct cp_info_traits<cp_fieldref_info_t> {cp_tag tag = cp_tag::FIELDREF; };

  class method;
  class field;

  /**
   * Owns the class data.
   * Fields in sub-structures have been converted to host endian except
   * for unkown fields in attributes.
   */
  class class_file {
  public:
    std::string describe() const;

    const method *find_method(const char *name, const char *descriptor) const;

    std::string utf8_constant(uint16_t idx) const;

    template<typename T>
    const T *constant(uint16_t idx) const {
      auto cp_entry = constant_pool.at(idx - 1);
      if (cp_entry->tag != cp_info_traits<T>().tag) {
        throw std::runtime_error("invalid tag for given type");
      }

      return reinterpret_cast<const T *>(cp_entry);
    }

  protected:
    friend class parser;
    friend class field;
    friend class method;

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

    std::vector<char> buffer;
  };
}

#endif // !_BYTECODE_CLASS_FILE_H
