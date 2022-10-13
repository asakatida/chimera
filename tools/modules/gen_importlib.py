# build="$1"
# shift
# cd "$(git rev-parse --show-toplevel || true)"
# output=stdlib/importlib/importlib.cpp

# "${build}/importlib" "$@" < stdlib/_importlib.py | \
#   clang-format -style=file >"${output}"

# clang-tidy \
#   -p="${build}" -quiet -fix -fix-errors -fix-notes \
#   "${output}"
