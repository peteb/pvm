// -*- c++ -*-

#ifndef _RUNTIME_CONTEXT_H
#define _RUNTIME_CONTEXT_H

#include <stack>

#include "bytecode/method.h"
#include "pvalue.h"

namespace runtime {
  class context {
  public:
    context(bytecode::class_file *klass)
      : klass(klass)
    {
    }

    void execute(const bytecode::method *method);

    OP(getstatic);
    OP(ldc);
    OP(invokevirtual);
    OP(returnn);

  private:
    bytecode::class_file *klass;

    std::stack<pvalue> s;
  };



}

#endif
