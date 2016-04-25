#include "Entity.h"

void Entity::Destroy(b2World* _world)
{
  //Destroy the capsule
  m_capsule.Destroy(_world);
}
void Entity::DrawDebug(GameEngine::DebugRenderer& _debugRenderer)
{
  //Draw the debug (colission outlines)
  m_capsule.DrawDebug(_debugRenderer);
}
