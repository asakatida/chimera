FROM ubuntu:22.04

COPY tools/docker.sh /tmp/docker.sh
RUN /tmp/docker.sh && rm /tmp/docker.sh

# this breaks gitpod ssh for some reason
# CMD [ "/usr/bin/fish" ]
