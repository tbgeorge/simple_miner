//=================================================================================
// GameEntity.hpp
// Author: Tyler George
// Date  : April 23, 2015
//=================================================================================

#pragma once

#ifndef __included_GameEntity__
#define __included_GameEntity__

#include <vector>
#include "Engine/Math/PhysicsMotion3D.hpp"
#include "Engine/Math/AABB3D.hpp"
#include "Engine/Renderer/OpenGLRenderer.hpp"

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
class GameEntity
{
public:
	///---------------------------------------------------------------------------------
	/// Constructors
	///---------------------------------------------------------------------------------
	GameEntity() {};
	explicit GameEntity( const Vector3& position, const EulerAngles& orientation, const Vector3& boundingBoxOffsets );
	~GameEntity() {};

	///---------------------------------------------------------------------------------
	/// Accessors/Queries
	///---------------------------------------------------------------------------------
	const Vector3 GetPosition() const;
	const Vector3 GetVelocity() const;
	const EulerAngles GetOrientation() const;
	const EulerAngles GetAngularVelocity() const;
	const Vector3s GetBoundingBoxVertices() const;
	const Vector3s GetBottomVertices() const;
	const Vector3 GetBottomPosition() const;

	///---------------------------------------------------------------------------------
	/// Mutators
	///---------------------------------------------------------------------------------
	void SetPosition( const Vector3& position );
	void SetVelocity( const Vector3& velocity );
	void SetOrientation( const EulerAngles& orientation );
	void SetAngularVelocity( const EulerAngles& angularVelocity );
	void ApplyAcceleration( const Vector3& acceleration );

	///---------------------------------------------------------------------------------
	/// Upadate
	///---------------------------------------------------------------------------------
	virtual void Update( double deltaSeconds );

	///---------------------------------------------------------------------------------
	/// Render
	///---------------------------------------------------------------------------------
	virtual void Render( OpenGLRenderer* renderer, bool debugModeEnabled ) const;

private:
	///---------------------------------------------------------------------------------
	/// Private member variables
	///---------------------------------------------------------------------------------
	PhysicsMotion3D m_physics;
	AABB3D m_boundingBox;
	Vector3 m_boundingBoxOffsets;
	Vector3 m_bottomPosition;
};

typedef std::vector< GameEntity* > GameEntities;

///---------------------------------------------------------------------------------
/// Inline function implementations
///---------------------------------------------------------------------------------
///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline GameEntity::GameEntity( const Vector3& position, const EulerAngles& orientation, const Vector3& boundingBoxOffsets )
{
	m_physics.m_position = position;
	m_physics.m_orientationDegrees = orientation;

	m_boundingBoxOffsets = boundingBoxOffsets;

	m_boundingBox.m_mins.x = -m_boundingBoxOffsets.x;
	m_boundingBox.m_mins.y = -m_boundingBoxOffsets.y;
	m_boundingBox.m_mins.z = -m_boundingBoxOffsets.z;

	m_boundingBox.m_maxs.x = m_boundingBoxOffsets.x;
	m_boundingBox.m_maxs.y = m_boundingBoxOffsets.y;
	m_boundingBox.m_maxs.z = m_boundingBoxOffsets.z;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline const Vector3 GameEntity::GetPosition() const
{
	return m_physics.m_position;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline const Vector3 GameEntity::GetVelocity() const
{
	return m_physics.m_velocity;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline const EulerAngles GameEntity::GetOrientation() const
{
	return m_physics.m_orientationDegrees;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline const EulerAngles GameEntity::GetAngularVelocity() const
{
	return m_physics.m_orientationVelocityDegreesPerSecond;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline const Vector3 GameEntity::GetBottomPosition() const
{
	return m_bottomPosition;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline void GameEntity::SetPosition( const Vector3& position )
{
	m_physics.m_position = position;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline void GameEntity::SetVelocity( const Vector3& velocity )
{
	m_physics.m_velocity = velocity;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline void GameEntity::SetOrientation( const EulerAngles& orientation )
{
	m_physics.m_orientationDegrees = orientation;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline void GameEntity::SetAngularVelocity( const EulerAngles& angularVelocity )
{
	m_physics.m_orientationVelocityDegreesPerSecond = angularVelocity;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline void GameEntity::ApplyAcceleration( const Vector3& acceleration )
{
	m_physics.ApplyAcceleration( acceleration );
}

#endif