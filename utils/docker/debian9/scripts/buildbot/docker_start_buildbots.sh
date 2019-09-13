#!/usr/bin/env bash
set -x

# Update the libc++ sources in the image in order to use the most recent version of
# run_buildbots.sh
cd /libcxx
git pull
/libcxx/utils/docker/debian9/scripts/buildbot/run_buildbot.sh "$@"
