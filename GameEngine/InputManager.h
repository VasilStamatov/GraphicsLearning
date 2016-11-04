#pragma once
#include <unordered_map>
#include <glm\glm.hpp>

namespace GameEngine
{
  // Input manager stores a key map that maps SDL_Keys to booleans.
  // If the value in the key map is true, then the key is pressed.
  // Otherwise, it is released.

  using KeyID = unsigned int;

  class InputManager
  {
  public:
    InputManager();
    ~InputManager();
    //update the key map
    void Update();

    void PressKey(KeyID _keyID);
    void ReleaseKey(KeyID _keyID);
    void SetMouseCoords(float _x, float _y);
    void SetRelativeMouseMotion(float _xrel, float _yrel);
    void SetMouseWheel(int32_t _xVal, int32_t _yVal);

    ///return true if the key is held down
    bool IsKeyDown(KeyID _keyID);

    ///returns true if the key is pressed this frame
    bool IsKeyPressed(KeyID _keyID);

    //getters
    glm::vec2 GetMouseCoords()         const noexcept { return m_mouseCoords; }
    glm::vec2 GetRelativeMouseMotion() const noexcept { return m_relativeMouseMotion; }
    glm::ivec2 GetMouseWheelValue()     const noexcept { return m_mouseWheel;  }

  private:
    bool wasKeyDown(KeyID _keyID);

    std::unordered_map<KeyID, bool> m_keyMap;
    std::unordered_map<KeyID, bool> m_previousKeyMap;

    glm::vec2 m_mouseCoords{ 0.0f, 0.0f };
    glm::vec2 m_relativeMouseMotion{ 0.0f, 0.0f };
    glm::ivec2 m_mouseWheel{ 0, 0 };
  };
}


