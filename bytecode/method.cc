#include "method.h"

#include <cassert>
#include <iostream>
#include <unordered_map>
#include <functional>
#include <stdexcept>
#include <stack>

#include "utils.h"
#include "class_file.h"
#include "class_file_str.h"
#include "types.h"

using namespace bytecode;


bytecode::method::method(class_file *class_info)
  : field(class_info)
{
}
