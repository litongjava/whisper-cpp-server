#include <iostream>
#include <SDL.h>

int main(int argc, char *argv[]) {
  SDL_version compiled;
  SDL_version linked;

  SDL_VERSION(&compiled); // get compiled SDL version
  SDL_GetVersion(&linked); // get linked SDL version

  std::cout << "We compiled against SDL version "
            << static_cast<int>(compiled.major) << "."
            << static_cast<int>(compiled.minor) << "."
            << static_cast<int>(compiled.patch) << std::endl;

  std::cout << "We are linking against SDL version "
            << static_cast<int>(linked.major) << "."
            << static_cast<int>(linked.minor) << "."
            << static_cast<int>(linked.patch) << std::endl;

  return 0;
}
