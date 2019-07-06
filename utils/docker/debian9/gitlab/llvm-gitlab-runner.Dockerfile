
#===-------------------------------------------------------------------------------------------===//
# llvm-gitlab-runner-working - The image used by gitlab runners for LLVM
#===-------------------------------------------------------------------------------------------===//
FROM ericwf/llvm-builder-base:latest AS llvm-gitlab-runner

# Copy over the GCC and Clang installations
COPY --from=ericwf/gcc-5:latest /opt/gcc-5 /opt/gcc-5
COPY --from=ericwf/gcc-tot:latest /opt/gcc-tot /opt/gcc-tot
COPY --from=ericwf/llvm-4:latest /opt/llvm-4.0 /opt/llvm-4.0

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    apt-utils \
  && rm -rf /var/lib/apt/lists/*

ADD ../../scripts/buildbot/scripts/install_clang_packages.sh /tmp/install_clang_packages.sh
RUN /tmp/install_clang_packages.sh && rm /tmp/install_clang_packages.sh

RUN useradd --comment 'GitLab Runner' --create-home gitlab-runner --shell /bin/bash
USER gitlab-runner
ADD config/config.toml /
