#include "ParallaxBackground.h"
#include <GameEngine\ResourceManager.h>

ParallaxBackground::ParallaxBackground(float _scrollSpeed, float _depth, const std::string& _texturePath,
  const glm::vec2& _position, const glm::vec2& _dimensions, float _playerPosX, int _index) :

  m_scrollSpeed(_scrollSpeed), m_depth(_depth), m_position(_position),
  m_dimensions(_dimensions), m_previousPlayerPosX(_playerPosX), m_backgroundIndex(_index)
{
  m_texture = GameEngine::ResourceManager::GetTexture(_texturePath);
  m_startPosX = m_position.x;
  m_endPosX = m_position.x + m_dimensions.x;
  m_position.y = _position.y;
}


ParallaxBackground::~ParallaxBackground()
{
}

void ParallaxBackground::Draw(GameEngine::SpriteBatch& _spritebatch)
{
  glm::vec4 destRect(m_position.x, m_position.y, m_dimensions.x, m_dimensions.y);

  glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);

  GameEngine::ColorRGBA8 color(255, 255, 255, 255);

  _spritebatch.Draw(destRect, uvRect, m_texture.id, m_depth, color);
}

void ParallaxBackground::Update(const glm::vec2& _newPlayerPos, const glm::vec2& _bottomLeft)
{
  static float firstSecond = 0;
  if (firstSecond < 60.0f)
  {
    m_position.x = _bottomLeft.x + m_dimensions.x * m_backgroundIndex;
    m_position.y = _bottomLeft.y;
    m_startPosX = m_position.x;
    m_endPosX = m_position.x + m_dimensions.x;
    firstSecond += 1.0f;
  }
  if (m_scrollSpeed != 1.0f)
  {
    if (_newPlayerPos.x > m_endPosX + m_dimensions.x)
    {
      m_position.x = m_endPosX + m_dimensions.x * 2.0f;
      m_position.x -= 6.2f;
      m_startPosX = m_position.x;
      m_endPosX = m_position.x + m_dimensions.x;
    }
    else if (_newPlayerPos.x < m_startPosX - m_dimensions.x)
    {
      m_position.x = m_startPosX - m_dimensions.x * 3.0f;
      m_position.x += 6.2f;
      m_startPosX = m_position.x;
      m_endPosX = m_position.x + m_dimensions.x;
    }
  }
  m_position.y = _bottomLeft.y;

  if (m_previousPlayerPosX < _newPlayerPos.x)
  {
    m_position.x += ((_newPlayerPos.x - m_previousPlayerPosX) * m_scrollSpeed);
  }
  else if (m_previousPlayerPosX > _newPlayerPos.x)
  {
    m_position.x -= ((m_previousPlayerPosX - _newPlayerPos.x) * m_scrollSpeed);
  }
  m_previousPlayerPosX = _newPlayerPos.x;

  //std::cout << "Sprite: " + m_texture.filePath + "\n" + "start pox x: " + std::to_string(m_startPosX) + " Index: " << m_backgroundIndex << std::endl;
  //std::cout << "Player pos x: " << _newPlayerPos.x << std::endl;
}