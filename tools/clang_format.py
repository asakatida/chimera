# cd "$(git rev-parse --show-toplevel || true)"

# python tools/g-ls-tree.py '(' -name '*.cpp' -or -name '*.h' -or -name '*.hpp' ')' | \
#   python tools/xargs.py -n1 -P3 -- \
#   clang-format -style=file -i "$@"
