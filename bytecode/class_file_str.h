// -*- c++ -*-

#ifndef _BYTECODE_CLASS_FILE_STR_H
#define _BYTECODE_CLASS_FILE_STR_H

#include <string>

#include "class_file.h"

namespace bytecode {
  class class_file_str : public class_file {
  public:
    operator std::string() const;

    std::string describe(const cp_info_t *info) const;
    std::string describe_class(uint16_t idx) const;
    std::string describe_name_and_type(uint16_t idx) const;
    std::string describe_attributes(const std::vector<attribute_info_t *> attributes, int indent = 0) const;

    static const class_file_str *from(const class_file *klass) {
      return static_cast<const class_file_str *>(klass);
    }

  private:
    class_file *class_info;
  };
}
#endif
