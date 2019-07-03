#!/usr/bin/env bash

set -x
set -e

# Install docker
curl -sSL https://get.docker.com/ | sh

# Install docker-machine
readonly base=https://github.com/docker/machine/releases/download/v0.16.0
curl -L $base/docker-machine-$(uname -s)-$(uname -m) >/tmp/docker-machine
sudo install /tmp/docker-machine /usr/local/bin/docker-machine
sudo docker-machine version

# Install additional scripts for docker-machine
for i in docker-machine-prompt.bash docker-machine-wrapper.bash docker-machine.bash
do
  sudo wget "$base/contrib/completion/bash/${i}" -P /etc/bash_completion.d
done
