// -*- c++ -*-

#ifndef _RUNTIME_CLASS_LOADER_H
#define _RUNTIME_CLASS_LOADER_H

#include <unordered_map>
#include <string>

namespace runtime {
  class pclass;

  class class_loader {
  public:
    pclass *resolve(const char *name);

  private:
    pclass *load(const char *name);

    std::unordered_map<std::string, pclass*> loaded_classes;
  };

}

#endif
