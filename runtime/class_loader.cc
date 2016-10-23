#include "class_loader.h"

#include "bytecode/parser.h"
#include "bytecode/class_file.h"

#include "pclass.h"
#include "utils.h"

#include <iostream>
#include <fstream>
#include <cassert>
#include <memory>

runtime::pclass *runtime::class_loader::resolve(const char *name) {
  auto it = loaded_classes.find(name);
  if (it != loaded_classes.end()) {
    return it->second;
  }

  runtime::pclass *klass = load(name);
  loaded_classes.insert({std::string(name), klass});
  return klass;
}

runtime::pclass *runtime::class_loader::load(const char *name) {
  const std::string filename = std::string(name) + ".class";

  std::cout << "[" << name << "] reading from " << filename << "..." << std::endl;
  std::ifstream file(filename.c_str(), std::ios::binary);
  assert(file.is_open() && "failed to open file");

  bytecode::parser p;
  char buffer[1024];
  auto inbuf = file.rdbuf();

  while (auto sz = inbuf->sgetn(buffer, 1024)) {
    p.write(buffer, sz);
  }

  file.close();

  auto klass = make_unique<runtime::pclass>(p.parse(), this);
  std::cout << "[" << name << "] read and parsed" << std::endl;

  return klass.release();
}
