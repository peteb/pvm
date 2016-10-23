// -*- c++ -*-

#ifndef _RUNTIME_PMETHOD_H
#define _RUNTIME_PMETHOD_H

#include "pobject.h"
#include "pvalue.h"

#include <vector>

namespace bytecode {
  class method;
}

namespace runtime {
  class pclass;

  class pmethod : public pobject {
  public:
    pmethod(const bytecode::method *info,
            runtime::pclass *owner)
      : info(info)
      , owner(owner)
    {
    }

    void execute(runtime::pvalue self, std::vector<runtime::pvalue> arguments);

  private:
    const bytecode::method *info;
    runtime::pclass *owner;
  };
}

#endif
