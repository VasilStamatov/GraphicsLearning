#include <SDL\SDL.h>
#include <GL\glew.h>
#include "GameEngine.h"

namespace GameEngine
{
  int Init()
  {
    //initialize SDL
    SDL_Init(SDL_INIT_EVERYTHING);
    //The color bits
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    //Tell SDL that we want a double buffered window so we dont get any flickering
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    //set the depth size to 24 (default 16)
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    //set the stencil size to 8 (default 0)
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    //Enable multisampling
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    return 0;
  }
}
