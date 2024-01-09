#!/bin/bash
# export PATH=/home/wanji/qt5.14.2/gcc_64/bin:$PATH
# export LD_LIBRARY_PATH=/home/wanji/qt5.14.2/gcc_64/lib:$LD_LIBRARY_PATH
# export LD_LIBRARY_PATH=/home/wanji/ll-use/QtProject/FFmpegDemo/third_party:$LD_LIBRARY_PATH
# export LD_LIBRARY_PATH=/home/wanji/ll-use/QtProject/FFmpegDemo/third_party/ffmpeg/lib:$LD_LIBRARY_PATH
# export QT_PLUGIN_PATH=/home/wanji/qt5.14.2/gcc_64/plugins:$QT_PLUGIN_PATH
APP_NAME=Rtsp.cpython-36m-x86_64-linux-gnu
VERSION=1.0.5


linuxdeployqt Rtsp -appimage
