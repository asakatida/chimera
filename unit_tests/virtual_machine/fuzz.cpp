#include <catch2/catch_test_macros.hpp>

#include "fuzz/cases.hpp"

using namespace std::literals;

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
