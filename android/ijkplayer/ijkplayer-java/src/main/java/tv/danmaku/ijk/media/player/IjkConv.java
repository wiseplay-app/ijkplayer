/*
 * Copyright (C) 2014-2022 3U Digital Business Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package tv.danmaku.ijk.media.player;

public class IjkConv
{
    /**
     * Default library loader
     * Load them by yourself, if your libraries are not installed at default place.
     */
    private static IjkLibLoader sLocalLibLoader = new IjkLibLoader() {
        @Override
        public void loadLibrary(String libName) throws UnsatisfiedLinkError, SecurityException {
            System.loadLibrary(libName);
        }
    };

    private static volatile boolean mIsLibLoaded = false;
    public static void loadLibrariesOnce(IjkLibLoader libLoader) {
        synchronized (IjkConv.class) {
            if (!mIsLibLoaded) {
                if (libLoader == null)
                    libLoader = sLocalLibLoader;

                libLoader.loadLibrary("ijkffmpeg");
                libLoader.loadLibrary("ijksdl");
                libLoader.loadLibrary("ijkconv");
                mIsLibLoaded = true;
            }
        }
    }

    private static volatile boolean mIsNativeInitialized = false;
    private static void initNativeOnce() {
        synchronized (IjkConv.class) {
            if (!mIsNativeInitialized) {
                mIsNativeInitialized = true;
            }
        }
    }

    public IjkConv() {
        this(sLocalLibLoader);
    }

    public IjkConv(IjkLibLoader libLoader) {
        loadLibrariesOnce(libLoader);
    }

    public native boolean await(int pid);

    public int process(String[] args) {
        String[] params = new String[args.length + 1];

        params[0] = "ffmpeg";
        System.arraycopy(args, 0, params, 1, args.length);

        return run(params);
    }

    private native int run(String[] args);

    public native void stop(int pid);
}
