#include "parser.h"

#include <iostream>
#include <iomanip>
#include <stdexcept>

#include "class_file.h"
#include "utils.h"


using bytecode::class_file;
using bytecode::f2h;

bytecode::parser::parser() {
  object = std::make_shared<class_file>();
  push_state(new header_state);
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
  if (!state.top()->parse(*this)) {
    // rewind
    std::cout << "rewind" << std::endl;
    cursor = start;
    return false;
  }

  destruct.clear();
  return true;
}

/*bool bytecode::parser::parse_header() {
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
  }*/

bool bytecode::parser::header_state::parse(bytecode::parser &in) {
  header_t *hdr = in.read<header_t>();
  if (!hdr) {
    return false;
  }

  uint32_t magic = f2h(hdr->magic);
  if (magic != 0xCAFEBABE) {
    throw std::runtime_error("Invalid header magic");
  }

  in.object->magic = magic;
  in.object->minor_version = f2h(hdr->minor_version);
  in.object->major_version = f2h(hdr->major_version);
  std::cout << "minor version: " << in.object->minor_version << " major version: " << in.object->major_version << std::endl;

  in.replace_state(new constant_pool_state);
  return true;
}

bool bytecode::parser::constant_pool_state::parse(bytecode::parser &in) {
  if (!parsed_header) {
    uint16_t *num_items_f = in.read<uint16_t>();
    if (!num_items_f) {
      return false;
    }

    items_left = f2h(*num_items_f) - 1;
    std::cout << "items: " << items_left << std::endl;
    in.object->constant_pool.clear();
    in.object->constant_pool.reserve(items_left);
    parsed_header = true;
  }

  if (items_left--) {
    // It's a bit wasteful to allocate a new state for every item,
    // but it simplifies the code.
    in.push_state(new constant_pool_item_state);
  }
  else {
    // We're done
    in.replace_state(new midriff_state);
  }

  return true;
}

bool bytecode::parser::constant_pool_item_state::parse(bytecode::parser &in) {
  auto *info = in.peek<cp_info_t>();
  if (!info) {
    return false;
  }

  std::size_t size;

  // Don't manipulate the buffer before the read further down
  switch (info->tag) {
  case cp_tag::UTF8: {
    auto *utf8_info = in.peek<cp_utf8_info_t>();
    size = sizeof(cp_utf8_info_t) + f2h(utf8_info->length);
    break;
  }

  case cp_tag::INTEGER:       size = sizeof(cp_integer_info_t); break;
  case cp_tag::STRING:        size = sizeof(cp_string_info_t); break;
  case cp_tag::METHODREF:     size = sizeof(cp_methodref_info_t); break;
  case cp_tag::FIELDREF:      size = sizeof(cp_fieldref_info_t); break;
  case cp_tag::CLASS:         size = sizeof(cp_class_info_t); break;
  case cp_tag::NAME_AND_TYPE: size = sizeof(cp_name_and_type_info_t); break;

  default:
    std::cerr << "invalid cp_tag: " << +(uint8_t)info->tag << std::endl;
    return false;
  }

  // Let's actually read the data; we know the size.
  info = in.read<cp_info_t>(size);
  if (!info) {
    return false;
  }

  // Fixup endian
  if (info->tag == cp_tag::UTF8) {
    auto utf8_info = reinterpret_cast<cp_utf8_info_t *>(info);
    utf8_info->length = f2h(utf8_info->length);
  }

  in.object->constant_pool.push_back(info);
  in.pop_state();

  return true;
}

bool bytecode::parser::midriff_state::parse(bytecode::parser &in) {
  auto *values = in.read<uint16_t>(sizeof(uint16_t) * 3);
  if (!values) {
    return false;
  }

  in.object->access_flags = f2h(values[0]);
  in.object->this_class = f2h(values[1]);
  in.object->super_class = f2h(values[2]);
  in.replace_state(new interfaces_state);

  return true;
}

