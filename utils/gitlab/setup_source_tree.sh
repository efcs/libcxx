#!/usr/bin/env bash

set -x
set -e

pwd
mkdir -p /tmp/$CI_PROJECT_PATH
mv -t /tmp/$CI_PROJECT_PATH/ *
ls
mkdir build/ stage/ ci/
mv /tmp/$CI_PROJECT_NAMESPACE ci/
git clone --depth=1 https://github.com/llvm/llvm-project.git llvm-project/
touch build/BUILD_HERE
touch stage/STAGE_HERE
ls
