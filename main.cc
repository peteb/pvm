#include <iostream>
#include <string>
#include <cassert>

#include "bytecode/parser.h"
#include "bytecode/class_file.h"
#include "bytecode/method.h"

#include "runtime/context.h"

int main() {
  bytecode::parser p;

  char buffer[1024];
  auto inbuf = std::cin.rdbuf();
  while (auto sz = inbuf->sgetn(buffer, 1024)) {
    p.write(buffer, sz);
  }

  auto class_data = p.parse();
  auto *method = class_data->find_method("main", "([Ljava/lang/String;)V");
  assert(method && "couldn't find main method");

  runtime::context context(class_data.get());
  context.execute(method);
}