bool bytecode::parser::interfaces_state::parse(bytecode::parser &in) {
  auto *num_items_f = in.read<uint16_t>();
  if (!num_items_f) {
    return false;
  }

  uint16_t num_items = f2h(*num_items_f);
  auto *items_f = in.read<uint16_t>(sizeof(uint16_t) * num_items);
  if (!items_f) {
    return false;
  }

  for (int i = 0; i < num_items; ++i) {
    items_f[i] = f2h(items_f[i]);
  }

  in.object->num_interfaces = num_items;
  in.object->interfaces = items_f;
  in.replace_state(new fields_state(&in.object->fields));

  return true;
}

bool bytecode::parser::fields_state::parse(bytecode::parser &in) {
  if (!parsed_header) {
    uint16_t *num_items_f = in.read<uint16_t>();
    if (!num_items_f) {
      return false;
    }

    items_left = f2h(*num_items_f);
    std::cout << "fields: " << items_left << std::endl;
    receiver->clear();
    receiver->reserve(items_left);
    parsed_header = true;
  }

  if (items_left--) {
    // It's a bit wasteful to allocate a new state for every item,
    // but it simplifies the code.
    in.push_state(new field_item_state(receiver));
  }
  else {
    // We're done
    //in.replace_state(new methods_state);
    std::cout << "Read fields" << std::endl;
  }

  return true;
}

bool bytecode::parser::field_item_state::parse(bytecode::parser &in) {
  if (!parsed_header) {
    auto *info_f = in.read<field_info_t>();
    if (!info_f) {
      return false;
    }

    current_field.access_flags = f2h(info_f->access_flags);
    current_field.name_idx = f2h(info_f->name_idx);
    current_field.descriptor_idx = f2h(info_f->descriptor_idx);
    attributes_left = f2h(info_f->attributes_count);
  }

  // TODO: can be simplified by pulling out the "parsed_header" into something
  //       like a callback for when a state bubbled up

  if (attributes_left--) {
    in.push_state(new attribute_items_state(attributes_left, &current_field.attributes));
  }
  else {
    receiver->push_back(current_field);
    in.pop_state();
  }

  return true;
}

bool bytecode::parser::attribute_items_state::parse(bytecode::parser &in) {
  auto *info_f = in.peek<attribute_info_t>();
  if (!info_f) {
    return false;
  }

  uint32_t length = f2h(info_f->length);

  info_f = in.read<attribute_info_t>(sizeof(attribute_info_t) + length);
  if (!info_f) {
    return false;
  }

  info_f->length = length;
  info_f->name_idx = f2h(info_f->name_idx);
  receiver->push_back(info_f);
  in.pop_state();

  return true;
}

/*


bool bytecode::parser::parse_fields() {
  if (uint16_t *num_fields_f = read<uint16_t>()) {
    field_items_left = f2h(*num_fields_f);
    state = &parser::parse_field_item;
    return true;
  }

  return false;
}

bool bytecode::parser::parse_field_item() {
  if (field_items_left == method_items_left == 0) {
    state = &parser::parse_attributes;
    return true;
  }
  else if (field_items_left == 0) {
    state = &parser::parse_methods;
    return true;
  }

  auto *info_f = read<field_info_t>();
  if (!info_f) {
    return false;
  }

  uint16_t num_attributes = f2h(info_f->attributes_count);
  constructing_field_info.access_flags = f2h(info_f->access_flags);
  constructing_field_info.name_idx = f2h(info_f->name_idx);
  constructing_field_info.descriptor_idx = f2h(info_f->descriptor_idx);
  constructing_field_info.attributes.clear();
  constructing_field_info.attributes.reserve(num_attributes);
  attribute_items_left = num_attributes;

  state = &parser::parse_attribute_items;

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
    if (fields_items_left == method_items_left == 0) {
      // These were for the class
    }
    else if (fields_items_left == 0) {
      state = &parser::parse_field_item;
      return true;
    }
    else if (method_items_left == 0) {
      state = &parser::parse
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
*/
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

void bytecode::parser::replace_state(parser_state *new_state) {
  pop_state();
  push_state(new_state);
}

void bytecode::parser::push_state(parser_state *new_state) {
  state.push(std::unique_ptr<parser_state>(new_state));
}

void bytecode::parser::pop_state() {
  if (state.empty()) {
    throw std::runtime_error("State stack is empty, can't pop");
  }

  destruct.emplace_back(std::move(state.top()));
  state.pop();
}
