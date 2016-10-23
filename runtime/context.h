// -*- c++ -*-

#ifndef _RUNTIME_CONTEXT_H
#define _RUNTIME_CONTEXT_H

#include <stack>

#include "bytecode/method.h"
#include "pvalue.h"

namespace bytecode {
  class class_file;
}

namespace runtime {
  class pclass;

  class context {
  public:
    context(runtime::pclass *klass,
            runtime::pvalue self,
            std::vector<runtime::pvalue> args)
      : klass(klass)
      , self(self)
      , args(args)
    {
    }

    void execute(const bytecode::method *method);

    OP(getstatic);
    OP(ldc);
    OP(invokevirtual);
    OP(returnn);

  private:
    runtime::pclass *klass;
    runtime::pvalue self;
    std::stack<pvalue> s;
    std::vector<pvalue> args;
  };



}

#endif
