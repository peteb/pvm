#include "parser.h"

#include <iostream>
#include <iomanip>
#include <stdexcept>

#include "class_file.h"
#include "utils.h"

using bytecode::class_file;
using bytecode::f2h;

void bytecode::parser::write(const char *data, std::size_t bytes) {
  buffer.insert(buffer.end(), data, data + bytes);
}

std::shared_ptr<class_file> bytecode::parser::parse() {
  object = std::make_shared<class_file>();
  cursor = 0;

  header();
  constant_pool();

  object->access_flags = f2h(*read<uint16_t>());
  object->this_class = f2h(*read<uint16_t>());
  object->super_class = f2h(*read<uint16_t>());

  interfaces();
  fields();
  methods();
  attributes();

  if (cursor != buffer.size()) {
    throw std::runtime_error("failed to consume all input");
  }

  auto object_tmp = object;
  object_tmp->buffer = std::move(buffer);
  object = nullptr;

  return object_tmp;
}

void bytecode::parser::header() {
  auto *hdr_f = read<header_t>();
  object->magic = f2h(hdr_f->magic);
  object->minor_version = f2h(hdr_f->minor_version);
  object->major_version = f2h(hdr_f->major_version);

  if (object->magic != 0xCAFEBABE) {
    throw std::runtime_error("invalid header magic");
  }
}

void bytecode::parser::constant_pool() {
  uint16_t constant_pool_count = f2h(*read<uint16_t>());
  object->constant_pool.clear();
  object->constant_pool.reserve(constant_pool_count);

  for (int i = 0; i < constant_pool_count - 1; ++i) {
    object->constant_pool.push_back(constant_pool_item());
  }
}

bytecode::cp_info_t *bytecode::parser::constant_pool_item() {
  auto *info = peek<cp_info_t>();
  std::size_t size;

  switch (info->tag) {
  case cp_tag::UTF8: {
    auto *utf8_info = read<cp_utf8_info_t>();
    utf8_info->length = f2h(utf8_info->length);
    read<char>(utf8_info->length);
    return reinterpret_cast<cp_info_t *>(utf8_info);
  }

  case cp_tag::CLASS: {
    auto *class_info = read<cp_class_info_t>();
    class_info->name_idx = f2h(class_info->name_idx);
    return reinterpret_cast<cp_info_t *>(class_info);
  }

  case cp_tag::INTEGER:       size = sizeof(cp_integer_info_t); break;
  case cp_tag::STRING: {
    auto *string_info = read<cp_string_info_t>();
    string_info->string_idx = f2h(string_info->string_idx);
    return reinterpret_cast<cp_info_t *>(string_info);
  }

  case cp_tag::METHODREF: {
    auto *methodref_info = read<cp_methodref_info_t>();
    methodref_info->class_idx = f2h(methodref_info->class_idx);
    methodref_info->name_and_type_idx = f2h(methodref_info->name_and_type_idx);
    return reinterpret_cast<cp_info_t *>(methodref_info);
  }

  case cp_tag::FIELDREF: {
    auto *fieldref = read<cp_fieldref_info_t>();
    fieldref->class_idx = f2h(fieldref->class_idx);
    fieldref->name_and_type_idx = f2h(fieldref->name_and_type_idx);
    return reinterpret_cast<cp_info_t *>(fieldref);
  }

  case cp_tag::NAME_AND_TYPE: {
    auto *name_and_type_info = read<cp_name_and_type_info_t>();
    name_and_type_info->name_idx = f2h(name_and_type_info->name_idx);
    name_and_type_info->descriptor_idx = f2h(name_and_type_info->descriptor_idx);
    return reinterpret_cast<cp_info_t *>(name_and_type_info);
  }

  default:
    throw std::runtime_error("invalid cp_tag");
  }

  return read<cp_info_t>(size);
}

void bytecode::parser::interfaces() {
  uint16_t interface_count = f2h(*read<uint16_t>());
  uint16_t *interfaces = read<uint16_t>(sizeof(uint16_t) * interface_count);

  for (int i = 0; i < interface_count; ++i) {
    interfaces[i] = f2h(interfaces[i]);
  }

  object->num_interfaces = interface_count;
  object->interfaces = interfaces;
}

void bytecode::parser::fields() {
  uint16_t field_count = f2h(*read<uint16_t>());
  object->fields.clear();
  object->fields.reserve(field_count);

  for (int i = 0; i < field_count; ++i) {
    object->fields.push_back(field());
  }
}

bytecode::field bytecode::parser::field() {
  auto *info = read<field_info_t>();

  bytecode::field new_field;
  new_field.access_flags = f2h(info->access_flags);
  new_field.name_idx = f2h(info->name_idx);
  new_field.descriptor_idx = f2h(info->descriptor_idx);

  uint16_t attributes_count = f2h(info->attributes_count);
  new_field.attributes.reserve(attributes_count);

  for (int i = 0; i < attributes_count; ++i) {
    new_field.attributes.push_back(attribute());
  }

  return new_field;
}

void bytecode::parser::methods() {
  uint16_t field_count = f2h(*read<uint16_t>());
  object->methods.clear();
  object->methods.reserve(field_count);

  for (int i = 0; i < field_count; ++i) {
    object->methods.push_back(field());
  }
}

bytecode::attribute_info_t *bytecode::parser::attribute() {
  auto *info = read<attribute_info_t>();
  info->name_idx = f2h(info->name_idx);
  info->length = f2h(info->length);
  read<char>(info->length);

  return info;
}

void bytecode::parser::attributes() {
  uint16_t attribute_count = f2h(*read<uint16_t>());
  object->attributes.clear();
  object->attributes.reserve(attribute_count);

  for (int i = 0; i < attribute_count; ++i) {
    object->attributes.push_back(attribute());
  }
}

void bytecode::parser::dump_buffer() {
  int pos = 0;
  auto format(std::cout.flags());

  for (char byte : buffer) {
    std::cout << std::setfill('0') << std::setw(2) << std::hex << +byte;

    if (pos == 15) {
      std::cout << std::endl;
    }
    else if (pos % 4 == 3) {
      std::cout << "  ";
    }
    else {
      std::cout << " ";
    }

    ++pos;
  }

  std::cout.flags(format);
}
