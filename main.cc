#include <iostream>
#include <string>
#include <cassert>

#include "bytecode/parser.h"
#include "bytecode/class_file.h"
#include "bytecode/method.h"

#include "runtime/context.h"
#include "runtime/pclass.h"
#include "runtime/class_loader.h"
#include "runtime/pmethod.h"

int main() {
  runtime::class_loader cl;
  runtime::pclass *klass = cl.resolve("HelloWorld");

  auto main = klass->fetch_method("main", "([Ljava/lang/String;)V");
  main.as<runtime::pmethod>()->execute(runtime::pvalue::ref(nullptr), {});
}
