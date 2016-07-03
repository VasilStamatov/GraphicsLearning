#include "GameEngineErrors.h"
#include <cstdlib>
#include <iostream>
#include <SDL/SDL.h>

namespace GameEngine
{
  //use this for fatal errors and when you want to exit immediately
  void FatalError(const std::string& errorString)
  {
    std::cout << errorString << std::endl;
    std::cout << "Enter any key to quit. . .";
    std::cin.get();
    SDL_Quit();
    exit(1);
  }
}
