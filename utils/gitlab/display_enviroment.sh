#!/usr/bin/env bash

set -x
set -e

env
pwd
ls
ls | xargs ls
ls ..
ls ../..
clang++ --version
g++ --version
cmake --version
ninja --version
