#include "Timing.h"
#include <SDL\SDL.h>
namespace GameEngine
{

  FpsLimiter::FpsLimiter()
  {
  }

  void FpsLimiter::Init(float _maxFPS)
  {
    SetMaxFPS(_maxFPS);
  }

  void FpsLimiter::SetMaxFPS(float _maxFPS)
  {
    m_maxFPS = _maxFPS;
  }

  //called at the start of the frame
  void FpsLimiter::BeginFrame()
  {
    m_startTicks = SDL_GetTicks();
  }

  //called at the end of the frame
  float FpsLimiter::End()
  {
    CalculateFPS();
    float frameTicks = (float)SDL_GetTicks() - m_startTicks;
    //check if the time it took this frame to be completed is lower than the desired frame time
    if (1000.0f / m_maxFPS > frameTicks)
    {
      //if the frame was completed too fast, delay it to limit the fps
      SDL_Delay((Uint32)(1000.0f / m_maxFPS - frameTicks));
    }
    return m_fps;
  }

  void FpsLimiter::CalculateFPS()
  {
    static constexpr float MS_PER_SECOND = 1000; // Number of milliseconds in a second
    const float DESIRED_FRAMETIME = MS_PER_SECOND / m_maxFPS; // The desired frame time per frame

    //The number of frames to average
    static const int NUM_SAMPLES = 10;
    //Stores all the frametimes for each frame that we will average
    static float frameTimes[NUM_SAMPLES];
    //The current frame we are on
    static int currentFrame = 0;
    //the ticks of the previous frame
    static float previousTicks = static_cast<float>(SDL_GetTicks());
    //Ticks for the current frame
    float currentTicks = static_cast<float>(SDL_GetTicks());

    //Calculate the number of ticks (ms) for this frame
    m_frameTime = (float)currentTicks - previousTicks;

    m_deltaTime = m_frameTime / DESIRED_FRAMETIME;
    //currentFrame % NUM_SAMPLES -> makes sure that current frame isn't higher then the number of samples
    frameTimes[currentFrame % NUM_SAMPLES] = m_frameTime;
    //current ticks is now previous ticks
    previousTicks = currentTicks;

    int count;
    currentFrame++;

    if (currentFrame < NUM_SAMPLES)
    {
      count = currentFrame;
    }
    else
    {
      count = NUM_SAMPLES;
    }
    //Average all the frame times
    float frameTimeAvg = 0;
    for (int i = 0; i < count; i++)
    {
      frameTimeAvg += frameTimes[i];
    }
    frameTimeAvg /= count;
    //Calculate FPS
    if (frameTimeAvg > 0)
    {
      m_fps = 1000.0f / frameTimeAvg;
    }
    else
    {
      m_fps = 60.0f;
    }
  }

  HRTimer::HRTimer()
  {
    Start();
  }

  HRTimer::~HRTimer()
  {
  }

  void HRTimer::Start(bool _reset /*= true*/)
  {
    if (!m_isActive)
    {
      if (_reset)
      {
        m_start = HRTimePoint::clock::now();
      }
      m_isActive = true;
    }
  }

  void HRTimer::Stop()
  {
    m_stop = HRTimePoint::clock::now();
    m_isActive = false;
  }

  float HRTimer::Seconds() const
  {
    if (m_isActive)
    {
      float elapsedMilli = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(HRTimePoint::clock::now() - m_start).count();
      return elapsedMilli / 1000.0f;
    }
    float elapsedMilli = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(m_stop - m_start).count();
    return elapsedMilli / 1000.0f;
  }
}