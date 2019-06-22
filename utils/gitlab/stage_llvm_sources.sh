#!/usr/bin/env bash

set -x
set -e

echo "Reconfiguring source layout for $CI_PROJECT_DIR"

pushd $CI_PROJECT_DIR
mkdir -p /tmp/$CI_PROJECT_PATH
mv -t /tmp/$CI_PROJECT_PATH/ *
mkdir build/ stage/ ci/
mv /tmp/$CI_PROJECT_NAMESPACE ci/
git clone --depth=1 https://github.com/llvm/llvm-project.git llvm-project/
rm -rf llvm-project/.git/
popd
