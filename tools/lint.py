# case "$(uname)" in
#   Darwin )
#     export PATH="${PWD}/env/bin:${PATH}"
#     ;;
#   Linux )
#     if command -v apt; then
#       echo 'Apt found, expecting Dockerfile is used'
#       export PATH="/opt/virtualenv/bin:${PATH}"
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

# python3 tools/g-ls-tree.py -name '*.py' | \
#   python3 tools/xargs.py -- \
#   isort --python-version auto
# python3 tools/g-ls-tree.py -name '*.py' | \
#   python3 tools/xargs.py -- \
#   black --preview --target-version py311
# python3 tools/g-ls-tree.py -name '*.py' | \
#   python3 tools/xargs.py -- \
#   pylama
# python3 tools/g-ls-tree.py -not -path "./stdlib/*" -name '*.py' | \
#   python3 tools/xargs.py -- \
#   mypy

# if [[ -z "${PREBUILD_CHECK}" ]]; then
#   python3 tools/generate_utf8_id_continue.py
#   python3 tools/generate_utf8_id_start.py
#   python3 tools/generate_utf8_space.py
#   git ls-tree --full-tree --name-only -r -z HEAD | \
#     python3 tools/xargs.py -- \
#     python3 tools/trim.py
#   python3 tools/clang-format.py
#   python3 tools/clang_tidy.py build/debug
# fi
