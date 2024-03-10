//
// Created by Tong Li on 2024/3/10.
//

#include "hello_handler.h"

// 使用具体的参数类型实现hello_action
void hello_action(uWS::HttpResponse<false> *res, uWS::HttpRequest *req) {
  res->end("Hello World!");
}