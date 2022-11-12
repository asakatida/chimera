#!/usr/bin/env bash

set -ex -o pipefail

cleanup() {
  truncate -s0 /etc/machine-id
  apt-get autoclean
  apt-get clean
  # rm -rf /etc/{bash_completion.d,cron.{daily,weekly},kernel} /var/cache/apt/* /var/lib/apt/lists/* /var/log
  # find /var/cache
  # rm -rf /etc/{bash_completion.d,cron.{daily,weekly},kernel} /var/{cache,log}
  rm -rf /tmp/env
}
cleanup
trap cleanup EXIT

apt-get update
apt-get install --no-install-recommends --yes </dev/null \
   apt \
   apt-transport-https \
   apt-utils \
   aptitude \
   libapt-pkg6.0
 apt-get update
 apt-get install --no-install-recommends --yes </dev/null \
   adduser \
   python3-venv \
   sudo

groupadd --gid 122 github
useradd \
  --create-home \
  --gid 122 \
  --no-user-group \
  --uid 1001 \
  github
cat <<SUDOERS >/etc/sudoers.d/github
github ALL=(ALL) NOPASSWD:ALL
SUDOERS

python3 -m venv /tmp/env
/tmp/env/bin/pip install --upgrade pip setuptools wheel
/tmp/env/bin/pip install -r /tmp/requirements.txt
/tmp/env/bin/ansible-playbook /tmp/boot.yml
