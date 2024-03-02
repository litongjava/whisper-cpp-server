set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "vcpkg toolchain file")
include("$CACHE{CMAKE_TOOLCHAIN_FILE}")

# find uWebSockets head file path
find_path(UWEBSOCKETS_INCLUDE_DIRS "uwebsockets/App.h")
# find zlib
find_package(ZLIB REQUIRED GLOBAL)

# find libuv
find_package(libuv CONFIG REQUIRED GLOBAL)
# find uSockets
find_library(USOCKETS_LIBRARY uSockets)

# find SDL2 library
find_package(SDL2 CONFIG REQUIRED GLOBAL)

find_package(SampleRate CONFIG REQUIRED GLOBAL)
find_package(FFMPEG REQUIRED GLOBAL)
# 查找 SpeexDSP 库
find_library(SPEEXDSP_LIBRARY NAMES speexdsp)
# 查找头文件
find_path(SPEEXDSP_INCLUDE_DIRS "speex/speex_preprocess.h")

# httplib
find_package(httplib CONFIG REQUIRED GLOBAL)

# nlohmann json
find_package(nlohmann_json CONFIG REQUIRED GLOBAL)

#Whispercpp
set(BUILD_SHARED_LIBS OFF)
add_subdirectory(whisper.cpp)

# dr_lib
add_library(dr_lib_header-only INTERFACE)
target_include_directories(dr_lib_header-only INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/dr_libs)