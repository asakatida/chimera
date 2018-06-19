cmake_minimum_required(VERSION 3.0)
project(
  chimera
  VERSION 1.0.0
  LANGUAGES CXX)

add_compile_options(
  -Wno-c++11-extensions
  -Wno-c++98-compat-pedantic
  -Wno-date-time
  -Wno-padded)

add_library(
  chimera_grammar
  library/asdl/asdl.hpp
  library/grammar/rules/action.hpp
  library/grammar/rules/captures.hpp
  library/grammar/rules/control.hpp
  library/grammar/rules/rules.hpp
  library/grammar/rules/set.hpp
  library/grammar/rules/stack.hpp
  library/grammar/expr.hpp
  library/grammar/exprfwd.hpp
  library/grammar/flags.hpp
  library/grammar/grammar.hpp
  library/grammar/identifier.hpp
  library/grammar/input.hpp
  library/grammar/keys.hpp
  library/grammar/number.hpp
  library/grammar/oper.hpp
  library/grammar/parse.hpp
  library/grammar/rules.hpp
  library/grammar/stmt.hpp
  library/grammar/string.hpp
  library/grammar/utf8_id_continue.hpp
  library/grammar/utf8_id_start.hpp
  library/grammar/utf8_space.hpp
  library/grammar/whitespace.hpp
  library/virtual_machine/parse.cpp)

target_include_directories(chimera_grammar PUBLIC library)
target_include_directories(
  chimera_grammar
  SYSTEM PRIVATE
  external/GSL/include
  external/metal/include
  external/PEGTL/include)

add_library(
  chimera_object_number
  library/object/number/add.cpp
  library/object/number/add.hpp
  library/object/number/and.cpp
  library/object/number/and.hpp
  library/object/number/compare.cpp
  library/object/number/compare.hpp
  library/object/number/debug.cpp
  library/object/number/debug.hpp
  library/object/number/details.cpp
  library/object/number/details.hpp
  library/object/number/div.cpp
  library/object/number/div.hpp
  library/object/number/floor_div.cpp
  library/object/number/floor_div.hpp
  library/object/number/invert.cpp
  library/object/number/invert.hpp
  library/object/number/left_shift.cpp
  library/object/number/left_shift.hpp
  library/object/number/less.cpp
  library/object/number/less.hpp
  library/object/number/mod.cpp
  library/object/number/mod.hpp
  library/object/number/mult.cpp
  library/object/number/mult.hpp
  library/object/number/negative.cpp
  library/object/number/negative.hpp
  library/object/number/number.cpp
  library/object/number/number.hpp
  library/object/number/or.cpp
  library/object/number/or.hpp
  library/object/number/overflow.cpp
  library/object/number/overflow.hpp
  library/object/number/positive.cpp
  library/object/number/positive.hpp
  library/object/number/repr.cpp
  library/object/number/repr.hpp
  library/object/number/right_shift.cpp
  library/object/number/right_shift.hpp
  library/object/number/sub.cpp
  library/object/number/sub.hpp
  library/object/number/util.hpp
  library/object/number/xor.cpp
  library/object/number/xor.hpp)

target_include_directories(chimera_object_number PUBLIC library)
target_include_directories(
  chimera_object_number
  SYSTEM PRIVATE
  external/GSL/include)

