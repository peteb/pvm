// -*- c++ -*-

#ifndef _RUNTIME_PVALUE_H
#define _RUNTIME_PVALUE_H

#include <cassert>
#include <cstdint>

namespace runtime {
  enum class val_type : uint8_t {
    INTEGER,
    REFERENCE
  };

  class pobject;

  struct pvalue {
    val_type type;

    union {
      int integer;
      pobject *reference;
    };

    template<typename T>
    T *as() {
      assert(type == runtime::val_type::REFERENCE && "trying to downcast non-reference value type");
      return dynamic_cast<T *>(reference);
    }

    static pvalue ref(pobject *object) {
      pvalue value;
      value.type = val_type::REFERENCE;
      value.reference = object;
      return value;
    }
  };
}

#endif
