#! /usr/bin/env bash
#
# Copyright (C) 2019-2022 3U Digital Business Ltd
# Copyright (C) 2023 Apollo Apps, LLC
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

IJK_DAV1D_UPSTREAM=git@github.com:wiseplay-app/ijkplayer-dav1d-android.git
IJK_DAV1D_COMMIT=main
IJK_DAV1D_LOCAL_REPO=extra/dav1d

set -e
TOOLS=tools

echo "== pull dav1d base =="
sh $TOOLS/pull-repo-base.sh $IJK_DAV1D_UPSTREAM $IJK_DAV1D_LOCAL_REPO

echo "== pull dav1d fork =="
sh $TOOLS/pull-repo-ref.sh $IJK_DAV1D_UPSTREAM android/contrib/dav1d ${IJK_DAV1D_LOCAL_REPO}

echo "Done!"