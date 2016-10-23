#include "context.h"

#include <iostream>

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
    method->execute(opcode_handlers, this);
  }

  OP(context::getstatic) {
    uint16_t idx = operands[0] << 8 | operands[1];
    const auto *fieldref = klass->constant<bytecode::cp_fieldref_info_t>(idx);
    std::cout << bytecode::class_file_str::from(klass)->describe(fieldref) << std::endl;

    /*const cp_name_and_type_info_t *nat = klass->constant<cp_name_and_type_info_t>(fieldref->name_and_type_idx);
      const pclass *receiver = klass->resolve_class(fieldref->class_idx);
      receiver->fetch_static(klass->utf8_constant(receiver->name_idx));
      Check that the class matches descriptor.
    */
    pvalue value;
    value.type = val_type::INTEGER;
    value.integer = 123456;
    s.push(value);
  }

  OP(context::ldc) {

  }

  OP(context::invokevirtual) {

  }

  OP(context::returnn) {

  }

}
