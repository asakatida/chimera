# cd "$(git rev-parse --show-toplevel || true)"
# build_root=build/debug

# rm -rf "${build_root}"

# python3 tools/boot.py

# python3 tools/ninja.py "${build_root}"
# python3 tools/ninja.py "${build_root}" check
# python3 tools/ninja.py "${build_root}" check-rand
