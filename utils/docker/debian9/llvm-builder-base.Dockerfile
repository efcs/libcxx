#===----------------------------------------------------------------------===//
#
# Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#===----------------------------------------------------------------------===//

FROM launcher.gcr.io/google/debian9:latest AS llvm-builder-base
LABEL maintainer "libc++ Developers"

ARG LIBCXX_ROOT
ARG DOCKER_ROOT

RUN echo aeu ${LIBCXX_ROOT}

