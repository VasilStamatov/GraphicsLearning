#include "Agent.h"
#include <algorithm>

Agent::Agent()
{
}

Agent::Agent(float _speed, float _health, const glm::vec2 & _startPos,
		const GameEngine::GLTexture & _texture, GameEngine::ColorRGBA8 & _color, std::weak_ptr<World> _world) :
		m_movementSpeed(_speed), m_health(_health), m_worldPos(_startPos), m_texture(_texture), m_color(_color), m_world(_world)
{
}


Agent::~Agent() {}

void Agent::Draw(GameEngine::SpriteBatch & _batch)
{
		const glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);

		glm::vec4 destRect;
		destRect.x = m_worldPos.x;		 //bottom left world pos
		destRect.y = m_worldPos.y;		 //bottom left world pos
		destRect.z = AGENT_DIAMETER; //width
		destRect.w = AGENT_DIAMETER; //height

		_batch.Draw(destRect, uvRect, m_texture.id, 0.0f, m_color, m_direction);
}

bool Agent::CollideWithLevel()
{
		std::vector<glm::vec2> collideTilePositions;
		// Check the four corners
		// First corner (bottom left)
		CheckTilePosition(collideTilePositions, m_worldPos.x, m_worldPos.y);

		// Second Corner (bottom right)
		CheckTilePosition(collideTilePositions, m_worldPos.x + AGENT_DIAMETER, m_worldPos.y);

		// Third Corner (top left)
		CheckTilePosition(collideTilePositions, m_worldPos.x, m_worldPos.y + AGENT_DIAMETER);

		// Fourth Corner (top right)
		CheckTilePosition(collideTilePositions, m_worldPos.x + AGENT_DIAMETER, m_worldPos.y + AGENT_DIAMETER);

		// Check if there was no collision
		if (collideTilePositions.empty())
		{
				return false;
		}
		//Store the world center position of the player to use it for sorting
		glm::vec2 localWorld = m_worldPos + glm::vec2(AGENT_RADIUS); 

		/*sort the tiles to collide based on distance from the center of the player,
		so that you collide with the nearest walls first and avoid the getting stuck on walls bug */
		std::sort(collideTilePositions.begin(), collideTilePositions.end(), [&localWorld](const glm::vec2& _p1, const glm::vec2& _p2) 
		{ 
				auto distance1 = glm::distance(localWorld, _p1);
				auto distance2 = glm::distance(localWorld, _p2);
				return distance1 < distance2;
		});

		// Do the collision starting from closes tile to collide with to furthest
		for (auto& tilePos : collideTilePositions)
		{
				CollideWithTile(tilePos);
		}
		return true;
}

bool Agent::CollideWithAgent(Agent* _agent)
{
		// direction vector between the two agents (center of this agent minus center of other agent)
		glm::vec2 direction = GetCenterPos() - _agent->GetCenterPos();

		// Length of the direction vector
		float distance = glm::length(direction);

		// Depth of the collision
		float collisionDepth = AGENT_DIAMETER - distance;

		// If collision depth > 0 then we did collide
		if (collisionDepth > 0)
		{
				// Get the direction times the collision depth so we can push them away from each other
				glm::vec2 collisionDepthVec = glm::normalize(direction) * collisionDepth;

				// Push them in opposite directions
				m_worldPos += collisionDepthVec / 2.0f;
				_agent->m_worldPos -= collisionDepthVec / 2.0f;

				return true;
		}
		return false;
}

void Agent::ApplyDamage(float _damage)
{
		m_health -= _damage;
}

void Agent::CheckTilePosition(std::vector<glm::vec2>& _collideTilePositions, float _x, float _y)
{
		//Get the node/tile at this agent's world pos
		std::weak_ptr<Node> node = m_world.lock()->GetWorldGrid().lock()->GetNodeAt(glm::vec2(_x, _y));
		//if this is not a walkable tile, then collide with it
		if (!node.lock()->walkable)
		{
				_collideTilePositions.push_back(node.lock()->worldPos);
		}
}

void Agent::CollideWithTile(const glm::vec2 & _tilePos)
{
		constexpr float TILE_RADIUS = TILE_WIDTH / 2.0f;

		// The minimum distance before a collision occurs
		constexpr float MIN_DISTANCE = AGENT_RADIUS + TILE_RADIUS;

		// Center position of the agent
		glm::vec2 centerAgentPos = m_worldPos + glm::vec2(AGENT_RADIUS);

		// direction vector from the agent to the tile
		glm::vec2 direction = centerAgentPos - _tilePos;

		// Get the depth of the collision
		float xDepth = MIN_DISTANCE - abs(direction.x);
		float yDepth = MIN_DISTANCE - abs(direction.y);

		// If both the depths are > 0, then we collided
		if (xDepth > 0 && yDepth > 0)
		{
				// Check which collision depth is less
				if (std::max(xDepth, 0.0f) < std::max(yDepth, 0.0f))
				{
						// X collsion depth is smaller so we push in X direction
						if (direction.x < 0)
						{
								m_worldPos.x -= xDepth;
						}
						else
						{
								m_worldPos.x += xDepth;
						}
				}
				else
				{
						// Y collsion depth is smaller so we push in X direction
						if (direction.y < 0)
						{
								m_worldPos.y -= yDepth;
						}
						else
						{
								m_worldPos.y += yDepth;
						}
				}
		}
}
