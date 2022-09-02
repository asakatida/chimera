#!/usr/bin/env bash

set -ex -o pipefail

apt-get update
apt-get install --yes </dev/null \
  apt \
  apt-transport-https \
  apt-utils \
  aptitude
apt-get update
apt-get install --yes </dev/null \
  adduser \
  autoconf \
  automake \
  base-files \
  base-passwd \
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
  libapt-pkg6.0 \
  libattr1 \
  libaudit-common \
  libaudit1 \
  libblkid1 \
  libbz2-1.0 \
  libbz2-dev \
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
apt-get autoclean
apt-get clean
rm -rf /var/cache/apt/* /var/lib/apt/lists/*

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
scan_build_cmd="$(command -v scan-build-15)"
ln -s "${clang___cmd}" /usr/local/bin/clang++
ln -s "${clang_cmd}" /usr/local/bin/clang
ln -s "${clang_format_cmd}" /usr/local/bin/clang-format
ln -s "${clang_tidy_cmd}" /usr/local/bin/clang-tidy
ln -s "${scan_build_cmd}" /usr/local/bin/scan-build

# curl -fsSL https://tailscale.com/install.sh | sh
