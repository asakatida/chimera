FROM ubuntu:22.04

COPY requirements.txt /tmp/requirements.txt
COPY tools/boot.yml /tmp/boot.yml
COPY tools/docker.sh /tmp/docker.sh
RUN /tmp/docker.sh && rm /tmp/docker.sh

# this breaks gitpod ssh for some reason
# CMD [ "/usr/bin/fish" ]
