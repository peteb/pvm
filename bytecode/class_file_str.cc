#include "class_file_str.h"

#include <iomanip>
#include <sstream>

#include "method.h"
#include "field.h"
#include "utils.h"

using namespace bytecode;

namespace {
  static std::string access_flags_str(uint16_t flags) {
    std::vector<std::string> segs;

    if (flags & cp_access_flags::PUBLIC) {
      segs.push_back("ACC_PUBLIC");
    }

    if (flags & cp_access_flags::PRIVATE) {
      segs.push_back("ACC_PRIVATE");
    }

    if (flags & cp_access_flags::PROTECTED) {
      segs.push_back("ACC_PROTECTED");
    }

    if (flags & cp_access_flags::FINAL) {
      segs.push_back("ACC_FINAL");
    }

    if (flags & cp_access_flags::SUPER) {
      segs.push_back("ACC_SUPER");
    }

    if (flags & cp_access_flags::STATIC) {
      segs.push_back("ACC_STATIC");
    }

    if (flags & cp_access_flags::INTERFACE) {
      segs.push_back("ACC_INTERFACE");
    }

    if (flags & cp_access_flags::ABSTRACT) {
      segs.push_back("ACC_ABSTRACT");
    }

    if (flags & cp_access_flags::SYNTHETIC) {
      segs.push_back("ACC_SYNTHETIC");
    }

    if (flags & cp_access_flags::ANNOTATION) {
      segs.push_back("ACC_ANNOTATION");
    }

    if (flags & cp_access_flags::ENUM) {
      segs.push_back("ACC_ENUM");
    }

    std::string ret;
    for (int i = 0; i < segs.size(); ++i) {
      ret += segs[i];
      if (i != segs.size() - 1) {
        ret += ", ";
      }
    }

    return ret;
  }

  static std::string modifiers_str(uint16_t flags, bool include_class = true) {
    std::vector<std::string> segs;

    if (flags & cp_access_flags::PUBLIC) {
      segs.push_back("public");
    }

    if (flags & cp_access_flags::PRIVATE) {
      segs.push_back("private");
    }

    if (flags & cp_access_flags::FINAL) {
      segs.push_back("final");
    }

    if (flags & cp_access_flags::STATIC) {
      segs.push_back("static");
    }

    if (flags & cp_access_flags::ABSTRACT) {
      segs.push_back("abstract");
    }

    if (flags & cp_access_flags::INTERFACE) {
      segs.push_back("interface");
    }

    if (flags & cp_access_flags::ENUM) {
      segs.push_back("enum");
    }

    if (include_class) {
      if (!(flags & cp_access_flags::ENUM) && !(flags & cp_access_flags::INTERFACE)) {
        segs.push_back("class");
      }
    }

    std::string ret;
    for (int i = 0; i < segs.size(); ++i) {
      ret += segs[i];
      if (i != segs.size() - 1) {
        ret += " ";
      }
    }

    return ret;
  }
}


bytecode::class_file_str::operator std::string() const {
  std::stringstream ss;

  auto self = constant<cp_class_info_t>(this_class);
  auto self_name = constant<cp_utf8_info_t>(self->name_idx);
  ss << modifiers_str(access_flags) << " " << std::string(self_name->bytes, self_name->length) << "\n";
  ss << "  minor version: " << minor_version << "\n";
  ss << "  major version: " << major_version << "\n";
  ss << "  flags: " << access_flags_str(access_flags) << "\n";
  ss << "Constant pool:\n";

  int i = 1;
  for (auto *info : constant_pool) {
    ss << std::setw(5) << std::right << "#" + lexical_cast(i);
    ss << " = " << describe(info) << std::endl;
    ++i;
  }

  ss << "{\n";

  for (const auto &method : methods) {
    ss << "  " << modifiers_str(method.access_flags, false) + " " + utf8_constant(method.name_idx) << "\n";
    ss << "    descriptor: " << utf8_constant(method.descriptor_idx) << "\n";
    ss << "    flags: " << access_flags_str(method.access_flags) << "\n";

    for (const auto &attr : method.attributes) {
      ss << utf8_constant(attr->name_idx) << "\n";
    }
  }

  ss << "}\n";
  ss << describe_attributes(attributes);

  return ss.str();
}

