#pragma once

#include <random>
#include <chrono>

namespace GameEngine
{
  enum class SeedType
  {
    CLOCK_TICKS,
    TRUE_RANDOM
  };
  /// A seeded random number generator
  class Random
  {
  public:
    /** \brief Initialize the random generator with a seed
    * \param _seedType - the seed type the generator will use: 
    * CLOCK_TICKS = uses the current clock in milliseconds
    * TRUE_RANDOM = generates a true random number for the seed*/
    Random(SeedType _seedType);

    Random()
    {

    }

    ~Random();

    /** \brief Reinitialize the random number generator with a new seed
    * \param _seedType - the seed type the generator will use: 
    * CLOCK_TICKS = uses the current clock in milliseconds
    * TRUE_RANDOM = generates a true random number for the seed*/
    void GenSeed(SeedType _seedType);

    /** \brief Generate a random floating point number from [_min; _max] (inclusively)
    * \param _min - the minimum value:
    * \param _max - the max value:
    * @return the random value*/
    float GenRandFloat(float _min, float _max);

    /** \brief Generate a random int number from [_min; _max] (inclusively)
    * \param _min - the minimum value:
    * \param _max - the max value:
    * @return the random value*/
    int GenRandInt(int _min, int _max);
  private:
    using Clock = std::chrono::high_resolution_clock;
    Clock::time_point m_beginning = Clock::now();

    std::mt19937 m_generator;

    //std::seed_seq m_seed;

    std::random_device m_realRandom;
  };
}

