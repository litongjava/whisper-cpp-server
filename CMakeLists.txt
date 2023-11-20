cmake_minimum_required(VERSION 3.23)
project(whisper_cpp_server)

set(CMAKE_CXX_STANDARD 14)
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

add_executable(stream_components stream_components.cpp stream_components_audio.cpp stream_components_output.cpp stream_components_service.cpp)
target_link_libraries(stream_components  ${SDL2_LIBRARIES})

add_executable(server server.cpp common.cpp httplib.h json.hpp)
target_link_libraries(server whisper)

