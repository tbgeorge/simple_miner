//=================================================================================
// Player.hpp
// Author: Tyler George
// Date  : April 23, 2015
//=================================================================================

#pragma once

#ifndef __included_Player__
#define __included_Player__

///---------------------------------------------------------------------------------
/// Includes
///---------------------------------------------------------------------------------
#include "GameCode/GameEntity.hpp"
#include "Engine/Renderer/Camera3D.hpp"
#include "Engine/Input/InputSystem.hpp"
class World;

typedef Vector3 WorldCoords;

///---------------------------------------------------------------------------------
/// Constants
///---------------------------------------------------------------------------------
const float PLAYER_WALK_SPEED_UNITS_PER_SECOND = 4.22f;
const float PLAYER_RUN_SPEED_UNITS_PER_SECOND = 5.77f;
const float PLAYER_FLY_SPEED_UNITS_PER_SECOND = 9.09f;
const float YAW_DEGREES_PER_SECOND = 0.15f;
const float PITCH_DEGREES_PER_SECOND = 0.15f;

const Vector3 WATER_SPEED_MODIFIERS = Vector3( 0.7f, 0.7f, 0.3f );
const Vector3 JUMP_ACCELERATION = Vector3( 0.0f, 0.0f, 12.0f );

const float DISTANCE_CAN_AFFECT_BLOCKS = 8.0f;
const double CLICK_COOLDOWN_TIME = 0.2;

const Vector3 CAMERA_OFFSET_FROM_PLAYER_POS = Vector3( 0.0f, 0.0f, 0.695f );


enum PlayerMovementMode{
	PLAYER_WALK,
	PLAYER_RUN,
	PLAYER_FLY,
	PLAYER_NO_CLIP,
	NUM_PLAYER_MOVEMENT_MODES
};

///---------------------------------------------------------------------------------
/// Typedefs
///---------------------------------------------------------------------------------

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
class Player : public GameEntity
{
public:
	///---------------------------------------------------------------------------------
	/// Constructors/Destructors
	///---------------------------------------------------------------------------------
	Player( const Vector3& position, const EulerAngles& orientation, const Vector3& boundingBoxOffset );

	///---------------------------------------------------------------------------------
	/// Accessors/Queries
	///---------------------------------------------------------------------------------
	PlayerMovementMode GetCurrentMovementMode() { return m_currentMovementMode; }
	bool IsOnGround() { return m_isOnGround; }
	bool IsInWater() { return m_isInWater; }

	///---------------------------------------------------------------------------------
	/// Mutators
	///---------------------------------------------------------------------------------

	///---------------------------------------------------------------------------------
	/// Update
	///---------------------------------------------------------------------------------
	void ProcessInput( InputSystem* inputSystem, EulerAngles& orientation, World* world, WorldCoords& selectedBlockNormal, Vector3& selectedFaceNormal, bool debugRaycast );
	void Update( double deltaSeconds, World* world );

	///---------------------------------------------------------------------------------
	/// Render
	///---------------------------------------------------------------------------------
	void Render( OpenGLRenderer* renderer, bool debugModeEnabled ) const;


	///---------------------------------------------------------------------------------
	/// Public Member Variables
	///---------------------------------------------------------------------------------

	///---------------------------------------------------------------------------------
	/// Static Variables
	///---------------------------------------------------------------------------------


private:
	///---------------------------------------------------------------------------------
	/// Private Functions
	///---------------------------------------------------------------------------------

	///---------------------------------------------------------------------------------
	/// Private Member Variables
	///---------------------------------------------------------------------------------
	PlayerMovementMode m_currentMovementMode;
	bool m_isActivelyMoving;
	bool m_isInWater;
	bool m_isOnGround;
};

///---------------------------------------------------------------------------------
/// Inline function implementations
///---------------------------------------------------------------------------------

#endif