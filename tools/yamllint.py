# cd "$(git rev-parse --show-toplevel || true)"

# if [[ -d env/bin ]]; then
#   export PATH="${PWD}/env/bin:${PATH}"
# elif [[ -d /opt/virtualenv/bin ]]; then
#   export PATH="/opt/virtualenv/bin:${PATH}"
# else
#   python3 tools/venv.py python3
#   export PATH="${PWD}/env/bin:${PATH}"
# fi

# python3 tools/g-ls-tree.py -name '*.yml' | \
#   python3 tools/xargs.py -- \
#   yamllint "$@"
