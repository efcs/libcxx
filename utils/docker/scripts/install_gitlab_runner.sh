#!/usr/bin/env bash

set -e
set -x

curl -L --output /usr/local/bin/gitlab-runner https://gitlab-runner-downloads.s3.amazonaws.com/latest/binaries/gitlab-runner-linux-amd64
chmod +x /usr/local/bin/gitlab-runner
useradd --comment 'GitLab Runner' --create-home gitlab-runner --shell /bin/bash

# Create the directory used for the gitlab runner config

if [[ -z "$GITLAB_CONFIG_VOLUME" ]]; then
  echo "Error, GITLAB_CONFIG_VOLUME must be defined"
  exit 1
fi
if [[ -z "$GITLAB_TOOLS" ]]; then
  echo "Error, GITLAB_TOOLS must be defined"
  exit 1
fi

mkdir $GITLAB_CONFIG_VOLUME $GITLAB_TOOLS

cp gitlab/config.toml $GITLAB_CONFIG_VOLUME/config.toml
cp scripts/register_gitlab_runner.sh $GITLAB_TOOLS/
