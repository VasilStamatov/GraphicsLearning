#include "InputManager.h"

namespace GameEngine
{
  InputManager::InputManager()
  {
  }


  InputManager::~InputManager()
  {
  }

  void InputManager::Update()
  {
    //loop through key map using a for-each loop and copy it over to m_previousKeyMap
    for (auto& it : m_keyMap)
    {
      //use .first to access the first element of the keymap (uint), and .second to access the second (bool)
      m_previousKeyMap[it.first] = it.second;
    }
  }

  void InputManager::PressKey(KeyID _keyID)
  {
    // Here we are treating _keyMap as an associative array.
    // if keyID doesn't already exist in _keyMap, it will get added
    m_keyMap[_keyID] = true;
  }
  void InputManager::ReleaseKey(KeyID _keyID)
  {
    // Here we are treating _keyMap as an associative array.
    // switch the pressed button from true (being pressed) to false (released)
    m_keyMap[_keyID] = false;
  }
  void InputManager::SetMouseCoords(float _x, float _y)
  {
    m_mouseCoords.x = _x;
    m_mouseCoords.y = _y;
  }
  void InputManager::SetRelativeMouseMotion(float _xrel, float _yrel)
  {
    m_relativeMouseMotion.x = _xrel;
    m_relativeMouseMotion.y = _yrel;
  }
  void InputManager::SetMouseWheel(int32_t _xVal, int32_t _yVal)
  {
    m_mouseWheel.x = _xVal;
    m_mouseWheel.y = _yVal;
  }

  bool InputManager::IsKeyDown(KeyID _keyID)
  {
    // We dont want to use the associative array approach here
    // because we don't want to create a key if it doesnt exist.
    // So we do it manually
    auto it = m_keyMap.find(_keyID);
    if (it != m_keyMap.end())
    {
      // Found the key
      return it->second;
    }
    else
    {
      // Didn't find the key
      return false;
    }
  }

  bool InputManager::IsKeyPressed(KeyID _keyID)
  {
    //check if it was pressed this frame, and wasn't pressed last frame
    if (IsKeyDown(_keyID) == true && wasKeyDown(_keyID) == false)
    {
      return true;
    }
    return false;
  }

  bool InputManager::wasKeyDown(KeyID _keyID)
  {
    // We dont want to use the associative array approach here
    // because we don't want to create a key if it doesnt exist.
    // So we do it manually
    auto it = m_previousKeyMap.find(_keyID);
    if (it != m_previousKeyMap.end())
    {
      // Found the key
      return it->second;
    }
    else
    {
      // Didn't find the key
      return false;
    }
  }
}

