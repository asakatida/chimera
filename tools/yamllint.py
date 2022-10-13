# cd "$(git rev-parse --show-toplevel || true)"

# if [[ -d env/bin ]]; then
#   export PATH="${PWD}/env/bin:${PATH}"
# elif [[ -d /opt/virtualenv/bin ]]; then
#   true
# else
#   python3 tools/venv.py python3
#   export PATH="${PWD}/env/bin:${PATH}"
# fi

# python tools/g-ls-tree.py -name '*.yml' | \
#   python tools/xargs.py -- \
#   yamllint "$@"
