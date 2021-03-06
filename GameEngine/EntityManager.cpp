#include "EntityManager.h"
#include "Entity.h"

#include <algorithm>

namespace GameEngine
{
  EntityManager::EntityManager()
  {
  }


  EntityManager::~EntityManager()
  {
  }

  void EntityManager::Update(float _deltaTime)
  {
    for (auto& entity : m_entities)
    {
      entity->Update(_deltaTime);
    }
  }

  void EntityManager::Draw()
  {
    for (auto& entity : m_entities)
    {
      entity->Draw();
    }
  }

  void EntityManager::Refresh()
  {
    for (auto i(0u); i < MAX_GROUPS; ++i)
    {
      auto& vectorOfEntities(m_groupedEntities[i]);

      vectorOfEntities.erase(std::remove_if(std::begin(vectorOfEntities), std::end(vectorOfEntities),
        [i](Entity* _entity)
      {
        return !_entity->IsAlive() ||
          !_entity->HasGroup(i);
      }),
        std::end(vectorOfEntities));
    }

    m_entities.erase(
      std::remove_if(std::begin(m_entities), std::end(m_entities),
        [](const std::unique_ptr<Entity>& _entity)
    {
      return !_entity->IsAlive();
    }),
      std::end(m_entities));

    for (auto& entity : m_toAdd)
    {
      m_entities.emplace_back(std::move(entity));
    }
    m_toAdd.clear();
  }

  Entity* EntityManager::AddEntity()
  {
    //create the new entity unique pointer
    m_toAdd.emplace_back(std::make_unique<Entity>(*this));
    //return a reference of the new entity so that the user may use it for something
    return m_toAdd.back().get();
  }
}