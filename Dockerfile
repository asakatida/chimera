FROM ghcr.io/asakatida/chimera:python3.13 AS builder

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
        python3-dev=3.11.3-r11
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
    CPPFLAGS="-std=c++20" \
    CXX=clang++
ENV CXXFLAGS="${CPPFLAGS}"
ENV LDFLAGS="${CXXFLAGS} -rtlib=compiler-rt"
