//! top level grammars.

#pragma once

#include "asdl/asdl.hpp"
#include "grammar/expr.hpp"
#include "grammar/flags.hpp"
#include "grammar/parse.hpp"
#include "grammar/rules.hpp"
#include "grammar/stmt.hpp"

#include <algorithm>

namespace chimera::library::grammar {
  constexpr static flags::Flag option = flags::list<flags::IMPORT_ALL>;
  struct SingleInput : seq<opt<token::Token<option, one<';'>>>,
                           sor<NEWLINE, seq<CompoundStmt<option>, NEWLINE>,
                               SimpleStmt<option>>> {
    struct Transform : rules::Stack<asdl::StmtImpl> {
      template <typename Top>
      void success(Top &&top) {
        top.finalize(top, *this);
      }
    };
  };
  struct FileInput : seq<star<sor<NEWLINE, DocString<option>,
                                  token::Token<option, one<';'>>>>,
                         until<eof, Stmt<option>>> {
    struct Transform : rules::Stack<asdl::DocString, asdl::StmtImpl> {
      template <typename Top>
      void success(Top &&top) {
        top.finalize(top, *this);
      }
    };
  };
  struct EvalInput
      : seq<TestList<flags::list<>>, opt<token::Token<flags::list<>, one<';'>>>,
            opt<NEWLINE>, eof> {
    struct Transform : rules::Stack<asdl::ExprImpl> {
      template <typename Top>
      void success(Top &&top) {
        top.finalize(top, *this);
      }
    };
  };
} // namespace chimera::library::grammar
