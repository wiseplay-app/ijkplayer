#! /usr/bin/env bash
#
# Copyright (C) 2021-2022 3U Digital Business Ltd
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

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
