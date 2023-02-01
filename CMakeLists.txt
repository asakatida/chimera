cmake_minimum_required(VERSION 3.22.0)
project(
  chimera
  VERSION 1.0.0
  LANGUAGES CXX)

set(
  CMAKE_CXX_FLAGS_COVERAGE
  -O0 -DNDEBUG
  -fcoverage-mapping
  -fprofile-instr-generate
  -mllvm
  -runtime-counter-relocation
  CACHE STRING
  "Flags used by the C++ compiler during coverage builds.")
set(
  CMAKE_CXX_FLAGS_DEBUG
  -O0 -g -Wall -Wextra -Wpedantic -Werror
  CACHE STRING
  "Flags used by the C++ compiler during debug builds.")

if(MSVC)
  add_compile_options(/W4 /WX)
else()
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(
      -Wno-c++11-extensions
      -Wno-c++17-extensions
      -Wno-c++98-compat-pedantic)
  endif()
  add_compile_options(
    -Wno-date-time
    -Wno-padded)
endif()

add_library(
  chimera-grammar
  OBJECT
  library/asdl/asdl.cpp
  library/asdl/parse_expression_istream.cpp
  library/asdl/parse_expression_string_view.cpp
  library/asdl/parse_interactive_istream.cpp
  library/asdl/parse_interactive_string_view.cpp
  library/asdl/parse_module_istream.cpp
  library/asdl/parse_module_string_view.cpp)

target_compile_features(chimera-grammar PUBLIC c_std_23 cxx_std_20)

target_include_directories(chimera-grammar PUBLIC library stdlib)

target_include_directories(
  chimera-grammar
  SYSTEM PUBLIC
  external/GSL/include
  external/metal/include
  external/operators/include
  external/PEGTL/include)

add_library(
  chimera-core
  library/container/atomic_container.cpp
  library/container/atomic_map.cpp
  library/container/reverse.cpp
  library/object/number/add.cpp
  library/object/number/and.cpp
  library/object/number/compare.cpp
  library/object/number/div.cpp
  library/object/number/floor_div.cpp
  library/object/number/gcd.cpp
  library/object/number/invert.cpp
  library/object/number/left_shift.cpp
  library/object/number/less.cpp
  library/object/number/mod.cpp
  library/object/number/mult.cpp
  library/object/number/negative.cpp
  library/object/number/number.cpp
  library/object/number/or.cpp
  library/object/number/overflow.cpp
  library/object/number/positive.cpp
  library/object/number/right_shift.cpp
  library/object/number/sub.cpp
  library/object/number/xor.cpp
  library/object/object.cpp
  library/object/reference.cpp
  library/virtual_machine/bin_evaluator.cpp
  library/virtual_machine/bool_evaluator.cpp
  library/virtual_machine/call_evaluator.cpp
  library/virtual_machine/del_evaluator.cpp
  library/virtual_machine/evaluator.cpp
  library/virtual_machine/garbage.cpp
  library/virtual_machine/get_evaluator.cpp
  library/virtual_machine/global_context.cpp
  library/virtual_machine/process_context.cpp
  library/virtual_machine/push_stack.cpp
  library/virtual_machine/set_evaluator.cpp
  library/virtual_machine/slice_evaluator.cpp
  library/virtual_machine/thread_context.cpp
  library/virtual_machine/to_bool_evaluator.cpp
  library/virtual_machine/tuple_evaluator.cpp
  library/virtual_machine/unary_evaluator.cpp
  library/options.cpp
  library/version.cpp
  stdlib/builtins/builtins.cpp
  stdlib/importlib/importlib.cpp
  stdlib/marshal/marshal.cpp
  stdlib/sys/sys.cpp)

target_compile_features(chimera-core PUBLIC c_std_23 cxx_std_20)

target_include_directories(chimera-core PUBLIC library stdlib)

target_include_directories(
  chimera-core
  SYSTEM PUBLIC
  external/GSL/include
  external/metal/include
  external/operators/include
  external/PEGTL/include)

target_link_libraries(chimera-core chimera-grammar pthread)

add_library(
  libchimera
  include/chimera.h
  library/c-api/chimera.cpp)

target_compile_features(libchimera PUBLIC c_std_23 cxx_std_20)

target_include_directories(
  libchimera
  SYSTEM PUBLIC
  include
  external/GSL/include)

set_target_properties(libchimera PROPERTIES OUTPUT_NAME "chimera")

add_executable(chimera source/main.cpp)
target_compile_features(chimera PUBLIC c_std_23 cxx_std_20)
target_link_libraries(chimera chimera-core)

add_subdirectory(external/Catch EXCLUDE_FROM_ALL)

add_executable(
  unit-test
  unit_tests/fuzz/cases.cpp
  unit_tests/grammar/grammar.cpp
  unit_tests/grammar/identifier.cpp
  unit_tests/grammar/number_parse.cpp
  unit_tests/grammar/number.cpp
  unit_tests/grammar/statement.cpp
  unit_tests/number/number.cpp
  unit_tests/virtual_machine/parse.cpp
  unit_tests/virtual_machine/virtual_machine.cpp)

target_compile_features(unit-test PUBLIC c_std_23 cxx_std_20)

