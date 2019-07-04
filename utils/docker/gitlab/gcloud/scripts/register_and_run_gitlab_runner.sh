#!/usr/bin/env bash

set -e
set -x

if [[ -z "$GITLAB_CONFIG_VOLUME" ]]; then
  echo "Error, GITLAB_CONFIG_VOLUME must be defined"
  exit 1
fi
if [[ ! -d "$GITLAB_SCRIPTS" ]]; then
  echo "Error, GITLAB_SCRIPTS must be defined"
  exit 1
fi

export PATH=$PATH:$GITLAB_SCRIPTS/


#mkdir /tmp/secrets
#gsutil cp gs://llvm-gitlab-secrets/gitlab-token.encrypted /tmp/secrets/
#gcloud_decrypt.sh /tmp/secrets/gitlab-token.encrypted /tmp/secrets/gitlab-token.plaintext

readonly TOKEN=$(cat /run/secrets/llvm_gitlab_token)

docker run --rm -v $GITLAB_CONFIG_VOLUME:/etc/gitlab-runner gitlab/gitlab-runner register \
  --non-interactive \
  --executor "docker" \
  --docker-image ericwf/llvm-gitlab-runner-worker:latest \
  --url "https://gitlab.com/" \
  --registration-token "$TOKEN" \
  --description "llvm-docker-runner" \
  --tag-list "docker,google-cloud" \
  --run-untagged="true" \
  --locked="false" \
  --access-level="not_protected"
