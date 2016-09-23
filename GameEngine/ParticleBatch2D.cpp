#include "ParticleBatch2D.h"

namespace GameEngine
{

  ParticleBatch2D::ParticleBatch2D()
  {
    //empty
  }


  ParticleBatch2D::~ParticleBatch2D()
  {
    //delete[] m_particles;
  }

  //initialize the particle batch
  void ParticleBatch2D::Init(int _maxParticles,
                             float _decayRate,
                             const GLTexture& _texture,
                             std::function<void(Particle2D&, float)> _updateFunc /* = defaultParticleUpdate */)
  {
    m_maxParticles = _maxParticles;
    m_particles = std::make_unique<Particle2D[]>(_maxParticles);
    m_decayRate = _decayRate;
    m_texture = _texture;
    m_updateFunc = _updateFunc;
  }

  //add a particle
  void ParticleBatch2D::AddParticle(const glm::vec2& _position,
                                    const glm::vec2& _velocity,
                                    const ColorRGBA8& _color,
                                    float _width)
  {
    //find a free particle index, or overwrite the first one
    int particleIndex = FindFreeParticle();

    //edit the particle index values
    auto pointerToArray = m_particles.get();

    Particle2D& particle = pointerToArray[particleIndex];

    particle.m_life = 1.0f;

    particle.m_position = _position;

    particle.m_velocity = _velocity;

    particle.m_color = _color;

    particle.m_width = _width;
  }

  void ParticleBatch2D::Update(float _deltaTime)
  {
    auto pointerToArray = m_particles.get();
    for (int i = 0; i < m_maxParticles; i++)
    {
      // check if it is active
      if (pointerToArray[i].m_life > 0.0f)
      {
        // Update using function pointer
        m_updateFunc(pointerToArray[i], _deltaTime);
        pointerToArray[i].m_life -= m_decayRate * _deltaTime;
      }
    }
  }

  //Draw all active particles
  void ParticleBatch2D::Draw(SpriteBatch* _spritebBatch)
  {
    glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
    auto pointerToArray = m_particles.get();
    for (int i = 0; i < m_maxParticles; i++)
    {
      // check if it is active
      Particle2D& particle = pointerToArray[i];
      if (particle.m_life > 0.0f)
      {
        glm::vec4 destRect(particle.m_position.x, particle.m_position.y, particle.m_width, particle.m_width);
        _spritebBatch->Draw(destRect, uvRect, m_texture.id, 0.0f, particle.m_color);
      }
    }
  }

  int ParticleBatch2D::FindFreeParticle()
  {
    //check for a free particle from a range between the last free one, and the max possible particles
    for (int i = m_lastFreeParticle; i < m_maxParticles; i++)
    {
      if (m_particles[i].m_life <= 0.0f)
      {
        m_lastFreeParticle = i;
        return i;
      }
    }
    
    //check for a free particle from a range between the first free, and the last free one
    for (int i = 0; i < m_lastFreeParticle; i++)
    {
      if (m_particles[i].m_life <= 0.0f)
      {
        m_lastFreeParticle = i;
        return i;
      }
    }
    // no particles are free, overwrite first particle 
    return 0;
  }
}

