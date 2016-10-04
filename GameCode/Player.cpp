//=================================================================================
// Player.cpp
// Author: Tyler George
// Date  : April 24, 2015
//=================================================================================

#include "GameCode/Player.hpp"
#include "GameCode/World.hpp"

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Player::Player( const Vector3& position, const EulerAngles& orientation, const Vector3& boundingBoxOffset )
	: GameEntity( position, orientation, boundingBoxOffset )
	, m_currentMovementMode( PLAYER_WALK )
	, m_isActivelyMoving( false )
	, m_isInWater( false )
	, m_isOnGround( false )
{

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Player::ProcessInput( InputSystem* inputSystem, EulerAngles& orientation, World* world, WorldCoords& selectedBlockCoords, Vector3& selectedFaceNormal, bool debugRaycast )
{
	///---------------------------------------------------------------------------------
	/// Movement
	///---------------------------------------------------------------------------------
	float currentMovementSpeed = PLAYER_WALK_SPEED_UNITS_PER_SECOND;

	if( m_currentMovementMode == PLAYER_WALK )
		currentMovementSpeed = PLAYER_WALK_SPEED_UNITS_PER_SECOND;

	else if( m_currentMovementMode == PLAYER_RUN )
		currentMovementSpeed = PLAYER_RUN_SPEED_UNITS_PER_SECOND;

	else if( m_currentMovementMode == PLAYER_FLY )
		currentMovementSpeed = PLAYER_FLY_SPEED_UNITS_PER_SECOND;

	else if( m_currentMovementMode == PLAYER_NO_CLIP )
		currentMovementSpeed = PLAYER_FLY_SPEED_UNITS_PER_SECOND * 8.0f;

	float yawRadians = ConvertDegreesToRadians( orientation.yawDegrees );

	Vector3 playerForwardXY( cos( yawRadians ), sin( yawRadians ), 0.0f );
	Vector3 playerLeftXY( -playerForwardXY.y, playerForwardXY.x, 0.0f );

	Vector3 playerMovementVector( 0.0f, 0.0f, 0.0f );

	if( inputSystem->IsKeyDown( 'W' ) )
		playerMovementVector += playerForwardXY;
	if( inputSystem->IsKeyDown( 'A' ) )
		playerMovementVector += playerLeftXY;
	if( inputSystem->IsKeyDown( 'S' ) )
		playerMovementVector -= playerForwardXY;
	if( inputSystem->IsKeyDown( 'D' ) )
		playerMovementVector -= playerLeftXY;
	if( inputSystem->IsKeyDown( VK_SHIFT ) )
		playerMovementVector.z -= 1.0f;
	if( inputSystem->IsKeyDown( VK_SPACE ) && ( m_currentMovementMode == PLAYER_FLY || m_currentMovementMode == PLAYER_NO_CLIP ) )
		playerMovementVector.z += 1.0f;


	if( inputSystem->WasKeyJustReleased( VK_F1 ) )
	{
		m_currentMovementMode = (PlayerMovementMode) ( (int) m_currentMovementMode + 1 );

		if( m_currentMovementMode >= NUM_PLAYER_MOVEMENT_MODES )
			m_currentMovementMode = (PlayerMovementMode) 0;
	}
	playerMovementVector.Normalize();

	if( playerMovementVector != Vector3( 0.0f, 0.0f, 0.0f ) )
	{
		//ConsolePrintf("set velocity vector: %f, %f, %f\n ", playerMovementVector.x, playerMovementVector.y, playerMovementVector.z );
		m_isActivelyMoving = true;
		playerMovementVector *= currentMovementSpeed;
		playerMovementVector.z += GetVelocity().z;
		SetVelocity( playerMovementVector );
	}
	else
		m_isActivelyMoving = false;

	// jumping and water z movement
	if( inputSystem->IsKeyDown( VK_SPACE ) && ( m_currentMovementMode == PLAYER_WALK || m_currentMovementMode == PLAYER_RUN ) && m_isOnGround && !m_isInWater )
		SetVelocity( Vector3( GetVelocity().x, GetVelocity().y, JUMP_ACCELERATION.z ) );
	if( inputSystem->IsKeyDown( VK_SPACE ) && ( m_currentMovementMode == PLAYER_WALK || m_currentMovementMode == PLAYER_RUN ) && m_isInWater )
		SetVelocity( Vector3( GetVelocity().x, GetVelocity().y, GetVelocity().z + 9.0f ) );

	///---------------------------------------------------------------------------------
	/// Mouse Movement
	///---------------------------------------------------------------------------------
    static Vector2 mouseResetPosition;
    mouseResetPosition.x = 400.0f;
    mouseResetPosition.y = 300.0f;

	Vector2 mousePosition = inputSystem->GetMousePosition();
	Vector2 distanceMouseHasMovedSinceLastFrame = mousePosition - mouseResetPosition;
	inputSystem->SetMousePosition( mouseResetPosition );

	float newYaw = GetOrientation().yawDegrees - (YAW_DEGREES_PER_SECOND * distanceMouseHasMovedSinceLastFrame.x);
	float newPitch = GetOrientation().pitchDegrees + (PITCH_DEGREES_PER_SECOND * distanceMouseHasMovedSinceLastFrame.y);
    //ConsolePrintf( "Reset Pos: %f, %f", mouseResetPosition.x, mouseResetPosition.y );
    //ConsolePrintf( "Mouse Pos; %f, %f\n", mousePosition.x, mousePosition.y );
    //ConsolePrintf( "Dist mouse moved: x: %f  --  y: %f\n", distanceMouseHasMovedSinceLastFrame.x, distanceMouseHasMovedSinceLastFrame.y );
	if( newPitch < -89.0f )
		newPitch = -89.0f;
	if( newPitch > 89.0f )
		newPitch = 89.0f;

	SetOrientation( EulerAngles( newYaw, newPitch, GetOrientation().rollDegrees ) );
	orientation = GetOrientation();

	if( !debugRaycast )
	{		
		Vector3 cameraForward = GetOrientation().GetForwardVector();
		cameraForward *= DISTANCE_CAN_AFFECT_BLOCKS;

		if( world->AreWorldCoordsInWorld( GetPosition() + CAMERA_OFFSET_FROM_PLAYER_POS + cameraForward ) )
		{	
// 			m_raycastDebugStartPos = m_camera->m_position;
// 			m_raycastDebugEndPos = m_camera->m_position + cameraForward;

			RayCast3DResult result = world->RayCast( GetPosition() + CAMERA_OFFSET_FROM_PLAYER_POS, GetPosition() + CAMERA_OFFSET_FROM_PLAYER_POS + cameraForward, false );

			if( result.didImpact )
			{
				selectedBlockCoords = result.impactedBlock.m_chunk->GetWorldCoordsFromBlockIndex( result.impactedBlock.m_blockIndex );
				selectedFaceNormal = result.impactSurfaceNormal;
			}
			else
				selectedFaceNormal = Vector3( -1.0f, -1.0f, -1.0f );
		}
		else
			selectedFaceNormal = Vector3( -1.0f, -1.0f, -1.0f );
	}
	else
		selectedFaceNormal = Vector3( -1.0f, -1.0f, -1.0f );

	///---------------------------------------------------------------------------------
	/// Mouse Clicks
	///---------------------------------------------------------------------------------
	static double lastLeftClickTime = 0.0;
	static double lastRightClickTime = 0.0;

	if( inputSystem->IsMouseButtonDown(MOUSE_LEFT) && GetCurrentSeconds() - lastLeftClickTime >= CLICK_COOLDOWN_TIME )
	{
		world->DestroyBlockAtWorldCoords( selectedBlockCoords );
		lastLeftClickTime = GetCurrentSeconds();
	}

	if( inputSystem->IsMouseButtonDown(MOUSE_RIGHT) && GetCurrentSeconds() - lastRightClickTime >= CLICK_COOLDOWN_TIME )
	{
		world->PlaceBlockAtSelectedBlockWorldCoordsAndNormal();
		lastRightClickTime = GetCurrentSeconds();
	}
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Player::Update( double deltaSeconds, World* world )
{
	//ConsolePrintf( "Player z velocity: %f \n ", GetVelocity().z );

	if( !m_isActivelyMoving )
	{
		Vector3 velocity = GetVelocity();
		if( m_currentMovementMode == PLAYER_WALK || m_currentMovementMode == PLAYER_RUN )
			SetVelocity( Vector3( velocity.x * 0.6f, velocity.y * 0.6f, velocity.z ) );

		else if( m_currentMovementMode == PLAYER_FLY )
			SetVelocity( Vector3( velocity.x * 0.3f, velocity.y * 0.3f, velocity.z ) );

		else if( m_currentMovementMode == PLAYER_NO_CLIP )
			SetVelocity( GetVelocity() * 0.0f );
	}

	if( GetPosition().z < CHUNK_BLOCKS_Z && GetPosition().z >= 0.0f && m_currentMovementMode != PLAYER_NO_CLIP )
	{
		if( world->IsBlockWater( GetBottomPosition() ) )
			m_isInWater = true;
		else
			m_isInWater = false;

		WorldCoords blockBelow = WorldCoords( GetBottomPosition().x, GetBottomPosition().y, GetBottomPosition().z - 0.01f );

		if( world->IsBlockSolid( blockBelow ) )
			m_isOnGround = true;
		else
			m_isOnGround = false;
	}

	if( m_isInWater )
		SetVelocity( GetVelocity() * WATER_SPEED_MODIFIERS );

	GameEntity::Update(deltaSeconds);
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Player::Render( OpenGLRenderer* renderer, bool debugModeEnabled ) const
{
	if( !renderer )
		return;

	if( debugModeEnabled )
		renderer->DrawPoint3D( GetPosition(), Rgba::AQUA, 10.0f );
	
	GameEntity::Render( renderer, debugModeEnabled );
}