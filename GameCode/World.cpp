//=================================================================================
// World.cpp
// Author: Tyler George
// Date  : January 29, 2015
//=================================================================================

#include "GameCode/World.hpp"

BlockDataList World::s_dirtyBlocks;
Vector3s World::s_debugPoints;

Rgba World::s_lightLevels[ NUM_LIGHT_LEVELS ] = {
	Rgba( 0.0250f, 0.0250f, 0.0250f, 1.0f ),	// 0
	Rgba( 0.0900f, 0.0900f, 0.0900f, 1.0f ),	// 1
	Rgba( 0.1550f, 0.1550f, 0.1550f, 1.0f ),	// 2
	Rgba( 0.2200f, 0.2200f, 0.2200f, 1.0f ),	// 3
	Rgba( 0.2850f, 0.2850f, 0.2850f, 1.0f ),	// 4
	Rgba( 0.3500f, 0.3500f, 0.3500f, 1.0f ),	// 5
	Rgba( 0.4150f, 0.4150f, 0.4150f, 1.0f ),	// 6
	Rgba( 0.4800f, 0.4800f, 0.4800f, 1.0f ),	// 7
	Rgba( 0.5450f, 0.5450f, 0.5450f, 1.0f ),	// 8
	Rgba( 0.6100f, 0.6100f, 0.6100f, 1.0f ),	// 9
	Rgba( 0.6750f, 0.6750f, 0.6750f, 1.0f ),	// 10
	Rgba( 0.7400f, 0.7400f, 0.7400f, 1.0f ),	// 11
	Rgba( 0.8050f, 0.8050f, 0.8050f, 1.0f ),	// 12
	Rgba( 0.8700f, 0.8700f, 0.8700f, 1.0f ),	// 13
	Rgba( 0.9350f, 0.9350f, 0.9350f, 1.0f ),	// 14
	Rgba( 1.0000f, 1.0000f, 1.0000f, 1.0f )		// 15
};

