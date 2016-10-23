// -*- c++ -*-

#ifndef _RUNTIME_PVALUE_H
#define _RUNTIME_PVALUE_H

namespace runtime {
  enum class val_type : uint8_t {
    INTEGER,
    REFERENCE
  };

  struct pvalue {
    val_type type;

    union {
      int integer;
      void *reference;
    };
  };
}

#endif
