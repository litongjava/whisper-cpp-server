cmake_minimum_required(VERSION 3.23)
project(whisper_cpp_server)

set(CMAKE_CXX_STANDARD 20)
# find uWebSockets head file path
find_path(UWEBSOCKETS_INCLUDE_DIRS "uwebsockets/App.h")
# find zlib
find_package(ZLIB REQUIRED)

# find libuv
find_package(libuv CONFIG REQUIRED)
# find uSockets
find_library(USOCKETS_LIBRARY uSockets)

# find SDL2 library
find_package(SDL2 CONFIG REQUIRED)
message(STATUS "SDL2 include directories: ${SDL2_INCLUDE_DIRS}")
message(STATUS "SDL2 libraries: ${SDL2_LIBRARIES}")

include_directories(${SDL2_INCLUDE_DIRS})

find_package(SampleRate CONFIG REQUIRED)
find_package(FFMPEG REQUIRED)
# 查找 SpeexDSP 库
find_library(SPEEXDSP_LIBRARY NAMES speexdsp)
# 查找头文件
find_path(SPEEXDSP_INCLUDE_DIRS "speex/speex_preprocess.h")

# Detecting Operating Systems
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    # macOS
    include_directories(/Users/ping/code/cpp/project-litongjava/whisper.cpp)
    link_directories(/Users/ping/code/cpp/project-litongjava/whisper.cpp/cmake-build-release)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # Linux
    include_directories(/mnt/e/code/cpp/project-ping/whisper.cpp)
    link_directories(/mnt/e/code/cpp/project-ping/whisper.cpp/cmake-build-release)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # Windows
    include_directories(E:\\code\\cpp\\project-ping\\whisper.cpp)
    link_directories(E:\\code\\cpp\\project-ping\\whisper.cpp\\cmake-build-release)
    link_directories(E:\\code\\cpp\\project-ping\\whisper.cpp\\cmake-build-release\\bin)
endif ()

add_executable(audio_vad examples/audio_vad.cpp common/common.cpp
        stream/stream_components_service.cpp common/utils.cpp)
target_link_libraries(audio_vad PRIVATE whisper SampleRate::samplerate ${SPEEXDSP_LIBRARY})
# 链接头文件
target_include_directories(audio_vad PRIVATE ${SPEEXDSP_INCLUDE_DIRS})

add_executable(sdl_version examples/sdl_version.cpp)
target_link_libraries(sdl_version PRIVATE ${SDL2_LIBRARIES})

add_executable(simplest examples/simplest.cpp common/common.cpp common/utils.cpp)
target_link_libraries(simplest PRIVATE whisper SampleRate::samplerate)

add_executable(stream_local examples/stream_local.cpp common/common.cpp common/common-sdl.cpp common/utils.cpp
        stream/stream_components_service.cpp stream/stream_components_audio.cpp
        stream/stream_components_output.cpp
        )
target_link_libraries(stream_local whisper ${SDL2_LIBRARIES} SampleRate::samplerate)

add_executable(whisper_http_server_base_httplib whisper_http_server_base_httplib.cpp
        common/common.cpp common/utils.cpp handler/inference_handler.cpp params/whisper_params.cpp
        common/common-m4a.cpp)
target_include_directories(whisper_http_server_base_httplib PRIVATE ${FFMPEG_INCLUDE_DIRS})
target_link_directories(whisper_http_server_base_httplib PRIVATE ${FFMPEG_LIBRARY_DIRS})
target_link_libraries(whisper_http_server_base_httplib PRIVATE whisper SampleRate::samplerate ${FFMPEG_LIBRARIES})


add_executable(whisper_server_base_on_uwebsockets whisper_server_base_on_uwebsockets.cpp common/common.cpp
        stream/stream_components_service.cpp common/utils.cpp)
#add uwebsockets head files
target_include_directories(whisper_server_base_on_uwebsockets PRIVATE ${UWEBSOCKETS_INCLUDE_DIRS})
# linked uWebSockets、zlib、libuv 和 uSockets libs
# Detecting Operating Systems
if (WIN32)
    # if Windows
    target_link_libraries(whisper_server_base_on_uwebsockets PRIVATE libuv::uv)
elseif (APPLE)
    # if MacOS
    target_link_libraries(whisper_server_base_on_uwebsockets PRIVATE libuv::uv_a)
else ()
    # if others eg. Linux
    target_link_libraries(whisper_server_base_on_uwebsockets PRIVATE libuv::uv_a)
endif ()

target_link_libraries(whisper_server_base_on_uwebsockets PRIVATE whisper ZLIB::ZLIB ${USOCKETS_LIBRARY}
        SampleRate::samplerate ${SPEEXDSP_LIBRARY})
# 链接头文件
target_include_directories(whisper_server_base_on_uwebsockets PRIVATE ${SPEEXDSP_INCLUDE_DIRS})