target_include_directories(unit-test PUBLIC unit_tests)

target_include_directories(
  unit-test
  SYSTEM PUBLIC
  external/Catch/src)

target_link_libraries(unit-test Catch2WithMain chimera-core)

add_custom_target(check unit-test VERBATIM)
add_custom_target(check-stat perf stat ./unit-test VERBATIM)
add_custom_target(check-rand unit-test --order rand VERBATIM)

if(
  CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND
  NOT CMAKE_CXX_COMPILER_ID MATCHES "AppleClang")
  set(
    FUZZ_CXXFLAGS
    -fsanitize-coverage=no-prune
    -fsanitize-coverage=trace-cmp
    -fsanitize-coverage=trace-div
    -fsanitize-coverage=trace-gep
    -fsanitize-coverage=trace-loads
    -fsanitize-coverage=trace-stores
    -fsanitize=fuzzer)
  add_library(chimera-fuzz unit_tests/fuzz/cases.cpp)
  target_compile_options(
    chimera-fuzz
    PUBLIC
    ${FUZZ_CXXFLAGS})
  target_compile_features(chimera-fuzz PUBLIC c_std_23 cxx_std_20)
  target_include_directories(chimera-fuzz PUBLIC unit_tests)
  target_link_libraries(chimera-fuzz chimera-core pthread)
  add_custom_target(corpus)
  add_custom_target(fuzzers)
  add_custom_target(regression)
  set(
    FUZZERS
    ellipsis
    expression
    false
    interactive
    keywords
    module
    name
    none
    number
    statement
    string
    true
    vm
    vm-expression
    vm-interact)
  foreach(fuzzer IN LISTS FUZZERS)
    add_executable(fuzz-${fuzzer} unit_tests/fuzz/target-${fuzzer}.cpp)
    target_compile_features(fuzz-${fuzzer} PUBLIC c_std_23 cxx_std_20)
    target_compile_options(
      fuzz-${fuzzer}
      PUBLIC
      ${FUZZ_CXXFLAGS})
    target_include_directories(fuzz-${fuzzer} PUBLIC unit_tests)
    target_link_libraries(
      fuzz-${fuzzer}
      chimera-core
      chimera-fuzz
      ${FUZZ_CXXFLAGS}
      -rtlib=compiler-rt)
    add_custom_target(
      corpus-${fuzzer}
      fuzz-${fuzzer}
      -detect_leaks=0
      -dict=${CMAKE_SOURCE_DIR}/unit_tests/fuzz/dictionaries/${fuzzer}.dict
      -max_len=16384
      -max_total_time=120
      -print_final_stats=1
      -reduce_inputs=1
      -shrink=1
      -use_value_profile=1
      ${CMAKE_SOURCE_DIR}/unit_tests/fuzz/corpus
      ${CMAKE_SOURCE_DIR}/unit_tests/fuzz/crashes
      DEPENDS fuzz-${fuzzer}
      VERBATIM)
    add_custom_target(
      regression-${fuzzer}
      python ${CMAKE_SOURCE_DIR}/tools/regression.py ${fuzzer}
      ${CMAKE_SOURCE_DIR}/unit_tests/fuzz/corpus
      DEPENDS fuzz-${fuzzer}
      VERBATIM)
    add_dependencies(corpus corpus-${fuzzer})
    add_dependencies(fuzzers fuzz-${fuzzer})
    add_dependencies(regression regression-${fuzzer})
  endforeach()
endif()

add_executable(bnf2abnf tools/bnf2abnf.cpp)

target_compile_features(bnf2abnf PUBLIC c_std_23 cxx_std_20)

target_include_directories(bnf2abnf PUBLIC library)

target_include_directories(
  bnf2abnf
  SYSTEM PUBLIC
  external/GSL/include
  external/metal/include
  external/PEGTL/include)

add_library(modules-tools tools/modules/modules.cpp)

target_compile_features(modules-tools PUBLIC c_std_23 cxx_std_20)

target_include_directories(modules-tools PUBLIC library)

target_include_directories(
  modules-tools
  SYSTEM PUBLIC
  external/GSL/include
  external/metal/include
  external/operators/include)

target_include_directories(modules-tools PUBLIC tools)

add_executable(builtins tools/modules/builtins.cpp)
target_compile_features(builtins PUBLIC c_std_23 cxx_std_20)
target_link_libraries(builtins modules-tools chimera-core)

add_executable(importlib tools/modules/importlib.cpp)
target_compile_features(importlib PUBLIC c_std_23 cxx_std_20)
target_link_libraries(importlib modules-tools chimera-core)

add_custom_target(modules DEPENDS builtins importlib)

add_custom_target(
  spec
  chimera -m test --multiprocess 3
  DEPENDS chimera
  VERBATIM)

add_custom_target(
  spec-stat
  perf stat ./chimera -m test --multiprocess 3
  DEPENDS chimera
  VERBATIM)

install(
  TARGETS chimera libchimera
  ARCHIVE DESTINATION lib
  FRAMEWORK DESTINATION lib
  LIBRARY DESTINATION lib
  PUBLIC_HEADER DESTINATION include
  RUNTIME DESTINATION bin)
install(SCRIPT cmake/bootstrap_pip.cmake)
