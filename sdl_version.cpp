#include <iostream>
#include <SDL.h>

int main() {
  SDL_version compiled;
  SDL_version linked;

  SDL_VERSION(&compiled); // 获取编译时的SDL版本
  SDL_GetVersion(&linked); // 获取链接时的SDL版本

  std::cout << "We compiled against SDL version "
            << static_cast<int>(compiled.major) << "."
            << static_cast<int>(compiled.minor) << "."
            << static_cast<int>(compiled.patch) << std::endl;

  std::cout << "But we are linking against SDL version "
            << static_cast<int>(linked.major) << "."
            << static_cast<int>(linked.minor) << "."
            << static_cast<int>(linked.patch) << std::endl;

  return 0;
}