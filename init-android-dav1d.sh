#! /usr/bin/env bash

IJK_DAV1D_UPSTREAM=https://gitlab.com/wiseplay/ijkplayer-dav1d-android.git
IJK_DAV1D_COMMIT=master
IJK_DAV1D_LOCAL_REPO=extra/dav1d

set -e
TOOLS=tools

echo "== pull dav1d base =="
sh $TOOLS/pull-repo-base.sh $IJK_DAV1D_UPSTREAM $IJK_DAV1D_LOCAL_REPO

echo "== pull dav1d fork =="
sh $TOOLS/pull-repo-ref.sh $IJK_DAV1D_UPSTREAM android/contrib/dav1d ${IJK_DAV1D_LOCAL_REPO}
