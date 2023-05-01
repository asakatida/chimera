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

TEST_CASE(R"(fuzz `'\\xffff'`)") {
  auto test_case = R"('\\xffff')"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `'\\7777'`)") {
  auto test_case = R"('\\7777')"sv;
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

TEST_CASE(R"(fuzz `""#" `)") {
  auto test_case = R"(""#" )"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `assert8`)") {
  auto test_case = R"(assert8)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `in  `)") {
  auto test_case = R"(in  )"sv;
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

TEST_CASE(R"(fuzz `8_8_8_8_8_88_85 `)") {
  auto test_case = R"(8_8_8_8_8_88_85 )"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `0X7`)") {
  auto test_case = R"(0X7)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `or `)") {
  auto test_case = R"(or )"sv;
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

TEST_CASE(R"(fuzz `False# `)") {
  auto test_case = R"(False# )"sv;
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

TEST_CASE(R"(fuzz `in `)") {
  auto test_case = R"(in )"sv;
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

TEST_CASE(R"(fuzz `True#mT `)") {
  auto test_case = R"(True#mT )"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `"UUUU\\\\UU"`)") {
  auto test_case = R"("UUUU\\\\UU")"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `s `)") {
  auto test_case = R"(s )"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `'\\Ufffc'`)") {
  auto test_case = R"('\\Ufffc')"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `False `)") {
  auto test_case = R"(False )"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `in \\`)") {
  auto test_case = R"(in \\)"sv;
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

