/*
 * Copyright (c) 2021 Taner Sener
 *
 * This file is part of FFmpegKit.
 *
 * FFmpegKit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FFmpegKit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with FFmpegKit.  If not, see <http://www.gnu.org/licenses/>.
 */

package com.arthenica.ffmpegkit;

import android.os.Build;

import com.arthenica.smartexception.java.Exceptions;

import java.text.SimpleDateFormat;
import java.util.Collections;
import java.util.Date;
import java.util.List;

/**
 * <p>Responsible of loading native libraries.
 */
public class NativeLoader {

    static final String[] FFMPEG_LIBRARIES = {"ijkffmpeg"};

    static boolean isTestModeDisabled() {
        return (System.getProperty("enable.ffmpeg.kit.test.mode") == null);
    }

    private static void loadLibrary(final String libraryName) {
        if (isTestModeDisabled()) {
            try {
                System.loadLibrary(libraryName);
            } catch (final UnsatisfiedLinkError e) {
                throw new Error(String.format("FFmpegKit failed to start on %s.", getDeviceDebugInformation()), e);
            }
        }
    }

    private static List<String> loadExternalLibraries() {
        if (isTestModeDisabled()) {
            return Packages.getExternalLibraries();
        } else {
            return Collections.emptyList();
        }
    }

    static String loadVersion() {
        final String version = "4.3";

        if (isTestModeDisabled()) {
            return FFmpegKitConfig.getVersion();
        } else {
            return version;
        }
    }

    static int loadLogLevel() {
        if (isTestModeDisabled()) {
            return FFmpegKitConfig.getNativeLogLevel();
        } else {
            return Level.AV_LOG_DEBUG.getValue();
        }
    }

    static String loadBuildDate() {
        if (isTestModeDisabled()) {
            return FFmpegKitConfig.getBuildDate();
        } else {
            return new SimpleDateFormat("yyyyMMdd").format(new Date());
        }
    }

    static void enableRedirection() {
        if (isTestModeDisabled()) {
            FFmpegKitConfig.enableRedirection();
        }
    }

    static boolean loadFFmpeg() {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.LOLLIPOP) {

            /* LOADING LINKED LIBRARIES MANUALLY ON API < 21 */
            final List<String> externalLibrariesEnabled = loadExternalLibraries();
            if (externalLibrariesEnabled.contains("tesseract") || externalLibrariesEnabled.contains("x265") || externalLibrariesEnabled.contains("snappy") || externalLibrariesEnabled.contains("openh264") || externalLibrariesEnabled.contains("rubberband")) {
                loadLibrary("c++_shared");
            }

            for (String ffmpegLibrary : FFMPEG_LIBRARIES) {
                loadLibrary(ffmpegLibrary);
            }
        }

        return true;
    }

    static void loadFFmpegKit(final boolean nativeFFmpegTriedAndFailed) {
        loadLibrary("ijkffmpegkit");
    }

    @SuppressWarnings("deprecation")
    static String getDeviceDebugInformation() {
        final StringBuilder stringBuilder = new StringBuilder();

        stringBuilder.append("brand: ");
        stringBuilder.append(Build.BRAND);
        stringBuilder.append(", model: ");
        stringBuilder.append(Build.MODEL);
        stringBuilder.append(", device: ");
        stringBuilder.append(Build.DEVICE);
        stringBuilder.append(", api level: ");
        stringBuilder.append(Build.VERSION.SDK_INT);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            stringBuilder.append(", abis: ");
            stringBuilder.append(FFmpegKitConfig.argumentsToString(Build.SUPPORTED_ABIS));
            stringBuilder.append(", 32bit abis: ");
            stringBuilder.append(FFmpegKitConfig.argumentsToString(Build.SUPPORTED_32_BIT_ABIS));
            stringBuilder.append(", 64bit abis: ");
            stringBuilder.append(FFmpegKitConfig.argumentsToString(Build.SUPPORTED_64_BIT_ABIS));
        } else {
            stringBuilder.append(", cpu abis: ");
            stringBuilder.append(Build.CPU_ABI);
            stringBuilder.append(", cpu abi2s: ");
            stringBuilder.append(Build.CPU_ABI2);
        }

        return stringBuilder.toString();
    }

}
