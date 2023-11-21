cmake_minimum_required(VERSION 3.23)
project(whisper_cpp_server)

set(CMAKE_CXX_STANDARD 20)
# 查找 uWebSockets 的头文件路径
find_path(UWEBSOCKETS_INCLUDE_DIRS "uwebsockets/App.h")
# 查找 zlib 库
find_package(ZLIB REQUIRED)

# 查找 libuv 库
find_package(libuv CONFIG REQUIRED)
# 查找 uSockets 库
find_library(USOCKETS_LIBRARY uSockets)

# find SDL2 library
find_package(SDL2 CONFIG REQUIRED)
message(STATUS "SDL2 include directories: ${SDL2_INCLUDE_DIRS}")
message(STATUS "SDL2 libraries: ${SDL2_LIBRARIES}")

include_directories(${SDL2_INCLUDE_DIRS})

# 检测操作系统
# 检测操作系统
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
    # 添加你的 Windows 特定的 include 和 link 目录
    include_directories(E:\\code\\cpp\\project-ping\\whisper.cpp)
    link_directories(E:\\code\\cpp\\project-ping\\whisper.cpp\\cmake-build-release)
    link_directories(E:\\code\\cpp\\project-ping\\whisper.cpp\\cmake-build-release\\bin)
endif ()


add_executable(sdl_version sdl_version.cpp)
target_link_libraries(sdl_version ${SDL2_LIBRARIES})

add_executable(simplest simplest.cpp common.cpp)
target_link_libraries(simplest whisper)

add_executable(stream_local stream_local.cpp common.cpp common-sdl.cpp
        stream_components_service.cpp stream_components_audio.cpp
        stream_components_output.cpp
)
target_link_libraries(stream_local whisper ${SDL2_LIBRARIES})

add_executable(whisper_http_server_base_httplib whisper_http_server_base_httplib.cpp common.cpp httplib.h nlohmann/json.hpp inference_handler.cpp whisper_params.cpp)
target_link_libraries(whisper_http_server_base_httplib whisper)

add_executable(whisper_server_base_on_uwebsockets whisper_server_base_on_uwebsockets.cpp stream_components_service.cpp)
#添加头文件
target_include_directories(whisper_server_base_on_uwebsockets PRIVATE ${UWEBSOCKETS_INCLUDE_DIRS})
# 链接 uWebSockets、zlib、libuv 和 uSockets 库
# 检测操作系统
if(WIN32)
    # 如果是 Windows
    target_link_libraries(whisper_server_base_on_uwebsockets PRIVATE whisper ZLIB::ZLIB libuv::uv ${USOCKETS_LIBRARY})
elseif(APPLE)
    # 如果是 MacOS
    target_link_libraries(whisper_server_base_on_uwebsockets PRIVATE whisper ZLIB::ZLIB libuv::uv_a ${USOCKETS_LIBRARY})
else()
    # 对于其他系统，比如 Linux
    target_link_libraries(whisper_server_base_on_uwebsockets PRIVATE whisper ZLIB::ZLIB libuv::uv ${USOCKETS_LIBRARY})
endif()


