#ifndef _PARALLAXBACKGROUND_
#define _PARALLAXBACKGROUND_

#include <GameEngine/SpriteBatch.h>
#include <GameEngine\GLTexture.h>

class ParallaxBackground
{
public:
  ParallaxBackground(float _scrollSpeed, float _depth, const std::string& _texturePath,
    const glm::vec2& _position, const glm::vec2& _dimensions, float _playerPosX, int _index);

  ~ParallaxBackground();

  void Draw(GameEngine::SpriteBatch& _spritebatch);
  void Update(const glm::vec2& _newPlayerPos, const glm::vec2& _bottomLeft);

private:
  float m_startPosX = 0.0f;
  float m_endPosX = 0.0f;
  float m_scrollSpeed = 1.0f;
  float m_depth = 0.0f;
  float m_previousPlayerPosX = 0.0f;
  int m_backgroundIndex = 0;
  GameEngine::GLTexture m_texture;
  glm::vec2 m_position;
  glm::vec2 m_dimensions;
};

#endif // !_PARALLAXBACKGROUND_
