#!/usr/bin/env bash

set -ex -o pipefail

cleanup() {
  truncate -s0 /etc/machine-id
  apt-get autoclean
  apt-get clean
  # rm -rf /etc/{bash_completion.d,cron.{daily,weekly},kernel} /var/cache/apt/* /var/lib/apt/lists/* /var/log
  # find /var/cache
  # rm -rf /etc/{bash_completion.d,cron.{daily,weekly},kernel} /var/{cache,log}
}
cleanup
trap cleanup EXIT

apt-get update
apt-get install --yes </dev/null \
  apt \
  apt-transport-https \
  apt-utils \
  aptitude \
  libapt-pkg6.0
apt-get update
apt-get install --yes </dev/null \
  adduser \
  autoconf \
  automake \
  base-files \
  base-passwd \
  bash \
  brz \
  bsdutils \
  build-essential \
  bzip2 \
  bzr \
  ca-certificates \
  ccache \
  cmake \
  coreutils \
  curl \
  dash \
  debconf \
  debianutils \
  default-libmysqlclient-dev \
  diffutils \
  dirmngr \
  docker \
  dpkg \
  dpkg-dev \
  e2fsprogs \
  fdisk \
  file \
  findutils \
  fish \
  gcc-12-base \
  gettext \
  git \
  git-lfs \
  gnupg \
  gnupg-agent \
  gpgv \
  grep \
  gzip \
  hostname \
  htop \
  imagemagick \
  imagemagick-6.q16 \
  init-system-helpers \
  jq \
  less \
  libacl1 \
  libattr1 \
  libaudit-common \
  libaudit1 \
  libblkid1 \
  libbz2-1.0 \
  libbz2-dev \
  libc-bin \
  libc6 \
  libcap-ng0 \
  libcap2 \
  libcom-err2 \
  libcrypt1 \
  libcurl4-openssl-dev \
  libdb-dev \
  libdb5.3 \
  libdebconfclient0 \
  libdpkg-perl \
  libevent-dev \
  libext2fs2 \
  libfdisk1 \
  libffi-dev \
  libffi8 \
  libgcc-s1 \
  libgcrypt20 \
  libgdbm-dev \
  libglib2.0-0 \
  libglib2.0-dev \
  libgmp-dev \
  libgmp10 \
  libgnutls30 \
  libgpg-error0 \
  libgssapi-krb5-2 \
  libhogweed6 \
  libidn2-0 \
  libjpeg-dev \
  libk5crypto3 \
  libkeyutils1 \
  libkrb5-3 \
  libkrb5-dev \
  libkrb5support0 \
  liblz4-1 \
  liblzma-dev \
  liblzma5 \
  libmagickcore-6.q16-6 \
  libmagickcore-dev \
  libmagickwand-dev \
  libmaxminddb-dev \
  libmount1 \
  libncurses5-dev \
  libncurses6 \
  libncursesw5-dev \
  libncursesw6 \
  libnettle8 \
  libnsl2 \
  libp11-kit0 \
  libpam-modules \
  libpam-modules-bin \
  libpam-runtime \
  libpam0g \
  libpcre2-8-0 \
  libpcre3 \
  libpng-dev \
  libpq-dev \
  libprocps8 \
  libreadline-dev \
  libsasl2-2 \
  libseccomp2 \
  libselinux1 \
  libsemanage-common \
  libsemanage2 \
  libsepol2 \
  libsmartcols1 \
  libsqlite3-dev \
  libss2 \
  libssl-dev \
  libssl3 \
  libstdc++6 \
  libsystemd0 \
  libtasn1-6 \
  libtinfo6 \
  libtirpc-common \
  libtirpc3 \
  libtool \
  libudev1 \
  libunistring2 \
  libuuid1 \
  libwebp-dev \
  libwmf0.2-7 \
  libxml2-dev \
  libxslt1-dev \
  libxxhash0 \
  libyaml-dev \
  libzstd1 \
  locales \
  login \
  logsave \
  lsb-base \
  lsb-release \
  lsof \
  make \
  man-db \
  mawk \
  mount \
  multitail \
  ncurses-base \
  ncurses-bin \
  netbase \
  ninja-build \
  openssh-client \
  passwd \
  patch \
  perl-base \
  procps \
  python3-neovim \
  python3-venv \
  ripgrep \
  sed \
  sensible-utils \
  shellcheck \
  software-properties-common \
  ssl-cert \
  stow \
  sudo \
  sysvinit-utils \
  tar \
  time \
  ubuntu-keyring \
  unzip \
  usrmerge \
  util-linux \
  wget \
  xz-utils \
  zip \
  zlib1g \
  zlib1g-dev

python3 -m venv /tmp/env
/tmp/env/bin/pip install --upgrade pip setuptools wheel
/tmp/env/bin/pip install -r /tmp/requirements.txt
/tmp/env/bin/ansible-playbook /tmp/boot.yml
rm -rf /tmp/env
python3.12 -m venv /opt/virtualenv
/opt/virtualenv/bin/pip install --upgrade pip setuptools wheel
/opt/virtualenv/bin/pip install -r /tmp/requirements.txt

fish_cmd="$(command -v fish)"
chsh -s "${fish_cmd}"
fish -c 'set -U fish_user_paths /usr/local/bin'

curl https://apt.llvm.org/llvm.sh -o llvm.sh
chmod +x llvm.sh
./llvm.sh 15 all
rm llvm.sh
clang___cmd="$(command -v clang++-15)"
clang_cmd="$(command -v clang-15)"
clang_format_cmd="$(command -v clang-format-15)"
clang_tidy_cmd="$(command -v clang-tidy-15)"
llvm_cov_cmd="$(command -v llvm-cov-15)"
llvm_profdata_cmd="$(command -v llvm-profdata-15)"
scan_build_cmd="$(command -v scan-build-15)"
ln -s "${clang___cmd}" /usr/local/bin/clang++
ln -s "${clang_cmd}" /usr/local/bin/clang
ln -s "${clang_format_cmd}" /usr/local/bin/clang-format
ln -s "${clang_tidy_cmd}" /usr/local/bin/clang-tidy
ln -s "${llvm_cov_cmd}" /usr/local/bin/llvm-cov
ln -s "${llvm_profdata_cmd}" /usr/local/bin/llvm-profdata
ln -s "${scan_build_cmd}" /usr/local/bin/scan-build

groupadd --gid 122 github
useradd \
  --create-home \
  --gid 122 \
  --groups sudo \
  --no-user-group \
  --shell "${fish_cmd}" \
  --uid 1001 \
  github
cat <<SUDOERS >/etc/sudoers.d/github
github ALL=(ALL) NOPASSWD:ALL
SUDOERS

# curl -fsSL https://tailscale.com/install.sh | sh
