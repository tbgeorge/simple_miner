//=================================================================================
// World.hpp
// Author: Tyler George
// Date  : January 29, 2015
//=================================================================================

#pragma once
#ifndef __included_World__
#define __included_World__

#include "Engine/Math/Vector2.hpp"
#include "Engine/Renderer/OpenGLRenderer.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/Math2D.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/AnimatedTexture.hpp"
#include "Engine/Utilities/Time.hpp"
#include "Engine/Utilities/Console.hpp"
#include "GameCode/Chunk.hpp"
#include "GameCode/GameCommon.hpp"
#include "GameCode/UserInterface.hpp"
#include "GameCode/Player.hpp"


///---------------------------------------------------------------------------------
/// World Constants
///---------------------------------------------------------------------------------
const int CHUNK_INNER_VISIBILITY_RADIUS = 8;
const int CHUNK_OUTER_VISIBILITY_OFFSET = 2;
const int CHUNK_OUTER_VISIBILITY_RADIUS = CHUNK_OUTER_VISIBILITY_OFFSET + CHUNK_INNER_VISIBILITY_RADIUS;

const float SEA_LEVEL = 64.0f;

///---------------------------------------------------------------------------------
/// 2D Worms
///---------------------------------------------------------------------------------
const int CHUNKS_PER_AXIS_PER_PERLIN_CELL = 2;
const int CHUNK_WORM_VISIBLITY_OFFSET = 25;
const int CHUNK_WORM_GENERATION_RADIUS = CHUNK_WORM_VISIBLITY_OFFSET + CHUNK_INNER_VISIBILITY_RADIUS;
const float WORM_MAX_DISPLACEMENT = ( CHUNK_BLOCKS_X > CHUNK_BLOCKS_Y ? CHUNK_BLOCKS_Y : CHUNK_BLOCKS_X ) * ( CHUNK_WORM_VISIBLITY_OFFSET - 3 );
const float WORM_MAX_YAW_RADIANS = ConvertDegreesToRadians( 165.0f );
const float WORM_MAX_PITCH_RADIANS = ConvertDegreesToRadians( 80.0f );
const int WORM_MIN_SEGS = 8;
const int WORM_MAX_SEGS = 50;
const float WORM_MIN_SEG_LENGTH = 5.0f;
const float WORM_MAX_SEG_LENGTH = 25.0f;
const float WORM_MIN_RADIUS = 1.0f;
const float WORM_MAX_RADIUS = 3.0f;


const int DAY_LIGHT_VALUE = 15;
const int NIGHT_LIGHT_VALUE = 6;
const int MORNING_LIGHT_VALUE = 9;
const int EVENING_LIGHT_VALUE = 8;

const int NUM_LIGHT_LEVELS = 16;

const int NUM_CHUNKS_TO_RELIGHT_PER_FRAME = 1;
const int OUTDOOR_LIGHT_VALUE = DAY_LIGHT_VALUE;
const float REAL_MINUTES_PER_GAME_DAY = 20.0f;
const float REAL_SECONDS_PER_GAME_DAY = REAL_MINUTES_PER_GAME_DAY * 60.0f;

const Vector3 GRAVITY( 0.0f, 0.0f, -2.5f );

///---------------------------------------------------------------------------------
/// Player Constants
///---------------------------------------------------------------------------------
const Vector3 PLAYER_START_POS = Vector3( 1000.0f, 1000.0f, 100.0f ); 
const EulerAngles PLAYER_START_ORIENTATION = EulerAngles( 0.0f, 0.0f, 0.0f );
const Vector3 PLAYER_BBOX_OFFSETS = Vector3( 0.3f, 0.3f, 0.925f );

const float STEP_AND_SAMPLE_T_INTERVAL = 0.001f;



///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
class BlockData
{
public:
	BlockData( Chunk* chunk, const BlockIndex& blockIndex ) 
		: m_chunk( chunk ), m_blockIndex( blockIndex ), m_distanceToCamera( -1.0f ) {}

