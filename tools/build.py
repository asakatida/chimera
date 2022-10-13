# env | cut -f1 -d= | sort -u

# cd "$(git rev-parse --show-toplevel || true)"
# debug_root=build/debug
# release_root=build/release

# export PATH="${PWD}/env/bin:${PATH}"

# python3 tools/lint.py

# export LLVM_PROFILE_FILE="${PWD}/build/coverage/llvm-profile.%c%p.profraw"

# python3 tools/ninja.py "${debug_root}"
# python3 tools/ninja.py "${debug_root}"
# python3 tools/ninja.py "${release_root}"
# python3 tools/ninja.py "${debug_root}" check-rand
# python3 tools/ninja.py "${debug_root}" regression
# python3 tools/ninja.py "${debug_root}" corpus
# python3 tools/ninja.py "${debug_root}" spec
# python3 tools/ninja.py "${release_root}" spec

# docker build -t chimera/base "tools/docker/base/"
# docker build -t chimera/clang "tools/docker/clang/"
# docker build -t chimera/gcc "tools/docker/gcc/"

# docker-exec chimera/clang python3 tools/clang-fix.py
# docker-exec chimera/gcc python3 tools/gcc-sanity.py

# docker-exec chimera/clang python3 tools/ninja-intense.py build/clang
# docker-exec chimera/gcc python3 tools/ninja-intense.py build/gcc
