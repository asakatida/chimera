FROM debian:unstable

RUN apt-get update
RUN apt-get install -y libc++-dev clang-6.0 cmake ninja-build

ENV CHIMERA_PATH ":/app/stdlib"
ENV CXXFLAG "-stdlib=c++"

WORKDIR /app
ADD . /app

RUN cmake -GNinja -DCMAKE_CXX_COMPILER="clang++-6.0" .
RUN ninja
RUN ninja spec
RUN ninja corpus
