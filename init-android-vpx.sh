#! /usr/bin/env bash

IJK_VPX_UPSTREAM=https://github.com/wiseplay-app/vpx-android.git
IJK_VPX_COMMIT=1.1.7-1
IJK_VPX_LOCAL_REPO=extra/vpx-android

set -e
TOOLS=tools

echo "== pull vpx base =="
sh $TOOLS/pull-repo-base.sh $IJK_VPX_UPSTREAM $IJK_VPX_LOCAL_REPO

echo "== pull vpx fork =="
sh $TOOLS/pull-repo-ref.sh $IJK_VPX_UPSTREAM android/contrib/vpx-android ${IJK_VPX_LOCAL_REPO}
cd android/contrib/vpx-android
git checkout ${IJK_VPX_COMMIT}
cd -