TEST_CASE(R"(fuzz `if 0`)") {
  auto test_case = R"(if 0)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `R`)") {
  auto test_case = R"(R)"sv;
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

TEST_CASE(R"(fuzz `as \\`)") {
  auto test_case = R"(as \\)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `False#s `)") {
  auto test_case = R"(False#s )"sv;
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

TEST_CASE(R"(fuzz `as `)") {
  auto test_case = R"(as )"sv;
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

TEST_CASE(R"(fuzz `'' '' '' '' '' '' '' '' '' '' '' '' ''f'' '' ''`)") {
  auto test_case = R"('' '' '' '' '' '' '' '' '' '' '' '' ''f'' '' '')"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `F# `)") {
  auto test_case = R"(F# )"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `0#MMMMMKMMMMMMMM:MMMMMMMMMMMMMM  `)") {
  auto test_case = R"(0#MMMMMKMMMMMMMM:MMMMMMMMMMMMMM  )"sv;
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

TEST_CASE(R"(fuzz `w[`)") {
  auto test_case = R"(w[)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `--+++++++-++++++--++--+-++-Q`)") {
  auto test_case = R"(--+++++++-++++++--++--+-++-Q)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `--+++++++-+++++++--+-+++----Q`)") {
  auto test_case = R"(--+++++++-+++++++--+-+++----Q)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `-+---++++++----+++----Q`)") {
  auto test_case = R"(-+---++++++----+++----Q)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `1\
i\
\
\
`)") {
  auto test_case = R"(1\
i\
\
\
)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `...#

`)") {
  auto test_case = R"(...#

)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `e@`)") {
  auto test_case = R"(e@)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `w:`)") {
  auto test_case = R"(w:)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `[`)") {
  auto test_case = R"([)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `''
z'`)") {
  auto test_case = R"(''
z')"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `(`)") {
  auto test_case = R"(()"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `#`)") {
  auto test_case = R"(#)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `__ `)") {
  auto test_case = R"(__ )"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `None #      \`)") {
  auto test_case = R"(None #      \)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `--++++++++--+-++-Q`)") {
  auto test_case = R"(--++++++++--+-++-Q)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `-+++++++-+++++++---+++----Q`)") {
  auto test_case = R"(-+++++++-+++++++---+++----Q)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `0|`)") {
  auto test_case = R"(0|)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `\xe0\xa4\xbdr\xe0\xa4\xbdr\xe0\xa4\xbdr\xe0\xa4\xbdr#

#
`)") {
  auto test_case =
      R"(\xe0\xa4\xbdr\xe0\xa4\xbdr\xe0\xa4\xbdr\xe0\xa4\xbdr#

#
)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `0B0/'`)") {
  auto test_case = R"(0B0/')"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `... #p`)") {
  auto test_case = R"(... #p)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `i00 `)") {
  auto test_case = R"(i00 )"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `_ #`)") {
  auto test_case = R"(_ #)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `-_`)") {
  auto test_case = R"(-_)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `++++-_`)") {
  auto test_case = R"(++++-_)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `---+-+++Z`)") {
  auto test_case = R"(---+-+++Z)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `--++++--+++++++-+++-Q`)") {
  auto test_case = R"(--++++--+++++++-+++-Q)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `\xf0\x90\xa2\x90\xf0\x90\xa2\x90((`)") {
  auto test_case = R"(\xf0\x90\xa2\x90\xf0\x90\xa2\x90(()"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `_=_\xd1\x9d=_n=1`)") {
  auto test_case = R"(_=_\xd1\x9d=_n=1)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `---++------+-+++++++-+++Z`)") {
  auto test_case = R"(---++------+-+++++++-+++Z)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `None\
i\
\
\
\
\
\
\
\
`)") {
  auto test_case =
      R"(None\
i\
\
\
\
\
\
\
\
)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `~---+-+++-+-+Z`)") {
  auto test_case = R"(~---+-+++-+-+Z)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `\xef\xbe\x96rT\
\
\
\


\

`)") {
  auto test_case =
      R"(\xef\xbe\x96rT\
\
\
\


\

)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `\xf0\x90\xa4\xaa\xf0\x90\xa4\xaa\xf0\x90\xa4\xaa\xf0\x90\xa4\xaa[[[`)") {
  auto test_case =
      R"(\xf0\x90\xa4\xaa\xf0\x90\xa4\xaa\xf0\x90\xa4\xaa\xf0\x90\xa4\xaa[[[)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `''(q`)") {
  auto test_case = R"(''(q)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `\`)") {
  auto test_case = R"(\)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `N #`)") {
  auto test_case = R"(N #)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `---++-----++++Z`)") {
  auto test_case = R"(---++-----++++Z)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `+-_`)") {
  auto test_case = R"(+-_)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `f_0_1+++++++++''`)") {
  auto test_case = R"(f_0_1+++++++++'')"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `Yb `)") {
  auto test_case = R"(Yb )"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `----+++X`)") {
  auto test_case = R"(----+++X)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `*_`)") {
  auto test_case = R"(*_)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `---++------++++Z`)") {
  auto test_case = R"(---++------++++Z)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `---+----++++Z`)") {
  auto test_case = R"(---+----++++Z)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `---++----+----++++Z`)") {
  auto test_case = R"(---++----+----++++Z)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `\xf0\x90\xa4\xaa\xf0\x90\xa4\xaa\xf0\x90\xa4\xaa\xf0\x90\xa4\xaa[[[[`)") {
  auto test_case =
      R"(\xf0\x90\xa4\xaa\xf0\x90\xa4\xaa\xf0\x90\xa4\xaa\xf0\x90\xa4\xaa[[[[)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `-+---++++++-+-+++++-+++X`)") {
  auto test_case = R"(-+---++++++-+-+++++-+++X)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `e\xf0\x96\xbd\x8f\xf0\x96\xbd\x8f_e\xf0\x96\xbd\x8f\xf0\x96\xbd\x8f_s%-[\xd2\xa7`)") {
  auto test_case =
      R"(e\xf0\x96\xbd\x8f\xf0\x96\xbd\x8f_e\xf0\x96\xbd\x8f\xf0\x96\xbd\x8f_s%-[\xd2\xa7)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `+-+-+-_`)") {
  auto test_case = R"(+-+-+-_)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `""f''""(`)") {
  auto test_case = R"(""f''""()"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `RR `)") {
  auto test_case = R"(RR )"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(R"(fuzz `''f''f''(`)") {
  auto test_case = R"(''f''f''()"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `0																																																																																																																						`)",
    "[0493a71db86df8e715cf17e598e2ce4fe3b2f832b9d636af7a4f7a3a585f1eff]") {
  auto test_case =
      R"(0																																																																																																																						)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `False  # `)",
    "[096f9f49101b5c60c5aa859309f96a79d0587edb7244a94338ef3ef66d51cb73]") {
  auto test_case = R"(False  # )"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `--+++++++-++++++--+++++++-+++++++--+-+++----+++++--+-+++----Q`)",
    "[0eb7b8cb366072eb3204c8ea4b13cfc1299df627f47d8f617040df3e01b81f96]") {
  auto test_case =
      R"(--+++++++-++++++--+++++++-+++++++--+-+++----+++++--+-+++----Q)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `--+++++++-++++--+++++++-++++++--+++--+++++++-+++++++--+-++-++++-+++++++--+-++-Q`)",
    "[16b100f45e25107b50f339ab24f467c9057fc82b296157944ac3e6005c12b67a]") {
  auto test_case =
      R"(--+++++++-++++--+++++++-++++++--+++--+++++++-+++++++--+-++-++++-+++++++--+-++-Q)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `-+---++++++-+-+++++--+--++++++-+-+++++--+--++++X`)",
    "[1ab5815dfb47241d566178e4ff3eb8323ed8eebe322f147e88d007fbbf598bb2]") {
  auto test_case = R"(-+---++++++-+-+++++--+--++++++-+-+++++--+--++++X)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `0_0_0_0_0_0_0_0_0_0_7_0_1_1_1_1_1_0_1_0_1_1_1_1_0_0_1_0_0_1_1_0_1_`)",
    "[2646c79cad9d8d8cb0d5cd34b69f985a1c053fe8a61f4ddf4db916b86bdbcbf2]") {
  auto test_case =
      R"(0_0_0_0_0_0_0_0_0_0_7_0_1_1_1_1_1_0_1_0_1_1_1_1_0_0_1_0_0_1_1_0_1_)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `-+---++++++-+-+++++----++++++-+++++-+---++++X`)",
    "[27bb6b68451c33fac0ec6adcd7d48f19077a821a40e34b7ece7f3b05f8d56790]") {
  auto test_case = R"(-+---++++++-+-+++++----++++++-+++++-+---++++X)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `0_0_0_5_0_0_0_0_0_0_0_0_0_1_0_1_1_1_0_1_0_1_1_1_1_0_0_1_0_0_1_1_0_1_`)",
    "[2a6e1a4eae5de3af0be222758a7c6ae4d10be6af6b0e710de22694037b6a010e]") {
  auto test_case =
      R"(0_0_0_5_0_0_0_0_0_0_0_0_0_1_0_1_1_1_0_1_0_1_1_1_1_0_0_1_0_0_1_1_0_1_)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `0																																																											`)",
    "[31584c7536b2830a582c71207774a21e30029e7c5d81ebdceab91302716914ea]") {
  auto test_case =
      R"(0																																																											)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `0_8_8_7_8_8_7_8_8_8_8_4_8_8_7_8_81_8_8_8_8_8_8_8_7_8_8_7_8_8_8_8_8_8_3`)",
    "[336748a85982c8d3b5f25fafae7d77adc78ba67b6d7e247c99fafe62aeb7832e]") {
  auto test_case =
      R"(0_8_8_7_8_8_7_8_8_8_8_4_8_8_7_8_81_8_8_8_8_8_8_8_7_8_8_7_8_8_8_8_8_8_3)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `__ `)",
    "[33e73a75a6e1781b727b20b10d22648f431bf54c305d1cc9ccf5d57aed71becb]") {
  auto test_case = R"(__ )"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `None #      \`)",
    "[36b78b8734122935a54303b55f6e2826493b09a2ead33dfd544dfb6a81d004a3]") {
  auto test_case = R"(None #      \)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `...																									#p`)",
    "[543ba37c6487257702ff3d93f88e974986d756cc619df389cbfa7e88fc1e1445]") {
  auto test_case =
      R"(...																									#p)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `yield `)",
    "[593bcedd7b8fc47e6e8e14f7591f431b705f016bab867dc1f801e9133ed1d2c2]") {
  auto test_case = R"(yield )"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `i00 `)",
    "[60486782b85d5042cc914da1d0c52c6d8ccfcd45360d78d5b4e54a40d6a57091]") {
  auto test_case = R"(i00 )"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `yield 	 `)",
    "[62130cfd2e925748f5c310ccc415f53a54ff1eed045eab9f5c0253f26bb1d97e]") {
  auto test_case = R"(yield 	 )"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `_ #`)",
    "[6608e898c0d59a599a49d4862e4843af366fadef18a7b923d33b327f3bdad7cc]") {
  auto test_case = R"(_ #)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `000000000_8_8_8_8_8_8_8_1_1_0_1_0_1_1_8_8_8_8_8_8_8_8_8_8_8_8_8_8_8_7_8_8_8_8_000`)",
    "[6781404ad55757f849ded980351cb4b98cfcd0ddb45f190ddffd0afa6840e6fb]") {
  auto test_case =
      R"(000000000_8_8_8_8_8_8_8_1_1_0_1_0_1_1_8_8_8_8_8_8_8_8_8_8_8_8_8_8_8_7_8_8_8_8_000)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `0																																																																																																																					`)",
    "[729d3023e589cac2d4b37b6dec00c803846a92e0dc82627e245c87fdd450a634]") {
  auto test_case =
      R"(0																																																																																																																					)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `--+++++++-++++++--+++++++-+++++++--+-+++----+--+-+++----Q`)",
    "[72e2e04dfcee8c0baebe9b21cadd31882649ff1ff1d9d591eb71ed64163b76cc]") {
  auto test_case =
      R"(--+++++++-++++++--+++++++-+++++++--+-+++----+--+-+++----Q)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `'''\
\
\
\
\
b'$\\z\$?\\z\$%\z0\$%\z0\\z\
\$?\\z\$%\z0\$%\
\
\z0\\1'\
\
\
\
\
\
\
\
'''`)",
    "[76773339b4c895e48e0761ecf5261e98b56a74c9d242b9a15efba390ad01b98d]") {
  auto test_case = R"('''\
\
\
\
\
b'$\\z\$?\\z\$%\z0\$%\z0\\z\
\$?\\z\$%\z0\$%\
\
\z0\\1'\
\
\
\
\
\
\
\
''')"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `False `)",
    "[7dc8ab7d42640d61f297a57173051a24d662a2950b29b93566124141f189d9c7]") {
  auto test_case = R"(False )"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `""f''""f''""f''""f''""f''""f'"f''""f''""f''""f''""f'''""r`)",
    "[819fdc668fcf1d3ad52cb67694b55f5bc7a12b29edfa34158740c11918df2abd]") {
  auto test_case =
      R"(""f''""f''""f''""f''""f''""f'"f''""f''""f''""f''""f'''""r)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `0																																																					`)",
    "[8268736a1b39782202999e5c763e835de1900aeca9e0210b686378e0fce5f4f9]") {
  auto test_case =
      R"(0																																																					)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `False `)",
    "[83b97f82c4b517ba79989cdccc8138c53f27b9cac42438eb680c6713a57ad283]") {
  auto test_case = R"(False )"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `-+---++++++-++++----++++++-----+-------------------------------+----------------------+++++++++----QX`)",
    "[8ba365d8857c7da9e4edac2247324d322c1756c077bf8fcb1a0e8c5e4faa670d]") {
  auto test_case =
      R"(-+---++++++-++++----++++++-----+-------------------------------+----------------------+++++++++----QX)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `... `)",
    "[8bd8a1516120146e49beba08a57ab6e6ef4adffefbf1e8922d12d643feac25a1]") {
  auto test_case = R"(... )"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `--+ +++++-++++++--+++++++-+++++++--+-+++----+--+-++++-++++++--+++++++-+++++++--+-+++-+++----Q`)",
    "[8c118bdf04da117fccf4f4857f4d0c8e25490c2637ce5d586c28187ff4b046b3]") {
  auto test_case =
      R"(--+ +++++-++++++--+++++++-+++++++--+-+++----+--+-++++-++++++--+++++++-+++++++--+-+++-+++----Q)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `-+---++++++-+++++++------++++++-+---Q`)",
    "[95c9c86d97ae4249eaab7f1b6dc8bc9a2841fc191d1393a6079109103294083f]") {
  auto test_case = R"(-+---++++++-+++++++------++++++-+---Q)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `2 	 	 `)",
    "[97d416297be1cea56a3aea1f00c29c4ad519bad03999bc2e1cdcf35dea04f2bb]") {
  auto test_case = R"(2 	 	 )"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `'' '' '' '' '' '' ''f' '' '' '' '' ''f' ''' '' ' ''' '' '' '' '' '' ''n`)",
    "[9c58103366d3f5efe5b9d28a774b7654633b161e3260e72ed166dbe0cf866747]") {
  auto test_case =
      R"('' '' '' '' '' '' ''f' '' '' '' '' ''f' ''' '' ' ''' '' '' '' '' '' ''n)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `-+---++++++-+-+++++----++++++-+++---+-+---++++X`)",
    "[9d73d6cb21a46ad6b919acef17f4c7d15d11bd90e080f2c97fb6476a8607e3d4]") {
  auto test_case = R"(-+---++++++-+-+++++----++++++-+++---+-+---++++X)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `-+---++++++-++++----++++++-------------+-------------------------------+----------------------+++++++++----QX`)",
    "[9ef9feb8c9a06357323bb9b12aef46b87b8b5b9b33cb40251c58b88801dff601]") {
  auto test_case =
      R"(-+---++++++-++++----++++++-------------+-------------------------------+----------------------+++++++++----QX)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `-+---++++++-++++----++++++-------------+--------------------------------------+----------------------+++++++++----QX`)",
    "[9f9c6b0f4f06e5bab0863b66109cde70a9550f404c382f888f53e4531ba60c97]") {
  auto test_case =
      R"(-+---++++++-++++----++++++-------------+--------------------------------------+----------------------+++++++++----QX)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `++--+++++++-++++++--+++++++-+++++++--+-+++----+--+-+++-++-_`)",
    "[a26b79f88032a8baa5c9588be8ac76ff1b487e9a7e4b23db1b48ceb94d2f2026]") {
  auto test_case =
      R"(++--+++++++-++++++--+++++++-+++++++--+-+++----+--+-+++-++-_)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `01_0_0_0_0_0_0_0_0_0_0_0_0_1_0_1_1_1_0_1_0_1_1_1_1_0_0_1_0_0_1_1_0_1_`)",
    "[a27dfe5481cfe5308cdaf78d5f32b0b6aee72e2add49603792ab06f64c9d0044]") {
  auto test_case =
      R"(01_0_0_0_0_0_0_0_0_0_0_0_0_1_0_1_1_1_0_1_0_1_1_1_1_0_0_1_0_0_1_1_0_1_)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `--+++++++-++++++--+++++++-+++++++--+-++-Q`)",
    "[b088ef65f4a504b6f821f295567e7967534907e004c84b7f75ef1be26a965705]") {
  auto test_case = R"(--+++++++-++++++--+++++++-+++++++--+-++-Q)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `N																									#`)",
    "[b25ba040426dd97015ee7edc1e995333af282c2a6e133f9208884bda8da0d6de]") {
  auto test_case =
      R"(N																									#)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `--+++++++-++++++--+++++++-++++++--+++-+++++++--+-++-Q`)",
    "[c005ceee090e80a43ed9f14c98ba355d5b74bceeb7e7b923736f86aaded73780]") {
  auto test_case = R"(--+++++++-++++++--+++++++-++++++--+++-+++++++--+-++-Q)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `Yb `)",
    "[c1ce3fe6a74abc75cb49cdcb8ccbc71adae3795cd13d9bd5d1be02cb6fd09cc8]") {
  auto test_case = R"(Yb )"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `... #`)",
    "[c43ccee686ace4e51e891573b6e5903475ea874d03044fc8af2ebc46ef0956f6]") {
  auto test_case = R"(... #)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `0_0_0_0_0_0_0_0_0_3_0_0_0_1_0_1_1_1_0_1_0_1_0_1_1_1_1_0_0_1_0_0_1_1_0_1_`)",
    "[cc5d4cca0b6468c13280956cab03cb0646f953444de0427232db885a4bc4f846]") {
  auto test_case =
      R"(0_0_0_0_0_0_0_0_0_3_0_0_0_1_0_1_1_1_0_1_0_1_0_1_1_1_1_0_0_1_0_0_1_1_0_1_)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `00000000000000000000000000000021`)",
    "[ceb33bf693e2d3f05f9fc9519f0e41a02990636f3ce751e1e758b2ee183d5089]") {
  auto test_case = R"(00000000000000000000000000000021)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `--+++++++-++++--+++++++-++++++--+++--+-++++++++++-+++++++--+-++-++++-+++++++--+-++-Q`)",
    "[cedef69f653d64e1cd9b7cdccf73d354fc004098245a041386d12414655c2d2e]") {
  auto test_case =
      R"(--+++++++-++++--+++++++-++++++--+++--+-++++++++++-+++++++--+-++-++++-+++++++--+-++-Q)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `RR `)",
    "[d113258f46f4de1c57b0bcbae6c86accc6682663bee35f457f2a92fbe0e85c35]") {
  auto test_case = R"(RR )"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `--+++++++-+++++--+++++++-+++++++--+-++-Q`)",
    "[d47a02bce37a13b26556020866cf963b6fa2beb69ed650ef16e7c33f3229339d]") {
  auto test_case = R"(--+++++++-+++++--+++++++-+++++++--+-++-Q)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `None																																																										`)",
    "[de43d7b4793943c45b73901fb3fbdc3799e77edfebe80ef6b753ccee8324c6d3]") {
  auto test_case =
      R"(None																																																										)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `--+++++++-++++--+++++++-++++++--+++--+-++++++++++-+++++++--+-++-+++-Q`)",
    "[e31c3126308eacd31d01d67aed63b9b75420dc9d963167c28b5cfc1ecaacaed1]") {
  auto test_case =
      R"(--+++++++-++++--+++++++-++++++--+++--+-++++++++++-+++++++--+-++-+++-Q)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `False	`)",
    "[e43d33116b531c8c196cba716a559fe9c8e20782596348d064d9658dc86550d3]") {
  auto test_case = R"(False	)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `'' '' '' '' '' r'' '' '' '' '' ''f'' '' '' '' '' '' ''f'' '' '' ''f'' '' '' '' '' '' ''f'' '' ''`)",
    "[e7c4ddcfc941d41866b300240788ff77e95d6fc4177a4156a627701f978be182]") {
  auto test_case =
      R"('' '' '' '' '' r'' '' '' '' '' ''f'' '' '' '' '' '' ''f'' '' '' ''f'' '' '' '' '' '' ''f'' '' '')"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `0																																																										`)",
    "[eb283dcdb8bcca3b698502c7971932604508e6844120ab458c2bee27834681a7]") {
  auto test_case =
      R"(0																																																										)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `--+++++++-++++++++--+++++++-++++++--+++-+++++++--+-++-Q`)",
    "[ed0fab5d8146c772dc9a364b2ffc2e2107b661af83cfa7eef8ba48ec0a1f518f]") {
  auto test_case =
      R"(--+++++++-++++++++--+++++++-++++++--+++-+++++++--+-++-Q)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `...																												`)",
    "[ed8ca789ac616cd41ac89345b4ab15f10206c1e91520beb688bfc814043555c3]") {
  auto test_case =
      R"(...																												)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `00000000000000000000000000000011`)",
    "[efdf1e83c180b1c0c8658c819bb9afe2c03a285f2ce940802bbb3fbb811952f4]") {
  auto test_case = R"(00000000000000000000000000000011)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `0																																										 															`)",
    "[f0263cac616015b229323b9fb5788ef8c8b1a5ed9369fa30c6319fd52d12c55c]") {
  auto test_case =
      R"(0																																										 															)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `--+++++--+++++++-+++++++--+-++-Q`)",
    "[f05f9244bfe42be608e381a347dcb542e56417f909d1d6be9064956df52db899]") {
  auto test_case = R"(--+++++--+++++++-+++++++--+-++-Q)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `f:++++++++r++++D++\xd3\x9d=2++++r+f+++++++++''`)",
    "[f1a121177e4ba6f7918e7ac86fb8e483770e985651a0c5e77f9e7afac7ea33b6]") {
  auto test_case = R"(f:++++++++r++++D++\xd3\x9d=2++++r+f+++++++++'')"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `yield `)",
    "[f309f24eb5213e638c536bef0c3be556ff2e43b58a50162c957a7294f1edef49]") {
  auto test_case = R"(yield )"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `-+---++++++-+++++++------+++----Q`)",
    "[f3ebb2bedd9a7901eae9e7cadeacb45135c648c6f59baeeb04bd8787969daceb]") {
  auto test_case = R"(-+---++++++-+++++++------+++----Q)"sv;
  TestOne(test_case.data(), test_case.size());
}

TEST_CASE(
    R"(fuzz `70																																																																																																																						`)",
    "[fedec1f6b718c697e571b40e0170f236478cc76e22524f092a96c638441dbc86]") {
  auto test_case =
      R"(70																																																																																																																						)"sv;
  TestOne(test_case.data(), test_case.size());
}
