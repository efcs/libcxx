#!/usr/bin/env bash

set -e
set -x

gcloud kms decrypt --location global \
  --keyring storage --key llvm-gitlab-secrets \
  --ciphertext-file "$1" \
  --plaintext-file "$2"
