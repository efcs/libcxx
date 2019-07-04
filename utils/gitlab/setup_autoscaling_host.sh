#!/usr/bin/env bash

apt-get update
apt-get install -y --no-install-recommends \
      ca-certificates \
      gnupg \
      build-essential \
      wget \
      subversion \
      unzip \
      automake \
      python \
      cmake \
      ninja-build \
      curl \
      git \
      gcc-multilib \
      g++-multilib \
      libc6-dev \
      bison \
      flex \
      libtool \
      autoconf \
      binutils-dev \
      binutils-gold \
      software-properties-common \
      gnupg \
      apt-transport-https \
      systemd \
      sysvinit-utils \
      systemd-sysv \
       bash-completion \
    vim \
    apt-utils

# Install docker
curl -sSL https://get.docker.com/ | sh


# Install docker-machine
readonly DOCKER_MACHINE_VER=v0.16.0
readonly base=https://github.com/docker/machine/releases/download/$DOCKER_MACHINE_VER
curl -L $base/docker-machine-$(uname -s)-$(uname -m) >/tmp/docker-machine
install /tmp/docker-machine /usr/local/bin/docker-machine
docker-machine version

# Install additional scripts for docker-machine
readonly script_base=https://raw.githubusercontent.com/docker/machine/$DOCKER_MACHINE_VER
for i in docker-machine-prompt.bash docker-machine-wrapper.bash docker-machine.bash
do
  wget "$script_base/contrib/completion/bash/${i}" -P /etc/bash_completion.d
done

curl -L --output /usr/local/bin/gitlab-runner https://gitlab-runner-downloads.s3.amazonaws.com/latest/binaries/gitlab-runner-linux-amd64
chmod +x /usr/local/bin/gitlab-runner
useradd --comment 'LLVM GitLab Runner' --create-home gitlab-runner --shell /bin/bash

gitlab-runner install --user=gitlab-runner --working-directory=/home/gitlab-runner
