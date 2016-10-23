#include "context.h"

#include <iostream>
#include <algorithm>

#include "pclass.h"
#include "class_loader.h"
#include "pmethod.h"
#include "pstring.h"

#include "bytecode/file_format.h"
#include "bytecode/class_file.h"
#include "bytecode/class_file_str.h"

namespace runtime {
  static bytecode::opcode_handler_map<runtime::context> opcode_handlers {
    {0xb2, {&context::getstatic, 2}},
    {0x12, {&context::ldc, 1}},
    {0xb6, {&context::invokevirtual, 2}},
    {0xb1, {&context::returnn, 0}}
  };

  void context::execute(const bytecode::method *method) {
    if (method->access_flags & bytecode::cp_access_flags::NATIVE) {
      std::string function_name = klass->name() + "_" + method->name();
      std::replace(function_name.begin(), function_name.end(), '/', '_');
      std::cout << "invoke native method: " << function_name << std::endl;

      // TODO: don't hardcode native functions
      if (function_name == "java_io_PrintStream_println") {
        std::cout << args.at(0).as<runtime::pstring>()->str << std::endl;
      }
    }
    else {
      method->execute(opcode_handlers, this);
    }
  }

  OP(context::getstatic) {
    uint16_t idx = operands[0] << 8 | operands[1];
    const auto *fieldref = klass->info()->constant<bytecode::cp_fieldref_info_t>(idx);
    const auto *nat = klass->info()->constant<bytecode::cp_name_and_type_info_t>(fieldref->name_and_type_idx);

    pclass *target = klass->resolve_constant(fieldref->class_idx).as<pclass>();

    s.push(target->fetch_static_field(klass->info()->utf8_constant(nat->name_idx).c_str()));
    // TODO: Check that the static matches descriptor.
  }

  OP(context::ldc) {
    uint8_t idx = operands[0];
    s.push(klass->resolve_constant(idx));
  }

  OP(context::invokevirtual) {
    uint16_t idx = operands[0] << 8 | operands[1];
    pmethod *method = klass->resolve_constant(idx).as<pmethod>();

    pvalue arg = s.top();
    s.pop();
    pvalue self = s.top();
    s.pop();
    // TODO: get the correct number of arguments
    method->execute(self, {arg});
  }

  OP(context::returnn) {

  }

}
