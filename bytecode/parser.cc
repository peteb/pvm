#include "parser.h"

#include <iostream>
#include <iomanip>
#include <stdexcept>

#include "class_file.h"
#include "utils.h"

using bytecode::instruction;
using bytecode::class_file;
using bytecode::f2h;

bytecode::parser::parser() {
  state = &parser::parse_header;
}

void bytecode::parser::parse(const char *data, size_t size) {
  buffer.insert(buffer.end(), data, data + size);
  while (try_parse());
}

bool bytecode::parser::try_parse() {
  if (buffer.empty()) {
    return false;
  }

  const ptrdiff_t start = cursor;
  if (!state(this)) {
    // rewind
    cursor = start;
    return false;
  }

  return true;
}

bool bytecode::parser::parse_header() {
  if (auto *hdr = read<header_t>()) {
    uint32_t magic = f2h(hdr->magic);

    if (magic != 0xCAFEBABE) {
      throw std::runtime_error("Invalid header magic");
    }

    object = std::make_shared<class_file>();
    hdr->magic = magic;
    hdr->minor_version = f2h(hdr->minor_version);
    hdr->major_version = f2h(hdr->major_version);
    std::cout << "minor version: " << hdr->minor_version << " major version: " << hdr->major_version << std::endl;

    object->header = hdr;
    state = &parser::parse_cp;

    return true;
  }
  else {
    return false;
  }
}

bool bytecode::parser::parse_cp() {
  if (auto *num_cp_items_f = read<uint16_t>()) {
    uint16_t num_cp_items = f2h(*num_cp_items_f);
    object->constant_pool.clear();
    object->constant_pool.reserve(num_cp_items);
    cp_items_left = num_cp_items - 1;
    state = &parser::parse_cp_item;

    return true;
  }
  else {
    return false;
  }
}

bool bytecode::parser::parse_cp_item() {
  if (cp_items_left == 0) {
    state = &parser::parse_midriff;
    return true;
  }

  if (auto *info = peek<cp_info_t>()) {
    std::size_t size;

    // Don't manipulate the buffer before the read further down
    switch (info->tag) {
    case cp_tag::UTF8: {
      auto *utf8_info = peek<cp_utf8_info_t>();
      size = sizeof(cp_utf8_info_t::tag)
        + sizeof(cp_utf8_info_t::length)
        + f2h(utf8_info->length);
      break;
    }

    case cp_tag::INTEGER:       size = sizeof(cp_integer_info_t); break;
    case cp_tag::STRING:        size = sizeof(cp_string_info_t); break;
    case cp_tag::METHODREF:     size = sizeof(cp_methodref_info_t); break;
    case cp_tag::FIELDREF:      size = sizeof(cp_fieldref_info_t); break;
    case cp_tag::CLASS:         size = sizeof(cp_class_info_t); break;
    case cp_tag::NAME_AND_TYPE: size = sizeof(cp_name_and_type_info_t); break;

    default:
      std::cerr << "invalid cp_tag: " << +(uint8_t)info->tag << " at cursor " << cursor << std::endl;
      return false;
    }

    if (auto *info = read<cp_info_t>(size)) {
      // Fixup endian
      if (info->tag == cp_tag::UTF8) {
        auto utf8_info = reinterpret_cast<cp_utf8_info_t *>(info);
        utf8_info->length = f2h(utf8_info->length);
      }

      object->constant_pool.push_back(info);
      --cp_items_left;

      return true;
    }
  }

  return false;
}

bool bytecode::parser::parse_midriff() {
  if (uint16_t *values = read<uint16_t>(sizeof(uint16_t) * 3)) {
    object->access_flags = f2h(values[0]);
    object->this_class = f2h(values[1]);
    object->super_class = f2h(values[2]);
    state = &parser::parse_if;
    return true;
  }
  else {
    return false;
  }
}

