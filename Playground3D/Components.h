#pragma once

#include <GameEngine\Component.h>
#include <GameEngine\Entity.h>
#include <iostream>

struct CounterComponent : public GameEngine::Component
{
  CounterComponent(void) {}
  ~CounterComponent() {}

  float m_counter{ 0.0f };

  void Update(float _deltaTime) override
  {
    if (_deltaTime != 0.0f)
    {
      m_counter += 1.0f / _deltaTime;
    }
    //std::cout << m_counter << std::endl;
  }
};

struct KillCompontent : public GameEngine::Component
{
  KillCompontent(void) {}
  ~KillCompontent() {}

  CounterComponent* m_counter{ nullptr };
  void Init() override
  {
    m_counter = &m_entity->GetComponent<CounterComponent>();
  }
  void Update(float _deltaTIme) override
  {
    if (m_counter->m_counter >= 100.0f)
    {
      m_entity->Destroy();
    }
  }
};