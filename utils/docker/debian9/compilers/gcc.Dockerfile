#===- libcxx/utils/docker/debian9/Dockerfile --------------------------------------------------===//
#
# Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#===-------------------------------------------------------------------------------------------===//

# Build GCC versions
FROM ericwf/llvm-builder-base:latest
LABEL maintainer "libc++ Developers"

WORKDIR $LIBCXX_ROOT/utils/docker/debian9/compilers

ARG install_prefix
ARG branch
ARG cherry_pick=""

ADD scripts/build_gcc_version.sh
RUN scripts/build_gcc_version.sh \
    --install "$install_prefix" \
    --branch "$branch" \
    --cherry-pick "$cherry_pick"
