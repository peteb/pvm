// -*- c++ -*-

#ifndef _BYTECODE_METHOD_H
#define _BYTECODE_METHOD_H

#include <unordered_map>
#include <functional>
#include <cassert>

#include "field.h"
#include "utils.h"

#define OP(name) void name(const uint8_t *operands, std::size_t num_operands)

namespace {
  template<typename T>
  T read(const void *&pos, std::size_t bytes = sizeof(T)) {
    T ret = *static_cast<const T *>(pos);
    pos = static_cast<const char *>(pos) + bytes;
    return ret;
  }
}


namespace bytecode {
  class class_file;

  template<typename T>
  struct opcode {
    std::function<void(T*, const uint8_t*, std::size_t)> handler;
    int num_operands;
  };

  template<typename T>
  using opcode_handler_map = std::unordered_map<int, opcode<T>>;

  class method : public field {
  public:
    method(class_file *class_info);

    // TODO: don't define the function here
    template<typename T>
    void execute(const opcode_handler_map<T> &handlers, T *context) const {
      auto *code_attr = find_attribute("Code");
      assert(code_attr && "failed to find Code attribute for method");

      const void *pos = code_attr->info;
      uint16_t max_stack = f2h(read<uint16_t>(pos));
      uint16_t max_locals = f2h(read<uint16_t>(pos));
      uint32_t code_length = f2h(read<uint32_t>(pos));

      const uint8_t *code = static_cast<const uint8_t *>(pos);
      const uint8_t *code_end = code + code_length;

      while (pos < code_end) {
        uint8_t opcode = read<uint8_t>(pos);
        auto iter = handlers.find(opcode);
        if (iter == handlers.end()) {
          throw std::runtime_error("invalid opcode " + lexical_cast(+opcode));
        }

        const uint8_t *operands = static_cast<const uint8_t *>(pos);
        read<uint8_t>(pos, iter->second.num_operands);
        iter->second.handler(context, operands, iter->second.num_operands);
      }
    }
  };
}

#endif
