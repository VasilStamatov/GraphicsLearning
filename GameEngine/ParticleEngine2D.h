#pragma once

#include <vector>


namespace GameEngine
{

  class ParticleBatch2D;
  class SpriteBatch;

  class ParticleEngine2D
  {
  public:
    ParticleEngine2D();
    ~ParticleEngine2D();

    //after adding a particle batch the ParticleEngine2D becomes responsible for the allocation
    void AddParticleBatch(ParticleBatch2D* _particleBatch);
    //updates all the batches
    void Update(float _deltaTime);
    //draws all the batches
    void Draw(SpriteBatch* _spriteBatch);

  private:
    std::vector<ParticleBatch2D*> m_batches;

  };

}

