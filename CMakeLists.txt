cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-fPIC)

project(Test)

add_compile_options(-fPIC)
add_compile_options(-shared)

link_directories(${CMAKE_SOURCE_DIR}/lib)
link_directories(${CMAKE_SOURCE_DIR}/lib/lib)
# 头文件引用
include_directories(${CMAKE_SOURCE_DIR}/)
include_directories(${CMAKE_SOURCE_DIR}/inc)
include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(Test main.cpp)
set(M_OPEN_LIBS "opencv_flann;opencv_dnn;opencv_features2d;opencv_photo;opencv_shape;opencv_highgui;opencv_video;opencv_calib3d;opencv_imgproc;opencv_imgcodecs;opencv_videoio;opencv_objdetect;opencv_superres;opencv_videostab;opencv_stitching;opencv_core;opencv_ml")
target_link_libraries(Test
        WJRtsp
        -lpthread
        -lliveMedia -lBasicUsageEnvironment -lgroupsock -lUsageEnvironment
        -lssl -lcrypto
        -lDecodeSdk
        ${M_OPEN_LIBS}
        )
add_executable(SaveBin savebin.cpp)
target_link_libraries(SaveBin
        WJRtsp
        -lpthread
        -lliveMedia -lBasicUsageEnvironment -lgroupsock -lUsageEnvironment
        -lssl -lcrypto
#        -lDecodeSdk
#        ${M_OPEN_LIBS}
        )


SET(Python3_INCLUDE_DIRS "/home/wanji/anaconda3/envs/py36/include/python3.6m")
SET(Python3_LIBRARIES "/home/wanji/anaconda3/envs/py36/lib/libpython3.6m.so")
SET( PYTHON_EXECUTABLE /home/wanji/anaconda3/envs/py36/bin/python3.6)
include_directories(/home/wanji/anaconda3/envs/py36/include/python3.6m)
include_directories(${CMAKE_SOURCE_DIR}/pybind11/include)
add_subdirectory(
        pybind11
)
pybind11_add_module(Rtsp "PyRtsp.cpp")
target_link_libraries(
        Rtsp PRIVATE
        WJRtsp
        -lliveMedia -lBasicUsageEnvironment -lgroupsock -lUsageEnvironment
        -lssl -lcrypto
)