int World::s_outdoorLightValue = 0;

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
World::World( const Vector2& displaySize )
	: m_displaySize( displaySize )
	, m_camera( NULL )
	, m_player( NULL )
	, m_userInterface( NULL )
	, m_skyboxTexture( NULL )
	, m_nightSkyboxTexture( NULL )
	, m_debugRaycast( false )
	, m_viewFrustumDebug( false )
	, m_selectedFaceNormal( Vector3( -1.0f, -1.0f, -1.0f ) )
	, m_timeOfDayNormalized( 0.0f )
	, m_replaceBlockType( BLOCK_TYPE_AIR )
{
	//m_seed = TripleRandomInt();
	m_seed = 12345678;

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
World::~World()
{
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void World::Startup()
{
	BlockDefinition::InitializeBlockDefinitions();
	m_camera = new Camera3D( PLAYER_START_POS, PLAYER_START_ORIENTATION );
	m_player = new Player( PLAYER_START_POS, PLAYER_START_ORIENTATION, PLAYER_BBOX_OFFSETS );
	m_userInterface = new UserInterface( m_displaySize );
// 	m_player->SetVelocity( Vector3( 0.0f, 0.0f, 0.0f ) );
	
	m_skyboxTexture = AnimatedTexture::CreateOrGetAnimatedTexture( "Data/Images/skybox_texture.png", 4, 3 );
	m_nightSkyboxTexture = AnimatedTexture::CreateOrGetAnimatedTexture( "Data/Images/night_skybox.png", 4, 3 );
	m_timeOfDayNormalized = 0.5f;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void World::Shutdown()
{

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
const RayCast3DResult World::StepAndSample( const Vector3& startWorldCoords, const Vector3& endWorldCoords ) const
{
	RayCast3DResult result;

	IntVector3 blockStartPos =  IntVector3( (int) floorf( startWorldCoords.x ), (int) floorf( startWorldCoords.y ), (int) floorf( startWorldCoords.z ) );
	IntVector3 blockEndPos = IntVector3( (int) floorf( endWorldCoords.x ), (int) floorf( endWorldCoords.y ), (int) floorf( endWorldCoords.z ) );

	// setup
	IntVector3 intRayDisplacement = blockEndPos - blockStartPos;
	Vector3 rayDisplacement = endWorldCoords - startWorldCoords;

	Vector3 previousPosition = startWorldCoords;
	Vector3 currentPosition;

	Vector3 previousBlockPosition = Vector3( floorf( startWorldCoords.x ), floorf( startWorldCoords.y ), floorf( startWorldCoords.z ) );
	Vector3 currentBlockPosition;

	for( float t = 0.0f; t <= 1.0f; t += STEP_AND_SAMPLE_T_INTERVAL )
	{
		currentPosition = Interpolate( startWorldCoords, endWorldCoords, t );

		currentBlockPosition = Vector3( floorf( currentPosition.x ), floorf( currentPosition.y ), floorf( currentPosition.z ) );
		ChunkCoords currentChunkCoords = Chunk::GetChunkCoordsFromWorldCoords( currentPosition );
		Chunk* currentChunk = NULL;
		if( m_activeChunks.find( currentChunkCoords ) != m_activeChunks.end() )
			currentChunk = m_activeChunks.at( currentChunkCoords );
		else
		{
			result.didImpact = false;
			return result;
		}

		LocalBlockCoords currentBlockLocalCoords = currentChunk->GetLocalBlockCoordsFromWorldCoords( currentBlockPosition );
		BlockIndex currentBlockIndex = Chunk::GetIndexFromLocalBlockCoords( currentBlockLocalCoords );
		Block& currentBlock = currentChunk->GetBlockAtIndex( currentBlockIndex );

		if( currentBlock.IsSolid() )
		{
			result.didImpact = true;
			result.impactFraction = t;
			result.impactedBlock = BlockData( currentChunk, currentBlockIndex );
			result.impactBlockWorldCoords = currentBlockPosition;

			if( previousBlockPosition.x < currentBlockPosition.x )
				result.impactSurfaceNormal = Vector3( -1.0f, 0.0f, 0.0f );
			else if( previousBlockPosition.x > currentBlockPosition.x )
				result.impactSurfaceNormal = Vector3( 1.0f, 0.0f, 0.0f );
			else if( previousBlockPosition.y < currentBlockPosition.y )
				result.impactSurfaceNormal = Vector3( 0.0f , -1.0f, 0.0f );
			else if( previousBlockPosition.y > currentBlockPosition.y )
				result.impactSurfaceNormal = Vector3( 0.0f, 1.0f, 0.0f );
			else if( previousBlockPosition.z < currentBlockPosition.z )
				result.impactSurfaceNormal = Vector3( 0.0f , 0.0f, -1.0f );
			else if( previousBlockPosition.z > currentBlockPosition.z )
				result.impactSurfaceNormal = Vector3( 0.0f, 0.0f, 1.0f );

			return result;
		}

		previousPosition = currentPosition;
		previousBlockPosition = currentBlockPosition;
	}

	result.didImpact = false;
	return result;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
const RayCast3DResult World::RayCast( const Vector3& startWorldCoords, const Vector3& endWorldCoords, bool tracePoints, float xRadius /* = 0.0f */, float yRadius /* = 0.0f */, float zRadius /* = 0.0f */ )
{
	if( tracePoints )
	{
		m_raycastDebugPoints.clear();
		m_cameraRaycastBlocks.clear();
	}

	WorldBlockCoords blockStartPos = Chunk::GetWorldBlockCoordsFromWorldCoords( startWorldCoords );
	WorldBlockCoords blockEndPos = Chunk::GetWorldBlockCoordsFromWorldCoords( endWorldCoords );

	// setup
	Vector3 worldRayDisplacement = endWorldCoords - startWorldCoords;
	IntVector3 blockRayDisplacement = blockEndPos - blockStartPos;

	WorldBlockCoords currentBlockCoords = blockStartPos;

	if( tracePoints )
		m_cameraRaycastBlocks.push_back( currentBlockCoords );

	ChunkCoords currentChunkCoords = Chunk::GetChunkCoordsFromWorldBlockCoords( currentBlockCoords );
	Chunk* currentChunk = NULL;
	if( m_activeChunks.find( currentChunkCoords ) != m_activeChunks.end() )
		currentChunk = m_activeChunks.at( currentChunkCoords );
	else
	{
		RayCast3DResult result;
		result.didImpact = false;
		return result;
	}

	BlockIndex currentBlockIndex;
	BlockData currentBlockData;

	if( currentBlockCoords.z < CHUNK_BLOCKS_Z || currentBlockCoords.z >= 0 )
	{
		currentBlockIndex = Chunk::GetIndexFromWorldBlockCoords( currentBlockCoords );
		currentBlockData = BlockData( currentChunk, currentBlockIndex );
		Block& currentBlock = currentBlockData.GetBlock();

		if( currentBlock.IsSolid() )
		{
			RayCast3DResult result;
			result.didImpact = true;
			result.impactBlockWorldCoords = Chunk::GetWorldCoordMinsFromWorldBlockCoords( currentBlockCoords );
			result.impactWorldCoords = startWorldCoords;
			result.impactSurfaceNormal = Vector3( 0.0f, 0.0f, 0.0f );
			result.impactedBlock = currentBlockData;
			result.impactFraction = 0.0f;

			if( tracePoints)
				m_raycastDebugPoints.push_back( Chunk::GetWorldCoordMinsFromWorldBlockCoords( currentBlockCoords ) );

			return result;
		}
	}

	float tDeltaX = FLT_MAX;
	float tDeltaY = FLT_MAX;
	float tDeltaZ = FLT_MAX;

	if( worldRayDisplacement.x != 0.f )
		tDeltaX = abs( 1.0f / worldRayDisplacement.x );

	if( worldRayDisplacement.y != 0.f )
		tDeltaY = abs( 1.0f / worldRayDisplacement.y );

	if( worldRayDisplacement.z != 0.f )
		tDeltaZ = abs( 1.0f / worldRayDisplacement.z );

	int blockStepX = worldRayDisplacement.x < 0.0f ? -1 : 1;
	int blockStepY = worldRayDisplacement.y < 0.0f ? -1 : 1;
	int blockStepZ = worldRayDisplacement.z < 0.0f ? -1 : 1;


	int offsetToLeadingEdgeX = ( blockStepX + 1 ) / 2;
	int offsetToLeadingEdgeY = ( blockStepY + 1 ) / 2;
	int offsetToLeadingEdgeZ = ( blockStepZ + 1 ) / 2;


	float firstXBlockIntersection = (float) ( blockStartPos.x + offsetToLeadingEdgeX );
	float firstYBlockIntersection = (float) ( blockStartPos.y + offsetToLeadingEdgeY );
	float firstZBlockIntersection = (float) ( blockStartPos.z + offsetToLeadingEdgeZ );


	WorldCoords firstAxisIntersections( firstXBlockIntersection, firstYBlockIntersection, firstZBlockIntersection );

	if( worldRayDisplacement.x > 0.0f )
		firstAxisIntersections.x -= xRadius;
	else
		firstAxisIntersections.x += xRadius;

	if( worldRayDisplacement.y > 0.0f )
		firstAxisIntersections.y -= yRadius;
	else
		firstAxisIntersections.y += yRadius;

	if( worldRayDisplacement.z > 0.0f )
		firstAxisIntersections.z -= zRadius;
	else					   
		firstAxisIntersections.z += zRadius;

	float tOfNextXCrossing = ( abs( firstAxisIntersections.x - startWorldCoords.x ) * tDeltaX );
	float tOfNextYCrossing = ( abs( firstAxisIntersections.y - startWorldCoords.y ) * tDeltaY ); 
	float tOfNextZCrossing = ( abs( firstAxisIntersections.z - startWorldCoords.z ) * tDeltaZ ); 


	// main loop
	for( ; ; )
	{
		if( tOfNextXCrossing < tOfNextYCrossing && tOfNextXCrossing < tOfNextZCrossing )
		{
			if( tOfNextXCrossing > 1.0f )
			{
				RayCast3DResult result;
				result.didImpact = false;
				return result;
			}
			else
			{
				// increment
				currentBlockCoords.x += blockStepX;
				if( tracePoints )
					m_cameraRaycastBlocks.push_back( currentBlockCoords );

				//get block
				currentChunkCoords = Chunk::GetChunkCoordsFromWorldBlockCoords( currentBlockCoords );
				if( m_activeChunks.find( currentChunkCoords ) != m_activeChunks.end() )
					currentChunk = m_activeChunks.at( currentChunkCoords );
				else
				{
					RayCast3DResult result;
					result.didImpact = false;
					return result;
				}

				if( currentBlockCoords.z < CHUNK_BLOCKS_Z || currentBlockCoords.z >= 0 )
				{
					currentBlockIndex = Chunk::GetIndexFromWorldBlockCoords( currentBlockCoords );
					currentBlockData = BlockData( currentChunk, currentBlockIndex );
					Block& currentBlock = currentBlockData.GetBlock();

					if( currentBlock.IsSolid() )
					{
						RayCast3DResult result;
						result.didImpact = true;
						result.impactBlockWorldCoords = Chunk::GetWorldCoordMinsFromWorldBlockCoords( currentBlockCoords );
						result.impactWorldCoords = Interpolate( startWorldCoords, endWorldCoords, tOfNextXCrossing );
						result.impactedBlock = currentBlockData;
						result.impactFraction = tOfNextXCrossing;
						result.impactSurfaceNormal = Vector3( (float) -blockStepX, 0.0f, 0.0f );
						return result;
					}
					else
					{
						Vector3 intersection = Interpolate( startWorldCoords, endWorldCoords, tOfNextXCrossing );
						if( tracePoints)
							m_raycastDebugPoints.push_back( intersection );
						tOfNextXCrossing += tDeltaX;
					}
				}
			}
		}
		else if( tOfNextYCrossing <= tOfNextXCrossing && tOfNextYCrossing <= tOfNextZCrossing )
		{
			if( tOfNextYCrossing > 1.0f )
			{
				RayCast3DResult result;
				result.didImpact = false;
				return result;
			}
			else
			{
				//increment
				currentBlockCoords.y += blockStepY;
				if( tracePoints )
					m_cameraRaycastBlocks.push_back( currentBlockCoords );

				// get block
				currentChunkCoords = Chunk::GetChunkCoordsFromWorldBlockCoords( currentBlockCoords );
				if( m_activeChunks.find( currentChunkCoords ) != m_activeChunks.end() )
					currentChunk = m_activeChunks.at( currentChunkCoords );
				else
				{
					RayCast3DResult result;
					result.didImpact = false;
					return result;
				}

				if( currentBlockCoords.z < CHUNK_BLOCKS_Z || currentBlockCoords.z >= 0 )
				{
					currentBlockIndex = Chunk::GetIndexFromWorldBlockCoords( currentBlockCoords );
					currentBlockData = BlockData( currentChunk, currentBlockIndex );
					Block& currentBlock = currentBlockData.GetBlock();

					if( currentBlock.IsSolid() )
					{
						RayCast3DResult result;
						result.didImpact = true;
						result.impactBlockWorldCoords = Chunk::GetWorldCoordMinsFromWorldBlockCoords( currentBlockCoords );
						result.impactWorldCoords = Interpolate( startWorldCoords, endWorldCoords, tOfNextYCrossing );
						result.impactedBlock = currentBlockData;
						result.impactFraction = tOfNextYCrossing;
						result.impactSurfaceNormal = Vector3( 0.0f, (float) -blockStepY, 0.0f );
						return result;
					}
					// todo fix to allow for raycasting into the world from outside the world
					else
					{
						Vector3 intersection = Interpolate( startWorldCoords, endWorldCoords, tOfNextYCrossing );
						if( tracePoints)
							m_raycastDebugPoints.push_back( intersection );
						tOfNextYCrossing += tDeltaY;
					}
				}
			}
		}

		else if( tOfNextZCrossing <= tOfNextXCrossing && tOfNextZCrossing <= tOfNextYCrossing )
		{
			if( tOfNextZCrossing > 1.0f )
			{
				RayCast3DResult result;
				result.didImpact = false;
				return result;
			}
			else
			{
				//increment
				currentBlockCoords.z += blockStepZ;
				if( tracePoints )
					m_cameraRaycastBlocks.push_back( currentBlockCoords );

				// get block
				currentChunkCoords = Chunk::GetChunkCoordsFromWorldBlockCoords( currentBlockCoords );
				if( m_activeChunks.find( currentChunkCoords ) != m_activeChunks.end() )
					currentChunk = m_activeChunks.at( currentChunkCoords );
				else
				{
					RayCast3DResult result;
					result.didImpact = false;
					return result;
				}

				if( currentBlockCoords.z < CHUNK_BLOCKS_Z || currentBlockCoords.z >= 0 )
				{
					currentBlockIndex = Chunk::GetIndexFromWorldBlockCoords( currentBlockCoords );
					currentBlockData = BlockData( currentChunk, currentBlockIndex );
					Block& currentBlock = currentBlockData.GetBlock();

					if( currentBlock.IsSolid() )
					{
						RayCast3DResult result;
						result.didImpact = true;
						result.impactBlockWorldCoords = Chunk::GetWorldCoordMinsFromWorldBlockCoords( currentBlockCoords );
						result.impactWorldCoords = Interpolate( startWorldCoords, endWorldCoords, tOfNextZCrossing );
						result.impactedBlock = currentBlockData;
						result.impactFraction = tOfNextZCrossing;
						result.impactSurfaceNormal = Vector3( 0.0f, 0.0f, (float) -blockStepZ );
						return result;
					}
					else
					{
						Vector3 intersection = Interpolate( startWorldCoords, endWorldCoords, tOfNextZCrossing );
						if( tracePoints)
							m_raycastDebugPoints.push_back( intersection );
						tOfNextZCrossing += tDeltaZ;
					}
				}
			}
		}
	}
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
bool World::IsChunkActive( const ChunkCoords& chunkCoords )
{
	if( m_activeChunks.find( chunkCoords ) != m_activeChunks.end() )
		return true;
	return false;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Block& World::GetBlock( const BlockData& blockData )
{
	return blockData.m_chunk->GetBlockAtIndex(blockData.m_blockIndex);
}

///---------------------------------------------------------------------------------
/// 
///---------------------------------------------------------------------------------
BlockData World::GetBlockData( Chunk* currentChunk, const LocalBlockCoords& requestedBlockCoords )
{
	
	Chunk* chunk = NULL;
	BlockIndex index = 0;

	if( requestedBlockCoords.z == CHUNK_BLOCKS_Z )
		return BlockData( chunk, index );
	else if( requestedBlockCoords.z == -1 )
		return BlockData( chunk, index );
	else if( requestedBlockCoords.x == CHUNK_BLOCKS_X )
	{
		chunk = currentChunk->m_chunkToEast;
		index = Chunk::GetIndexFromLocalBlockCoords( IntVector3( 0, requestedBlockCoords.y, requestedBlockCoords.z ) );
	}
	else if( requestedBlockCoords.x == -1 )
	{
		chunk = currentChunk->m_chunkToWest;
		index = Chunk::GetIndexFromLocalBlockCoords( IntVector3( CHUNK_BLOCKS_X - 1, requestedBlockCoords.y, requestedBlockCoords.z ) );
	}
	else if( requestedBlockCoords.y == CHUNK_BLOCKS_Y )
	{
		chunk = currentChunk->m_chunkToNorth;
		index = Chunk::GetIndexFromLocalBlockCoords( IntVector3( requestedBlockCoords.x, 0, requestedBlockCoords.z ) );
	}
	else if( requestedBlockCoords.y == -1 )
	{
		chunk = currentChunk->m_chunkToSouth;
		index = Chunk::GetIndexFromLocalBlockCoords( IntVector3( requestedBlockCoords.x, CHUNK_BLOCKS_Y - 1, requestedBlockCoords.z ) );
	}
	else
	{
		chunk = currentChunk;
		index = Chunk::GetIndexFromLocalBlockCoords( requestedBlockCoords );
	}

	return BlockData( chunk, index );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
BlockData BlockData::GetEastNeighbor() const
{
	if( !IsValid() )
		return BlockData( NULL, 0 );
	if( IsOnEastEdge() )
	{
		Chunk* eastChunk = m_chunk->m_chunkToEast;
		if( eastChunk )
		{
			BlockIndex eastNeighborIndex = m_blockIndex & ~CHUNK_X_MASK;
			return BlockData( eastChunk, eastNeighborIndex );
		}
		return BlockData( NULL, 0 );
	}
	else
		return BlockData( m_chunk, m_blockIndex + 1 );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
BlockData BlockData::GetNorthNeighbor() const
{
	if( !IsValid() )
		return BlockData( NULL, 0 );
	if( IsOnNorthEdge() )
	{
		Chunk* northChunk = m_chunk->m_chunkToNorth;
		if( northChunk )
		{
			BlockIndex northNeighborIndex = m_blockIndex & 0xff0f;//~CHUNK_Y_MASK;
			return BlockData( northChunk, northNeighborIndex );
		}
		return BlockData( NULL, 0 );
	}
	else
		return BlockData( m_chunk, m_blockIndex + CHUNK_BLOCKS_X );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
BlockData BlockData::GetWestNeighbor() const
{
	if( !IsValid() )
		return BlockData( NULL, 0 );
	if( IsOnWestEdge() )
	{
		Chunk* westChunk = m_chunk->m_chunkToWest;
		if( westChunk )
		{
			BlockIndex westNeighborIndex = m_blockIndex | CHUNK_X_MASK;
			return BlockData( westChunk, westNeighborIndex );
		}
		return BlockData( NULL, 0 );
	}
	else
		return BlockData( m_chunk, m_blockIndex - 1 );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
BlockData BlockData::GetSouthNeighbor() const
{
	if( !IsValid() )
		return BlockData( NULL, 0 );
	if( IsOnSouthEdge() )
	{
		Chunk* southChunk = m_chunk->m_chunkToSouth;
		if( southChunk )
		{
			BlockIndex southNeighborIndex = m_blockIndex | 0x00f0;//CHUNK_Y_MASK;
			return BlockData( southChunk, southNeighborIndex );
		}
		return BlockData( NULL, 0 );
	}
	else
		return BlockData( m_chunk, m_blockIndex -  CHUNK_BLOCKS_X );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
BlockData BlockData::GetTopNeighbor() const
{
	if( !IsValid() )
		return BlockData( NULL, 0 );
	if( IsOnTopEdge() )
		return BlockData( NULL, 0 );
	else
		return BlockData( m_chunk, m_blockIndex + CHUNK_BLOCKS_PER_LAYER );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
BlockData BlockData::GetBottomNeighbor() const
{
	if( !IsValid() )
		return BlockData( NULL, 0 );
	if( IsOnBottomEdge() )
		return BlockData( NULL, 0 );
	else
		return BlockData( m_chunk, m_blockIndex - CHUNK_BLOCKS_PER_LAYER );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
int World::GetOutdoorLightLevelFromTimeOfDay()
{
	float timeOfDay = 0.0f;

	bool sunSet = false;
	if( m_timeOfDayNormalized > 0.5f )
	{
		timeOfDay = m_timeOfDayNormalized - 0.5f;
		sunSet = true;
	}
	else
	{
		timeOfDay = m_timeOfDayNormalized;
		sunSet = false;
	}

	if( sunSet )
		timeOfDay = 0.5f - timeOfDay;

	float outdoorLightf = RangeMap( timeOfDay, 0.0f, 0.5f, -19.0f, 35.0f );
	outdoorLightf = Clamp( outdoorLightf, 6.0f, 15.0f );
	outdoorLightf = floorf( outdoorLightf );

	return (int) floorf( outdoorLightf );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void World::ProcessInput( InputSystem* inputSystem )
{
	if( inputSystem->WasKeyJustReleased( VK_ESCAPE ) )
	{
		SaveAllActiveChunks();
		inputSystem->SetSystemQuit();
	}

	if( g_manuallyAdvanceLighting && inputSystem->WasKeyJustReleased( 'L' ) )
		UpdateLighting();

	if( inputSystem->WasKeyJustReleased( 'K' ) )
		m_debugRaycast = !m_debugRaycast;

	if( inputSystem->WasKeyJustReleased( 'F' ) )
		m_viewFrustumDebug = !m_viewFrustumDebug;

	m_player->ProcessInput( inputSystem, m_camera->m_orientation, this, m_selectedBlockCoords, m_selectedFaceNormal, m_debugRaycast );

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void World::Update( InputSystem* inputSystem, double deltaSeconds, bool debugModeEnabled )
{
// 	static int frameNumber = 0;
// 	frameNumber ++;
// 	static double timeLastUpdateBegan = GetCurrentSeconds();
// 	double timeNow = GetCurrentSeconds();
// 	double elapsedSeconds = timeNow - timeLastUpdateBegan;
// 	timeLastUpdateBegan = timeNow;

	if( debugModeEnabled )
	{
		m_test2DWorm.points.clear();
		m_test2DWorm.pointRadii.clear();
		//GeneratePerlinWorm2D( Vector3( 0.0f, 0.0f, 75.0f ), ConvertDegreesToRadians( 45.0f ), m_seed, m_test2DWorm );
	}

	//update time of day and light levels
	m_timeOfDayNormalized += ( (float) deltaSeconds ) / REAL_SECONDS_PER_GAME_DAY;
	if( m_timeOfDayNormalized >= 1.0f )
		m_timeOfDayNormalized -= 1.0f;

	s_outdoorLightValue = GetOutdoorLightLevelFromTimeOfDay();
	UpdateOutdoorLighting();


	// apply gravity to player once per update
	if( m_player->GetCurrentMovementMode() == PLAYER_WALK || m_player->GetCurrentMovementMode() == PLAYER_RUN )
		m_player->ApplyAcceleration( GRAVITY );

	double moveDeltaSeconds = deltaSeconds;

	while( moveDeltaSeconds > 0.0f )
		moveDeltaSeconds -= MovePlayer( moveDeltaSeconds );


	m_camera->m_position = m_player->GetPosition() + CAMERA_OFFSET_FROM_PLAYER_POS;

	//update world
	bool activatedChunk = ActivateNearestInactiveChunkInsideVisibilityRadius();
	bool deactivatedChunk = DeactivateFurthestActiveChunkOutsideVisibilityRadius();

	if( activatedChunk && !deactivatedChunk )
		ActivateNearestInactiveChunkInsideVisibilityRadius();

	if( deactivatedChunk && !activatedChunk )
		DeactivateFurthestActiveChunkOutsideVisibilityRadius();

	if( !g_manuallyAdvanceLighting )
		UpdateLighting();

	//update UI
	m_userInterface->Update( deltaSeconds, inputSystem );
	m_currentBlockType = m_userInterface->GetCurrentBlockType();

	//update chunks

	for( ChunkMap::const_iterator chunkIter = m_activeChunks.begin(); chunkIter != m_activeChunks.end(); ++chunkIter )
		chunkIter->second->UpdateDistanceToCamera( m_camera->m_position );

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
double World::MovePlayer( double deltaSeconds )
{

	double deltaSecondsInitial = deltaSeconds;

	Vector3 playerStartPos = m_player->GetPosition();
	Vector3s playerStartPosPoints = m_player->GetBoundingBoxVertices();
	m_player->Update( deltaSeconds, this );
	Vector3 playerEndPos = m_player->GetPosition();
	Vector3s playerEndPosPoints = m_player->GetBoundingBoxVertices();


	PlayerMovementMode movementMode = m_player->GetCurrentMovementMode();

	if( movementMode != PLAYER_NO_CLIP )
	{
		if( playerEndPos.z < CHUNK_BLOCKS_Z && playerEndPos.z >= 0.0f )
		{
			// raycast from each bounding box point
			RayCast3DResults results;
			Vector3s::const_iterator startPointIter = playerStartPosPoints.begin();
			Vector3s::const_iterator endPointIter = playerEndPosPoints.begin();
			for( ; startPointIter != playerStartPosPoints.end(); ++startPointIter, ++endPointIter )
			{
				const Vector3& startPoint = ( *startPointIter );
				const Vector3& endPoint = ( *endPointIter );

				RayCast3DResult result = RayCast( startPoint, endPoint, false );
				results.push_back( result );
			}

			// loop through results to find first impact
			float smallestImpactFraction = 1.1f;
			RayCast3DResult firstImpact;
			int bboxCornerNumber = -1;

			int bboxCornerCounter = 0;
			for( RayCast3DResults::const_iterator resultIter = results.begin(); resultIter != results.end(); ++resultIter )
			{
				RayCast3DResult result = ( *resultIter );

				if( result.didImpact && result.impactFraction < smallestImpactFraction )
				{
					smallestImpactFraction = result.impactFraction;
					firstImpact = result;
					bboxCornerNumber = bboxCornerCounter;
				}
				++bboxCornerCounter;
			}

			if( smallestImpactFraction <= 1.0f )
			{
				if( firstImpact.impactFraction == 0.0f )
 					m_player->SetVelocity( Vector3( 0.0f, 0.0f, 0.0f ) );
 
 				Vector3 positionOffset = firstImpact.impactSurfaceNormal * 0.001f;
 				
				if( bboxCornerNumber == 0 )
				{
					positionOffset.x += PLAYER_BBOX_OFFSETS.x;
					positionOffset.y += PLAYER_BBOX_OFFSETS.y; 
					positionOffset.z += PLAYER_BBOX_OFFSETS.z;
				}
				if( bboxCornerNumber == 1 )
				{
					positionOffset.x -= PLAYER_BBOX_OFFSETS.x;
					positionOffset.y += PLAYER_BBOX_OFFSETS.y; 
					positionOffset.z += PLAYER_BBOX_OFFSETS.z;
				}
				if( bboxCornerNumber == 2 )
				{
					positionOffset.x -= PLAYER_BBOX_OFFSETS.x;
					positionOffset.y -= PLAYER_BBOX_OFFSETS.y; 
					positionOffset.z += PLAYER_BBOX_OFFSETS.z;
				}
				if( bboxCornerNumber == 3 )
				{
					positionOffset.x += PLAYER_BBOX_OFFSETS.x;
					positionOffset.y -= PLAYER_BBOX_OFFSETS.y; 
					positionOffset.z += PLAYER_BBOX_OFFSETS.z;
				}
				if( bboxCornerNumber == 4 )
				{
					positionOffset.x += PLAYER_BBOX_OFFSETS.x;
					positionOffset.y += PLAYER_BBOX_OFFSETS.y; 
				}
				if( bboxCornerNumber == 5 )
				{
					positionOffset.x -= PLAYER_BBOX_OFFSETS.x;
					positionOffset.y += PLAYER_BBOX_OFFSETS.y; 
				}
				if( bboxCornerNumber == 6 )
				{
					positionOffset.x -= PLAYER_BBOX_OFFSETS.x;
					positionOffset.y -= PLAYER_BBOX_OFFSETS.y; 
				}
				if( bboxCornerNumber == 7 )
				{
					positionOffset.x += PLAYER_BBOX_OFFSETS.x;
					positionOffset.y -= PLAYER_BBOX_OFFSETS.y; 
				}
				if( bboxCornerNumber == 8 )
				{
					positionOffset.x += PLAYER_BBOX_OFFSETS.x;
					positionOffset.y += PLAYER_BBOX_OFFSETS.y; 
					positionOffset.z -= PLAYER_BBOX_OFFSETS.z;
				}
				if( bboxCornerNumber == 9 )
				{
					positionOffset.x -= PLAYER_BBOX_OFFSETS.x;
					positionOffset.y += PLAYER_BBOX_OFFSETS.y; 
					positionOffset.z -= PLAYER_BBOX_OFFSETS.z;
				}
				if( bboxCornerNumber == 10 )
				{
					positionOffset.x -= PLAYER_BBOX_OFFSETS.x;
					positionOffset.y -= PLAYER_BBOX_OFFSETS.y; 
					positionOffset.z -= PLAYER_BBOX_OFFSETS.z;
				}
				if( bboxCornerNumber == 11 )
				{
					positionOffset.x += PLAYER_BBOX_OFFSETS.x;
					positionOffset.y -= PLAYER_BBOX_OFFSETS.y; 
					positionOffset.z -= PLAYER_BBOX_OFFSETS.z;
				}
				
				m_player->SetPosition( firstImpact.impactWorldCoords + positionOffset );
 
 				Vector3 oldVelocity = m_player->GetVelocity();
 
 				if( firstImpact.impactSurfaceNormal.x != 0.0f )
 					m_player->SetVelocity( Vector3(0.0f, oldVelocity.y, oldVelocity.z ));
 
 				if( firstImpact.impactSurfaceNormal.y != 0.0f )
 					m_player->SetVelocity( Vector3(oldVelocity.x, 0.0f, oldVelocity.z ));
 
 				if( firstImpact.impactSurfaceNormal.z != 0.0f )
 					m_player->SetVelocity( Vector3(oldVelocity.x, oldVelocity.y, 0.0f ));
 
 				deltaSeconds = firstImpact.impactFraction * deltaSeconds;
			}


			// minkowski space -- NOT WORKING

// 			RayCast3DResult result = RayCast( playerStartPos, playerEndPos, false, PLAYER_BBOX_OFFSETS.x, PLAYER_BBOX_OFFSETS.y, PLAYER_BBOX_OFFSETS.z );
// 			if( result.didImpact )
// 			{
// 
// 				if( result.impactFraction == 0.0f )
// 					m_player->SetVelocity( Vector3( 0.0f, 0.0f, 0.0f ) );
// 
// 				Vector3 positionOffset = result.impactSurfaceNormal * 0.0001f;
// 				m_player->SetPosition( result.impactWorldCoords + positionOffset );
// 
// 				Vector3 oldVelocity = m_player->GetVelocity();
// 
// 				if( result.impactSurfaceNormal.x != 0.0f )
// 					m_player->SetVelocity( Vector3(0.0f, oldVelocity.y, oldVelocity.z ));
// 
// 				if( result.impactSurfaceNormal.y != 0.0f )
// 					m_player->SetVelocity( Vector3(oldVelocity.x, 0.0f, oldVelocity.z ));
// 
// 				if( result.impactSurfaceNormal.z != 0.0f )
// 					m_player->SetVelocity( Vector3(oldVelocity.x, oldVelocity.y, 0.0f ));
// 
// 				deltaSeconds = result.impactFraction * deltaSeconds;
// 			}
		}
	}

	if( m_player->GetVelocity() == Vector3( 0.0f, 0.0f, 0.0f) )
	{
		return deltaSecondsInitial;
	}
	return deltaSeconds;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void World::UpdateOutdoorLighting()
{
	Chunks chunksThatNeedUpdating;
	GetChunksWithGreatestLightingDisparity( chunksThatNeedUpdating );

	if( chunksThatNeedUpdating.empty() )
		return;

	for( int chunkToRelightIndex = 0; chunkToRelightIndex < NUM_CHUNKS_TO_RELIGHT_PER_FRAME; ++chunkToRelightIndex )
	{
		if( chunksThatNeedUpdating.empty() )
			GetChunksWithGreatestLightingDisparity( chunksThatNeedUpdating );

		Chunk* chunkToUpdate = GetChunkToUpdateOutdoorLighting( chunksThatNeedUpdating );
		UpdateChunkOutdoorLighting( chunkToUpdate );
	}
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void World::UpdateChunkOutdoorLighting( Chunk* chunk )
{
	//ConsolePrintf( "Updating outdoor lighting for chunk at %i,%i\n", chunk->m_chunkCoords.x, chunk->m_chunkCoords.y );
	chunk->SetOutdoorLightLevel( s_outdoorLightValue );
	for( BlockIndex blockIndex = 0; blockIndex < NUM_BLOCKS_PER_CHUNK; ++blockIndex )
	{
		Block& block = chunk->GetBlockAtIndex( blockIndex );
		if( block.IsSky() )
			SetBlockDirty( BlockData( chunk, blockIndex ) );
	}
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void World::GetChunksWithGreatestLightingDisparity( Chunks& out_chunksWithGreatestLightingDisparity )
{
	int greatestLightLevelDifference = 0;
	for( ChunkMap::const_iterator chunkIter = m_activeChunks.begin(); chunkIter != m_activeChunks.end(); ++chunkIter )
	{
		Chunk* chunk = chunkIter->second;
		int lightingDisparity = abs( chunk->GetOutdoorLightLevel() - s_outdoorLightValue );

		if( lightingDisparity > greatestLightLevelDifference )
		{
			greatestLightLevelDifference = lightingDisparity;
			out_chunksWithGreatestLightingDisparity.clear();
			out_chunksWithGreatestLightingDisparity.push_back( chunk );
		}
		else if( lightingDisparity == greatestLightLevelDifference )
			out_chunksWithGreatestLightingDisparity.push_back( chunk );
	}
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Chunk* World::GetChunkToUpdateOutdoorLighting(  const Chunks& chunkList )
{
	if( chunkList.empty() )
		return NULL;

	int winningChunkXValue = chunkList[ 0 ]->m_chunkCoords.x;
	Chunk* winningChunk = chunkList[ 0 ];

	for( Chunks::const_iterator chunkIter = chunkList.begin(); chunkIter != chunkList.end(); ++chunkIter )
	{
		Chunk* chunk = ( *chunkIter );
		if( chunk->m_chunkCoords.x > winningChunkXValue )
		{
			winningChunk = chunk;
			winningChunkXValue = chunk->m_chunkCoords.x;
		}
	}

	return winningChunk;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
bool World::AreWorldCoordsInWorld( const WorldCoords& worldCoords ) const
{
	if( worldCoords.z >= 0.0f && worldCoords.z < (float) CHUNK_BLOCKS_Z )
		return true;
	return false;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
bool World::IsBlockWater( const WorldCoords& worldCoords ) const
{
	if( !AreWorldCoordsInWorld( worldCoords ) )
		return false;

	ChunkCoords chunkCoords = Chunk::GetChunkCoordsFromWorldCoords( worldCoords );
	Chunk* chunk = NULL;
	if( m_activeChunks.find( chunkCoords ) != m_activeChunks.end() )
		chunk = m_activeChunks.at( chunkCoords );
	else
		return false;

	BlockIndex blockIndex = chunk->GetIndexFromWorldCoords( worldCoords );

	if( BlockData( chunk, blockIndex ).GetBlock().m_blockType == BLOCK_TYPE_WATER )
		return true;
	return false;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
bool World::IsBlockSolid( const WorldCoords& worldCoords ) const
{
	if( !AreWorldCoordsInWorld( worldCoords ) )
		return false;

	ChunkCoords chunkCoords = Chunk::GetChunkCoordsFromWorldCoords( worldCoords );
	Chunk* chunk = NULL;
	if( m_activeChunks.find( chunkCoords ) != m_activeChunks.end() )
		chunk = m_activeChunks.at( chunkCoords );
	else
		return false;

	BlockIndex blockIndex = chunk->GetIndexFromWorldCoords( worldCoords );

	if( BlockData( chunk, blockIndex ).GetBlock().IsSolid() )
		return true;
	return false;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
bool World::ActivateNearestInactiveChunkInsideVisibilityRadius()
{
	const int innerChunkDistanceSquaredQualifier = CHUNK_INNER_VISIBILITY_RADIUS * CHUNK_INNER_VISIBILITY_RADIUS + 1;
	int leadingCandidateDistanceSquared = innerChunkDistanceSquaredQualifier;
	ChunkCoords playerChunkCoords = Chunk::GetChunkCoordsFromWorldCoords( m_camera->m_position );
	ChunkCoords chunkToActivateCoords = ChunkCoords( 0, 0 );

	for( int chunkXCoord = playerChunkCoords.x - CHUNK_INNER_VISIBILITY_RADIUS; chunkXCoord <= playerChunkCoords.x + CHUNK_INNER_VISIBILITY_RADIUS; ++chunkXCoord )
	{
		for( int chunkYCoord = playerChunkCoords.y - CHUNK_INNER_VISIBILITY_RADIUS; chunkYCoord <= playerChunkCoords.y + CHUNK_INNER_VISIBILITY_RADIUS; ++chunkYCoord )
		{
			ChunkCoords candidateCoords( chunkXCoord, chunkYCoord );
			int distanceToChunkSquared = CalcDistanceSquared( playerChunkCoords, candidateCoords );
			if( distanceToChunkSquared < leadingCandidateDistanceSquared )
			{
				if( !IsChunkActive( candidateCoords ) ) 
				{
					leadingCandidateDistanceSquared = distanceToChunkSquared;
					chunkToActivateCoords = candidateCoords;
				}
			}
		}
	}

	if( leadingCandidateDistanceSquared < innerChunkDistanceSquaredQualifier )
	{
		ActivateChunk( chunkToActivateCoords );
		return true;
	}
	else 
		return false;

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void World::ActivateChunk( const ChunkCoords& chunkCoords )
{
	//ConsolePrintf( "Activating chunk at: %i, %i \n", chunkCoords.x, chunkCoords.y );

	Chunk* newChunk = NULL;
	
	newChunk = CreateChunkFromFile( chunkCoords );
	if( !newChunk )
	{
		newChunk = CreateChunkFromPerlinNoise( chunkCoords );
	}

	m_activeChunks[ chunkCoords ] = newChunk;
	OnChunkActivated( newChunk );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Chunk* World::CreateChunkFromFile( const ChunkCoords& chunkCoords )
{
	Chunk* chunk = new Chunk( chunkCoords );
	bool chunkLoaded = chunk->LoadChunkFromDisk();
	if( chunkLoaded )
		return chunk;

	delete chunk;
	return NULL;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Chunk* World::CreateChunkFromPerlinNoise( const ChunkCoords& chunkCoords )
{
	Chunk* chunk = new Chunk( chunkCoords );
	chunk->GenerateStartingBlocks_2DPerlinNoise( m_seed );
	//m_rivers.clear();
 	//chunk->GenerateFlatChunk( 64, 70 );
	return chunk;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
bool World::DeactivateFurthestActiveChunkOutsideVisibilityRadius()
{
	const int outerChunkDistanceSquaredQualifier = ( CHUNK_OUTER_VISIBILITY_RADIUS - 1 ) * ( CHUNK_OUTER_VISIBILITY_RADIUS - 1 );
	int leadingCandidateDistanceSquared = outerChunkDistanceSquaredQualifier;
	ChunkCoords playerChunkCoords = Chunk::GetChunkCoordsFromWorldCoords( m_camera->m_position );
	ChunkCoords chunkToDeactivateCoords = ChunkCoords( 0, 0 );

	for( ChunkMap::iterator chunkIter = m_activeChunks.begin(); chunkIter != m_activeChunks.end(); ++chunkIter )
	{
		Chunk* candidateChunk = ( chunkIter->second );
		int distanceToChunkSquared = CalcDistanceSquared( playerChunkCoords, candidateChunk->m_chunkCoords );
		//ConsolePrintf( "Distance to Chunk: %d \n", distanceToChunkSquared );
		
		if( distanceToChunkSquared > leadingCandidateDistanceSquared )
		{
			leadingCandidateDistanceSquared = distanceToChunkSquared;
			chunkToDeactivateCoords = candidateChunk->m_chunkCoords;
		}
	}
	

	if( leadingCandidateDistanceSquared > outerChunkDistanceSquaredQualifier )
	{
		DeactivateChunk( chunkToDeactivateCoords );
		return true;
	}
	else 
		return false;

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void World::DeactivateChunk( const ChunkCoords& chunkCoords )
{
	//ConsolePrintf( "NOT Deactivating chunk at: %i, %i \n", chunkCoords.x, chunkCoords.y );
	m_activeChunks[ chunkCoords ]->SaveChunkToDisk();
	OnChunkDeactivated( m_activeChunks[ chunkCoords ] );
	delete m_activeChunks[ chunkCoords ];
	m_activeChunks.erase( chunkCoords );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void World::SaveAllActiveChunks()
{
	for( ChunkMap::iterator chunkIter = m_activeChunks.begin(); chunkIter != m_activeChunks.end(); ++chunkIter )
	{
		Chunk* chunk = chunkIter->second;
		chunk->SaveChunkToDisk();
		delete chunk;
	}

	m_activeChunks.clear();
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void World::OnChunkActivated( Chunk* chunk )
{
	UpdateChunkNeighborPointersActivate( chunk );

// 	m_rivers = GenerateRivers( m_seed );
// 	CarveRiverWorms( m_rivers, 0.8f, chunk->m_chunkCoords );

	m_caves = GenerateCaves( m_seed );
	CarveCaveWorms( m_caves, 0.6f, chunk->m_chunkCoords );

	chunk->InitializeLighting( s_outdoorLightValue );
	chunk->SetVBODirty();
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void World::OnChunkDeactivated( Chunk* chunk )
{
	UpdateChunkNeighborPointersDeactivate( chunk );
	RemoveBlocksFromDirtyList( chunk );
	chunk->DeleteVBO();
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void World::UpdateChunkNeighborPointersActivate( Chunk* chunk )
{

	ChunkCoords activatedChunkCoords = chunk->m_chunkCoords;

	ChunkMap::iterator chunkToNorth = m_activeChunks.find( ChunkCoords( activatedChunkCoords.x, activatedChunkCoords.y + 1 ) );
	ChunkMap::iterator chunkToSouth = m_activeChunks.find( ChunkCoords( activatedChunkCoords.x, activatedChunkCoords.y - 1  ) );
	ChunkMap::iterator chunkToEast = m_activeChunks.find( ChunkCoords( activatedChunkCoords.x + 1, activatedChunkCoords.y ) );
	ChunkMap::iterator chunkToWest = m_activeChunks.find( ChunkCoords( activatedChunkCoords.x - 1, activatedChunkCoords.y ) );

	if( chunkToNorth != m_activeChunks.end() )
	{
		Chunk* northernChunk = chunkToNorth->second;
		chunk->m_chunkToNorth = northernChunk;
		northernChunk->m_chunkToSouth = chunk;
	}

	if( chunkToSouth != m_activeChunks.end() )
	{
		Chunk* southernChunk = chunkToSouth->second;
		chunk->m_chunkToSouth = southernChunk;
		southernChunk->m_chunkToNorth = chunk;
	}

	if( chunkToEast != m_activeChunks.end() )
	{
		Chunk* easternChunk = chunkToEast->second;
		chunk->m_chunkToEast = easternChunk;
		easternChunk->m_chunkToWest = chunk;
	}

	if( chunkToWest != m_activeChunks.end() )
	{
		Chunk* westernChunk = chunkToWest->second;
		chunk->m_chunkToWest = westernChunk;
		westernChunk->m_chunkToEast = chunk;
	}
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void World::UpdateChunkNeighborPointersDeactivate( Chunk* chunk )
{
	if( chunk->m_chunkToEast )
		(chunk->m_chunkToEast)->m_chunkToWest = NULL;
	
	if( chunk->m_chunkToNorth )
		(chunk->m_chunkToNorth)->m_chunkToSouth = NULL;
	
	if( chunk->m_chunkToWest )
		(chunk->m_chunkToWest)->m_chunkToEast = NULL;
	
	if( chunk->m_chunkToSouth )
		(chunk->m_chunkToSouth)->m_chunkToNorth = NULL;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void World::RemoveBlocksFromDirtyList( Chunk* chunk )
{
	for( BlockDataList::iterator blockDataIter = s_dirtyBlocks.begin(); blockDataIter != s_dirtyBlocks.end(); )
	{
		BlockData blockData = ( *blockDataIter );
		if( blockData.m_chunk == chunk )
			blockDataIter = s_dirtyBlocks.erase( blockDataIter );
		else
			++blockDataIter;
	}
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void World::UpdateLighting()
{
	//ConsolePrintf( "UpdateLighting: %i blocks are dirty\n", s_dirtyBlocks.size() );
	if( g_manuallyAdvanceLighting )
	{
		BlockDataList dirtyBlocks;
		dirtyBlocks.swap( s_dirtyBlocks );

// 		for( BlockDataList::const_iterator dirtyBlockIter = s_dirtyBlocks.begin(); dirtyBlockIter != s_dirtyBlocks.end(); ++dirtyBlockIter )
// 			dirtyBlocks.push_back( *dirtyBlockIter );
// 		
// 		s_dirtyBlocks.clear();

		while( !dirtyBlocks.empty() )
		{
			BlockData blockData = dirtyBlocks.back();
			dirtyBlocks.pop_back();

			UpdateLightingForBlock( blockData );
		}

	}
	
	else
	{	
		while( !s_dirtyBlocks.empty() )
		{
			BlockData blockData = s_dirtyBlocks.back();
			s_dirtyBlocks.pop_back();

			UpdateLightingForBlock( blockData );
		}
	}
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void World::UpdateLightingForBlock( const BlockData& blockData )
{
	int idealLight = CalcIdealLightForBlock( blockData );
	Block& block = blockData.m_chunk->GetBlockAtIndex( blockData.m_blockIndex);

	if( block.GetLightValue() != idealLight )
	{
			block.SetLightValue( idealLight );
			MarkNeighborsDirty( blockData );
	}
	block.MarkAsNotDirty();
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
int World::CalcIdealLightForBlock( const BlockData& blockData )
{
	int highestLightLevel = 0;

	highestLightLevel = max( highestLightLevel, CalcIdealLightFromNeighbors( blockData ) );
	highestLightLevel = max( highestLightLevel, CalcIdealLightFromSky( blockData ) );
	highestLightLevel = max( highestLightLevel, CalcIdealLightFromSelf( blockData ) );

	return highestLightLevel;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
int World::CalcIdealLightFromNeighbors( const BlockData& blockData )
{
	int highestLightLevel = 0;

	highestLightLevel = max( highestLightLevel, CalcIdealLightFromEastNeighbor( blockData ) );
	highestLightLevel = max( highestLightLevel, CalcIdealLightFromNorthNeighbor( blockData ) );
	highestLightLevel = max( highestLightLevel, CalcIdealLightFromWestNeighbor( blockData ) );
	highestLightLevel = max( highestLightLevel, CalcIdealLightFromSouthNeighbor( blockData ) );
	highestLightLevel = max( highestLightLevel, CalcIdealLightFromTopNeighbor( blockData ) );
	highestLightLevel = max( highestLightLevel, CalcIdealLightFromBottomNeighbor( blockData ) );

	return highestLightLevel;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
int World::CalcIdealLightFromEastNeighbor( const BlockData& blockData )
{
	BlockData eastNeighbor = blockData.GetEastNeighbor();
	if( eastNeighbor.IsValid() )
	{
		Block& block = eastNeighbor.m_chunk->GetBlockAtIndex( eastNeighbor.m_blockIndex );
		return block.GetLightValue() - 1;
	}
	else
		return 0;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
int World::CalcIdealLightFromNorthNeighbor( const BlockData& blockData )
{
	BlockData northNeighbor = blockData.GetNorthNeighbor();
	if( northNeighbor.IsValid() )
	{
		Block& block = northNeighbor.m_chunk->GetBlockAtIndex( northNeighbor.m_blockIndex );
		return block.GetLightValue() - 1;
	}
	else
		return 0;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
int World::CalcIdealLightFromWestNeighbor( const BlockData& blockData )
{
	BlockData westNeighbor = blockData.GetWestNeighbor();
	if( westNeighbor.IsValid() )
	{
		Block& block = westNeighbor.m_chunk->GetBlockAtIndex( westNeighbor.m_blockIndex );
		return block.GetLightValue() - 1;
	}
	else
		return 0;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
int World::CalcIdealLightFromSouthNeighbor( const BlockData& blockData )
{
	BlockData southNeighbor = blockData.GetSouthNeighbor();
	if( southNeighbor.IsValid() )
	{
		Block& block = southNeighbor.m_chunk->GetBlockAtIndex( southNeighbor.m_blockIndex );
		return block.GetLightValue() - 1;
	}
	else
		return 0;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
int World::CalcIdealLightFromTopNeighbor( const BlockData& blockData )
{
	BlockData topNeighbor = blockData.GetTopNeighbor();
	if( topNeighbor.IsValid() )
	{
		Block& block = topNeighbor.m_chunk->GetBlockAtIndex( topNeighbor.m_blockIndex );
		return block.GetLightValue() - 1;
	}
	else
		return 0;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
int World::CalcIdealLightFromBottomNeighbor( const BlockData& blockData )
{
	BlockData bottomNeighbor = blockData.GetBottomNeighbor();
	if( bottomNeighbor.IsValid() )
	{
		Block& block = bottomNeighbor.m_chunk->GetBlockAtIndex( bottomNeighbor.m_blockIndex );
		return block.GetLightValue() - 1;
	}
	else
		return 0;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
int World::CalcIdealLightFromSky( const BlockData& blockData )
{
	if( blockData.IsValid() )
		if( blockData.m_chunk->GetBlockAtIndex( blockData.m_blockIndex ).IsSky() )
			return blockData.m_chunk->GetOutdoorLightLevel();
	return 0;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
int World::CalcIdealLightFromSelf( const BlockData& blockData )
{
	if( blockData.IsValid() )
		return blockData.m_chunk->GetBlockAtIndex( blockData.m_blockIndex ).GetInternalLightValue();
	return 0;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
PerlinWormWorldCells World::GeneratePerlinWormCells( const ChunkCoords& centerPosition )
{
	PerlinWormWorldCells perlinWormCells;
	for( int chunkWormVisibiltyX = centerPosition.x - CHUNK_WORM_GENERATION_RADIUS; chunkWormVisibiltyX <= centerPosition.x + CHUNK_WORM_GENERATION_RADIUS; ++chunkWormVisibiltyX )
	{
		for( int chunkWormVisibiltyY = centerPosition.y - CHUNK_WORM_GENERATION_RADIUS; chunkWormVisibiltyY <= centerPosition.y + CHUNK_WORM_GENERATION_RADIUS; ++chunkWormVisibiltyY )
		{
			float distance = CalcDistance( centerPosition, IntVector2( chunkWormVisibiltyX, chunkWormVisibiltyY ) );
			int xOffset = chunkWormVisibiltyX - centerPosition.x;
			int yOffset = chunkWormVisibiltyY - centerPosition.y;

			if( xOffset > 0 )
				++xOffset;
			if( yOffset > 0 )
				++yOffset;

			if( distance <= CHUNK_WORM_GENERATION_RADIUS )
			{
// 				ConsolePrintf("chunk coords: %d, %d \n", centerPosition.x + xOffset, centerPosition.y + yOffset );
				if( ( centerPosition.x + xOffset ) % CHUNKS_PER_AXIS_PER_PERLIN_CELL == 0 && ( centerPosition.y + yOffset ) % CHUNKS_PER_AXIS_PER_PERLIN_CELL == 0 )
				{
					PerlinWormWorldCell perlinWormCell;
					perlinWormCell.cellCoords = ( centerPosition + ChunkCoords( xOffset, yOffset ) ) / CHUNKS_PER_AXIS_PER_PERLIN_CELL;

					perlinWormCell.minChunkVals = ( centerPosition + ChunkCoords( xOffset, yOffset ) );
					perlinWormCell.maxChunkVals = ( perlinWormCell.minChunkVals + ChunkCoords( CHUNKS_PER_AXIS_PER_PERLIN_CELL, CHUNKS_PER_AXIS_PER_PERLIN_CELL  ) );
					perlinWormCells.push_back( perlinWormCell );
				}
			}
		}
	}

	return perlinWormCells;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
PerlinWormParametersList World::GeneratePerlinWormParameterList2D( const PerlinWormWorldCells& perlinCells, int seed )
{
	PerlinWormParametersList parametersList;
// 	ConsolePrintf("Generate parameters");
	for( PerlinWormWorldCells::const_iterator perlinCellIter = perlinCells.begin(); perlinCellIter != perlinCells.end(); ++perlinCellIter )
	{
		PerlinWormWorldCell perlinCell = ( *perlinCellIter );
// 		ConsolePrintf( "Cell coords: %d, %d \n", perlinCell.cellCoords.x, perlinCell.cellCoords.y );

		bool cellContainsWormStart = DoesCellContainWormStart( perlinCell, seed );
// 		ConsolePrintf( "Cell: %d, %d, contains worm start: %d \n", perlinCell.cellCoords.x, perlinCell.cellCoords.y, cellContainsWormStart );
		if( cellContainsWormStart )
		{
			PerlinWormParameters parameters;
			float startPosX = GetPseudoRandomNoiseValueZeroToOne2D( perlinCell.minChunkVals.x, perlinCell.maxChunkVals.x );
			startPosX = RangeMap( startPosX, 0.0f, 1.0f, 0.0f, (float) ( CHUNK_BLOCKS_X * CHUNKS_PER_AXIS_PER_PERLIN_CELL ) );
			float startPosY = GetPseudoRandomNoiseValueZeroToOne2D (perlinCell.minChunkVals.y, perlinCell.maxChunkVals.y );
			startPosY = RangeMap( startPosY, 0.0f, 1.0f, 0.0f, (float) ( CHUNK_BLOCKS_Y * CHUNKS_PER_AXIS_PER_PERLIN_CELL ) );
			float startPosZ = SEA_LEVEL;

			WorldCoords startPosition = Chunk::GetChunkWorldMinsFromChunkCoords( perlinCell.minChunkVals );
			startPosition.x += startPosX;
			startPosition.y += startPosY;
			startPosition.z = startPosZ;

			parameters.startPosition = startPosition;

			float startYawRadians = GetPseudoNoiseAngleRadians2D( (int) floorf( startPosition.x ), (int) floorf( startPosition.y ) );
			startYawRadians = RangeMap( startYawRadians, 0.0f, TWO_PI, -WORM_MAX_YAW_RADIANS, WORM_MAX_YAW_RADIANS );
// 			ConsolePrintf( "Start degrees: %f \n", ConvertRadiansToDegrees(startYawRadians));

			parameters.startYawRadians = startYawRadians;
			parameters.startPitchRadians = 0.0f;

			parametersList.push_back( parameters );

		}
	}

	return parametersList;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
PerlinWormParametersList World::GeneratePerlinWormParameterList3D( const PerlinWormWorldCells& perlinCells, int seed )
{
	PerlinWormParametersList parametersList;
	// 	ConsolePrintf("Generate parameters");
	for( PerlinWormWorldCells::const_iterator perlinCellIter = perlinCells.begin(); perlinCellIter != perlinCells.end(); ++perlinCellIter )
	{
		PerlinWormWorldCell perlinCell = ( *perlinCellIter );
		// 		ConsolePrintf( "Cell coords: %d, %d \n", perlinCell.cellCoords.x, perlinCell.cellCoords.y );

		bool cellContainsWormStart = DoesCellContainWormStart( perlinCell, seed );
		// 		ConsolePrintf( "Cell: %d, %d, contains worm start: %d \n", perlinCell.cellCoords.x, perlinCell.cellCoords.y, cellContainsWormStart );
		if( cellContainsWormStart )
		{
			PerlinWormParameters parameters;
			float startPosX = GetPseudoRandomNoiseValueZeroToOne2D( perlinCell.minChunkVals.x, perlinCell.maxChunkVals.x );
			startPosX = RangeMap( startPosX, 0.0f, 1.0f, 0.0f, (float) ( CHUNK_BLOCKS_X * CHUNKS_PER_AXIS_PER_PERLIN_CELL ) );
			float startPosY = GetPseudoRandomNoiseValueZeroToOne2D (perlinCell.minChunkVals.y, perlinCell.maxChunkVals.y );
			startPosY = RangeMap( startPosY, 0.0f, 1.0f, 0.0f, (float) ( CHUNK_BLOCKS_Y * CHUNKS_PER_AXIS_PER_PERLIN_CELL ) );
			float startPosZ = GetPseudoRandomNoiseValueZeroToOne2D( (int) floorf( startPosX ), (int) floorf( startPosY ) );
			startPosZ = RangeMap( startPosZ, 0.0f, 1.0f, 0.0f, (float) CHUNK_BLOCKS_Z - 1.0f );

			WorldCoords startPosition = Chunk::GetChunkWorldMinsFromChunkCoords( perlinCell.minChunkVals );
			startPosition.x += startPosX;
			startPosition.y += startPosY;
			startPosition.z += startPosZ;

			parameters.startPosition = startPosition;

			float startYawRadians = GetPseudoNoiseAngleRadians2D( (int) floorf( startPosition.x ), (int) floorf( startPosition.y ) );
			startYawRadians = RangeMap( startYawRadians, 0.0f, TWO_PI, -WORM_MAX_YAW_RADIANS, WORM_MAX_YAW_RADIANS );
			// 			ConsolePrintf( "Start degrees: %f \n", ConvertRadiansToDegrees(startYawRadians));

			float startPitchRadians = GetPseudoNoiseAngleRadians2D( (int) floorf( startPosition.y ), (int) floorf( startPosition.z ) );
			startPitchRadians = RangeMap( startPitchRadians, 0.0f, TWO_PI, -WORM_MAX_PITCH_RADIANS, WORM_MAX_PITCH_RADIANS );

			parameters.startYawRadians = startYawRadians;
			parameters.startPitchRadians = startPitchRadians;

			parametersList.push_back( parameters );

		}
	}

	return parametersList;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
bool World::DoesCellContainWormStart( const PerlinWormWorldCell& perlinCell, int seed )
{
	Vector2 perlinCoords = Vector2( (float) perlinCell.cellCoords.x, (float) perlinCell.cellCoords.y );

	float perlinCellWormProbabilityFactor = ComputePerlinNoiseValueAtPosition2D( perlinCoords, 15, 5, 20, 0.5f, seed );
	
	bool highestOfNeighbors = true;
	bool lowestOfNeighbors = true;

// 	ConsolePrintf( "My worm factor: %f \n", perlinCellWormProbabilityFactor );

	IntVector2s cellsToCheck;
	cellsToCheck.push_back( IntVector2( perlinCell.cellCoords.x - 1, perlinCell.cellCoords.y ) );
	cellsToCheck.push_back( IntVector2( perlinCell.cellCoords.x + 1, perlinCell.cellCoords.y ) );
	cellsToCheck.push_back( IntVector2( perlinCell.cellCoords.x, perlinCell.cellCoords.y - 1 ) );
	cellsToCheck.push_back( IntVector2( perlinCell.cellCoords.x, perlinCell.cellCoords.y + 1 ) );


	for( IntVector2s::const_iterator cellIter = cellsToCheck.begin(); cellIter != cellsToCheck.end(); ++cellIter )
	{
		IntVector2 cell = ( *cellIter );
		int cellX = cell.x;
		int cellY = cell.y;

		if( cellX == perlinCell.cellCoords.x && cellY == perlinCell.cellCoords.y )
			continue;

		float cellWormProbabilityFactor = ComputePerlinNoiseValueAtPosition2D( Vector2( (float) cellX, (float) cellY ), 15, 5, 20, 0.5f, seed );
// 		ConsolePrintf( "Neighboring worm: %d, %d. Worm factor: %f \n", cellX, cellY, cellWormProbabilityFactor );
		if( cellWormProbabilityFactor >= perlinCellWormProbabilityFactor )
		{
			highestOfNeighbors = false;
		}

		if( cellWormProbabilityFactor <= perlinCellWormProbabilityFactor )
		{
			lowestOfNeighbors = false;
		}
	}
	
	return highestOfNeighbors || lowestOfNeighbors; 
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
PerlinWorms World::GenerateRivers( int seed )
{
	PerlinWormWorldCells cells = GeneratePerlinWormCells( Chunk::GetChunkCoordsFromWorldCoords( m_player->GetPosition() ) );
	PerlinWormParametersList parametersList = GeneratePerlinWormParameterList2D( cells, seed );

	PerlinWorms rivers;
	for( PerlinWormParametersList::const_iterator parameterListIter = parametersList.begin(); parameterListIter != parametersList.end(); ++parameterListIter )
	{
		PerlinWormParameters parameters = ( *parameterListIter );
		PerlinWorm worm;
// 		ConsolePrintf( "parameters start yaw: %f \n", parameters.startYawRadians );
		GeneratePerlinWorm2D( parameters.startPosition, parameters.startYawRadians, seed, worm );
		rivers.push_back( worm );
	}

	return rivers;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
PerlinWorms World::GenerateCaves( int seed )
{
	PerlinWormWorldCells cells = GeneratePerlinWormCells( Chunk::GetChunkCoordsFromWorldCoords( m_player->GetPosition() ) );
	PerlinWormParametersList parametersList = GeneratePerlinWormParameterList3D( cells, seed );

	PerlinWorms rivers;
	for( PerlinWormParametersList::const_iterator parameterListIter = parametersList.begin(); parameterListIter != parametersList.end(); ++parameterListIter )
	{
		PerlinWormParameters parameters = ( *parameterListIter );
		PerlinWorm worm;
// 		ConsolePrintf( "parameters start yaw: %f \n", parameters.startYawRadians );
		GeneratePerlinWorm3D( parameters.startPosition, parameters.startYawRadians, parameters.startPitchRadians, seed, worm );
		rivers.push_back( worm );
	}

	return rivers;
}


///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void World::GeneratePerlinWorm2D( const Vector3& startPosition, float startYawRadians, int seed, PerlinWorm& out_worm )
{
// 	ConsolePrintf( "start yaw degrees: %f \n", ConvertRadiansToDegrees( startYawRadians ) );
	//first position
	out_worm.points.push_back( startPosition );
	float startRadius = ComputePerlinNoiseValueAtPosition2D( Vector2( startPosition.x, startPosition.y ), 60, 2, 1, 0.5f, seed );
	startRadius = RangeMap( startRadius, -1.0f, 1.0f, WORM_MIN_RADIUS, WORM_MAX_RADIUS );
	out_worm.pointRadii.push_back( startRadius );

	// number of segments
	float numSegs = GetPseudoRandomNoiseValueZeroToOne1D( (int) floorf( startPosition.x ) );
	numSegs = RangeMap( numSegs, 0.0f, 1.0f, (float) WORM_MIN_SEGS, (float) WORM_MAX_SEGS );
// 	ConsolePrintf( "Num Segs: %f \n", numSegs );
	Vector3 lastPoint = startPosition; 
	EulerAngles lastOrientation = EulerAngles( startYawRadians, 0.0f, 0.0f );
	for( int segNum = 0; segNum < numSegs; ++segNum )
	{
		// direction
		EulerAngles orientation;
		orientation.pitchDegrees = 0.0f;
		orientation.rollDegrees = 0.0f;
		orientation.yawDegrees = lastOrientation.yawDegrees + ConvertRadiansToDegrees( ComputePerlinNoiseValueAtPosition2D( Vector2( lastPoint.x, lastPoint.y ), 30, 4, WORM_MAX_YAW_RADIANS, 0.5f, seed ));
		
		lastOrientation = orientation;
// 		if( segNum == 0 )
// 			orientation.yawDegreesAboutZ = startYawRadians;

// 		ConsolePrintf( "Orientation yaw: %f \n", orientation.yawDegreesAboutZ );
		Vector3 forward = orientation.GetForwardVector();
// 		ConsolePrintf( "forward vector: %f, %f, %f \n", forward.x, forward.y, forward.z );
		// distance
		float segLength = GetPseudoRandomNoiseValueZeroToOne1D( (int) floorf( lastPoint.y ) );
		segLength = RangeMap( segLength, 0.0f, 1.0f, WORM_MIN_SEG_LENGTH, WORM_MAX_SEG_LENGTH );

		// get next point
// 		ConsolePrintf( "Seg Length: %f, forwardVector: %f, %f, %f \n", segLength, forward.x, forward.y, forward.z );
		Vector3 nextPoint = lastPoint + ( segLength * forward );
		lastPoint = nextPoint;

		// radius at next point
		float radius = ComputePerlinNoiseValueAtPosition2D( Vector2( nextPoint.x, nextPoint.y ), 60, 2, 1, 0.5f, seed );
		radius = RangeMap( radius, -1.0f, 1.0f, (float) WORM_MIN_RADIUS, (float) WORM_MAX_RADIUS );

// 		ConsolePrintf( "Max worm displacement: %f \n", WORM_MAX_DISPLACEMENT );
// 		ConsolePrintf( "Current Displacement: %f \n", CalcDistance( startPosition, lastPoint ) );
		if( CalcDistance( startPosition, lastPoint ) < WORM_MAX_DISPLACEMENT )
		{
			out_worm.points.push_back( nextPoint );
			out_worm.pointRadii.push_back( radius );
		}
		else
			break;
	}
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void World::GeneratePerlinWorm3D( const Vector3& startPosition, float startYawRadians, float startPitchRadians, int seed, PerlinWorm& out_worm )
{
// 	ConsolePrintf( "start yaw degrees: %f \n", ConvertRadiansToDegrees( startYawRadians ) );
	//first position
	out_worm.points.push_back( startPosition );
	float startRadius = ComputePerlinNoiseValueAtPosition2D( Vector2( startPosition.x, startPosition.y ), 60, 2, 1, 0.5f, seed );
	startRadius = RangeMap( startRadius, -1.0f, 1.0f, WORM_MIN_RADIUS, WORM_MAX_RADIUS );
	out_worm.pointRadii.push_back( startRadius );

	// number of segments
	float numSegs = GetPseudoRandomNoiseValueZeroToOne1D( (int) floorf( startPosition.x ) );
	numSegs = RangeMap( numSegs, 0.0f, 1.0f, (float) WORM_MIN_SEGS, (float) WORM_MAX_SEGS );
	// 	ConsolePrintf( "Num Segs: %f \n", numSegs );
	Vector3 lastPoint = startPosition; 
	EulerAngles lastOrientation = EulerAngles( startYawRadians, startPitchRadians, 0.0f );
	//ConsolePrintf("new worm \n");
	for( int segNum = 0; segNum < numSegs; ++segNum )
	{
		// direction
		EulerAngles orientation;
		orientation.pitchDegrees = lastOrientation.pitchDegrees + ConvertRadiansToDegrees( ComputePerlinNoiseValueAtPosition2D( Vector2( lastPoint.y, lastPoint.z ), 30, 4, WORM_MAX_PITCH_RADIANS, 0.5f, seed ) );
		//ConsolePrintf( "pitch degrees: %f \n", orientation.pitchDegreesAboutY );

		orientation.pitchDegrees = Clamp( orientation.pitchDegrees, -65.0f, 65.0f );
		//ConsolePrintf( "pitch degrees: %f \n", orientation.pitchDegreesAboutY );
		orientation.yawDegrees = lastOrientation.yawDegrees + ConvertRadiansToDegrees( ComputePerlinNoiseValueAtPosition2D( Vector2( lastPoint.x, lastPoint.y ), 30, 4, WORM_MAX_YAW_RADIANS, 0.5f, seed ));
		orientation.rollDegrees = 0.0f;


		lastOrientation = orientation;
		// 		if( segNum == 0 )
		// 			orientation.yawDegreesAboutZ = startYawRadians;

		// 		ConsolePrintf( "Orientation yaw: %f \n", orientation.yawDegreesAboutZ );
		Vector3 forward = orientation.GetForwardVector();
				//ConsolePrintf( "forward vector: %f, %f, %f \n", forward.x, forward.y, forward.z );
		// distance
		float segLength = GetPseudoRandomNoiseValueZeroToOne1D( (int) floorf( lastPoint.y ) );
		segLength = RangeMap( segLength, 0.0f, 1.0f, WORM_MIN_SEG_LENGTH, WORM_MAX_SEG_LENGTH );

		// get next point
		// 		ConsolePrintf( "Seg Length: %f, forwardVector: %f, %f, %f \n", segLength, forward.x, forward.y, forward.z );
		Vector3 nextPoint = lastPoint + ( segLength * forward );
		lastPoint = nextPoint;

		// radius at next point
		float radius = ComputePerlinNoiseValueAtPosition2D( Vector2( nextPoint.x, nextPoint.y ), 60, 2, 1, 0.5f, seed );
		radius = RangeMap( radius, -1.0f, 1.0f, (float) WORM_MIN_RADIUS, (float) WORM_MAX_RADIUS );

		// 		ConsolePrintf( "Max worm displacement: %f \n", WORM_MAX_DISPLACEMENT );
		// 		ConsolePrintf( "Current Displacement: %f \n", CalcDistance( startPosition, lastPoint ) );
		if( CalcDistance( startPosition, lastPoint ) < WORM_MAX_DISPLACEMENT )
		{
			out_worm.points.push_back( nextPoint );
			out_worm.pointRadii.push_back( radius );
		}
		else
			break;
	}
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void World::CarveRiverWorms( const PerlinWorms& worms, float percentageOfRadiusToFloor, const ChunkCoords& currentChunkCoords )
{
	for( PerlinWorms::const_iterator wormIter = worms.begin(); wormIter != worms.end(); ++wormIter )
	{
		PerlinWorm worm = ( *wormIter );
		
		Vector3s points = worm.points;
		std::vector<float> radii = worm.pointRadii;
		Vector3s::const_iterator pointIter = points.begin();
		std::vector<float>::const_iterator radiiIter = radii.begin();
		for( ; pointIter != points.end(); ++pointIter, ++radiiIter )
		{
			Vector3 point = ( *pointIter );
			bool hasNextPoint = true;
			Vector3 nextPoint;
			if( pointIter + 1 != points.end() )
				nextPoint = ( *( pointIter + 1 ) );
			else
				hasNextPoint = false;
			float radius = ( *radiiIter );
			
			if( hasNextPoint )
			{
				for( float t = 0.0f; t <= 1.0f; t += 0.1f )
				{
					Vector3 sphereCenter = Interpolate( point, nextPoint, t );

					for( float x = sphereCenter.x - radius; x <= sphereCenter.x + radius; ++x )
					{
						for( float y = sphereCenter.y - radius; y <= sphereCenter.y + radius; ++y )
						{
							for( float z = sphereCenter.z - ( radius * percentageOfRadiusToFloor ); z <= sphereCenter.z + radius; ++z )
							{
								WorldCoords blockPos = WorldCoords( x, y, z );
								ChunkCoords chunkCoords = Chunk::GetChunkCoordsFromWorldCoords( blockPos );
								//BlockIndex blockIndex = Chunk::GetIndexFromWorldCoords( blockPos );
						
								if( chunkCoords == currentChunkCoords )
								{
									if( CalcDistance( sphereCenter, blockPos ) < radius )
									{
										if( z <= SEA_LEVEL + 1 )
											m_replaceBlockType = BLOCK_TYPE_WATER;
										DestroyBlockAtWorldCoords( blockPos, false );
										m_replaceBlockType = BLOCK_TYPE_AIR;
									}
								}
							}
						}
					}		
				}
			}
		}
	}
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void World::CarveCaveWorms( const PerlinWorms& worms, float percentageOfRadiusToFloor, const ChunkCoords& currentChunkCoords )
{
	for( PerlinWorms::const_iterator wormIter = worms.begin(); wormIter != worms.end(); ++wormIter )
	{
		PerlinWorm worm = ( *wormIter );

		Vector3s points = worm.points;
		std::vector<float> radii = worm.pointRadii;
		Vector3s::const_iterator pointIter = points.begin();
		std::vector<float>::const_iterator radiiIter = radii.begin();
		for( ; pointIter != points.end(); ++pointIter, ++radiiIter )
		{
			Vector3 point = ( *pointIter );
			bool hasNextPoint = true;
			Vector3 nextPoint;
			if( pointIter + 1 != points.end() )
				nextPoint = ( *( pointIter + 1 ) );
			else
				hasNextPoint = false;
			float radius = ( *radiiIter );

			if( hasNextPoint )
			{
				for( float t = 0.0f; t <= 1.0f; t += 0.1f )
				{
					Vector3 sphereCenter = Interpolate( point, nextPoint, t );

					for( float x = sphereCenter.x - radius; x <= sphereCenter.x + radius; ++x )
					{
						for( float y = sphereCenter.y - radius; y <= sphereCenter.y + radius; ++y )
						{
							for( float z = sphereCenter.z - ( radius * percentageOfRadiusToFloor ); z <= sphereCenter.z + radius; ++z )
							{
								WorldCoords blockPos = WorldCoords( x, y, z );

								if( !AreWorldCoordsInWorld( blockPos ) )
									continue;

								ChunkCoords chunkCoords = Chunk::GetChunkCoordsFromWorldCoords( blockPos );
								//BlockIndex blockIndex = Chunk::GetIndexFromWorldCoords( blockPos );

								if( chunkCoords == currentChunkCoords )
								{
									if( CalcDistance( sphereCenter, blockPos ) < radius )
									{
// 										if( z <= SEA_LEVEL + 1 )
// 											m_replaceBlockType = BLOCK_TYPE_WATER;
										if( blockPos.z > 0 )
											DestroyBlockAtWorldCoords( blockPos, false );
// 										m_replaceBlockType = BLOCK_TYPE_AIR;
									}
								}
							}
						}
					}		
				}
			}
		}
	}
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void World::DestroyBlockBelow()
{
	ChunkCoords currentChunkCoords = Chunk::GetChunkCoordsFromWorldCoords( m_camera->m_position );
	Chunk* currentChunk = m_activeChunks[ currentChunkCoords ];

	LocalBlockCoords currentBlockCoords = Chunk::GetLocalBlockCoordsFromWorldCoords( m_camera->m_position );
	
	bool blockDestroyed = false;
	while( !blockDestroyed )
	{
		BlockIndex currentBlockIndex = currentChunk->GetIndexFromLocalBlockCoords( currentBlockCoords );
		if( currentChunk->GetBlockAtIndex( currentBlockIndex ).m_blockType == BLOCK_TYPE_AIR )
			currentBlockCoords.z -= 1;

		else
		{
			currentChunk->SetBlockType( currentBlockIndex, BLOCK_TYPE_AIR );
			blockDestroyed = true;
		}
	}

	currentBlockCoords.z += 1;
	BlockIndex currentBlockIndex = currentChunk->GetIndexFromLocalBlockCoords( LocalBlockCoords( currentBlockCoords.x, currentBlockCoords.y, currentBlockCoords.z ) );
	bool blockAboveIsSky = currentChunk->GetBlockAtIndex( currentBlockIndex ).IsSky();
	while( blockAboveIsSky )
	{
		currentBlockCoords.z -= 1;
		currentBlockIndex = currentChunk->GetIndexFromLocalBlockCoords( currentBlockCoords );

		if( !currentChunk->GetBlockAtIndex( currentBlockIndex ).IsOpaque() )
		{	
			currentChunk->GetBlockAtIndex( currentBlockIndex ).MarkAsSky();
			currentChunk->GetBlockAtIndex( currentBlockIndex ).MarkAsDirty();
			World::s_dirtyBlocks.push_back( BlockData( currentChunk, currentBlockIndex ) );
			WorldCoords currentBlockWorldCoords = currentChunk->GetWorldCoordsFromLocalCoords( currentBlockCoords );
			World::s_debugPoints.push_back( Vector3( currentBlockWorldCoords.x + 0.5f, currentBlockWorldCoords.y + 0.5f, currentBlockWorldCoords.z + 0.5f ) );
			currentChunk->SetVBODirty();

		}
		else
			blockAboveIsSky = false;
	}
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void World::PlaceBlockBelow()
{
	ChunkCoords currentChunkCoords = Chunk::GetChunkCoordsFromWorldCoords( m_camera->m_position );
	Chunk* currentChunk = m_activeChunks[ currentChunkCoords ];

	LocalBlockCoords currentBlockCoords = Chunk::GetLocalBlockCoordsFromWorldCoords( m_camera->m_position );

	bool blockPlaced = false;
	while( !blockPlaced )
	{
		BlockIndex currentBlockIndex = currentChunk->GetIndexFromLocalBlockCoords( currentBlockCoords );
		if( currentChunk->GetBlockAtIndex( currentBlockIndex ).m_blockType == BLOCK_TYPE_AIR )
			currentBlockCoords.z -= 1;

		else
		{
			currentBlockCoords.z += 1;
			currentBlockIndex = currentChunk->GetIndexFromLocalBlockCoords( currentBlockCoords );
			currentChunk->SetBlockType( currentBlockIndex, BLOCK_TYPE_GLOWSTONE );
			currentChunk->GetBlockAtIndex( currentBlockIndex ).SetLightValue( BlockDefinition::s_blockDefinitions[ currentChunk->GetBlockAtIndex( currentBlockIndex ).m_blockType ].m_illuminationLevel );
			currentChunk->GetBlockAtIndex( currentBlockIndex ).MarkAsNotSky();
			

			if( BlockDefinition::s_blockDefinitions[ currentChunk->GetBlockAtIndex( currentBlockIndex ).m_blockType ].m_illuminationLevel != 0 )
				MarkNeighborsDirty( BlockData( currentChunk, currentBlockIndex ) );
			blockPlaced = true;
		}
	}

	bool blockBelowIsOpaque = false;
	while( !blockBelowIsOpaque )
	{
		currentBlockCoords.z -= 1;
		BlockIndex currentBlockIndex = currentChunk->GetIndexFromLocalBlockCoords( currentBlockCoords );

		if( !BlockDefinition::s_blockDefinitions[ currentChunk->GetBlockAtIndex( currentBlockIndex ).m_blockType ].m_isOpaque )
		{
			currentChunk->GetBlockAtIndex( currentBlockIndex ).MarkAsNotSky();
			currentChunk->GetBlockAtIndex( currentBlockIndex ).MarkAsDirty();
			World::s_dirtyBlocks.push_back( BlockData( currentChunk, currentBlockIndex ) );
			WorldCoords currentBlockWorldCoords = currentChunk->GetWorldCoordsFromLocalCoords( currentBlockCoords );
			World::s_debugPoints.push_back( Vector3( currentBlockWorldCoords.x + 0.5f, currentBlockWorldCoords.y + 0.5f, currentBlockWorldCoords.z + 0.5f ) );
			currentChunk->SetVBODirty();

		}
		else
			blockBelowIsOpaque = true;
	}
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void World::DestroyBlockAtWorldCoords( const WorldCoords& worldCoords, bool playBreakSound )
{
// 	if( m_selectedFaceNormal == Vector3( -1.0f, -1.0f, -1.0f ) )
// 		return;

	ChunkCoords currentChunkCoords = Chunk::GetChunkCoordsFromWorldCoords( worldCoords );
	Chunk* currentChunk = NULL;
	if( m_activeChunks.find( currentChunkCoords ) != m_activeChunks.end() )
		 currentChunk = m_activeChunks.at( currentChunkCoords );
	else
		return;

	LocalBlockCoords currentBlockCoords = Chunk::GetLocalBlockCoordsFromWorldCoords( worldCoords );
	BlockIndex currentBlockIndex = currentChunk->GetIndexFromLocalBlockCoords( currentBlockCoords );

	//if( playBreakSound )
	//	currentChunk->GetBlockAtIndex( currentBlockIndex ).PlayRandomBreakSound();

	currentChunk->SetBlockType( currentBlockIndex, m_replaceBlockType );
	SetBlockDirty( BlockData( currentChunk, currentBlockIndex ) );

	currentBlockCoords.z += 1;
	bool blockAboveIsSky = false;
	if( currentBlockCoords.z < CHUNK_BLOCKS_Z )
	{
		currentBlockIndex = currentChunk->GetIndexFromLocalBlockCoords( LocalBlockCoords( currentBlockCoords.x, currentBlockCoords.y, currentBlockCoords.z ) );
		blockAboveIsSky = currentChunk->GetBlockAtIndex( currentBlockIndex ).IsSky();
	}
	else
		blockAboveIsSky = true;

	while( blockAboveIsSky )
	{
		currentBlockCoords.z -= 1;
		currentBlockIndex = currentChunk->GetIndexFromLocalBlockCoords( currentBlockCoords );

		if( !currentChunk->GetBlockAtIndex( currentBlockIndex ).IsOpaque() )
		{	
			currentChunk->GetBlockAtIndex( currentBlockIndex ).MarkAsSky();
			SetBlockDirty( BlockData( currentChunk, currentBlockIndex ) );


		}
		else
			blockAboveIsSky = false;
	}
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void World::PlaceBlockAtSelectedBlockWorldCoordsAndNormal()
{
	if( m_selectedFaceNormal == Vector3( -1.0f, -1.0f, -1.0f ) )
		return;

	if( m_selectedBlockCoords.z == (float) ( CHUNK_BLOCKS_Z - 1 ) && m_selectedFaceNormal == Vector3( 0.0f, 0.0f, 1.0f ) )
		return;

	if( m_selectedBlockCoords.z >= (float) CHUNK_BLOCKS_Z || m_selectedBlockCoords.z < 0.0f )
		return;

	if( m_selectedBlockCoords.z == 0.0f && m_selectedFaceNormal == Vector3( 0.0f, 0.0f, -1.0f ) )
		return;

	ChunkCoords currentChunkCoords;
	Chunk* currentChunk = NULL;
	LocalBlockCoords currentBlockCoords( -1, -1, -1 );
	BlockIndex currentBlockIndex = 0;

	//East
	if( m_selectedFaceNormal == Vector3( 1.0f, 0.0f, 0.0f ) )
	{
		
		currentChunkCoords = Chunk::GetChunkCoordsFromWorldCoords( WorldCoords( m_selectedBlockCoords.x + 1.0f, m_selectedBlockCoords.y, m_selectedBlockCoords.z ) );
		currentChunk = m_activeChunks[ currentChunkCoords ];
		currentBlockCoords = Chunk::GetLocalBlockCoordsFromWorldCoords( WorldCoords( m_selectedBlockCoords.x + 1.0f, m_selectedBlockCoords.y, m_selectedBlockCoords.z ) );
		currentBlockIndex = currentChunk->GetIndexFromLocalBlockCoords( currentBlockCoords );

	}

	//West
	if( m_selectedFaceNormal == Vector3( -1.0f, 0.0f, 0.0f ) )
	{

		currentChunkCoords = Chunk::GetChunkCoordsFromWorldCoords( WorldCoords( m_selectedBlockCoords.x + -1.0f, m_selectedBlockCoords.y, m_selectedBlockCoords.z ) );
		currentChunk = m_activeChunks[ currentChunkCoords ];
		currentBlockCoords = Chunk::GetLocalBlockCoordsFromWorldCoords( WorldCoords( m_selectedBlockCoords.x + -1.0f, m_selectedBlockCoords.y, m_selectedBlockCoords.z ) );
		currentBlockIndex = currentChunk->GetIndexFromLocalBlockCoords( currentBlockCoords );

	}

	//North
	if( m_selectedFaceNormal == Vector3( 0.0f, 1.0f, 0.0f ) )
	{

		currentChunkCoords = Chunk::GetChunkCoordsFromWorldCoords( WorldCoords( m_selectedBlockCoords.x, m_selectedBlockCoords.y + 1.0f, m_selectedBlockCoords.z ) );
		currentChunk = m_activeChunks[ currentChunkCoords ];
		currentBlockCoords = Chunk::GetLocalBlockCoordsFromWorldCoords( WorldCoords( m_selectedBlockCoords.x, m_selectedBlockCoords.y + 1.0f, m_selectedBlockCoords.z ) );
		currentBlockIndex = currentChunk->GetIndexFromLocalBlockCoords( currentBlockCoords );

	}

	//South
	if( m_selectedFaceNormal == Vector3( 0.0f, -1.0f, 0.0f ) )
	{

		currentChunkCoords = Chunk::GetChunkCoordsFromWorldCoords( WorldCoords( m_selectedBlockCoords.x, m_selectedBlockCoords.y - 1.0f, m_selectedBlockCoords.z ) );
		currentChunk = m_activeChunks[ currentChunkCoords ];
		currentBlockCoords = Chunk::GetLocalBlockCoordsFromWorldCoords( WorldCoords( m_selectedBlockCoords.x, m_selectedBlockCoords.y - 1.0f, m_selectedBlockCoords.z ) );
		currentBlockIndex = currentChunk->GetIndexFromLocalBlockCoords( currentBlockCoords );

	}

	if( m_selectedFaceNormal == Vector3( 0.0f, 0.0f, 1.0f ) )
	{

		currentChunkCoords = Chunk::GetChunkCoordsFromWorldCoords( WorldCoords( m_selectedBlockCoords.x, m_selectedBlockCoords.y, m_selectedBlockCoords.z + 1.0f ) );
		currentChunk = m_activeChunks[ currentChunkCoords ];
		currentBlockCoords = Chunk::GetLocalBlockCoordsFromWorldCoords( WorldCoords( m_selectedBlockCoords.x, m_selectedBlockCoords.y, m_selectedBlockCoords.z + 1.0f ) );
		currentBlockIndex = currentChunk->GetIndexFromLocalBlockCoords( currentBlockCoords );

	}

	if( m_selectedFaceNormal == Vector3( 0.0f, 0.0f, -1.0f ) )
	{

		currentChunkCoords = Chunk::GetChunkCoordsFromWorldCoords( WorldCoords( m_selectedBlockCoords.x, m_selectedBlockCoords.y, m_selectedBlockCoords.z - 1.0f ) );
		currentChunk = m_activeChunks[ currentChunkCoords ];
		currentBlockCoords = Chunk::GetLocalBlockCoordsFromWorldCoords( WorldCoords( m_selectedBlockCoords.x, m_selectedBlockCoords.y, m_selectedBlockCoords.z - 1.0f ) );
		currentBlockIndex = currentChunk->GetIndexFromLocalBlockCoords( currentBlockCoords );

	}

	currentChunk->SetBlockType( currentBlockIndex, m_currentBlockType );
	//currentChunk->GetBlockAtIndex( currentBlockIndex ).PlayRandomPlaceSound();
	MarkNeighborsDirty( BlockData( currentChunk, currentBlockIndex ) );

	BlockData currentBlockData( currentChunk, currentBlockIndex );
	if( currentBlockData.GetBlock().IsOpaque() )
	{
		currentBlockData.GetBlock().MarkAsNotSky();

		currentBlockCoords.z += 1;
		BlockIndex blockAboveIndex = currentChunk->GetIndexFromLocalBlockCoords( currentBlockCoords );

		currentBlockCoords.z -= 1;
		bool blockAboveIsSky = currentChunk->GetBlockAtIndex( blockAboveIndex ).IsSky();
		while( blockAboveIsSky )
		{
			currentBlockCoords.z -= 1;
			currentBlockIndex = currentChunk->GetIndexFromLocalBlockCoords( currentBlockCoords );

			if( !currentChunk->GetBlockAtIndex( currentBlockIndex ).IsOpaque() )
			{	
				currentChunk->GetBlockAtIndex( currentBlockIndex ).MarkAsNotSky();
				SetBlockDirty( BlockData( currentChunk, currentBlockIndex ) );

			}
			else
				blockAboveIsSky = false;
		}
	}
	else
	{
		currentBlockData.GetBlock().MarkAsSky();
		SetBlockDirty( currentBlockData );

	}

	
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void World::SetBlockDirty( const BlockData& blockData )
{
	if( !blockData.IsValid() )
		return;

	Chunk* chunk = blockData.m_chunk;
	BlockIndex blockIndex = blockData.m_blockIndex;

	if( chunk->GetBlockAtIndex( blockIndex ).IsDirty() )
		return;

	chunk->GetBlockAtIndex( blockIndex ).MarkAsDirty();
	World::s_dirtyBlocks.push_back( blockData );

	chunk->SetVBODirty();

	if( blockData.IsOnNorthEdge() && chunk->m_chunkToNorth )
		chunk->m_chunkToNorth->SetVBODirty();
	if( blockData.IsOnEastEdge() && chunk->m_chunkToEast )
		chunk->m_chunkToEast->SetVBODirty();
	if( blockData.IsOnSouthEdge() && chunk->m_chunkToSouth )
		chunk->m_chunkToSouth->SetVBODirty();
	if( blockData.IsOnWestEdge() && chunk->m_chunkToWest )
		chunk->m_chunkToWest->SetVBODirty();

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void World::MarkNeighborsDirty( const BlockData& blockData )
{
	WorldCoords blockCoords = blockData.m_chunk->GetWorldCoordsFromBlockIndex( blockData.m_blockIndex );
	
	
	BlockData northBlockData = blockData.GetNorthNeighbor();
	BlockData southBlockData = blockData.GetSouthNeighbor();
	BlockData eastBlockData = blockData.GetEastNeighbor();
	BlockData westBlockData = blockData.GetWestNeighbor();
	BlockData topBlockData = blockData.GetTopNeighbor();
	BlockData bottomBlockData = blockData.GetBottomNeighbor();


	if( northBlockData.IsValid() )
	{
		Block& northBlock = northBlockData.GetBlock();
		if( !northBlock.IsOpaque() && !northBlock.IsDirty() )
		{
			World::SetBlockDirty( northBlockData );
		}
	}

	if( southBlockData.IsValid() )
	{
		Block& southBlock = southBlockData.GetBlock();
		if( !southBlock.IsOpaque() && !southBlock.IsDirty() )
		{
			World::SetBlockDirty( southBlockData );
		}
	}

	if( eastBlockData.IsValid() )
	{
		Block& eastBlock = eastBlockData.GetBlock();
		if( !eastBlock.IsOpaque() && !eastBlock.IsDirty())
		{
			World::SetBlockDirty( eastBlockData );
		}
	}

	if( westBlockData.IsValid() )
	{
		Block& westBlock = westBlockData.GetBlock();
		if( !westBlock.IsOpaque() && !westBlock.IsDirty() )
		{
			World::SetBlockDirty( westBlockData );
		}
	}

	if( topBlockData.IsValid() )
	{
		Block& topBlock = topBlockData.GetBlock();
		if( !topBlock.IsOpaque() && !topBlock.IsDirty() )
		{
			World::SetBlockDirty( topBlockData );
		}
	}

	if( bottomBlockData.IsValid() )
	{
		Block& bottomBlock = bottomBlockData.GetBlock();
		if( !bottomBlock.IsOpaque() && !bottomBlock.IsDirty() )
		{
			World::SetBlockDirty( bottomBlockData );
		}
	}
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void World::MarkNSEWNonSkyNeighborsDirty( const BlockData& blockData )
{
	WorldCoords blockCoords = blockData.m_chunk->GetWorldCoordsFromBlockIndex( blockData.m_blockIndex );


	BlockData northBlockData = blockData.GetNorthNeighbor();
	BlockData southBlockData = blockData.GetSouthNeighbor();
	BlockData eastBlockData =  blockData.GetEastNeighbor();
	BlockData westBlockData =  blockData.GetWestNeighbor();


	if( northBlockData.IsValid() )
	{
		Block& northBlock = northBlockData.GetBlock();
		if( !northBlock.IsOpaque() && !northBlock.IsDirty() && !northBlock.IsSky() )
		{
			World::SetBlockDirty( northBlockData );
		}
	}

	if( southBlockData.IsValid() )
	{
		Block& southBlock = southBlockData.GetBlock();
		if( !southBlock.IsOpaque() && !southBlock.IsDirty() && !southBlock.IsSky() )
		{
			World::SetBlockDirty( southBlockData );
		}
	}

	if( eastBlockData.IsValid() )
	{
		Block& eastBlock = eastBlockData.GetBlock();
		if( !eastBlock.IsOpaque() && !eastBlock.IsDirty() && !eastBlock.IsSky() )
		{
			World::SetBlockDirty( eastBlockData );
		}
	}

	if( westBlockData.IsValid() )
	{
		Block& westBlock = westBlockData.GetBlock();
		if( !westBlock.IsOpaque() && !westBlock.IsDirty() && !westBlock.IsSky() )
		{
			World::SetBlockDirty( westBlockData );
		}
	}
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void World::PopulateDebugPoints()
{
	World::s_debugPoints.clear();
	World::s_debugPoints.reserve( World::s_dirtyBlocks.size() + m_raycastDebugPoints.size() );

	for( BlockDataList::const_iterator blockDataIter = World::s_dirtyBlocks.begin(); blockDataIter != World::s_dirtyBlocks.end(); ++blockDataIter )
	{
		BlockData blockData = ( *blockDataIter );

		WorldCoords blockCoords = blockData.m_chunk->GetWorldCoordsFromBlockIndex( blockData.m_blockIndex );
		World::s_debugPoints.push_back( Vector3( blockCoords.x + 0.5f, blockCoords.y + 0.5f, blockCoords.z + 0.5f ) );

	}

	for( Vector3s::const_iterator raycastIter = m_raycastDebugPoints.begin(); raycastIter != m_raycastDebugPoints.end(); ++raycastIter )
	{
		Vector3 raycastPoint = ( *raycastIter );
		World::s_debugPoints.push_back( raycastPoint );
	}
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
bool ChunkSortingFunction( Chunk* i, Chunk* j )
{
	return ( i->m_distanceToCamera > j->m_distanceToCamera );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void World::Draw( OpenGLRenderer* renderer, bool debugModeEnabled )
{
	if( !renderer )
		return;

	renderer->ApplyCameraTransformZUp( *m_camera );

	renderer->Disable( GL_ALPHA_TEST );
	renderer->BlendFunct(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//DrawTexturedSkybox(renderer, m_skyboxTexture , 1.0f );
	DrawColoredSkybox( renderer, 1.0f, Rgba( 0x87ceeb ) );

	float timeOfDay = 0.0f;

	bool sunSet = false;
	if( m_timeOfDayNormalized > 0.5f )
	{
		timeOfDay = m_timeOfDayNormalized - 0.5f;
		sunSet = true;
	}
	else
	{
		timeOfDay = m_timeOfDayNormalized;
		sunSet = false;
	}

	if( sunSet )
		timeOfDay = 0.5f - timeOfDay;

 	float nightAlpha = RangeMap( timeOfDay, 0.0f, 0.5f, 4.0f, -2.5f );
	nightAlpha = Clamp( nightAlpha, 0.0f, 1.0f );
// 	ConsolePrintf( "NightBox alpha: %f \n", nightAlpha );
 	DrawColoredSkybox( renderer, 0.9f, Rgba( 0.0f, 0.0f, 0.0f, nightAlpha ) );
	
	//DrawTexturedSkybox( renderer, m_nightSkyboxTexture, 0.9f );
	//renderer->Disable( GL_DEPTH_TEST );
	DrawSunAndMoon( renderer, debugModeEnabled );

	renderer->Enable( GL_DEPTH_TEST );
	renderer->Enable( GL_ALPHA_TEST );
	glAlphaFunc( GL_GREATER, 0.5f );

	renderer->DrawOriginAxes(10.0f);

	for( ChunkMap::const_iterator chunkIter = m_activeChunks.begin(); chunkIter != m_activeChunks.end(); ++chunkIter )
	{
		Chunk* chunk = ( chunkIter->second );
		if( !m_viewFrustumDebug )
		{
			m_frustumDebugCameraPos = m_camera->m_position;
			m_frustumDebugCameraForward = m_player->GetOrientation().GetForwardVector();
		}
        Vector3 fwdPos = m_frustumDebugCameraPos + m_frustumDebugCameraForward;

        //renderer->DrawPoint3D( m_frustumDebugCameraPos, Rgba::MAGENTA, 10 );
        //renderer->DrawPoint3D( fwdPos, Rgba::AQUA, 10 );

		if( chunk->ShouldRenderChunk( m_frustumDebugCameraPos, m_frustumDebugCameraForward ) )
		{
			bool highlightChunk = false;
			ChunkCoords chunkCoords = Chunk::GetChunkCoordsFromWorldCoords( m_camera->m_position );
			if( chunkCoords == chunk->m_chunkCoords )
				highlightChunk = true;

			if( g_renderingMode == GL_BEGIN )
				chunk->RenderWithGLBegin( renderer, highlightChunk );
			else if( g_renderingMode == GL_VERTEX_ARRAYS )
				chunk->RenderWithVAs( renderer );
			else if( g_renderingMode == GL_VBOS )
				chunk->RenderWithVBOs( renderer );
		}
	}

	Chunks chunks;

	for( ChunkMap::const_iterator chunkIter = m_activeChunks.begin(); chunkIter != m_activeChunks.end(); ++chunkIter )
		chunks.push_back( chunkIter->second );

	std::sort( chunks.begin(), chunks.end(), ChunkSortingFunction );

	for( Chunks::const_iterator chunkIter = chunks.begin(); chunkIter != chunks.end(); ++chunkIter )
	{
		Chunk* chunk = ( *chunkIter );
		chunk->RenderTranslucentFaces( m_camera->m_position, renderer, debugModeEnabled );
	}

	if( m_selectedFaceNormal != Vector3( -1.0f, -1.0f, -1.0f ) )
	{
		WorldBlockCoords selectedBlockCoords = Chunk::GetWorldBlockCoordsFromWorldCoords( m_selectedBlockCoords );
		DrawBlockOutline( renderer, debugModeEnabled, selectedBlockCoords, Rgba::BLACK );
		m_selectedFaceNormal = Vector3( -1.0f, -1.0f, -1.0f );
	}


	if( debugModeEnabled )
	{
		for( std::vector< WorldBlockCoords >::const_iterator raycastBlockIter = m_cameraRaycastBlocks.begin(); raycastBlockIter != m_cameraRaycastBlocks.end(); raycastBlockIter++ )
		{
			WorldBlockCoords worldCoords = *raycastBlockIter;
			DrawBlockOutline( renderer, debugModeEnabled, worldCoords, Rgba::AQUA );
		}
	}

	//renderer->Disable( GL_DEPTH_TEST );
	m_player->Render( renderer, debugModeEnabled );


	if( debugModeEnabled )
	{

// 		DrawWorm( renderer, debugModeEnabled, m_test2DWorm );
		for( PerlinWorms::const_iterator wormIter = m_caves.begin(); wormIter != m_caves.end(); ++wormIter )
		{
			PerlinWorm cave = ( *wormIter );
			DrawWorm( renderer, debugModeEnabled, cave );
		}

		PopulateDebugPoints();
	
		static float t = 0.0f;
		static float tIncrement = 0.0f;

		if( t <= 0.0f )
			tIncrement = 0.01f;
		if( t >= 1.0f )
			tIncrement = -0.01f;

		t += tIncrement;

		if( t <= 0.0f )
			t = 0.0f;
		if( t >= 1.0f )
			t = 1.0f;

		float debugRValue = Interpolate( Rgba::MAGENTA.Rf(), Rgba::BLACK.Rf(), t );
		float debugBValue = Interpolate( Rgba::MAGENTA.Bf(), Rgba::BLACK.Bf(), t );

		Rgba debugColor( debugRValue, 0.0f, debugBValue, 1.0f );


		for( Vector3s::const_iterator debugIter = s_debugPoints.begin(); debugIter != s_debugPoints.end(); ++debugIter )
		{
			Vector3 debugPoint = ( *debugIter );
			renderer->DrawPoint3D( debugPoint, debugColor, 10.0f );
		}

		renderer->Disable( GL_DEPTH_TEST );
		for( Vector3s::const_iterator debugIter = s_debugPoints.begin(); debugIter != s_debugPoints.end(); ++debugIter )
		{
			Vector3 debugPoint = ( *debugIter );
			renderer->DrawPoint3D( debugPoint, debugColor, 0.5f );
		}

		renderer->Enable( GL_DEPTH_TEST );
		renderer->DrawLineSegment3D( m_raycastDebugStartPos, m_raycastDebugEndPos, Rgba::AQUA );
	}

	m_userInterface->Render( renderer );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void World::DrawTexturedSkybox( OpenGLRenderer* renderer, AnimatedTexture* skyboxTexture, float distanceFromCamera )
{
	// skybox
	renderer->Disable( GL_DEPTH_TEST );

	renderer->PushMatrix();
	renderer->SetModelViewTranslation( m_camera->m_position.x , m_camera->m_position.y, m_camera->m_position.z );
	Vector2 skyboxTexCoordSizePerTile = skyboxTexture->GetTexCoordSizePerCell();

	Vector2 skyboxTopTextureMins = skyboxTexture->CalcTextureCoordMinsAtSpriteCoordinates( IntVector2( 1, 0 ) );
	Vector2 skyboxTopTextureMaxs = Vector2( skyboxTopTextureMins.x + skyboxTexCoordSizePerTile.x, skyboxTopTextureMins.y + skyboxTexCoordSizePerTile.y );
	Vector2 skyboxBottomTextureMins = skyboxTexture->CalcTextureCoordMinsAtSpriteCoordinates( IntVector2( 1, 2 ) );
	Vector2 skyboxBottomTextureMaxs = Vector2( skyboxBottomTextureMins.x + skyboxTexCoordSizePerTile.x, skyboxBottomTextureMins.y + skyboxTexCoordSizePerTile.y );
	Vector2 skyboxNorthTextureMins = skyboxTexture->CalcTextureCoordMinsAtSpriteCoordinates( IntVector2( 0, 1 ) );
	Vector2 skyboxNorthTextureMaxs = Vector2( skyboxNorthTextureMins.x + skyboxTexCoordSizePerTile.x, skyboxNorthTextureMins.y + skyboxTexCoordSizePerTile.y );
	Vector2 skyboxSouthTextureMins = skyboxTexture->CalcTextureCoordMinsAtSpriteCoordinates( IntVector2( 2, 1 ) );
	Vector2 skyboxSouthTextureMaxs = Vector2( skyboxSouthTextureMins.x + skyboxTexCoordSizePerTile.x, skyboxSouthTextureMins.y + skyboxTexCoordSizePerTile.y );
	Vector2 skyboxEastTextureMins = skyboxTexture->CalcTextureCoordMinsAtSpriteCoordinates( IntVector2( 1, 1 ) );
	Vector2 skyboxEastTextureMaxs = Vector2( skyboxEastTextureMins.x + skyboxTexCoordSizePerTile.x, skyboxEastTextureMins.y + skyboxTexCoordSizePerTile.y );
	Vector2 skyboxWestTextureMins = skyboxTexture->CalcTextureCoordMinsAtSpriteCoordinates( IntVector2( 3, 1 ) );
	Vector2 skyboxWestTextureMaxs = Vector2( skyboxWestTextureMins.x + skyboxTexCoordSizePerTile.x, skyboxWestTextureMins.y + skyboxTexCoordSizePerTile.y );

	Vector3s vertices;
	Vector2s texCoords;

	// top
	vertices.push_back( Vector3( distanceFromCamera, distanceFromCamera, distanceFromCamera ) );
	vertices.push_back( Vector3( distanceFromCamera, -distanceFromCamera, distanceFromCamera ) );
	vertices.push_back( Vector3( -distanceFromCamera, -distanceFromCamera, distanceFromCamera ) );
	vertices.push_back( Vector3( -distanceFromCamera, distanceFromCamera, distanceFromCamera ) );



	texCoords.push_back( Vector2( skyboxTopTextureMins.x, skyboxTopTextureMaxs.y ) );
	texCoords.push_back( Vector2( skyboxTopTextureMaxs.x, skyboxTopTextureMaxs.y ) );
	texCoords.push_back( Vector2( skyboxTopTextureMaxs.x, skyboxTopTextureMins.y ) );
	texCoords.push_back( Vector2( skyboxTopTextureMins.x, skyboxTopTextureMins.y ) );

	renderer->DrawTexturedQuad3D( *skyboxTexture, vertices, texCoords );
	vertices.clear();
	texCoords.clear();


	// bottom
	vertices.push_back( Vector3( -distanceFromCamera, -distanceFromCamera, -distanceFromCamera ) );
	vertices.push_back( Vector3( distanceFromCamera, -distanceFromCamera, -distanceFromCamera ) );
	vertices.push_back( Vector3( distanceFromCamera, distanceFromCamera, -distanceFromCamera ) );
	vertices.push_back( Vector3( -distanceFromCamera, distanceFromCamera, -distanceFromCamera ) );

	texCoords.push_back( Vector2( skyboxBottomTextureMins.x, skyboxBottomTextureMaxs.y ) );
	texCoords.push_back( Vector2( skyboxBottomTextureMaxs.x, skyboxBottomTextureMaxs.y ) );
	texCoords.push_back( Vector2( skyboxBottomTextureMaxs.x, skyboxBottomTextureMins.y ) );
	texCoords.push_back( Vector2( skyboxBottomTextureMins.x, skyboxBottomTextureMins.y ) );
	renderer->DrawTexturedQuad3D( *skyboxTexture, vertices, texCoords );
	vertices.clear();
	texCoords.clear();

	// north
	vertices.push_back( Vector3( -distanceFromCamera, distanceFromCamera, -distanceFromCamera ) );
	vertices.push_back( Vector3( distanceFromCamera, distanceFromCamera, -distanceFromCamera ) );
	vertices.push_back( Vector3( distanceFromCamera, distanceFromCamera, distanceFromCamera ) );
	vertices.push_back( Vector3( -distanceFromCamera, distanceFromCamera, distanceFromCamera ) );

	texCoords.push_back( Vector2( skyboxNorthTextureMins.x, skyboxNorthTextureMaxs.y ) );
	texCoords.push_back( Vector2( skyboxNorthTextureMaxs.x, skyboxNorthTextureMaxs.y ) );
	texCoords.push_back( Vector2( skyboxNorthTextureMaxs.x, skyboxNorthTextureMins.y ) );
	texCoords.push_back( Vector2( skyboxNorthTextureMins.x, skyboxNorthTextureMins.y ) );
	renderer->DrawTexturedQuad3D( *skyboxTexture, vertices, texCoords );
	vertices.clear();
	texCoords.clear();

	// south
	vertices.push_back( Vector3( distanceFromCamera, -distanceFromCamera, -distanceFromCamera ) );
	vertices.push_back( Vector3( -distanceFromCamera, -distanceFromCamera, -distanceFromCamera ) );
	vertices.push_back( Vector3( -distanceFromCamera, -distanceFromCamera, distanceFromCamera ) );
	vertices.push_back( Vector3( distanceFromCamera, -distanceFromCamera, distanceFromCamera ) );

	texCoords.push_back( Vector2( skyboxSouthTextureMins.x, skyboxSouthTextureMaxs.y ) );
	texCoords.push_back( Vector2( skyboxSouthTextureMaxs.x, skyboxSouthTextureMaxs.y ) );
	texCoords.push_back( Vector2( skyboxSouthTextureMaxs.x, skyboxSouthTextureMins.y ) );
	texCoords.push_back( Vector2( skyboxSouthTextureMins.x, skyboxSouthTextureMins.y ) );
	renderer->DrawTexturedQuad3D( *skyboxTexture, vertices, texCoords );
	vertices.clear();
	texCoords.clear();


	// east
	vertices.push_back( Vector3( distanceFromCamera, distanceFromCamera, -distanceFromCamera ) );
	vertices.push_back( Vector3( distanceFromCamera, -distanceFromCamera, -distanceFromCamera ) );
	vertices.push_back( Vector3( distanceFromCamera, -distanceFromCamera, distanceFromCamera ) );
	vertices.push_back( Vector3( distanceFromCamera, distanceFromCamera, distanceFromCamera ) );

	texCoords.push_back( Vector2( skyboxEastTextureMins.x, skyboxEastTextureMaxs.y ) );
	texCoords.push_back( Vector2( skyboxEastTextureMaxs.x, skyboxEastTextureMaxs.y ) );
	texCoords.push_back( Vector2( skyboxEastTextureMaxs.x, skyboxEastTextureMins.y ) );
	texCoords.push_back( Vector2( skyboxEastTextureMins.x, skyboxEastTextureMins.y ) );
	renderer->DrawTexturedQuad3D( *skyboxTexture, vertices, texCoords );
	vertices.clear();
	texCoords.clear();


	// west
	vertices.push_back( Vector3( -distanceFromCamera, -distanceFromCamera, -distanceFromCamera ) );
	vertices.push_back( Vector3( -distanceFromCamera, distanceFromCamera, -distanceFromCamera ) );
	vertices.push_back( Vector3( -distanceFromCamera, distanceFromCamera, distanceFromCamera ) );
	vertices.push_back( Vector3( -distanceFromCamera, -distanceFromCamera, distanceFromCamera ) );

	texCoords.push_back( Vector2( skyboxWestTextureMins.x, skyboxWestTextureMaxs.y ) );
	texCoords.push_back( Vector2( skyboxWestTextureMaxs.x, skyboxWestTextureMaxs.y ) );
	texCoords.push_back( Vector2( skyboxWestTextureMaxs.x, skyboxWestTextureMins.y ) );
	texCoords.push_back( Vector2( skyboxWestTextureMins.x, skyboxWestTextureMins.y ) );
	renderer->DrawTexturedQuad3D( *skyboxTexture, vertices, texCoords );
	vertices.clear();
	texCoords.clear();


	renderer->PopMatrix();
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void World::DrawColoredSkybox(OpenGLRenderer* renderer, float distanceFromCamera, const Rgba& color)
{
	// skybox
	renderer->Disable( GL_DEPTH_TEST );

	renderer->PushMatrix();
	renderer->SetModelViewTranslation( m_camera->m_position.x , m_camera->m_position.y, m_camera->m_position.z );

	Vector3s vertices;

	// top
	vertices.push_back( Vector3( distanceFromCamera, distanceFromCamera, distanceFromCamera ) );
	vertices.push_back( Vector3( distanceFromCamera, -distanceFromCamera, distanceFromCamera ) );
	vertices.push_back( Vector3( -distanceFromCamera, -distanceFromCamera, distanceFromCamera ) );
	vertices.push_back( Vector3( -distanceFromCamera, distanceFromCamera, distanceFromCamera ) );

	renderer->DrawQuad3D( vertices, color );
	vertices.clear();


	// bottom
	vertices.push_back( Vector3( -distanceFromCamera, -distanceFromCamera, -distanceFromCamera ) );
	vertices.push_back( Vector3( distanceFromCamera, -distanceFromCamera, -distanceFromCamera ) );
	vertices.push_back( Vector3( distanceFromCamera, distanceFromCamera, -distanceFromCamera ) );
	vertices.push_back( Vector3( -distanceFromCamera, distanceFromCamera, -distanceFromCamera ) );

	renderer->DrawQuad3D( vertices, color );
	vertices.clear();


	// north
	vertices.push_back( Vector3( -distanceFromCamera, distanceFromCamera, -distanceFromCamera ) );
	vertices.push_back( Vector3( distanceFromCamera, distanceFromCamera, -distanceFromCamera ) );
	vertices.push_back( Vector3( distanceFromCamera, distanceFromCamera, distanceFromCamera ) );
	vertices.push_back( Vector3( -distanceFromCamera, distanceFromCamera, distanceFromCamera ) );

	renderer->DrawQuad3D( vertices, color );
	vertices.clear();

	// south
	vertices.push_back( Vector3( distanceFromCamera, -distanceFromCamera, -distanceFromCamera ) );
	vertices.push_back( Vector3( -distanceFromCamera, -distanceFromCamera, -distanceFromCamera ) );
	vertices.push_back( Vector3( -distanceFromCamera, -distanceFromCamera, distanceFromCamera ) );
	vertices.push_back( Vector3( distanceFromCamera, -distanceFromCamera, distanceFromCamera ) );

	renderer->DrawQuad3D( vertices, color );
	vertices.clear();


	// east
	vertices.push_back( Vector3( distanceFromCamera, distanceFromCamera, -distanceFromCamera ) );
	vertices.push_back( Vector3( distanceFromCamera, -distanceFromCamera, -distanceFromCamera ) );
	vertices.push_back( Vector3( distanceFromCamera, -distanceFromCamera, distanceFromCamera ) );
	vertices.push_back( Vector3( distanceFromCamera, distanceFromCamera, distanceFromCamera ) );

	renderer->DrawQuad3D( vertices, color );
	vertices.clear();


	// west
	vertices.push_back( Vector3( -distanceFromCamera, -distanceFromCamera, -distanceFromCamera ) );
	vertices.push_back( Vector3( -distanceFromCamera, distanceFromCamera, -distanceFromCamera ) );
	vertices.push_back( Vector3( -distanceFromCamera, distanceFromCamera, distanceFromCamera ) );
	vertices.push_back( Vector3( -distanceFromCamera, -distanceFromCamera, distanceFromCamera ) );
	renderer->DrawQuad3D( vertices, color );
	vertices.clear();


	renderer->PopMatrix();
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void World::DrawSunAndMoon(OpenGLRenderer* renderer, bool debugModeEnabled)
{
	UNUSED( debugModeEnabled );

	renderer->PushMatrix();
	renderer->SetModelViewTranslation( m_camera->m_position.x , m_camera->m_position.y, m_camera->m_position.z );
	
	// calculate sun position

	float sunAngleOffHorizonDegrees = RangeMap( m_timeOfDayNormalized, 0.0f, 1.0f, 0.0f, 360.0f );

	// correction for starting position
	sunAngleOffHorizonDegrees -= 90.0f;
	float moonAngleOffHorizonDegrees = sunAngleOffHorizonDegrees + 180.0f;

	float sunAngleOffHorizonRadians = ConvertDegreesToRadians( sunAngleOffHorizonDegrees );
	float moonAngleOffHorizonRadians = ConvertDegreesToRadians( moonAngleOffHorizonDegrees );

	//ConsolePrintf("Time of Day is %f, the sun is at angle %f \n", m_timeOfDayNormalized, sunAngleOffHorizonDegrees );
	
	Vector3 sunPosition( cos( sunAngleOffHorizonRadians ), 0.0f, sin( sunAngleOffHorizonRadians ) );
	Vector3 moonPosition( cos( moonAngleOffHorizonRadians ), 0.0f, sin( moonAngleOffHorizonRadians ) );
	
	//ConsolePrintf( "SunPosition Z: %f, time of day %f \n", sunPosition.z, m_timeOfDayNormalized );

	float sunSize = 0.05f;
	float sinSunX = RangeMap( sin( sunAngleOffHorizonRadians ), -1.0f, 1.0f, -sunSize, sunSize );
	float cosSunZ = RangeMap( cos( sunAngleOffHorizonRadians ), -1.0f, 1.0f, -sunSize, sunSize );

	float moonSize = 0.05f;
	float sinMoonX = RangeMap( sin( moonAngleOffHorizonRadians ), -1.0f, 1.0f, -moonSize, moonSize );
	float cosMoonZ = RangeMap( cos( moonAngleOffHorizonRadians ), -1.0f, 1.0f, -moonSize, moonSize );

	//renderer->SetModelViewRotation( sunAngleOffHorizonDegrees, 0.0f, 1.0f, 0.0f );

	renderer->PushMatrix();
	renderer->SetModelViewTranslation( sunPosition.x, sunPosition.y, sunPosition.z );

// 	if( debugModeEnabled )
// 	{
		Vector3s vertices;
		vertices.push_back( Vector3( sinSunX, sunSize, -cosSunZ ) );
		vertices.push_back( Vector3( sinSunX, -sunSize, -cosSunZ ) );
		vertices.push_back( Vector3( -sinSunX, -sunSize, cosSunZ ) );
		vertices.push_back( Vector3( -sinSunX, sunSize, cosSunZ ) );

		// sun

		renderer->DrawFilledPolygon3D( vertices, Rgba::YELLOW );
		renderer->PopMatrix();
		vertices.clear();

		renderer->PushMatrix();
		renderer->SetModelViewTranslation( moonPosition.x, moonPosition.y, moonPosition.z );
		//renderer->SetModelViewRotation( moonAngleOffHorizonDegrees, 0.0f, 1.0f, 0.0f );
		
		vertices.push_back( Vector3( sinMoonX, moonSize, -cosMoonZ ) );
		vertices.push_back( Vector3( sinMoonX, -moonSize, -cosMoonZ ) );
		vertices.push_back( Vector3( -sinMoonX, -moonSize, cosMoonZ ) );
		vertices.push_back( Vector3( -sinMoonX, moonSize, cosMoonZ ) );

		// moon
		renderer->DrawFilledPolygon3D( vertices, Rgba::WHITE );
/*	}*/

	renderer->PopMatrix();
	renderer->PopMatrix();
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void World::DrawBlockOutline( OpenGLRenderer* renderer, bool debugModeEnabled, const WorldBlockCoords& worldBlockCoords, const Rgba& outlineColor )
{
	UNUSED(debugModeEnabled);

	renderer->Disable( GL_TEXTURE_2D );

	Vector3s faceCoords;
	float lineWidth = 3.0f;

	WorldCoords worldCoords = Chunk::GetWorldCoordMinsFromWorldBlockCoords( worldBlockCoords );

// 	if( m_selectedFaceNormal != Vector3( -1.0f, -1.0f, -1.0f ) )
// 	{
		// top
		// 	if( m_selectedFaceNormal == Vector3( 0.0f, 0.0f, 1.0f ) )
		// 	{
		faceCoords.push_back( Vector3( worldCoords.x, worldCoords.y, worldCoords.z + 1.0f ) );
		faceCoords.push_back( Vector3( worldCoords.x + 1.0f, worldCoords.y, worldCoords.z + 1.0f ) );
		faceCoords.push_back( Vector3( worldCoords.x + 1.0f, worldCoords.y + 1.0f, worldCoords.z + 1.0f ) );
		faceCoords.push_back( Vector3( worldCoords.x, worldCoords.y + 1.0f, worldCoords.z + 1.0f ) );
		renderer->DrawPolygon3D( faceCoords, outlineColor, lineWidth );
		faceCoords.clear();
		// 	}
		// 
		// 	// bottom
		// 	if( m_selectedFaceNormal == Vector3( 0.0f, 0.0f, -1.0f ) )
		// 	{
		faceCoords.push_back( Vector3( worldCoords.x + 1.0f, worldCoords.y, worldCoords.z ) );
		faceCoords.push_back( Vector3( worldCoords.x, worldCoords.y, worldCoords.z ) );
		faceCoords.push_back( Vector3( worldCoords.x, worldCoords.y + 1.0f, worldCoords.z ) );
		faceCoords.push_back( Vector3( worldCoords.x + 1.0f, worldCoords.y + 1.0f, worldCoords.z ) );
		renderer->DrawPolygon3D( faceCoords, outlineColor, lineWidth );
		faceCoords.clear();
		// 	}
		// 
		// 	// north
		// 	if( m_selectedFaceNormal == Vector3( 0.0f, 1.0f, 0.0f ) )
		// 	{
		faceCoords.push_back( Vector3( worldCoords.x + 1.0f, worldCoords.y + 1.0f, worldCoords.z ) );
		faceCoords.push_back( Vector3( worldCoords.x, worldCoords.y + 1.0f, worldCoords.z ) );
		faceCoords.push_back( Vector3( worldCoords.x, worldCoords.y + 1.0f, worldCoords.z + 1.0f ) );
		faceCoords.push_back( Vector3( worldCoords.x + 1.0f, worldCoords.y + 1.0f, worldCoords.z + 1.0f ) );
		renderer->DrawPolygon3D( faceCoords, outlineColor, lineWidth );
		faceCoords.clear();
		// 	}
		// 
		// 	// south
		// 	if( m_selectedFaceNormal == Vector3( 0.0f, -1.0f, 0.0f ) )
		// 	{
		faceCoords.push_back( Vector3( worldCoords.x, worldCoords.y, worldCoords.z ) );
		faceCoords.push_back( Vector3( worldCoords.x + 1.0f, worldCoords.y, worldCoords.z ) );
		faceCoords.push_back( Vector3( worldCoords.x + 1.0f, worldCoords.y, worldCoords.z + 1.0f ) );
		faceCoords.push_back( Vector3( worldCoords.x, worldCoords.y, worldCoords.z + 1.0f ) );
		renderer->DrawPolygon3D( faceCoords, outlineColor, lineWidth );
		faceCoords.clear();
		// 	}
		// 
		// 	// east
		// 	if( m_selectedFaceNormal == Vector3( 1.0f, 0.0f, 0.0f ) )
		// 	{
		faceCoords.push_back( Vector3( worldCoords.x + 1.0f, worldCoords.y, worldCoords.z ) );
		faceCoords.push_back( Vector3( worldCoords.x + 1.0f, worldCoords.y + 1.0f, worldCoords.z ) );
		faceCoords.push_back( Vector3( worldCoords.x + 1.0f, worldCoords.y + 1.0f, worldCoords.z + 1.0f ) );
		faceCoords.push_back( Vector3( worldCoords.x + 1.0f, worldCoords.y, worldCoords.z + 1.0f ) );
		renderer->DrawPolygon3D( faceCoords, outlineColor, lineWidth );
		faceCoords.clear();
		// 	}
		// 
		// 	// west
		// 	if( m_selectedFaceNormal == Vector3( -1.0f, 0.0f, 0.0f ) )
		// 	{
		faceCoords.push_back( Vector3( worldCoords.x, worldCoords.y + 1.0f, worldCoords.z ) );
		faceCoords.push_back( Vector3( worldCoords.x, worldCoords.y, worldCoords.z ) );
		faceCoords.push_back( Vector3( worldCoords.x, worldCoords.y, worldCoords.z + 1.0f ) );
		faceCoords.push_back( Vector3( worldCoords.x, worldCoords.y + 1.0f, worldCoords.z + 1.0f) );
		renderer->DrawPolygon3D( faceCoords, outlineColor, lineWidth );
		// 	}
	//}
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void World::DrawWorm( OpenGLRenderer* renderer, bool debugModeEnabled, const PerlinWorm& worm )
{
	UNUSED( debugModeEnabled );
	if( !renderer )
		return;

	Vector3s points = worm.points;
	if( points.size() == 0 )
		return;

// 	ConsolePrintf( "Num points: %d \n", points.size() );
	for( Vector3s::const_iterator pointIter = points.begin(); pointIter != points.end(); ++pointIter )
	{
		if( pointIter + 1 == points.end() )
			break;

		Vector3 startPoint = ( *pointIter );
// 		ConsolePrintf( "Worm Point: %f, %f, %f \n", startPoint.x, startPoint.y, startPoint.z );

		Vector3 endPoint = ( *(pointIter + 1) );
		renderer->SetLineSize( 10.0f );
		renderer->DrawLineSegment3D( startPoint, endPoint, Rgba::MAGENTA );
	}
}