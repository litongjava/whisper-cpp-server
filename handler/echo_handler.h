//
// Created by Ping Lee on 2024/3/10.
//

#ifndef WHISPER_CPP_SERVER_ECHO_HANDLER_H
#define WHISPER_CPP_SERVER_ECHO_HANDLER_H

#include <uwebsockets/WebSocketProtocol.h>

// WebSocket /echo handler
auto ws_echo_handler = [](auto *ws, std::string_view message, uWS::OpCode opCode) {
  ws->send(message, opCode);
};


#endif //WHISPER_CPP_SERVER_ECHO_HANDLER_H
