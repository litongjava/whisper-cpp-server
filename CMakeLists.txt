cmake_minimum_required(VERSION 3.16)
project(whisper_cpp_server)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(thirdparty)

add_executable(audio_vad examples/audio_vad.cpp common/common.cpp
        stream/stream_components_service.cpp common/utils.cpp)
target_link_libraries(audio_vad PRIVATE whisper SampleRate::samplerate ${SPEEXDSP_LIBRARY} nlohmann_json::nlohmann_json dr_lib_header-only)
#  linke header file
target_include_directories(audio_vad PRIVATE ${SPEEXDSP_INCLUDE_DIRS})


add_executable(sdl_version examples/sdl_version.cpp)
target_link_libraries(sdl_version PRIVATE SDL2::SDL2-static)

add_executable(simplest examples/simplest.cpp common/common.cpp common/utils.cpp)
target_link_libraries(simplest PRIVATE whisper SampleRate::samplerate nlohmann_json::nlohmann_json dr_lib_header-only)

add_executable(stream_local examples/stream_local.cpp common/common.cpp common/common-sdl.cpp common/utils.cpp
        stream/stream_components_service.cpp stream/stream_components_audio.cpp
        stream/stream_components_output.cpp
        )
target_link_libraries(stream_local whisper SDL2::SDL2-static SampleRate::samplerate nlohmann_json::nlohmann_json dr_lib_header-only)

add_executable(whisper_http_server_base_httplib whisper_http_server_base_httplib.cpp
        common/common.cpp common/utils.cpp handler/inference_handler.cpp params/whisper_params.cpp
        common/common-m4a.cpp handler/ws_save_handler.h)
target_include_directories(whisper_http_server_base_httplib PRIVATE ${FFMPEG_INCLUDE_DIRS})
target_link_directories(whisper_http_server_base_httplib PRIVATE ${FFMPEG_LIBRARY_DIRS})
target_link_libraries(whisper_http_server_base_httplib PRIVATE whisper SampleRate::samplerate ${FFMPEG_LIBRARIES} httplib::httplib nlohmann_json::nlohmann_json dr_lib_header-only)


add_executable(whisper_server_base_on_uwebsockets whisper_server_base_on_uwebsockets.cpp common/common.cpp
        stream/stream_components_service.cpp common/utils.cpp handler/hello_handler.cpp)
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
        SampleRate::samplerate ${SPEEXDSP_LIBRARY} nlohmann_json::nlohmann_json dr_lib_header-only)
# linked header file
target_include_directories(whisper_server_base_on_uwebsockets PRIVATE ${SPEEXDSP_INCLUDE_DIRS})

