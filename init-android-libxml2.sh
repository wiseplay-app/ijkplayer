#! /usr/bin/env bash

IJK_LIBXML2_UPSTREAM=https://github.com/wiseplay-app/libxml2-android.git
IJK_LIBXML2_COMMIT=master
IJK_LIBXML2_LOCAL_REPO=extra/libxml2

set -e
TOOLS=tools

echo "== pull libxml2 base =="
sh $TOOLS/pull-repo-base.sh $IJK_LIBXML2_UPSTREAM $IJK_LIBXML2_LOCAL_REPO

echo "== pull libxml2 fork =="
sh $TOOLS/pull-repo-ref.sh $IJK_LIBXML2_UPSTREAM android/contrib/libxml2 ${IJK_LIBXML2_LOCAL_REPO}
