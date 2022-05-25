#! /usr/bin/env bash
#
# Copyright (C) 2014-2022 3U Digital Business Ltd
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

IJK_LIBXML2_UPSTREAM=https://github.com/wiseplay-app/libxml2-android.git
IJK_LIBXML2_COMMIT=master
IJK_LIBXML2_LOCAL_REPO=extra/libxml2

set -e
TOOLS=tools

echo "== pull libxml2 base =="
sh $TOOLS/pull-repo-base.sh $IJK_LIBXML2_UPSTREAM $IJK_LIBXML2_LOCAL_REPO

echo "== pull libxml2 fork =="
sh $TOOLS/pull-repo-ref.sh $IJK_LIBXML2_UPSTREAM android/contrib/libxml2 ${IJK_LIBXML2_LOCAL_REPO}
