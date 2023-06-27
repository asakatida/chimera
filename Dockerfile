FROM alpine:3.18 AS builder

ENV TZ Etc/UTC

RUN <<SHELL sh
    set -ex
    apk update
    apk add --no-cache \
        ca-certificates=20230506-r0 \
        git=2.40.1-r0
    apk add --no-cache \
        python3-dev=3.11.4-r0
    apk add --no-cache \
        build-base=0.5-r3 \
        gdb=13.1-r8 \
        gdbm-dev=1.23-r1 \
        libbz2=1.0.8-r5 \
        libffi-dev=3.4.4-r2 \
        libncursesw=6.4_p20230506-r0 \
        openssl-dev=3.1.1-r1 \
        readline-dev=8.2.1-r1 \
        sqlite-dev=3.41.2-r2 \
        xz-dev=5.4.3-r0 \
        zlib-dev=1.2.13-r1
    apk cache clean
    rm -rf /var/cache/apk/*
SHELL

RUN git clone --depth 1 https://github.com/python/cpython.git /tmp/cpython
WORKDIR /tmp/cpython
RUN <<SHELL sh
    set -ex
    ./configure
    make -j3
    ./python -c 'import sys; assert "3.13" < sys.version < "3.14", sys.version'
    make install
SHELL

FROM alpine:3.18

COPY --from=builder /usr/local/bin/python3.13 /usr/local/bin/python3.13
COPY --from=builder /usr/local/lib/python3.13 /usr/local/lib/python3.13

ENV TZ Etc/UTC

COPY . /tmp

RUN <<SHELL sh
    set -ex
    apk update
    apk add --no-cache \
        gcc=12.2.1_git20220924-r10 \
        musl-dev=1.2.4-r0 \
        python3-dev=3.11.4-r0
    python3 -m venv /tmp/env
    /tmp/env/bin/pip install -r /tmp/requirements.core.txt
    /tmp/env/bin/pip install -r /tmp/requirements.txt
    /tmp/env/bin/ansible-playbook /tmp/tools/boot.yml
    rm -rf /root/.ansible /root/.cache /tmp/* /var/tmp/*
    apk cache clean
    rm -rf /var/cache/apk/*
SHELL

USER github:github

ENV CC=clang \
    CMAKE_BUILD_TYPE=Debug \
    CMAKE_C_COMPILER_LAUNCHER=ccache \
    CMAKE_CXX_COMPILER_LAUNCHER=ccache \
    CMAKE_EXPORT_COMPILE_COMMANDS=YES \
    CTEST_PARALLEL_LEVEL=3 \
    CXX=clang++ \
    LDFLAGS="-lgcc_s"
