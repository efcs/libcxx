#!/usr/bin/env bash

set -x
set -e

# Install docker
curl -sSL https://get.docker.com/ | sh


# Install docker-machine
readonly DOCKER_MACHINE_VER=v0.16.0
readonly base=https://github.com/docker/machine/releases/download/$DOCKER_MACHINE_VER
curl -L $base/docker-machine-$(uname -s)-$(uname -m) >/tmp/docker-machine
sudo install /tmp/docker-machine /usr/local/bin/docker-machine
sudo docker-machine version

# Install additional scripts for docker-machine
readonly script_base=https://raw.githubusercontent.com/docker/machine/$DOCKER_MACHINE_VER
for i in docker-machine-prompt.bash docker-machine-wrapper.bash docker-machine.bash
do
  sudo wget "$script_base/contrib/completion/bash/${i}" -P /etc/bash_completion.d
done
