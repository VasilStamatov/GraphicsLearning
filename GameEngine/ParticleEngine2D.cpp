#include "ParticleEngine2D.h"
#include "SpriteBatch.h"
#include "ParticleBatch2D.h"

namespace GameEngine
{
  ParticleEngine2D::ParticleEngine2D()
  {
    //empty
  }


  ParticleEngine2D::~ParticleEngine2D()
  {
    for (auto& b : m_batches)
    {
      delete b;
    }

  }

  void ParticleEngine2D::AddParticleBatch(ParticleBatch2D* _particleBatch)
  {
    //push back a particle batch
    m_batches.push_back(_particleBatch);
  }

  void ParticleEngine2D::Update(float _deltaTime)
  {
    //update every batch
    for (auto& b : m_batches)
    {
      b->Update(_deltaTime);
    }
  }

  void ParticleEngine2D::Draw(SpriteBatch* _spriteBatch)
  {
    //draw every batch
    for (auto& b : m_batches)
    {
      _spriteBatch->Begin();
      b->Draw(_spriteBatch);
      _spriteBatch->End();
      _spriteBatch->RenderBatch();
    }
  }
}

