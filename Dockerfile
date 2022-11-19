FROM ubuntu:22.04

ENV TZ Etc/UTC

COPY requirements.txt /tmp/requirements.txt
COPY tools/boot.yml /tmp/boot.yml

RUN <<SHELL bash
    export DEBIAN_FRONTEND=noninteractive
    set -ex
    apt-get update
    apt-get install --no-install-recommends --yes \
        apt-transport-https=2.4.8 \
        apt-utils=2.4.8 \
        apt=2.4.8 \
        aptitude=0.8.13-3ubuntu1 \
        libapt-pkg6.0=2.4.8
    apt-get update
    apt-get install --no-install-recommends --yes \
        adduser=3.118ubuntu5 \
        python3-venv=3.10.6-1~22.04 \
        sudo=1.9.9-1ubuntu2.1
    python3 -m venv /tmp/env
    /tmp/env/bin/pip install --upgrade pip setuptools wheel
    /tmp/env/bin/pip install -r /tmp/requirements.txt
    /tmp/env/bin/ansible-playbook /tmp/boot.yml
    apt-get clean
    rm -rf /var/lib/apt/lists
    rm -rf /tmp/env
    rm /tmp/boot.yml /tmp/requirements.txt
SHELL

USER github:github

ENV CC /usr/local/bin/clang
ENV CMAKE_C_COMPILER_LAUNCHER ccache
ENV CMAKE_CXX_COMPILER_LAUNCHER ccache
ENV CMAKE_EXPORT_COMPILE_COMMANDS YES
ENV CXX /usr/local/bin/clang++
