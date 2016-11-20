#pragma once

#include <glm\vec2.hpp>

#include <GameEngine\SpriteBatch.h>
#include <GameEngine\GLTexture.h>
#include "World.h"

constexpr float AGENT_RADIUS = 14.0f;
constexpr float AGENT_DIAMETER = AGENT_RADIUS * 2.0f;

//Base class for conscious entities.
class Agent
{
public:
		/** Agent default constructor */
		Agent();

		/** Agent second constructor */
		Agent(float _speed, float _health, const glm::vec2& _startPos, const GameEngine::GLTexture& _texture,
				GameEngine::ColorRGBA8& _color, std::weak_ptr<World> _world);

		/** Agent virtual destructor */
		virtual ~Agent();

		/** \brief The update function to be called every frame by the agents
		 *  \param _deltaTime - the deltaTime for frame independence */
		virtual void Update(float _deltaTime) = 0;

		/** \brief The Draw function to be called every frame by the agents
		 *  \param _batch - the batch which this agent's sprite will be submitted to */
		virtual void Draw(GameEngine::SpriteBatch& _batch);

		/** \brief Collision handler with the world (current level) */
		bool CollideWithLevel();

		/** \brief Collision handler with other agents
		 *  \param _agent - the other agent to be collided with (pointer passed for polymorphism) */
		bool CollideWithAgent(Agent* _agent);

		/** \brief subtracts _damage from m_health */
		void ApplyDamage(float _damage);

		/* Getters */
	 glm::vec2 GetCenterPos()								const noexcept { return m_worldPos + glm::vec2(AGENT_RADIUS); }
		const glm::vec2& GetPosition()		const noexcept { return m_worldPos; }
		const glm::vec2& GetDirection() const noexcept { return m_direction; }
		const float GetHealth()							  const noexcept { return m_health; }
		const float GetSpeed()										const noexcept { return m_movementSpeed; }

		/* Setters */
		void SetPosition(const glm::vec2& _worldPos)		 { m_worldPos = _worldPos; }
		void SetDirection(const glm::vec2& _direction) { m_direction = glm::normalize(_direction); }
		void SetHealth(float _newHealth)															{ m_health = _newHealth; }
		void SetSpeed(float _newSpeed)														   { m_movementSpeed = _newSpeed; }

protected:
		/** \brief Checks the tile at position _x, _y if it's collidable */
		void CheckTilePosition(std::vector<glm::vec2>& _collideTilePositions, float _x, float _y);

		/** \brief Collision handler with 1 tile at position _tilePos */
		void CollideWithTile(const glm::vec2& _tilePos);

protected:
		glm::vec2 m_worldPos{ 0.0f, 0.0f };  ///<the bottom left position in world space
		glm::vec2 m_direction{ 1.0f, 0.0f }; ///<normalized direction

		GameEngine::GLTexture m_texture;				 ///<the texture of this agent
		GameEngine::ColorRGBA8 m_color;					 ///<the tint of this agent

		float m_health{ 100.0f };											 ///<the current health of this agent
		float m_movementSpeed{ 1.5f };						 ///<the movement speed of this agent

		std::weak_ptr<World> m_world;								///<the world this agent belongs to
};

