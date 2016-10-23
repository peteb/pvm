# -*- cmake -*-

add_library(runtime context.cc class_loader.cc pclass.cc pmethod.cc)
target_link_libraries(runtime bytecode)
