# cd "$(git rev-parse --show-toplevel || true)"
# build=build/clang-ninja

# python tools/cmake.py "${PWD}/${build}"

# python tools/ninja.py "${build}"
# python tools/clang-format.py
# python tools/ninja.py "${build}"
# python tools/clang_tidy.py "${build}"
