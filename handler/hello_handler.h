//
// Created by Tong Li on 2024/3/10.
//

#ifndef WHISPER_CPP_SERVER_HELLO_HANDLER_H
#define WHISPER_CPP_SERVER_HELLO_HANDLER_H


#include <uwebsockets/App.h> // 确保这里包含了必要的定义

// 更新函数声明以使用具体的参数类型
// HTTP GET /hello handler
void hello_action(uWS::HttpResponse<false> *res, uWS::HttpRequest *req);

#endif //WHISPER_CPP_SERVER_HELLO_HANDLER_H
