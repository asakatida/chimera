#!/bin/bash

set -ex -o pipefail

version="$(git rev-parse --show-toplevel)/library/version.hpp"
current_branch="$(git rev-parse --abbrev-ref HEAD)"
exact_tag="$(git describe "${current_branch}" --abbrev=0)"
description="$(git describe "${current_branch}")"

cat <<-VERSION >"${version}"
#pragma once

#define CHIMERA_VERSION "${exact_tag}"
#define CHIMERA_GIT_HEAD "${description}"
VERSION
