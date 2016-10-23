#include "pmethod.h"
#include "pclass.h"

#include "runtime/context.h"

void runtime::pmethod::execute(runtime::pvalue self, std::vector<runtime::pvalue> arguments) {
  runtime::context context(owner, self, arguments);
  context.execute(info);
}
