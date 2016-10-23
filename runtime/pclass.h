// -*- c++ -*-

#ifndef _RUNTIME_PCLASS_H
#define _RUNTIME_PCLASS_H

#include <memory>
#include <unordered_map>
#include <string>

#include "pobject.h"
#include "pvalue.h"

namespace bytecode {
  class class_file;
  class method;
}

namespace runtime {
  class pmethod;
  class class_loader;

  class pclass : public pobject {
  public:
    pclass(std::shared_ptr<bytecode::class_file> class_info,
           runtime::class_loader *class_loader)
      : class_info(class_info)
      , class_loader(class_loader)
    {
    }

    pvalue fetch_static_field(const char *name) const;
    pvalue fetch_method(const std::string &name, const std::string &descriptor);
    pvalue resolve_constant(uint16_t idx);

    std::string name() const;

    const bytecode::class_file *info() const {
      return class_info.get();
    }

    runtime::class_loader *loader() const {
      return class_loader;
    }

  private:
    std::shared_ptr<bytecode::class_file> class_info;
    runtime::class_loader *class_loader;
    std::unordered_map<const bytecode::method*, runtime::pmethod*> resolved_methods;
    std::unordered_map<uint16_t, pvalue> resolved_constants;
  };
}

#endif
