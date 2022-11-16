FROM ubuntu:22.04

COPY requirements.txt /tmp/requirements.txt
COPY tools/boot.yml /tmp/boot.yml
COPY tools/docker.sh /tmp/docker.sh
RUN /tmp/docker.sh && rm /tmp/boot.yml /tmp/docker.sh /tmp/requirements.txt

ENV CC /usr/local/bin/clang
ENV CMAKE_C_COMPILER_LAUNCHER ccache
ENV CMAKE_CXX_COMPILER_LAUNCHER ccache
ENV CMAKE_EXPORT_COMPILE_COMMANDS YES
ENV CXX /usr/local/bin/clang++

USER github:github
