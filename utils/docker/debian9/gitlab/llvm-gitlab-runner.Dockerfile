
#===-------------------------------------------------------------------------------------------===//
# llvm-gitlab-runner-working - The image used by gitlab runners for LLVM
#===-------------------------------------------------------------------------------------------===//
FROM ericwf/builder-base:latest AS llvm-gitlab-runner-worker

# Copy over the GCC and Clang installations
COPY --from=ericwf/gcc-5:latest /opt/gcc-5 /opt/gcc-5
COPY --from=ericwf/gcc-tot:latest /opt/gcc-tot /opt/gcc-tot
COPY --from=ericwf/llvm-4:latest /opt/llvm-4.0 /opt/llvm-4.0

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    bash-completion \
    vim \
    sudo \
    apt-utils \
  && rm -rf /var/lib/apt/lists/*

ADD utils/docker/scripts/install_clang_packages.sh /tmp/install_clang_packages.sh
RUN /tmp/install_clang_packages.sh && rm /tmp/install_clang_packages.sh

RUN git clone https://git.llvm.org/git/libcxx.git /libcxx