std::string bytecode::class_file_str::describe_attributes(const std::vector<attribute_info_t *> attributes, int indent) const {
  std::stringstream ss;

  for (auto *attribute : attributes) {
    std::string name = utf8_constant(attribute->name_idx);
    ss << name + ": ";
    if (name == "SourceFile") {
      uint16_t source_idx = f2h(reinterpret_cast<attribute_source_file_t *>(attribute)->sourcefile_idx);
      ss << utf8_constant(source_idx);
    }
    ss << "\n";
  }

  return ss.str();
}

std::string bytecode::class_file_str::describe_class(uint16_t idx) const {
  auto klass = constant<cp_class_info_t>(idx);
  return utf8_constant(klass->name_idx);
}

std::string bytecode::class_file_str::describe_name_and_type(uint16_t idx) const {
  auto name_and_type = constant<cp_name_and_type_info_t>(idx);
  return utf8_constant(name_and_type->name_idx) + ":" + utf8_constant(name_and_type->descriptor_idx);
}

std::string bytecode::class_file_str::describe(const cp_info_t *info) const {
  std::stringstream ss;
  ss << std::setw(20) << std::left;

  switch (info->tag) {

  case cp_tag::UTF8: {
    auto *utf8 = reinterpret_cast<const cp_utf8_info_t *>(info);
    ss << "Utf8";
    ss << std::string(utf8->bytes, utf8->length);
    break;
  }

  case cp_tag::METHODREF: {
    auto *methodref = reinterpret_cast<const cp_methodref_info_t *>(info);
    ss << "Methodref";
    ss << std::setw(15) << std::left;
    ss << "#" + lexical_cast(methodref->class_idx) + ".#" + lexical_cast(methodref->name_and_type_idx);
    ss << "// " + describe_class(methodref->class_idx) + "." + describe_name_and_type(methodref->name_and_type_idx);
    break;
  }

  case cp_tag::CLASS: {
    auto *class_info = reinterpret_cast<const cp_class_info_t *>(info);
    ss << "Class";
    ss << std::setw(15) << std::left;
    ss << "#" + lexical_cast(class_info->name_idx);
    break;
  }

  case cp_tag::NAME_AND_TYPE: {
    auto *name_and_type = reinterpret_cast<const cp_name_and_type_info_t *>(info);
    ss << "NameAndType";
    ss << std::setw(15) << std::left;
    ss << "#" + lexical_cast(name_and_type->name_idx) + ":" + "#" + lexical_cast(name_and_type->descriptor_idx);
    ss << "// " + utf8_constant(name_and_type->name_idx) + ":" + utf8_constant(name_and_type->descriptor_idx);
    break;
  }

  case cp_tag::FIELDREF: {
    auto *fieldref = reinterpret_cast<const cp_fieldref_info_t *>(info);
    ss << "Fieldref";
    ss << std::setw(15) << std::left;
    ss << "#" + lexical_cast(fieldref->class_idx) + ".#" + lexical_cast(fieldref->name_and_type_idx);
    ss << "// " + describe_class(fieldref->class_idx) + "." + describe_name_and_type(fieldref->name_and_type_idx);
    break;
  }

  case cp_tag::STRING: {
    auto *string_info = reinterpret_cast<const cp_string_info_t *>(info);
    ss << "String";
    ss << std::setw(15) << std::left;
    ss << "#" + lexical_cast(string_info->string_idx);
    ss << "// " + utf8_constant(string_info->string_idx);
    break;
  }

  default:
    ss << "UNKNOWN_TAG: " << +uint8_t(info->tag);
  }

  return ss.str();
}
