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

import tv.danmaku.ijk.media.player.annotations.AccessedByNative;

public class IjkReader implements IMediaOptions
{
    @AccessedByNative
    private long mNativeContext;

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
        synchronized (IjkReader.class) {
            if (!mIsLibLoaded) {
                if (libLoader == null)
                    libLoader = sLocalLibLoader;

                libLoader.loadLibrary("ijkffmpeg");
                libLoader.loadLibrary("ijksdl");
                libLoader.loadLibrary("ijkreader");
                mIsLibLoaded = true;
            }
        }
    }

    private static volatile boolean mIsNativeInitialized = false;
    private static void initNativeOnce() {
        synchronized (IjkReader.class) {
            if (!mIsNativeInitialized) {
                native_init();
                mIsNativeInitialized = true;
            }
        }
    }

    public IjkReader() {
        this(sLocalLibLoader);
    }

    public IjkReader(IjkLibLoader libLoader) {
        loadLibrariesOnce(libLoader);
        initNativeOnce();

        native_setup();
    }

    /**
     * Sets the data source (path) to use. Call this
     * method before the rest of the methods in this class.
     *
     * @param path The url of the input media file.
     * @throws IllegalArgumentException If the path is invalid.
     */
    public native void setDataSource(String path) throws IllegalArgumentException;

    public void setOption(int category, String name, String value)
    {
        _setOption(category, name, value);
    }

    public void setOption(int category, String name, long value)
    {
        _setOption(category, name, value);
    }

    private native void _setOption(int category, String name, String value);
    private native void _setOption(int category, String name, long value);

    /**
     * Reads data from the stream to the specified buffer.
     *
     * @param buffer The buffer where store the data.
     * @return The read data size.
     * @throws IllegalArgumentException If the buffer is null.
     */
    public int read(byte[] buffer) throws IllegalArgumentException {
        return read(buffer, buffer.length);
    }

    /**
     * Reads data from the stream to the specified buffer.
     *
     * @param buffer The buffer where store the data.
     * @param size The amount of data to read.
     * @return The read data size.
     * @throws IllegalArgumentException If the buffer is null or the specified size is zero or negative.
     */
    public native int read(byte[] buffer, int size) throws IllegalArgumentException;

    /**
     * Call it when one is done with the object. This method releases the memory
     * allocated internally.
     */
    public native void release();

    private native void        native_setup();
    private static native void native_init();
    private native final  void native_finalize();

    protected void finalize() {
        native_finalize();
    }
}
