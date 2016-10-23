// -*- c++ -*-

#include "pclass.h"
#include "pmethod.h"
#include "utils.h"
#include "class_loader.h"
#include "pstring.h"

#include <cassert>

#include "bytecode/class_file.h"

runtime::pvalue runtime::pclass::fetch_method(const std::string &name, const std::string &descriptor) {
  const bytecode::method *method_info = info()->find_method(name.c_str(), descriptor.c_str());
  assert(method_info && "failed to lookup method");

  auto it = resolved_methods.find(method_info);
  if (it != resolved_methods.end()) {
    return runtime::pvalue::ref(it->second);
  }

  auto method = make_unique<runtime::pmethod>(method_info, this);
  resolved_methods.insert({method_info, method.get()});
  return runtime::pvalue::ref(method.release());
}

runtime::pvalue runtime::pclass::fetch_static_field(const char *name) const {
  runtime::pvalue val;
  val.type = val_type::INTEGER;
  val.integer = 12345;
  return val;
}

runtime::pvalue runtime::pclass::resolve_constant(uint16_t idx) {
  auto it = resolved_constants.find(idx);
  if (it != resolved_constants.end()) {
    return it->second;
  }

  runtime::pvalue new_value;

  switch (info()->constant_tag(idx)) {
  case bytecode::cp_tag::CLASS: {
    auto *cp_class = info()->constant<bytecode::cp_class_info_t>(idx);
    new_value = runtime::pvalue::ref(class_loader->resolve(info()->utf8_constant(cp_class->name_idx).c_str()));
    break;
  }

  case bytecode::cp_tag::STRING: {
    // TODO: pstring shouldn't exist, pull it out
    auto *cp_string = info()->constant<bytecode::cp_string_info_t>(idx);
    new_value = runtime::pvalue::ref(new runtime::pstring(info()->utf8_constant(cp_string->string_idx)));
    break;
  }

  case bytecode::cp_tag::METHODREF: {
    auto *methodref = info()->constant<bytecode::cp_methodref_info_t>(idx);
    runtime::pvalue class_value = resolve_constant(methodref->class_idx);
    auto *nat = info()->constant<bytecode::cp_name_and_type_info_t>(methodref->name_and_type_idx);
    new_value = class_value.as<runtime::pclass>()->fetch_method(info()->utf8_constant(nat->name_idx),
                                                                info()->utf8_constant(nat->descriptor_idx));
    break;
  }


  default:
    assert(0 && "invalid tag type for constant resolve");
  }

  resolved_constants.insert({idx, new_value});

  return new_value;
}

std::string runtime::pclass::name() const {
  auto self = info()->constant<bytecode::cp_class_info_t>(info()->this_class);
  return info()->utf8_constant(self->name_idx);
}
