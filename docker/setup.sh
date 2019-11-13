#!/usr/bin/env sh

set -ex

apk add --update afl build-base clang-dev cmake ninja
# apk add --update --repository http://dl-cdn.alpinelinux.org/alpine/edge/community/ creduce

rm /docker/setup.sh
