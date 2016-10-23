#include "field.h"
#include "class_file.h"

const bytecode::attribute_info_t *bytecode::field::find_attribute(const char *name) const {
  for (const auto *attribute : attributes) {
    if (class_data->utf8_constant(attribute->name_idx) == name) {
      return attribute;
    }
  }

  return nullptr;
}

bytecode::field::field(class_file *cf)
  : class_data(cf)
{
}
