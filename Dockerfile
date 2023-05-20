FROM ghcr.io/asakatida/chimera:python3.13 AS builder

FROM ubuntu:23.04

COPY --from=builder /usr/local/bin/python3.13 /usr/local/bin/python3.13
COPY --from=builder /usr/local/lib/python3.13 /usr/local/lib/python3.13

ENV TZ Etc/UTC

COPY . /tmp

RUN <<SHELL bash
    export DEBIAN_FRONTEND=noninteractive
    set -ex
    apt-get update
    apt-get install --no-install-recommends --yes \
        apt-transport-https=2.6.0 \
        apt-utils=2.6.0 \
        apt=2.6.0 \
        aptitude=0.8.13-5ubuntu1 \
        libapt-pkg6.0=2.6.0
    apt-get update
    apt-get install --no-install-recommends --yes \
        python3-venv=3.11.2-1
    python3 -m venv /tmp/env
    /tmp/env/bin/pip install -r /tmp/requirements.core.txt
    /tmp/env/bin/pip install -r /tmp/requirements.txt
    /tmp/env/bin/ansible-playbook /tmp/tools/boot.yml
    rm -rf /tmp/*
    apt-get clean
    rm -rf /var/lib/apt/lists
SHELL

USER github:github

ENV CC=clang \
    CMAKE_BUILD_TYPE=Debug \
    CMAKE_C_COMPILER_LAUNCHER=ccache \
    CMAKE_CXX_COMPILER_LAUNCHER=ccache \
    CMAKE_EXPORT_COMPILE_COMMANDS=YES \
    CPPFLAGS="-std=c++20" \
    CXX=clang++
ENV CXXFLAGS="${CPPFLAGS}"
ENV LDFLAGS="${CXXFLAGS} -rtlib=compiler-rt"