add_library(
  chimera_virtual_machine
  library/virtual_machine/modules/marshal.cpp
  library/virtual_machine/modules/marshal.hpp
  library/virtual_machine/modules/sys.cpp
  library/virtual_machine/modules/sys.hpp
  library/virtual_machine/bin_evaluator.cpp
  library/virtual_machine/bin_evaluator.hpp
  library/virtual_machine/bool_evaluator.cpp
  library/virtual_machine/bool_evaluator.hpp
  library/virtual_machine/builtins.cpp
  library/virtual_machine/builtins.hpp
  library/virtual_machine/call_evaluator.cpp
  library/virtual_machine/call_evaluator.hpp
  library/virtual_machine/del_evaluator.cpp
  library/virtual_machine/del_evaluator.hpp
  library/virtual_machine/evaluator.cpp
  library/virtual_machine/evaluator.hpp
  library/virtual_machine/fibonacci_heap.hpp
  library/virtual_machine/garbage.cpp
  library/virtual_machine/garbage.hpp
  library/virtual_machine/get_evaluator.cpp
  library/virtual_machine/get_evaluator.hpp
  library/virtual_machine/global_context.cpp
  library/virtual_machine/global_context.hpp
  library/virtual_machine/process_context.cpp
  library/virtual_machine/process_context.hpp
  library/virtual_machine/push_stack.cpp
  library/virtual_machine/push_stack.hpp
  library/virtual_machine/set_evaluator.cpp
  library/virtual_machine/set_evaluator.hpp
  library/virtual_machine/slice_evaluator.cpp
  library/virtual_machine/slice_evaluator.hpp
  library/virtual_machine/thread_context.cpp
  library/virtual_machine/thread_context.hpp
  library/virtual_machine/to_bool_evaluator.cpp
  library/virtual_machine/to_bool_evaluator.hpp
  library/virtual_machine/tuple_evaluator.cpp
  library/virtual_machine/tuple_evaluator.hpp
  library/virtual_machine/unary_evaluator.cpp
  library/virtual_machine/unary_evaluator.hpp
  library/virtual_machine/virtual_machine.cpp
  library/virtual_machine/virtual_machine.hpp)

target_include_directories(chimera_virtual_machine PUBLIC library)
target_include_directories(
  chimera_virtual_machine
  SYSTEM PRIVATE
  external/GSL/include
  external/metal/include)

add_library(
  chimera_object
  library/object/object.cpp
  library/object/object.hpp)

target_include_directories(chimera_object PUBLIC library)
target_include_directories(
  chimera_object
  SYSTEM PRIVATE
  external/GSL/include)

add_library(
  chimera_container
  library/container/atomic_container.cpp
  library/container/atomic_container.hpp
  library/container/atomic_map.cpp
  library/container/atomic_map.hpp
  library/container/reverse.cpp
  library/container/reverse.hpp)

target_include_directories(chimera_container PUBLIC library)
target_include_directories(
  chimera_container
  SYSTEM PRIVATE
  external/GSL/include)

add_library(
  chimera_core
  library/options.cpp
  library/options.hpp
  library/version.cpp
  library/version.hpp)

set_target_properties(chimera_core PROPERTIES LINKER_LANGUAGE "CXX")

target_include_directories(chimera_core PUBLIC library)
target_include_directories(
  chimera_core
  SYSTEM PRIVATE
  external/GSL/include)
target_link_libraries(
  chimera_core
  chimera_container
  chimera_grammar
  chimera_object
  chimera_object_number
  chimera_virtual_machine)

add_library(
  libchimera
  include/chimera.h
  library/c-api/chimera.cpp)

target_compile_features(libchimera PUBLIC cxx_std_17)

target_include_directories(libchimera PUBLIC library)
target_include_directories(
  libchimera
  SYSTEM PRIVATE
  include)

set_target_properties(libchimera PROPERTIES OUTPUT_NAME "chimera")
target_link_libraries(libchimera chimera_core)

target_compile_features(libchimera PUBLIC cxx_std_17)

add_executable(chimera source/main.cpp)
target_include_directories(chimera PRIVATE library)
target_include_directories(
  chimera
  SYSTEM PUBLIC
  external/GSL/include
  external/metal/include)
target_link_libraries(chimera chimera_core)

target_compile_features(chimera PUBLIC cxx_std_17)

add_executable(
  unit_test
  unit_tests/grammar/grammar.cpp
  unit_tests/grammar/identifier.cpp
  unit_tests/grammar/number_parse.cpp
  unit_tests/grammar/number.cpp
  unit_tests/grammar/test_parse.hpp
  unit_tests/number/number.cpp
  unit_tests/virtual_machine/virtual_machine.cpp
  unit_tests/main.cpp)
target_include_directories(unit_test PRIVATE unit_tests)
target_include_directories(
  unit_test
  SYSTEM PUBLIC
  external/Catch/single_include
  external/GSL/include
  external/metal/include
  external/PEGTL/include)
target_link_libraries(unit_test chimera_core)

target_compile_features(unit_test PUBLIC cxx_std_17)

add_executable(fuzz unit_tests/fuzz/target.cpp)

