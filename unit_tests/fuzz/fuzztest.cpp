#include "fuzz/cases.hpp"

#include <fuzztest/fuzztest.h>
#include <gtest/gtest.h>

namespace chimera::library {
  void FuzzerTestOneInput(const std::uint8_t *data, std::size_t size) {
    std::ignore = fuzz_expression_eval(data, size);
    std::ignore = fuzz_interactive_eval(data, size);
    std::ignore = fuzz_file_eval(data, size);
  }
} // namespace chimera::library

void TestOne(const std::string_view &data) {
  chimera::library::FuzzerTestOneInput(
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      reinterpret_cast<const std::uint8_t *>(data.data()), data.size());
}

TEST(FuzzTestSuite, TestOneBasic) { TestOne(""); }

FUZZ_TEST(FuzzTestSuite, TestOne)
    .WithSeeds(fuzztest::ReadFilesFromDirectory(std::getenv("CORPUS_DIR")));
