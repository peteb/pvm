// -*- c++ -*-

#ifndef _BYTECODE_UTILS_H
#define _BYTECODE_UTILS_H

#include <cstddef>
#include <type_traits>
#include <arpa/inet.h>

namespace bytecode {
  inline uint32_t f2h(uint32_t v) {
    return ntohl(v);
  }

  inline uint16_t f2h(uint16_t v) {
    return ntohs(v);
  }
}

#endif // !_BYTECODE_UTILS_H
