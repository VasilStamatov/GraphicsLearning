#pragma once

#include <functional>
#include <glm\glm.hpp>
#include "Vertex.h"
#include "SpriteBatch.h"
#include "GLTexture.h"

namespace GameEngine
{
  //the 2d particle
  class Particle2D
  {
  public:
    glm::vec2 m_position = glm::vec2(0.0f);
    glm::vec2 m_velocity = glm::vec2(0.0f);
    ColorRGBA8 m_color;
    float m_width = 0.0f;
    float m_life = 0.0f;

  };

  // Default function pointer
  inline void DefaultParticleUpdate(Particle2D _particle, float _deltaTime)
  {
    _particle.m_position += _particle.m_velocity * _deltaTime;
  }

  class ParticleBatch2D
  {
  public:
    ParticleBatch2D();
    ~ParticleBatch2D();
    //initialize the particle batch
    void Init(int _maxParticles, float _decayRate, GLTexture _texture, std::function<void(Particle2D&, float)> _updateFunc = DefaultParticleUpdate);
    //add a oarticle
    void AddParticle(const glm::vec2& _position,
                     const glm::vec2& _velocity,
                     const ColorRGBA8& _color,
                     float _width);
    //updates all the active particles
    void Update(float _deltaTime);
    //draws all the active particles
    void Draw(SpriteBatch* _spritebBatch);

  private:
    //finds a free particle position from all possible particles (maxParticles) or overwrites the first one
    int FindFreeParticle();

    std::function<void(Particle2D&, float)> m_updateFunc;

    float m_decayRate = 0.1f;

    Particle2D* m_particles = nullptr;
    int m_maxParticles = 0;
    int m_lastFreeParticle = 0;
    GLTexture m_texture;
  };
}