target_compile_options(
  fuzz
  PUBLIC
  -g
  -fsanitize-coverage=trace-cmp
  -fsanitize-coverage=trace-pc-guard
  -fsanitize=address,fuzzer)

target_include_directories(fuzz PUBLIC library)
target_include_directories(
  fuzz
  SYSTEM PUBLIC
  external/GSL/include
  external/metal/include
  external/PEGTL/include)

target_link_libraries(
  fuzz
  chimera_core
  -fsanitize-coverage=trace-cmp
  -fsanitize-coverage=trace-pc-guard
  -fsanitize=address,fuzzer
  -rtlib=compiler-rt)

target_compile_features(fuzz PUBLIC cxx_std_17)

add_executable(fuzz2 unit_tests/fuzz/target2.cpp)

target_compile_options(
  fuzz2
  PUBLIC
  -g
  -fsanitize-coverage=trace-cmp
  -fsanitize-coverage=trace-pc-guard
  -fsanitize=address,fuzzer)

target_include_directories(fuzz2 PUBLIC library)
target_include_directories(
  fuzz2
  SYSTEM PUBLIC
  external/GSL/include
  external/metal/include
  external/PEGTL/include)

target_link_libraries(
  fuzz2
  chimera_core
  -fsanitize-coverage=trace-cmp
  -fsanitize-coverage=trace-pc-guard
  -fsanitize=address,fuzzer
  -rtlib=compiler-rt)

target_compile_features(fuzz2 PUBLIC cxx_std_17)

add_executable(bnf2abnf tools/bnf2abnf.cpp)
target_include_directories(
  bnf2abnf
  SYSTEM PRIVATE
  external/GSL/include
  external/PEGTL/include)
target_compile_features(bnf2abnf PUBLIC cxx_std_17)

add_library(modules_tools tools/modules.cpp tools/modules.hpp)
target_include_directories(modules_tools PUBLIC library tools)
target_include_directories(
  modules_tools
  SYSTEM PRIVATE
  external/GSL/include
  external/metal/include)
target_link_libraries(modules_tools chimera_core)
target_compile_features(modules_tools PUBLIC cxx_std_17)

add_executable(builtins tools/builtins.cpp)
target_include_directories(builtins PUBLIC library tools)
target_include_directories(
  builtins
  SYSTEM PRIVATE
  external/GSL/include
  external/metal/include)
target_link_libraries(builtins chimera_core modules_tools)
target_compile_features(builtins PUBLIC cxx_std_17)

add_executable(importlib tools/importlib.cpp)
target_include_directories(importlib PUBLIC library tools)
target_include_directories(
  importlib
  SYSTEM PRIVATE
  external/GSL/include
  external/metal/include)
target_link_libraries(importlib chimera_core modules_tools)
target_compile_features(importlib PUBLIC cxx_std_17)

add_custom_target(check unit_test VERBATIM USES_TERMINAL)
add_custom_target(check_rand unit_test --order rand VERBATIM USES_TERMINAL)
add_custom_target(
  corpus_dir
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/corpus)
add_custom_target(
  corpus
  fuzz
  -max_total_time=500
  -shrink=1 -use_value_profile=1
  -only_ascii=1
  -jobs=8
  -dict="${CMAKE_SOURCE_DIR}/unit_tests/fuzz/chimera.txt"
  ${CMAKE_BINARY_DIR}/corpus
  DEPENDS check corpus_dir
  VERBATIM USES_TERMINAL)
add_custom_target(
  corpus2
  fuzz2
  -max_len=8
  -max_total_time=500
  -shrink=1 -use_value_profile=1
  -jobs=8
  -dict="${CMAKE_SOURCE_DIR}/unit_tests/fuzz/chimera.txt"
  ${CMAKE_BINARY_DIR}/corpus
  DEPENDS check corpus_dir
  VERBATIM USES_TERMINAL)
add_custom_target(
  spec
  chimera -m test -j3
  DEPENDS check
  VERBATIM USES_TERMINAL)

install(
  TARGETS chimera libchimera
  ARCHIVE DESTINATION lib
  FRAMEWORK DESTINATION lib
  LIBRARY DESTINATION lib
  PUBLIC_HEADER DESTINATION include
  RUNTIME DESTINATION bin)
install(SCRIPT cmake/bootstrap_pip.cmake)
