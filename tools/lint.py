# case "$(uname)" in
#   Darwin )
#     export PATH="${PWD}/env/bin:${PATH}"
#     ;;
#   Linux )
#     if command -v apt; then
#       echo 'Apt found, expecting Dockerfile is used'
#     else
#       echo 'No apt found, failed installation'
#       exit 1
#     fi
#     ;;
#   * )
#     uname
#     echo 'unknown os, failed installation'
#     exit 1
#     ;;
# esac

# cmakelint

# python tools/g-ls-tree.py -name '*.py' | \
#   python tools/xargs.py -- \
#   isort --python-version auto
# python tools/g-ls-tree.py -name '*.py' | \
#   python tools/xargs.py -- \
#   black --preview --target-version py311
# python tools/g-ls-tree.py -name '*.py' | \
#   python tools/xargs.py -- \
#   pylama
# python tools/g-ls-tree.py -not -path "./stdlib/*" -name '*.py' | \
#   python tools/xargs.py -- \
#   mypy

# if [[ -z "${PREBUILD_CHECK}" ]]; then
#   python tools/generate_utf8_id_continue.py
#   python tools/generate_utf8_id_start.py
#   python tools/generate_utf8_space.py
#   git ls-tree --full-tree --name-only -r -z HEAD | \
#     python tools/xargs.py -- \
#     python tools/trim.py
#   python tools/clang-format.py
#   python tools/clang_tidy.py build/debug
# fi
