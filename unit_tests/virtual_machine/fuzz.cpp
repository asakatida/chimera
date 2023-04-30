#include <catch2/catch_test_macros.hpp>

#include "fuzz/cases.hpp"

using namespace std::literals;

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
void LLVMFuzzerTestOneInput(const std::uint8_t *data, std::size_t size) {
  REQUIRE_NOTHROW(
      chimera::library::fuzz_parse<chimera::library::grammar::Ellipsis<0>>(
          data, size));
  REQUIRE_NOTHROW(
      chimera::library::fuzz_parse<chimera::library::grammar::Expr<0>,
                                   chimera::library::grammar::rules::Stack<
                                       chimera::library::asdl::ExprImpl>>(
          data, size));
  REQUIRE_NOTHROW(
      chimera::library::fuzz_parse<chimera::library::grammar::False<0>>(data,
                                                                        size));
  REQUIRE_NOTHROW(
      chimera::library::fuzz_parse<chimera::library::grammar::SingleInput,
                                   chimera::library::asdl::Interactive>(data,
                                                                        size));
  REQUIRE_NOTHROW(
      chimera::library::fuzz_parse<chimera::library::grammar::Keywords>(data,
                                                                        size));
  REQUIRE_NOTHROW(
      chimera::library::fuzz_parse<chimera::library::grammar::FileInput,
                                   chimera::library::asdl::Module>(data, size));
  REQUIRE_NOTHROW(
      chimera::library::fuzz_parse<chimera::library::grammar::Name<0>>(data,
                                                                       size));
  REQUIRE_NOTHROW(
      chimera::library::fuzz_parse<chimera::library::grammar::None<0>>(data,
                                                                       size));
  REQUIRE_NOTHROW(
      chimera::library::fuzz_parse<chimera::library::grammar::NUMBER<0>>(data,
                                                                         size));
  REQUIRE_NOTHROW(
      chimera::library::fuzz_parse<chimera::library::grammar::Stmt<0>,
                                   chimera::library::grammar::rules::Stack<
                                       chimera::library::asdl::StmtImpl>>(
          data, size));
  REQUIRE_NOTHROW(
      chimera::library::fuzz_parse<chimera::library::grammar::STRING<0>>(data,
                                                                         size));
  REQUIRE_NOTHROW(
      chimera::library::fuzz_parse<chimera::library::grammar::True<0>>(data,
                                                                       size));
  REQUIRE_NOTHROW(chimera::library::fuzz_expression_eval(data, size));
  REQUIRE_NOTHROW(chimera::library::fuzz_interactive_eval(data, size));
  REQUIRE_NOTHROW(chimera::library::fuzz_file_eval(data, size));
}

template <typename Data>
void TestOne(Data &&data, std::size_t size) {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  LLVMFuzzerTestOneInput(reinterpret_cast<const std::uint8_t *>(data), size);
}

