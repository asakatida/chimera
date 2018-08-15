#!/bin/bash

version="$(git rev-parse --show-toplevel)/library/version.hpp"
current_branch=$(git rev-parse --abbrev-ref HEAD)
exact_tag=$(git describe $current_branch --abbrev=0)
description=$(git describe $current_branch)

echo "#pragma once" > $version
echo "" >> $version
echo "#define CHIMERA_VERSION \"$exact_tag\"" >> $version
echo "#define CHIMERA_GIT_HEAD \"$description\"" >> $version
