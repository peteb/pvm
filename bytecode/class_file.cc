#include "class_file.h"
#include "class_file_str.h"

#include "utils.h"
#include "method.h"

using namespace bytecode;

std::string bytecode::class_file::describe() const {
  return *static_cast<const bytecode::class_file_str *>(this);
}

std::string bytecode::class_file::utf8_constant(uint16_t idx) const {
  auto utf8 = constant<cp_utf8_info_t>(idx);
  return std::string(utf8->bytes, utf8->length);
}

const bytecode::method *bytecode::class_file::find_method(const char *name, const char *descriptor) const {
  for (const auto &method : methods) {
    if (utf8_constant(method.name_idx) == name &&
        utf8_constant(method.descriptor_idx) == descriptor) {
      return &method;
    }
  }

  return nullptr;
}
