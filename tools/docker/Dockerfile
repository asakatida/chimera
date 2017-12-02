FROM debian:unstable

RUN apt-get update
RUN apt-get install -y libstdc++-7-dev g++-7 cmake ninja-build

RUN mkdir build
WORKDIR build

CMD cmake -GNinja ../chimera && ninja && ninja spec && ninja corpus
