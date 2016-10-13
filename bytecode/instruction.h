// -*- c++ -*-

#ifndef _BYTECODE_INSTRUCTION_H
#define _BYTECODE_INSTRUCTION_H

#include <string>

namespace bytecode {
  class instruction {
  public:

    operator std::string() const {
      return "instruction";
    }
  };


}


#endif // !_BYTECODE_INSTRUCTION_H
