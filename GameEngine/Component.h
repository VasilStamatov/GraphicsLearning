#pragma once

namespace GameEngine
{
  //define a typedef for the component ID type
  using ComponentID = unsigned int;

  //forward declare the entity class
  class Entity;
  /** The component class game components will inherit from*/
  struct Component
  {
    Component() {}
    //since this is going to be used polymorphically the destructor needs to have the virtual keyword
    virtual ~Component() {}

    //Let the component have an init function so it can be used as a constructor
    virtual void Init() {}
    //Usually game components will have an Update and Draw function
    virtual void Update(float _deltaTime) {}
    virtual void Draw() {}

    //Store the parent entity containing this component in a pointer due to forward declaration
    Entity* m_entity{ nullptr };
  };

  //Hide implementation details
  namespace Internal
  {
    inline ComponentID GetUniqueComponentID() noexcept
    {
      //by using a static variable every time this function is called it will refer to the same "lastID" instance
      //THis way it will always return a unique ID
      static ComponentID lastID{ 0u };
      return lastID++;
    }
  }

  template<typename T>
  inline ComponentID GetComponentTypeID() noexcept
  {
    //every time this function is called with specific type "T"
    //it is instantiating this template with its own unique "typeID" variable
    static_assert(std::is_base_of<Component, T>::value, "Must inherit from Component");

    static ComponentID typeID{ Internal::GetUniqueComponentID() };
    return typeID;
  }
}