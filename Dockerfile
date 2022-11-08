FROM asakatida/python:3.12

ENV TZ Etc/UTC

COPY requirements.txt /tmp/requirements.txt
COPY tools/boot.yml /tmp/boot.yml

RUN <<SHELL bash
    export DEBIAN_FRONTEND=noninteractive
    set -ex
    /usr/local/bin/python3.12 -m venv /tmp/env
    /tmp/env/bin/pip install --upgrade pip setuptools wheel
    /tmp/env/bin/pip install -r /tmp/requirements.txt
    /tmp/env/bin/ansible-playbook /tmp/boot.yml
    rm -rf /tmp/env
    rm /tmp/boot.yml /tmp/requirements.txt
SHELL

USER github:github

ENV CC /usr/local/bin/clang
ENV CMAKE_C_COMPILER_LAUNCHER ccache
ENV CMAKE_CXX_COMPILER_LAUNCHER ccache
ENV CMAKE_EXPORT_COMPILE_COMMANDS YES
ENV CXX /usr/local/bin/clang++