bool bytecode::parser::parse_if() {
  if (uint16_t *num_if_items_f = read<uint16_t>()) {
    uint16_t num_if_items = f2h(*num_if_items_f);

    if (uint16_t *interfaces = read<uint16_t>(sizeof(uint16_t) * num_if_items)) {
      for (uint16_t *item = interfaces; item != interfaces + num_if_items; ++item) {
        *item = f2h(*item);
      }

      object->num_interfaces = num_if_items;
      object->interfaces = interfaces;
      state = &parser::parse_fields;
      return true;
    }
  }

  return false;
}

bool bytecode::parser::parse_fields() {
  if (uint16_t *num_fields_f = read<uint16_t>()) {
    field_items_left = f2h(*num_fields_f);
    state = &parser::parse_field_item;
    return true;
  }

  return false;
}

bool bytecode::parser::parse_field_item() {
  if (field_items_left == 0) {
    state = &parser::parse_methods;
    return true;
  }

  if (auto *info_f = read<field_info_t>()) {
    uint16_t num_attributes = f2h(info_f->attributes_count);

    field_info field;
    field.access_flags = f2h(info_f->access_flags);
    field.name_idx = f2h(info_f->name_idx);
    field.descriptor_idx = f2h(info_f->descriptor_idx);

    for (int i = 0; i < num_attributes; ++i) {
      if (auto *attribute = read<attribute_info_t>()) {
        uint32_t attribute_length = f2h(attribute->length);

        if (read<char>(attribute_length)) {
          field.attributes.push_back(attribute);
        }
        else {
          return false;
        }
      }
      else {
        // This is quite wasteful but I wanted to keep the state handling simple
        return false;
      }
    }

    // At this point we've managed to read all the attributes and their data
    for (auto *attribute : field.attributes) {
      attribute->name_idx = f2h(attribute->name_idx);
      attribute->length = f2h(attribute->length);
    }

    --field_items_left;
    object->fields.push_back(field);

    return true;
  }

  return false;
}

bool bytecode::parser::parse_methods() {
  if (uint16_t *num_methods_f = read<uint16_t>()) {
    method_items_left = f2h(*num_methods_f);
    state = &parser::parse_method_items;
    return true;
  }

  return false;
}

bool bytecode::parser::parse_method_items() {
  if (method_items_left == 0) {
    state = &parser::parse_attributes;
    return true;
  }

  // This code is exactly like field_info, can we refactor a bit please
  if (auto *method_f = read<method_info_t>()) {
    uint16_t num_attributes = f2h(method_f->attributes_count);

    method_info method;
    method.access_flags = f2h(method_f->access_flags);
    method.name_idx = f2h(method_f->name_idx);
    method.descriptor_idx = f2h(method_f->descriptor_idx);

    for (int i = 0; i < num_attributes; ++i) {
      if (auto *attribute = read<attribute_info_t>()) {
        uint32_t attribute_length = f2h(attribute->length);

        if (read<char>(attribute_length)) {
          method.attributes.push_back(attribute);
        }
        else {
          return false;
        }
      }
      else {
        // This is quite wasteful but I wanted to keep the state handling simple
        return false;
      }
    }

    // At this point we've managed to read all the attributes and their data
    for (auto *attribute : method.attributes) {
      attribute->name_idx = f2h(attribute->name_idx);
      attribute->length = f2h(attribute->length);
    }

    --method_items_left;
    object->methods.push_back(method);

    return true;
  }

  return false;
}

bool bytecode::parser::parse_attributes() {
  if (uint16_t *num_attributes_f = read<uint16_t>()) {
    attribute_items_left = f2h(*num_attributes_f);
    state = &parser::parse_attribute_items;
    return true;
  }

  return false;
}

bool bytecode::parser::parse_attribute_items() {
  if (attribute_items_left == 0) {
    return true;
  }

  auto *attribute = read<attribute_info_t>();
  if (!attribute) {
    return false;
  }

  uint16_t length = f2h(attribute->length);
  if (!read<char>(length)) {
    return false;
  }

  attribute->name_idx = f2h(attribute->name_idx);
  attribute->length = f2h(attribute->length);
  object->attributes.push_back(attribute);
  --attribute_items_left;

  return true;
}

std::shared_ptr<bytecode::class_file> bytecode::parser::release() {
  auto obj = object;
  obj->buffer = std::move(buffer);
  object = nullptr;
  return obj;
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
