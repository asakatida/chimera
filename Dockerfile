FROM alpine

COPY docker/setup.sh /docker/setup.sh
RUN /docker/setup.sh
