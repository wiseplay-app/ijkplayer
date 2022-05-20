#! /usr/bin/env bash

#----------
UNI_BUILD_ROOT=`pwd`
FF_TARGET=$1
set -e
set +x

FF_ALL_ARCHS="armv7a arm64 x86 x86_64"

echo_archs() {
    echo "===================="
    echo "[*] check archs"
    echo "===================="
    echo "FF_ALL_ARCHS = $FF_ALL_ARCHS"
    echo ""
}

#----------
case "$FF_TARGET" in
    "")
        echo_archs
        sh tools/do-compile-x264.sh armv7a
    ;;
    armv7a|arm64|x86|x86_64)
        echo_archs
        sh tools/do-compile-x264.sh $FF_TARGET
    ;;
    all)
        echo_archs
        for ARCH in $FF_ALL_ARCHS
        do
            sh tools/do-compile-x264.sh $ARCH
        done
    ;;
    clean)
        echo_archs
        for ARCH in $FF_ALL_ARCHS
        do
            cd x264-$ARCH && git clean -xdf && cd -
        done
        rm -rf ./build/x264-*
    ;;
    check)
        echo_archs
    ;;
    *)
        echo "Usage:"
        echo "  compile-x264.sh armv7a|arm64|x86|x86_64"
        echo "  compile-x264.sh all"
        echo "  compile-x264.sh clean"
        echo "  compile-x264.sh check"
        exit 1
    ;;
esac

#----------
echo "\n--------------------"
echo "[*] Finished"
echo "--------------------"
echo "# to continue to build ffmpeg, run script below,"
echo "sh compile-ffmpeg.sh "
echo "# to continue to build ijkplayer, run script below,"
echo "sh compile-ijk.sh "
