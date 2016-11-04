#pragma once
#include <chrono>
namespace GameEngine
{
  ///Calculates FPS and also limits FPS
  class FpsLimiter
  {
  public:
    FpsLimiter();

    // Initializes the FPS limiter. For now, this is
    // analogous to setMaxFPS
    void Init(float _maxFPS);

    // Sets the desired max FPS
    void SetMaxFPS(float _maxFPS);

    //set the startTicks to the current frame ticks
    void BeginFrame();

    // end() will return the current FPS as a float
    float End();

    float GetCurrentFPS() { return m_fps; }
    float GetCurrentDT() { return m_deltaTime; }

  private:
    // Calculates the current FPS
    void CalculateFPS();

    float m_maxFPS{ 0.0f };
    float m_fps{ 0.0f };
    float m_frameTime{ 0.0f };
    float m_deltaTime{ 0.0f };
    unsigned int m_startTicks{ 0 };
  };

  class HRTimer
  {
  public:
    HRTimer();
    ~HRTimer();

    /* /brief starts the clock (sets the flag to true) and resets it unless explicitly set not to*/
    void Start(bool _reset = true);

    /* /brief sets the stop time point to now() and turns the active flag to false*/
    void Stop();

    /* /brief returns the elapsed time in seconds 
    (if the timer has been stopped it returns from the start to stop time)*/
    float Seconds() const;

				/* /brief returns the elapsed time in milliseconds
				(if the timer has been stopped it returns from the start to stop time) */
				float Milli() const;

    /* /brief Returns the flag if the timer is active*/
    const bool& IsActive() const noexcept { return m_isActive; }

  private:
    using HRTimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;

    HRTimePoint m_start;
    HRTimePoint m_stop;

    bool m_isActive{ false };
  };

  
}