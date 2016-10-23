#pragma once

#include <bitset>
#include <cassert>
#include "Component.h"
#include "EntityManager.h"

constexpr std::size_t MAX_COMPONENTS{ 32 };

namespace GameEngine
{
  //using Group = std::size_t;
  //using ComponentBitset = std::bitset<MAX_COMPONENTS>;
  //using GroupBitset = std::bitset<MAX_GROUPS>;
  //using ComponentArray = std::array<Component*, MAX_COMPONENTS>;

  //The entity class is used to contain components and to Update and Draw them
  class Entity
  {
  public:
    Entity(EntityManager& _manager) : m_manager(_manager) {}
    ~Entity() {}

    //Update the Entity
    void Update(float _deltaTime) { for (auto& component : m_components) { component->Update(_deltaTime); } }

    //Draw the entity
    void Draw() { for (auto& component : m_components) { component->Draw(); } }

    //Destroy the entity
    void Destroy() { m_alive = false; }
    //Get the isAlive flag
    bool IsAlive() const noexcept { return m_alive; }

    template<typename T> bool HasComponent() const
    {
      //ask the bitset if the bit if the bit at the position with the type T is true or false
      return m_componentBitset[GetComponentTypeID<T>()];
    }

    bool HasGroup(std::size_t _group) const noexcept
    {
      return m_groupBitset[_group];
    }

    void AddGroup(std::size_t _group) noexcept
    {
      m_groupBitset[_group] = true;
      m_manager.AddToGroup(this, _group);
    }

    void DelGroup(std::size_t _group) noexcept { m_groupBitset[_group] = false; }

    /** \brief Add components to this element of any type
    * \param[in] T is the component type
    * \param[in] TArgs is a parameter pack of types used to construct the component*/
    template<typename T, typename... TArgs>
    T& AddComponent(TArgs&&... _args)
    {
      //check if this component is not already added
      assert(!HasComponent<T>());

      //begin by allocating component of type T on the heap by forwarding the passed arguments to its constructor
      T* c(new T(std::forward<TArgs>(_args)...));
      c->m_entity = this;
      //wrap the raw pointer into a smart one to make sure there are no memory leaks
      std::unique_ptr<Component> component{ c };

      //Add the component to the vector (std::move is required as unique pointers cannot be copied)
      m_components.emplace_back(std::move(component));

      //when a component of type T is added, add it to the bitset and the array
      m_componentArray[GetComponentTypeID<T>()] = c;
      m_componentBitset[GetComponentTypeID<T>()] = true;

      //call the constructor of this component
      c->Init();

      //return a reference of the new component so that the user may use it for something
      return *c;
    }

    template<typename T> T& GetComponent() const
    {
      //get a specific component from the m_componentArray;
      //check if it has this component
      assert(HasComponent<T>());
      //get the component pointer
      auto componentPointer(m_componentArray[GetComponentTypeID<T>()]);
      //return the dereferenced component pointer (casted to the derived type T)
      return *reinterpret_cast<T*>(componentPointer);
    }

  private:
    //Keep track whether the entity is alive or dead with a boolean flag
    bool m_alive{ true };

    EntityManager& m_manager;

    //An entity is also composed of numerous components
    //Therefore the components will be stored in an std::vector as unique pointers to allow polymorphism
    std::vector<std::unique_ptr<Component>> m_components;

    // A bitset to check the existance of a component with a specific ID
    std::bitset<MAX_COMPONENTS> m_componentBitset;
    std::bitset<MAX_GROUPS> m_groupBitset;
    // An array to get a component with specific ID
    std::array<Component*, MAX_COMPONENTS> m_componentArray;
  };
}