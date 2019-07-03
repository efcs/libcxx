#!/usr/bin/env bash

set -e
set -x

readonly REGISTRATION_TOKEN=$1

if [[ -z "$GITLAB_CONFIG_VOLUME" ]]; then
  echo "Error, GITLAB_CONFIG_VOLUME must be defined"
  exit 1
fi

docker run --rm -v $GITLAB_CONFIG_VOLUME:/etc/gitlab-runner gitlab/gitlab-runner register \
  --non-interactive \
  --executor "docker" \
  --docker-image ericwf/llvm-gitlab-runner-worker:latest \
  --url "https://gitlab.com/" \
  --registration-token "$REGISTRATION_TOKEN" \
  --description "llvm-docker-runner" \
  --tag-list "docker,google-cloud" \
  --run-untagged="true" \
  --locked="false" \
  --access-level="not_protected"