TEST_CASE(R"(fuzz `()`)") {
  auto test_case = R"(())"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `'\Uffffffffffffffffffff'`)") {
  auto test_case = R"('\Uffffffffffffffffffff')"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `'\xffff'`)") {
  auto test_case = R"('\xffff')"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `'\7777'`)") {
  auto test_case = R"('\7777')"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `raise`)") {
  auto test_case = R"(raise)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `0o40000000000`)") {
  auto test_case = "0o40000000000"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `\n`)") {
  auto test_case = R"(\n)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `b'''.\\\\'''`)") {
  auto test_case = R"(b'''.\\\\''')"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `_0`)") {
  auto test_case = R"(_0)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `""#"\\\n\\\n`)") {
  auto test_case = R"(""#"\\\n\\\n)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `assert8`)") {
  auto test_case = R"(assert8)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `in\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\n`)") {
  auto test_case =
      R"(in\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\n)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `0XAcccccccccccccccccCcccccccccccR`)") {
  auto test_case = R"(0XAcccccccccccccccccCcccccccccccR)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `""r`)") {
  auto test_case = R"(""r)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `''''\\s'''`)") {
  auto test_case = R"(''''\\s''')"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `8_8_8_8_8_88_85\n`)") {
  auto test_case = R"(8_8_8_8_8_88_85\n)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `0X7`)") {
  auto test_case = R"(0X7)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `or\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t`)") {
  auto test_case =
      R"(or\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `c`)") {
  auto test_case = R"(c)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `'\\UFm'`)") {
  auto test_case = R"('\\UFm')"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `'\\2f'`)") {
  auto test_case = R"('\\2f')"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `or#'&__bases__11f#'&`)") {
  auto test_case = R"(or#'&__bases__11f#'&)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `'\\U119'`)") {
  auto test_case = R"('\\U119')"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `r`)") {
  auto test_case = R"(r)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `0O1`)") {
  auto test_case = R"(0O1)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `None\\p`)") {
  auto test_case = R"(None\\p)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `False#\n`)") {
  auto test_case = R"(False#\n)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `TE`)") {
  auto test_case = R"(TE)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `r"""e;"""`)") {
  auto test_case = R"(r"""e;""")"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `'('\t1`)") {
  auto test_case = R"('('\t1)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `in\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n`)") {
  auto test_case =
      R"(in\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `f\t`)") {
  auto test_case = R"(f\t)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `rf''rf''fr''rf`)") {
  auto test_case = R"(rf''rf''fr''rf)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `in\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n`)") {
  auto test_case = R"(in\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `in\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c`)") {
  auto test_case =
      R"(in\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `True#mT\n`)") {
  auto test_case = R"(True#mT\n)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `"UUUU\\\\UU"`)") {
  auto test_case = R"("UUUU\\\\UU")"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `s\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t`)") {
  auto test_case = R"(s\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `'\\Ufffc'`)") {
  auto test_case = R"('\\Ufffc')"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `False\n`)") {
  auto test_case = R"(False\n)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `in\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\`)") {
  auto test_case =
      R"(in\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `0_0_0_0_00_0_0_0_0_0_0_1_0_1_1_1_0_1_1_0_1_1_1_0_0_1_0_1_1_1_0_1_0_1`)") {
  auto test_case =
      R"(0_0_0_0_00_0_0_0_0_0_0_1_0_1_1_1_0_1_1_0_1_1_1_0_0_1_0_1_1_1_0_1_0_1)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `"\\uee"`)") {
  auto test_case = R"("\\uee")"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `'\\.\\.'`)") {
  auto test_case = R"('\\.\\.')"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `if\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t0`)") {
  auto test_case =
      R"(if\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t0)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `R`)") {
  auto test_case = R"(R)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `False `)") {
  auto test_case = R"(False )"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `None       (`)") {
  auto test_case = R"(None       ()"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `b""b""b""b`)") {
  auto test_case = R"(b""b""b""b)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `5,1`)") {
  auto test_case = R"(5,1)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `in\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\n`)") {
  auto test_case =
      R"(in\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\n)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `'\\uD'`)") {
  auto test_case = R"('\\uD')"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `None~`)") {
  auto test_case = R"(None~)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `False&`)") {
  auto test_case = R"(False&)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `as\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\`)") {
  auto test_case =
      R"(as\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `False#s\n`)") {
  auto test_case = R"(False#s\n)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `"\\U000CCCC\\U0C0CCCC\\U000CCCC\\U0C00CCCC"`)") {
  auto test_case = R"("\\U000CCCC\\U0C0CCCC\\U000CCCC\\U0C00CCCC")"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `0XAn`)") {
  auto test_case = R"(0XAn)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000;`)") {
  auto test_case =
      R"(0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000;)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `in\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n`)") {
  auto test_case =
      R"(in\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n\\\n)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `as\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b`)") {
  auto test_case =
      R"(as\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `B"""\n\\\n)\\\\*"""`)") {
  auto test_case = R"(B"""\n\\\n)\\\\*""")"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `r''''@br'0'''`)") {
  auto test_case = R"(r''''@br'0''')"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `True,`)") {
  auto test_case = R"(True,)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `""""\\"\\j"""`)") {
  auto test_case = R"(""""\\"\\j""")"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `0b_0`)") {
  auto test_case = R"(0b_0)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `0O021000'`)") {
  auto test_case = R"(0O021000')"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `3J`)") {
  auto test_case = R"(3J)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `r"""""\'"""`)") {
  auto test_case = R"(r"""""\'""")"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `'\\UC788885A\\UC78888\\UC788885A\\UC78878'`)") {
  auto test_case = R"('\\UC788885A\\UC78888\\UC788885A\\UC78878')"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `r"""fe;"""`)") {
  auto test_case = R"(r"""fe;""")"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `se__gt__`)") {
  auto test_case = R"(se__gt__)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `lcacammb`)") {
  auto test_case = R"(lcacammb)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `''\x0b''\x0b''\x0b''\x0b''\x0b''\x0b''\x0b''\x0b''\x0b''\x0b''\x0b''\x0b''f''\x0b''\x0b''`)") {
  auto test_case =
      R"(''\x0b''\x0b''\x0b''\x0b''\x0b''\x0b''\x0b''\x0b''\x0b''\x0b''\x0b''\x0b''f''\x0b''\x0b'')"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `F#\n`)") {
  auto test_case = R"(F#\n)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `0#MMMMMKMMMMMMMM:MMMMMMMMMMMMMM\x0b\n`)") {
  auto test_case = R"(0#MMMMMKMMMMMMMM:MMMMMMMMMMMMMM\x0b\n)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `b"""\xb0\x19"""`)") {
  auto test_case = R"(b"""\xb0\x19""")"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `b'\xFalse1'`)") {
  auto test_case = R"(b'\xFalse1')"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `False.x=x=1`)") {
  auto test_case = "False.x=x=1"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `w\x5b`)") {
  auto test_case = R"(w\x5b)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `\x2d\x2d\x2b\x2b\x2b\x2b\x2b\x2b\x2b\x2d\x2b\x2b\x2b\x2b\x2b\x2b\x2d\x2d\x2b\x2b\x2d\x2d\x2b\x2d\x2b\x2b\x2dQ`)") {
  auto test_case =
      R"(\x2d\x2d\x2b\x2b\x2b\x2b\x2b\x2b\x2b\x2d\x2b\x2b\x2b\x2b\x2b\x2b\x2d\x2d\x2b\x2b\x2d\x2d\x2b\x2d\x2b\x2b\x2dQ)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `\x2d\x2d\x2b\x2b\x2b\x2b\x2b\x2b\x2b\x2d\x2b\x2b\x2b\x2b\x2b\x2b\x2b\x2d\x2d\x2b\x2d\x2b\x2b\x2b\x2d\x2d\x2d\x2dQ`)") {
  auto test_case =
      R"(\x2d\x2d\x2b\x2b\x2b\x2b\x2b\x2b\x2b\x2d\x2b\x2b\x2b\x2b\x2b\x2b\x2b\x2d\x2d\x2b\x2d\x2b\x2b\x2b\x2d\x2d\x2d\x2dQ)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `\x2d\x2b\x2d\x2d\x2d\x2b\x2b\x2b\x2b\x2b\x2b\x2d\x2d\x2d\x2d\x2b\x2b\x2b\x2d\x2d\x2d\x2dQ`)") {
  auto test_case =
      R"(\x2d\x2b\x2d\x2d\x2d\x2b\x2b\x2b\x2b\x2b\x2b\x2d\x2d\x2d\x2d\x2b\x2b\x2b\x2d\x2d\x2d\x2dQ)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `\x31\x5c\x0ai\x5c\x0a\x5c\x0a\x5c\x0a`)") {
  auto test_case = R"(\x31\x5c\x0ai\x5c\x0a\x5c\x0a\x5c\x0a)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `\x2e\x2e\x2e\x23\x0a\x0a`)") {
  auto test_case = R"(\x2e\x2e\x2e\x23\x0a\x0a)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `e\x40`)") {
  auto test_case = R"(e\x40)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `w\x3a`)") {
  auto test_case = R"(w\x3a)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `\x5b`)") {
  auto test_case = R"(\x5b)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `\x27\x27\x0az\x27`)") {
  auto test_case = R"(\x27\x27\x0az\x27)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `\x28`)") {
  auto test_case = R"(\x28)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `\x23`)") {
  auto test_case = R"(\x23)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `\x5f\x5f\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20`)") {
  auto test_case =
      R"(\x5f\x5f\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `None\x20\x20\x20\x20\x20\x20\x20\x20\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x23\x0c\x0c\x0c\x0c\x0c\x0c\x5c`)") {
  auto test_case =
      R"(None\x20\x20\x20\x20\x20\x20\x20\x20\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x23\x0c\x0c\x0c\x0c\x0c\x0c\x5c)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `\x2d\x2d\x2b\x2b\x2b\x2b\x2b\x2b\x2b\x2b\x2d\x2d\x2b\x2d\x2b\x2b\x2dQ`)") {
  auto test_case =
      R"(\x2d\x2d\x2b\x2b\x2b\x2b\x2b\x2b\x2b\x2b\x2d\x2d\x2b\x2d\x2b\x2b\x2dQ)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `\x2d\x2b\x2b\x2b\x2b\x2b\x2b\x2b\x2d\x2b\x2b\x2b\x2b\x2b\x2b\x2b\x2d\x2d\x2d\x2b\x2b\x2b\x2d\x2d\x2d\x2dQ`)") {
  auto test_case =
      R"(\x2d\x2b\x2b\x2b\x2b\x2b\x2b\x2b\x2d\x2b\x2b\x2b\x2b\x2b\x2b\x2b\x2d\x2d\x2d\x2b\x2b\x2b\x2d\x2d\x2d\x2dQ)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `\x30\x7c`)") {
  auto test_case = R"(\x30\x7c)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `\xe0\xa4\xbdr\xe0\xa4\xbdr\xe0\xa4\xbdr\xe0\xa4\xbdr\x23\x0a\x0a\x23\x0a`)") {
  auto test_case =
      R"(\xe0\xa4\xbdr\xe0\xa4\xbdr\xe0\xa4\xbdr\xe0\xa4\xbdr\x23\x0a\x0a\x23\x0a)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `\x30B\x30\x2f\x27`)") {
  auto test_case = R"(\x30B\x30\x2f\x27)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `\x2e\x2e\x2e\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x23p`)") {
  auto test_case =
      R"(\x2e\x2e\x2e\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x23p)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `i\x30\x30\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20`)") {
  auto test_case =
      R"(i\x30\x30\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `\x5f\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x23`)") {
  auto test_case =
      R"(\x5f\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x23)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `\x2d\x5f`)") {
  auto test_case = R"(\x2d\x5f)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `\x2b\x2b\x2b\x2b\x2d\x5f`)") {
  auto test_case = R"(\x2b\x2b\x2b\x2b\x2d\x5f)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `\x2d\x2d\x2d\x2b\x2d\x2b\x2b\x2bZ`)") {
  auto test_case = R"(\x2d\x2d\x2d\x2b\x2d\x2b\x2b\x2bZ)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `\x2d\x2d\x2b\x2b\x2b\x2b\x2d\x2d\x2b\x2b\x2b\x2b\x2b\x2b\x2b\x2d\x2b\x2b\x2b\x2dQ`)") {
  auto test_case =
      R"(\x2d\x2d\x2b\x2b\x2b\x2b\x2d\x2d\x2b\x2b\x2b\x2b\x2b\x2b\x2b\x2d\x2b\x2b\x2b\x2dQ)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `\xf0\x90\xa2\x90\xf0\x90\xa2\x90\x28\x28`)") {
  auto test_case = R"(\xf0\x90\xa2\x90\xf0\x90\xa2\x90\x28\x28)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `False\x0c`)") {
  auto test_case = R"(False\x0c)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `False\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c`)") {
  auto test_case =
      R"(False\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `\x5f\x3d\x5f\xd1\x9d\x3d\x5fn\x3d\x31`)") {
  auto test_case = R"(\x5f\x3d\x5f\xd1\x9d\x3d\x5fn\x3d\x31)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `\x2d\x2d\x2d\x2b\x2b\x2d\x2d\x2d\x2d\x2d\x2d\x2b\x2d\x2b\x2b\x2b\x2b\x2b\x2b\x2b\x2d\x2b\x2b\x2bZ`)") {
  auto test_case =
      R"(\x2d\x2d\x2d\x2b\x2b\x2d\x2d\x2d\x2d\x2d\x2d\x2b\x2d\x2b\x2b\x2b\x2b\x2b\x2b\x2b\x2d\x2b\x2b\x2bZ)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `None\x5c\x0ai\x5c\x0a\x5c\x0a\x5c\x0a\x5c\x0a\x5c\x0a\x5c\x0a\x5c\x0a\x5c\x0a`)") {
  auto test_case =
      R"(None\x5c\x0ai\x5c\x0a\x5c\x0a\x5c\x0a\x5c\x0a\x5c\x0a\x5c\x0a\x5c\x0a\x5c\x0a)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `\x7e\x2d\x2d\x2d\x2b\x2d\x2b\x2b\x2b\x2d\x2b\x2d\x2bZ`)") {
  auto test_case = R"(\x7e\x2d\x2d\x2d\x2b\x2d\x2b\x2b\x2b\x2d\x2b\x2d\x2bZ)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `\xef\xbe\x96rT\x5c\x0a\x5c\x0a\x5c\x0a\x5c\x0a\x0a\x0a\x5c\x0a\x0a`)") {
  auto test_case =
      R"(\xef\xbe\x96rT\x5c\x0a\x5c\x0a\x5c\x0a\x5c\x0a\x0a\x0a\x5c\x0a\x0a)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `\xf0\x90\xa4\xaa\xf0\x90\xa4\xaa\xf0\x90\xa4\xaa\xf0\x90\xa4\xaa\x5b\x5b\x5b`)") {
  auto test_case =
      R"(\xf0\x90\xa4\xaa\xf0\x90\xa4\xaa\xf0\x90\xa4\xaa\xf0\x90\xa4\xaa\x5b\x5b\x5b)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `\x27\x27\x28q`)") {
  auto test_case = R"(\x27\x27\x28q)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `\x5c`)") {
  auto test_case = R"(\x5c)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `N\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x23`)") {
  auto test_case =
      R"(N\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x09\x23)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `\x2d\x2d\x2d\x2b\x2b\x2d\x2d\x2d\x2d\x2d\x2b\x2b\x2b\x2bZ`)") {
  auto test_case =
      R"(\x2d\x2d\x2d\x2b\x2b\x2d\x2d\x2d\x2d\x2d\x2b\x2b\x2b\x2bZ)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `\x2b\x2d\x5f`)") {
  auto test_case = R"(\x2b\x2d\x5f)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `f\x5f\x30\x5f\x31\x2b\x2b\x2b\x2b\x2b\x2b\x2b\x2b\x2b\x27\x27`)") {
  auto test_case =
      R"(f\x5f\x30\x5f\x31\x2b\x2b\x2b\x2b\x2b\x2b\x2b\x2b\x2b\x27\x27)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `Yb\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20`)") {
  auto test_case =
      R"(Yb\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `\x2d\x2d\x2d\x2d\x2b\x2b\x2bX`)") {
  auto test_case = R"(\x2d\x2d\x2d\x2d\x2b\x2b\x2bX)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `\x2a\x5f`)") {
  auto test_case = R"(\x2a\x5f)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `\x2d\x2d\x2d\x2b\x2b\x2d\x2d\x2d\x2d\x2d\x2d\x2b\x2b\x2b\x2bZ`)") {
  auto test_case =
      R"(\x2d\x2d\x2d\x2b\x2b\x2d\x2d\x2d\x2d\x2d\x2d\x2b\x2b\x2b\x2bZ)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `\x2d\x2d\x2d\x2b\x2d\x2d\x2d\x2d\x2b\x2b\x2b\x2bZ`)") {
  auto test_case = R"(\x2d\x2d\x2d\x2b\x2d\x2d\x2d\x2d\x2b\x2b\x2b\x2bZ)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `\x2d\x2d\x2d\x2b\x2b\x2d\x2d\x2d\x2d\x2b\x2d\x2d\x2d\x2d\x2b\x2b\x2b\x2bZ`)") {
  auto test_case =
      R"(\x2d\x2d\x2d\x2b\x2b\x2d\x2d\x2d\x2d\x2b\x2d\x2d\x2d\x2d\x2b\x2b\x2b\x2bZ)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `False\x09`)") {
  auto test_case = R"(False\x09)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `\xf0\x90\xa4\xaa\xf0\x90\xa4\xaa\xf0\x90\xa4\xaa\xf0\x90\xa4\xaa\x5b\x5b\x5b\x5b`)") {
  auto test_case =
      R"(\xf0\x90\xa4\xaa\xf0\x90\xa4\xaa\xf0\x90\xa4\xaa\xf0\x90\xa4\xaa\x5b\x5b\x5b\x5b)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `\x2d\x2b\x2d\x2d\x2d\x2b\x2b\x2b\x2b\x2b\x2b\x2d\x2b\x2d\x2b\x2b\x2b\x2b\x2b\x2d\x2b\x2b\x2bX`)") {
  auto test_case =
      R"(\x2d\x2b\x2d\x2d\x2d\x2b\x2b\x2b\x2b\x2b\x2b\x2d\x2b\x2d\x2b\x2b\x2b\x2b\x2b\x2d\x2b\x2b\x2bX)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `e\xf0\x96\xbd\x8f\xf0\x96\xbd\x8f\x5fe\xf0\x96\xbd\x8f\xf0\x96\xbd\x8f\x5fs\x25\x2d\x5b\xd2\xa7`)") {
  auto test_case =
      R"(e\xf0\x96\xbd\x8f\xf0\x96\xbd\x8f\x5fe\xf0\x96\xbd\x8f\xf0\x96\xbd\x8f\x5fs\x25\x2d\x5b\xd2\xa7)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `\x2b\x2d\x2b\x2d\x2b\x2d\x5f`)") {
  auto test_case = R"(\x2b\x2d\x2b\x2d\x2b\x2d\x5f)"sv;
  TestOne(test_case.data(), test_case.size());
}
