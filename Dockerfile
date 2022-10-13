FROM ubuntu:22.10

COPY requirements.txt /tmp/requirements.txt
COPY tools/asyncio_cmd.py /tmp/asyncio_cmd.py
COPY tools/boot.yml /tmp/boot.yml
COPY tools/docker.py /tmp/docker.py
COPY tools/llvm.py /tmp/llvm.py
RUN apt-get update && \
    apt-get install --yes python3-venv && \
    python3 -m venv /tmp/env && \
    /tmp/env/bin/pip install --upgrade pip setuptools wheel && \
    /tmp/env/bin/pip install -r /tmp/requirements.txt && \
    /tmp/env/bin/python /tmp/docker.py && \
    rm -rf /tmp/env && \
    rm /tmp/boot.yml /tmp/docker.py /tmp/requirements.txt
