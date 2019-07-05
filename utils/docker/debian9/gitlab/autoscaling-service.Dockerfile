
#===-------------------------------------------------------------------------------------------===//
# llvm-gitlab-runner-service - The image used to run gitlab-runner workers
#===-------------------------------------------------------------------------------------------===//
FROM ericwf/builder-base:latest AS llvm-gitlab-runner-service


ENV GITLAB_CONFIG_VOLUME=/srv/llvm-gitlab-runner-config
ENV GITLAB_SCRIPTS=/llvm-gitlab-scripts

# Install Docker (used by gitlab runner)
ADD scripts/install_docker.sh /tmp/install_docker.sh
RUN /tmp/install_docker.sh && rm /tmp/install_docker.sh

# Install gcloud SDK
ADD scripts/install_cloud_sdk.sh /tmp/
RUN /tmp/install_cloud_sdk.sh && rm /tmp/install_cloud_sdk.sh


RUN curl -L --output /usr/local/bin/gitlab-runner https://gitlab-runner-downloads.s3.amazonaws.com/latest/binaries/gitlab-runner-linux-amd64 \
  &&  chmod +x /usr/local/bin/gitlab-runner \
  && useradd --comment 'LLVM GitLab Runner' --create-home gitlab-runner --shell /bin/bash \
  && sudo gitlab-runner install --user=gitlab-runner --working-directory=/home/gitlab-runner



ADD config/ /srv/gitlab-runner/config
ADD config/ /etc/gitlab-runner

ARG GITLAB_TOKEN
RUN echo "$GITLAB_TOKEN"



CMD gitlab-runner \
  register \
  --non-interactive \
  --executor "docker+machine" \
  --registration-token "zxtgxrWApsiKsdxUgi5i" \
  --url "https://gitlab.com/" \
  --description "llvm-docker-runner" \
  --tag-list "docker,google-cloud"
