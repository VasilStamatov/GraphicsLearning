#pragma once

#include <memory>
#include <vector>
#include <array>

constexpr std::size_t MAX_GROUPS{ 32 };

namespace GameEngine
{
  class Entity;
  //The entity manager class is used to contain the entities and to Update and Draw them
  class EntityManager
  {
  public:
    EntityManager();
    ~EntityManager();

    void Update(float _deltaTime);

    void Draw();

    void Refresh();

    Entity* AddEntity();

    void AddToGroup(Entity* _entity, std::size_t _group)
    {
      m_groupedEntities[_group].emplace_back(_entity);
    }

    std::vector<Entity*>& getEntitiesByGroup(std::size_t _group)
    {
      return m_groupedEntities[_group];
    }
  private:
    //An entity manager contains numerous components
    //Therefore the components will be stored in an std::vector as unique pointers to allow polymorphism
    std::vector<std::unique_ptr<Entity>> m_entities;
    std::vector<std::unique_ptr<Entity>> m_toAdd;
    std::array<std::vector<Entity*>, MAX_GROUPS> m_groupedEntities;
  };
}


