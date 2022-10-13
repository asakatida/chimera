# cd "$(git rev-parse --show-toplevel || true)"
# build=build/clang-ninja

# python3 tools/cmake.py "${PWD}/${build}"

# python3 tools/ninja.py "${build}"
# python3 tools/clang-format.py
# python3 tools/ninja.py "${build}"
# python3 tools/clang_tidy.py "${build}"
