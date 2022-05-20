#! /usr/bin/env bash

IJK_X264_UPSTREAM=https://code.videolan.org/videolan/x264.git
IJK_X264_COMMIT=2451a728
IJK_X264_LOCAL_REPO=extra/x264

set -e
TOOLS=tools

echo "== pull x264 base =="
sh $TOOLS/pull-repo-base.sh $IJK_X264_UPSTREAM $IJK_X264_LOCAL_REPO

function pull_fork()
{
    echo "== pull x264 fork $1 =="
    sh $TOOLS/pull-repo-ref.sh $IJK_X264_UPSTREAM android/contrib/x264-$1 ${IJK_X264_LOCAL_REPO}
    cd android/contrib/x264-$1
    git checkout ${IJK_X264_COMMIT}
    cd -
}

pull_fork "armv7a"
pull_fork "arm64"
pull_fork "x86"
pull_fork "x86_64"
