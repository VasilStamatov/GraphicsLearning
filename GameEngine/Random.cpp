#include "Random.h"

namespace GameEngine
{
  Random::Random(SeedType _seedType)
  {
    Clock::duration durotation = Clock::now() - m_beginning;
    switch (_seedType)
    {
    case GameEngine::SeedType::CLOCK_TICKS:
      m_generator.seed(durotation.count());
      break;
    case GameEngine::SeedType::TRUE_RANDOM:
      m_generator.seed(m_realRandom());
      break;
    default:
      break;
    }
  }

  Random::~Random()
  {
  }

  void Random::GenSeed(SeedType _seedType)
  {
    Clock::duration durotation = Clock::now() - m_beginning;
    switch (_seedType)
    {
    case GameEngine::SeedType::CLOCK_TICKS:
      m_generator.seed(durotation.count());
      break;
    case GameEngine::SeedType::TRUE_RANDOM:
      m_generator.seed(m_realRandom());
      break;
    default:
      break;
    }
  }

  float Random::GenRandFloat(float _min, float _max)
  {
    std::uniform_real_distribution<float> realDis(_min, _max);
    return realDis(m_generator);
  }

  int Random::GenRandInt(int _min, int _max)
  {
    std::uniform_int_distribution<int> intDis(_min, _max);
    return intDis(m_generator);
  }
}