cmake_minimum_required(VERSION 3.23)
project(whisper_cpp_server)

set(CMAKE_CXX_STANDARD 14)
# find SDL2 library
find_package(SDL2 CONFIG REQUIRED)
message(STATUS "SDL2 include directories: ${SDL2_INCLUDE_DIRS}")
message(STATUS "SDL2 libraries: ${SDL2_LIBRARIES}")

include_directories(${SDL2_INCLUDE_DIRS})

add_executable(sdl_version sdl_version.cpp)
target_link_libraries(sdl_version ${SDL2_LIBRARIES})

add_executable(stream_components stream_components.cpp)
target_link_libraries(stream_components whisper ${SDL2_LIBRARIES})

#add_executable(server server.cpp httplib.h json.hpp)
#target_link_libraries(server common.cpp whisper)

