#!/usr/bin/env bash

set -e
set -x

gcloud kms encrypt --location global \
  --keyring storage --key llvm-gitlab-secrets \
  --plaintext-file "$1" \
  --ciphertext-file "$2"
