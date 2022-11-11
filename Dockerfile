FROM ubuntu:22.04

COPY requirements.txt /tmp/requirements.txt
COPY tools/boot.yml /tmp/boot.yml
COPY tools/docker.sh /tmp/docker.sh
RUN /tmp/docker.sh && rm /tmp/boot.yml /tmp/docker.sh /tmp/requirements.txt

USER github:github
