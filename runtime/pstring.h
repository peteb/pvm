// -* c++ -*-

#ifndef _RUNTIME_PSTRING_H
#define _RUNTIME_PSTRING_H

#include "pobject.h"

namespace runtime {
  class pstring : public pobject {
  public:
    pstring(std::string str)
      : str(str)
    {
    }

    std::string str;
  };
}

#endif
