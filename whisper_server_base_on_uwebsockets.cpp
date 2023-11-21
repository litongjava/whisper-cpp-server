#include <uwebsockets/App.h>
#include <iostream>

int main(int argc, char **argv) {
  auto hello_action = [](auto *res, auto *req) {
    res->end("Hello World!");
  };
  
  auto started_handler = [](auto *token) {
    if (token) {
      std::cout << "Server started on port 3000" << std::endl;
    } else {
      std::cerr << "Failed to start server" << std::endl;
    }
  };

  uWS::App().get("/hello", hello_action).listen(3000, started_handler).run();
}