	BlockData() : m_chunk( NULL ), m_blockIndex( 0 ), m_distanceToCamera( -1.0f ) {}

	Chunk* m_chunk;
	BlockIndex m_blockIndex;
	float m_distanceToCamera;

	bool IsValid() const;
	bool IsOnEastEdge() const;
	bool IsOnNorthEdge() const;
	bool IsOnWestEdge() const;
	bool IsOnSouthEdge() const;
	bool IsOnTopEdge() const;
	bool IsOnBottomEdge() const;

	BlockData GetEastNeighbor() const;
	BlockData GetNorthNeighbor() const;
	BlockData GetWestNeighbor() const;
	BlockData GetSouthNeighbor() const;
	BlockData GetTopNeighbor() const;
	BlockData GetBottomNeighbor() const;

	WorldCoords GetWorldCoords() const;

	Block& GetBlock() const;

	bool operator ==( const BlockData& rhs );

};

///---------------------------------------------------------------------------------
/// Inline functions for BlockData
///---------------------------------------------------------------------------------
///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline bool BlockData::IsValid() const
{
	return m_chunk == NULL ? false : true;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline bool BlockData::IsOnEastEdge() const 
{
	int localX = Chunk::GetLocalXCoordFromBlockIndex( m_blockIndex );
	return localX == CHUNK_BLOCKS_X - 1;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline bool BlockData::IsOnNorthEdge() const 
{
	int localY = Chunk::GetLocalYCoordFromBlockIndex( m_blockIndex );
	return localY == CHUNK_BLOCKS_Y - 1;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline bool BlockData::IsOnWestEdge() const 
{
	int localX = Chunk::GetLocalXCoordFromBlockIndex( m_blockIndex );
	return localX == 0;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline bool BlockData::IsOnSouthEdge() const 
{
	int localY = Chunk::GetLocalYCoordFromBlockIndex( m_blockIndex );
	return localY == 0;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline bool BlockData::IsOnTopEdge() const 
{
	int localZ = Chunk::GetLocalZCoordFromBlockIndex( m_blockIndex );
	return localZ == CHUNK_BLOCKS_Z - 1;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline bool BlockData::IsOnBottomEdge() const 
{
	int localZ = Chunk::GetLocalZCoordFromBlockIndex( m_blockIndex );
	return localZ == 0;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline WorldCoords BlockData::GetWorldCoords() const
{
	return m_chunk->GetWorldCoordsFromBlockIndex( m_blockIndex );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline Block& BlockData::GetBlock() const
{
	return m_chunk->GetBlockAtIndex( m_blockIndex );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline bool BlockData::operator==( const BlockData& rhs )
{
	if( IsValid() && rhs.IsValid() )
	{
		if( m_chunk->m_chunkCoords == rhs.m_chunk->m_chunkCoords && m_blockIndex == m_blockIndex )
			return true;
		return false;
	}
	else if( !IsValid() && !rhs.IsValid() )
		return true;
	else
		return false;
}

///---------------------------------------------------------------------------------
/// BlockData typedefs
///---------------------------------------------------------------------------------
typedef std::vector< BlockData > BlockDataList;


///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
struct RayCast3DResult 
{
	RayCast3DResult() {};
	bool didImpact;
	Vector3 impactBlockWorldCoords;
	WorldCoords impactWorldCoords;
	BlockData impactedBlock;
	float impactFraction;
	Vector3 impactSurfaceNormal;
};

///---------------------------------------------------------------------------------
/// RayCast3DResult typedefs
///---------------------------------------------------------------------------------
typedef std::vector< RayCast3DResult > RayCast3DResults;

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
struct PerlinWormWorldCell
{
	ChunkCoords minChunkVals;
	ChunkCoords maxChunkVals;
	IntVector2 cellCoords;
};
typedef std::vector< PerlinWormWorldCell > PerlinWormWorldCells;

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
struct PerlinWormParameters
{
	Vector3 startPosition;
	float startPitchRadians;
	float startYawRadians;
};
typedef std::vector< PerlinWormParameters > PerlinWormParametersList;

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
struct PerlinWorm
{
	Vector3s points;
	std::vector< float > pointRadii;

};
typedef std::vector< PerlinWorm > PerlinWorms;

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
class World
{
public:
	///---------------------------------------------------------------------------------
	/// Constructor/Destructor
	///---------------------------------------------------------------------------------
	World( const Vector2& displaySize );
	~World();

	///---------------------------------------------------------------------------------
	/// Initialization Functions
	///---------------------------------------------------------------------------------
	void Startup();
	void Shutdown();

	///---------------------------------------------------------------------------------
	/// Accessors/Queries
	///---------------------------------------------------------------------------------
	const RayCast3DResult StepAndSample( const Vector3& startWorldCoords, const Vector3& endWorldCoords ) const;
	const RayCast3DResult RayCast( const Vector3& startWorldCoords, const Vector3& endWorldCoords, bool tracePoints, float xRadius = 0.0f, float yRadius = 0.0f, float zRadius = 0.0f );

	bool IsChunkActive( const ChunkCoords& chunkCoords );
	static Block& GetBlock( const BlockData& blockData );

	static BlockData GetBlockData( Chunk* currentChunk, const LocalBlockCoords& requestedBlockCoords );

	int GetOutdoorLightLevelFromTimeOfDay();
	void GetChunksWithGreatestLightingDisparity( Chunks& out_chunksWithGreatestLightingDisparity );
	Chunk* GetChunkToUpdateOutdoorLighting( const Chunks& chunkList );

	bool AreWorldCoordsInWorld( const WorldCoords& worldCoords ) const;
	bool IsBlockWater( const WorldCoords& worldCoords ) const;
	bool IsBlockSolid( const WorldCoords& worldCoords ) const;

	///---------------------------------------------------------------------------------
	/// Update Functions
	///---------------------------------------------------------------------------------
	void ProcessInput( InputSystem* inputSystem );
	void Update( InputSystem* inputSystem, double deltaSeconds, bool debugModeEnabled );
	double MovePlayer( double deltaSeconds );

	void UpdateOutdoorLighting();
	void UpdateChunkOutdoorLighting( Chunk* chunk );

	bool ActivateNearestInactiveChunkInsideVisibilityRadius();
	void ActivateChunk( const ChunkCoords& chunkCoords );
	Chunk* CreateChunkFromFile( const ChunkCoords& chunkCoords );
	Chunk* CreateChunkFromPerlinNoise( const ChunkCoords& chunkCoords );
	bool DeactivateFurthestActiveChunkOutsideVisibilityRadius();
	void DeactivateChunk( const ChunkCoords& chunkCoords );

	void SaveAllActiveChunks();

	void OnChunkActivated( Chunk* chunk );
	void OnChunkDeactivated( Chunk* chunk );

	void UpdateChunkNeighborPointersActivate( Chunk* chunk );
	void UpdateChunkNeighborPointersDeactivate( Chunk* chunk );

	void RemoveBlocksFromDirtyList( Chunk* chunk );

	void UpdateLighting();
	void UpdateLightingForBlock( const BlockData& blockData );
	int CalcIdealLightForBlock( const BlockData& blockData );
	int CalcIdealLightFromNeighbors( const BlockData& blockData );
	int CalcIdealLightFromEastNeighbor( const BlockData& blockData );
	int CalcIdealLightFromNorthNeighbor( const BlockData& blockData );
	int CalcIdealLightFromWestNeighbor( const BlockData& blockData );
	int CalcIdealLightFromSouthNeighbor( const BlockData& blockData );
	int CalcIdealLightFromTopNeighbor( const BlockData& blockData );
	int CalcIdealLightFromBottomNeighbor( const BlockData& blockData );
	int CalcIdealLightFromSky( const BlockData& blockData );
	int CalcIdealLightFromSelf( const BlockData& blockData );

	// WORM GENERATION
	PerlinWormWorldCells GeneratePerlinWormCells( const ChunkCoords& centerPosition );
	PerlinWormParametersList GeneratePerlinWormParameterList2D( const PerlinWormWorldCells& perlinCells, int seed );
	PerlinWormParametersList GeneratePerlinWormParameterList3D( const PerlinWormWorldCells& perlinCells, int seed );
	bool DoesCellContainWormStart( const PerlinWormWorldCell& perlinCell, int seed );
	PerlinWorms GenerateRivers( int seed );
	PerlinWorms GenerateCaves( int seed );
	void GeneratePerlinWorm2D( const Vector3& startPosition, float startYawRadians, int seed, PerlinWorm& out_worm );
	void GeneratePerlinWorm3D( const Vector3& startPosition, float startYawRadians, float startPitch, int seed, PerlinWorm& out_worm );
	void CarveRiverWorms( const PerlinWorms& worms, float percentageOfRadiusToFloor, const ChunkCoords& currentChunkCoords );
	void CarveCaveWorms( const PerlinWorms& worms, float percentageOfRadiusToFloor, const ChunkCoords& currentChunkCoords );

	///---------------------------------------------------------------------------------
	/// Mutators
	///---------------------------------------------------------------------------------
	void DestroyBlockBelow();
	void PlaceBlockBelow();

	void DestroyBlockAtWorldCoords( const WorldCoords& worldCoords, bool playBreakSound = true );
	void PlaceBlockAtSelectedBlockWorldCoordsAndNormal();

	void SetCurrentBlockType( const BlockType& type );

	static void SetBlockDirty( const BlockData& blockData );
	void MarkNSEWNonSkyNeighborsDirty( const BlockData& blockData );
	void MarkNeighborsDirty( const BlockData& blockData );

	void PopulateDebugPoints();

	///---------------------------------------------------------------------------------
	/// Render Functions
	///---------------------------------------------------------------------------------
	void Draw( OpenGLRenderer* renderer, bool debugModeEnabled );
	void DrawTexturedSkybox(OpenGLRenderer* renderer, AnimatedTexture* skyboxTexture, float distanceFromCamera);
	void DrawColoredSkybox(OpenGLRenderer* renderer, float distanceFromCamera, const Rgba& color);
	void DrawSunAndMoon(OpenGLRenderer* renderer, bool debugModeEnabled);
	void DrawBlockOutline( OpenGLRenderer* renderer, bool debugModeEnabled, const WorldBlockCoords& worldCoords, const Rgba& outlineColor );
	void DrawWorm( OpenGLRenderer* renderer, bool debugModeEnabled, const PerlinWorm& worm );

	///---------------------------------------------------------------------------------
	/// Static variables
	///---------------------------------------------------------------------------------
	static BlockDataList s_dirtyBlocks;
	static Vector3s s_debugPoints;	

	static Rgba s_lightLevels[ NUM_LIGHT_LEVELS ];
	static int s_outdoorLightValue;



private:

	///---------------------------------------------------------------------------------
	/// Private member variables
	///---------------------------------------------------------------------------------
	Vector2		m_displaySize;
	Camera3D*	m_camera;
	Player*		m_player;
	WorldCoords m_selectedBlockCoords;
	Vector3		m_selectedFaceNormal;
	ChunkMap	m_activeChunks;
	BlockType	m_currentBlockType;
	BlockType	m_replaceBlockType;
	int			m_seed;

	PerlinWorm m_test2DWorm;
	PerlinWorms m_rivers;
	PerlinWorms m_caves;


	float m_timeOfDayNormalized;

	bool m_debugRaycast;
	bool m_viewFrustumDebug;

	Vector3 m_frustumDebugCameraPos;
	Vector3 m_frustumDebugCameraForward;
	Vector3s m_raycastDebugPoints;
	WorldCoords m_raycastDebugStartPos;
	WorldCoords m_raycastDebugEndPos;
	std::vector< WorldBlockCoords > m_cameraRaycastBlocks;

	UserInterface* m_userInterface;

	AnimatedTexture* m_skyboxTexture;
	AnimatedTexture* m_nightSkyboxTexture;

};

#endif