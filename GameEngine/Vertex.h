#pragma once
#include <GL/glew.h>
namespace GameEngine
{
  //the position struct
  struct Position
  {
    float x;
    float y;
  };
  //color struct
  struct ColorRGBA8
  {
    ColorRGBA8() : r(0), g(0), b(0), a(0) { }
    ColorRGBA8(GLubyte R, GLubyte G, GLubyte B, GLubyte A) : 
      r(R), g(G), b(B), a(A) { }
    GLubyte r;
    GLubyte g;
    GLubyte b;
    GLubyte a;
  };
  //UV struct
  struct UV
  {
    float u;
    float v;
  };

  //The vertex definition
  struct Vertex
  {
    //This is the position struct. When you store a struct or class
    //inside of another struct or class, it is called composition. This is
    //layed out exactly the same in memory as if we had a float position[2],
    //but doing it this way makes more sense.
    Position position;

    //4 bytes for r g b a color.
    ColorRGBA8 color;
    //UV texture coordinates.
    UV uv;

    void SetPosition(float _x, float _y)
    {
      position.x = _x;
      position.y = _y;
    }
    void SetColor(GLubyte _r, GLubyte _g, GLubyte _b, GLubyte _a)
    {
      color.r = _r;
      color.g = _g;
      color.b = _b;
      color.a = _a;
    }
    void SetUV(float _u, float _v)
    {
      uv.u = _u;
      uv.v = _v;
    }
  };
}
