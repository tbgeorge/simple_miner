//=================================================================================
// Chunk.hpp
// Author: Tyler George
// Date  : March 31, 2015
//=================================================================================

#pragma once
#ifndef __included_Chunk__
#define __included_Chunk__

#define UNUSED(x) (void)(x)

#include <vector>
#include <map>
#include <sstream>
#include <algorithm>
#include "Engine/Math/IntVector3.hpp"
#include "Engine/Renderer/OpenGLRenderer.hpp"
#include "Engine/Math/Noise.hpp"
#include "Engine/Utilities/FileUtilities.hpp"
#include "Engine/Renderer/Vertex3D.hpp"
#include "GameCode/Block.hpp"
#include "GameCode/BlockDefinition.hpp"


///---------------------------------------------------------------------------------
/// Forward Declarations
///---------------------------------------------------------------------------------
class BlockData;


///---------------------------------------------------------------------------------
/// Constants
///---------------------------------------------------------------------------------
const int CHUNK_BLOCKS_X_EXPONENT = 4;
const int CHUNK_BLOCKS_Y_EXPONENT = 4;
const int CHUNK_BLOCKS_Z_EXPONENT = 7;

const int CHUNK_BLOCKS_X = ( 1 << CHUNK_BLOCKS_X_EXPONENT );
const int CHUNK_BLOCKS_Y = ( 1 << CHUNK_BLOCKS_Y_EXPONENT );
const int CHUNK_BLOCKS_Z = ( 1 << CHUNK_BLOCKS_Z_EXPONENT );

const int CHUNK_BLOCKS_PER_LAYER = CHUNK_BLOCKS_X * CHUNK_BLOCKS_Y;
const int NUM_BLOCKS_PER_CHUNK = CHUNK_BLOCKS_X * CHUNK_BLOCKS_Y * CHUNK_BLOCKS_Z;

const int CHUNK_X_MASK = CHUNK_BLOCKS_X - 1;
const int CHUNK_Y_MASK = CHUNK_BLOCKS_Y - 1;
const int CHUNK_Z_MASK = CHUNK_BLOCKS_Z - 1;
const int CHUNK_LAYER_MASK = CHUNK_BLOCKS_PER_LAYER - 1;

const int RLE_BYTE_SIZE = 3; // DO NOT CHANGE
const int MAX_RLE_BYTES = NUM_BLOCKS_PER_CHUNK * RLE_BYTE_SIZE;


///---------------------------------------------------------------------------------
/// Typedefs
///---------------------------------------------------------------------------------
typedef IntVector3 LocalBlockCoords;
typedef IntVector3 WorldBlockCoords;
typedef unsigned short BlockIndex;
typedef Vector3 WorldCoords;
typedef IntVector2 ChunkCoords;


///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
class Chunk
{
public:
	///---------------------------------------------------------------------------------
	/// Constructor/Destructor
	///---------------------------------------------------------------------------------
	Chunk( const ChunkCoords& chunkCoords );
	~Chunk() {}

	///---------------------------------------------------------------------------------
	/// Initialization Functions
	///---------------------------------------------------------------------------------
	void InitializeLighting( int outdoorLightLevel );
	void GenerateFlatChunk( int maxStoneHeightInclusive, int maxDirtHeightInclusive );
	void GenerateStartingBlocks_2DPerlinNoise( int seed = 0 );
	void ReplaceTopLayerWithGrassSandOrWater();

	///---------------------------------------------------------------------------------
	/// Static Accessors/Queries
	///---------------------------------------------------------------------------------
	static LocalBlockCoords GetLocalBlockCoordsFromIndex( const BlockIndex& index );
	static BlockIndex		GetIndexFromLocalBlockCoords( const LocalBlockCoords& blockCoords );
	static ChunkCoords		GetChunkCoordsFromWorldCoords( const WorldCoords& worldCoords );
	static ChunkCoords		GetChunkCoordsFromWorldBlockCoords( const WorldBlockCoords& worldBlockCoords );
	static BlockIndex		GetIndexFromWorldBlockCoords( const WorldBlockCoords& worldBlockCoords );
	static BlockIndex		GetIndexFromWorldCoords( const WorldCoords& worldCoords );
	static WorldCoords		GetChunkWorldMinsFromChunkCoords( const ChunkCoords& chunkCoords );
	static WorldBlockCoords GetWorldBlockCoordsFromWorldCoords( const WorldCoords& worldCoords );
	static WorldCoords		GetWorldCoordMinsFromWorldBlockCoords( const WorldBlockCoords& worldBlockCoords );
	static LocalBlockCoords GetLocalBlockCoordsFromWorldCoords( const WorldCoords& worldCoords );
	static int				GetLocalXCoordFromBlockIndex( const BlockIndex& index );
	static int				GetLocalYCoordFromBlockIndex( const BlockIndex& index );
	static int				GetLocalZCoordFromBlockIndex( const BlockIndex& index );

	///---------------------------------------------------------------------------------
	/// Non-Static Accessors/Queries
	///---------------------------------------------------------------------------------
	WorldCoords GetBlockWorldMinsFromLayerIndex( BlockIndex xyIndex );
	WorldCoords GetWorldCoordsFromBlockIndex( BlockIndex blockIndex );
	WorldCoords GetWorldCoordsFromLocalCoords( const LocalBlockCoords& blockCoords );

	Block& GetBlockAtIndex( BlockIndex blockIndex ) { return m_blocks[ blockIndex ]; }
	int GetOutdoorLightLevel() { return m_chunkOutdoorLightLevel; }

	///---------------------------------------------------------------------------------
	/// Mutators
	///---------------------------------------------------------------------------------
	void SetBlockType( BlockIndex blockIndex, BlockType type );
	void SetVBODirty();
	void SetOutdoorLightLevel( int outdoorLightLevel );


	///---------------------------------------------------------------------------------
	/// Update functions
	///---------------------------------------------------------------------------------
	void Update( double deltaSeconds );
	void UpdateDistanceToCamera( const Vector3& cameraPos );

	///---------------------------------------------------------------------------------
	/// Render functions
	///---------------------------------------------------------------------------------
	bool ShouldRenderChunk( const Vector3& cameraPos, const Vector3& cameraForward );
		
		//GL_BEGIN
	void RenderWithGLBegin( OpenGLRenderer* renderer, bool highlighChunk );
	void DrawBlockAtIndex( BlockIndex index, OpenGLRenderer* renderer );
	bool ShouldRenderFace( const LocalBlockCoords& blockToCheckLocalCoords, Rgba& faceColor );
	bool ShouldRenderFace( const BlockData& blockToCheckData, Rgba& faceColor );

		//GL_VERTEX_ARRAYS
	void RenderWithVAs( OpenGLRenderer* renderer );
	void PopulateVertexArray( PUC_Vertexes& out_vertexArray );
	void AddBlockVertexes( BlockIndex blockIndex, PUC_Vertexes& out_vertexArray );

		//GL_VBOS
	void GenerateVertexArrayAndVBO();
	void RenderWithVBOs( OpenGLRenderer* renderer );
	void DeleteVBO();

	// Translucent faces
	void GenerateAndRenderTranslucentFaceVertexArray( const WorldCoords& cameraPosition );	
	void AddTranslucentVertexes( const BlockData& blockData, PUC_Vertexes& out_translucentFaceVertexes );
	bool ShouldRenderTranslucentFace( const BlockData& blockData, const BlockData& blockToCheckData, Rgba& out_faceColor );
	void RenderTranslucentFaces( const WorldCoords& cameraPosition, OpenGLRenderer* renderer, bool debugModeEnabled );

	
	///---------------------------------------------------------------------------------
	/// Saving/Loading functions
	///---------------------------------------------------------------------------------
	void SaveChunkToDisk() const;
	bool LoadChunkFromDisk();

	///---------------------------------------------------------------------------------
	/// Public memberVariables
	///---------------------------------------------------------------------------------
	ChunkCoords m_chunkCoords;
	Chunk* m_chunkToNorth;
	Chunk* m_chunkToSouth;
	Chunk* m_chunkToEast;
	Chunk* m_chunkToWest;
	float m_distanceToCamera;

private:
	///---------------------------------------------------------------------------------
	/// Private member functions
	///---------------------------------------------------------------------------------
	///---------------------------------------------------------------------------------
	/// Saving/Loading Helper functions
	///---------------------------------------------------------------------------------
	unsigned char*	CreateRLEBuffer( size_t& rleBufferSize ) const;
	std::string		GetFilePathToChunk() const;
	void			AppendRLEEntryToBuffer( const unsigned char& blockType, const unsigned short& blockCount, std::vector< unsigned char >& buffer ) const;
	void			PopulateFromRLEBuffer( unsigned char* buffer, const size_t& bufferSize );

	///---------------------------------------------------------------------------------
	/// Private member variables
	///---------------------------------------------------------------------------------
	Block m_blocks[ NUM_BLOCKS_PER_CHUNK ];
	PUC_Vertexes m_vertexes;
	PUC_Vertexes m_translucentFaceVertexes;

	unsigned int m_vboID;
	unsigned int m_numVertexesInVBO;
	bool m_vboIsDirty;

	int m_chunkOutdoorLightLevel;
};

typedef std::vector< Chunk* > Chunks;
typedef std::map< ChunkCoords, Chunk* > ChunkMap;

///---------------------------------------------------------------------------------
/// Inline functions
///---------------------------------------------------------------------------------
///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline LocalBlockCoords Chunk::GetLocalBlockCoordsFromIndex( const BlockIndex& index )
{
	int x = index & CHUNK_X_MASK;
	int y = ( index & CHUNK_LAYER_MASK ) >> CHUNK_BLOCKS_X_EXPONENT;
	int z = index >> ( CHUNK_BLOCKS_X_EXPONENT + CHUNK_BLOCKS_Y_EXPONENT );
	return LocalBlockCoords( x, y, z );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline BlockIndex Chunk::GetIndexFromLocalBlockCoords( const LocalBlockCoords& blockCoords )
{
	BlockIndex index = (unsigned short) ( blockCoords.x 
				     | ( blockCoords.y << CHUNK_BLOCKS_X_EXPONENT ) 
				     | ( blockCoords.z << ( CHUNK_BLOCKS_X_EXPONENT + CHUNK_BLOCKS_Y_EXPONENT ) ) );
	return index;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline WorldCoords Chunk::GetBlockWorldMinsFromLayerIndex( BlockIndex xyIndex )
{
	int x = xyIndex & CHUNK_X_MASK;
	int y = ( xyIndex & CHUNK_LAYER_MASK ) >> CHUNK_BLOCKS_X_EXPONENT;
	return WorldCoords( (float) ( ( m_chunkCoords.x * CHUNK_BLOCKS_X ) + x )
		              , (float) ( ( m_chunkCoords.y * CHUNK_BLOCKS_Y ) + y )
					  , 0.0f );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline WorldCoords Chunk::GetWorldCoordsFromBlockIndex( BlockIndex blockIndex )
{
	int x = blockIndex & CHUNK_X_MASK;
	int y = ( blockIndex & CHUNK_LAYER_MASK ) >> CHUNK_BLOCKS_X_EXPONENT;
	int z = blockIndex >> ( CHUNK_BLOCKS_X_EXPONENT + CHUNK_BLOCKS_Y_EXPONENT );
	return WorldCoords( (float) ( ( m_chunkCoords.x * CHUNK_BLOCKS_X ) + x )
		              , (float) ( ( m_chunkCoords.y * CHUNK_BLOCKS_Y ) + y )
					  , (float) z );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline BlockIndex Chunk::GetIndexFromWorldBlockCoords( const WorldBlockCoords& worldBlockCoords )
{
	int x = worldBlockCoords.x & CHUNK_X_MASK;
	int y = worldBlockCoords.y & CHUNK_Y_MASK;
	int z = worldBlockCoords.z > CHUNK_BLOCKS_Z - 1 ? CHUNK_BLOCKS_Z - 1 : worldBlockCoords.z;

	return GetIndexFromLocalBlockCoords( LocalBlockCoords( x, y, z ) );


// 	WorldCoords worldCoords = GetWorldCoordMinsFromWorldBlockCoords( worldBlockCoords );
// 	LocalBlockCoords localCoords = GetLocalBlockCoordsFromWorldCoords( worldCoords );
// 	BlockIndex index = GetIndexFromLocalBlockCoords( localCoords );
// 	return index;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline BlockIndex Chunk::GetIndexFromWorldCoords( const WorldCoords& worldCoords )
{
	int x = ( (int) floorf( worldCoords.x ) ) & CHUNK_X_MASK;
	int y = ( (int) floorf( worldCoords.y ) ) & CHUNK_Y_MASK;
	int z = ( (int) floorf( worldCoords.z ) ) > CHUNK_BLOCKS_Z - 1 ? CHUNK_BLOCKS_Z - 1 : (int) floorf( worldCoords.z );

	return GetIndexFromLocalBlockCoords( LocalBlockCoords( x, y, z ) );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline WorldCoords Chunk::GetWorldCoordsFromLocalCoords( const LocalBlockCoords& blockCoords )
{
	return WorldCoords( (float) ( ( m_chunkCoords.x * CHUNK_BLOCKS_X ) + blockCoords.x )
		              , (float) ( ( m_chunkCoords.y * CHUNK_BLOCKS_Y ) + blockCoords.y )
					  , (float) blockCoords.z);
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline WorldBlockCoords Chunk::GetWorldBlockCoordsFromWorldCoords( const WorldCoords& worldCoords )
{
	int x = (int) floorf( worldCoords.x );
	int y = (int) floorf( worldCoords.y );
	int z = (int) floorf( worldCoords.z );

	return WorldBlockCoords( x, y, z );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline WorldCoords Chunk::GetWorldCoordMinsFromWorldBlockCoords( const WorldBlockCoords& worldBlockCoords )
{
	return WorldCoords( (float) worldBlockCoords.x, (float) worldBlockCoords.y, (float) worldBlockCoords.z );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline ChunkCoords Chunk::GetChunkCoordsFromWorldCoords( const WorldCoords& worldCoords )
{
	int x = (int) floorf( worldCoords.x );
	int y = (int) floorf( worldCoords.y );
	int chunkX = x >> 4; 
	int chunkY = y >> 4;

	return ChunkCoords( chunkX, chunkY );
}

inline ChunkCoords Chunk::GetChunkCoordsFromWorldBlockCoords( const WorldBlockCoords& worldBlockCoords )
{
	int chunkX = worldBlockCoords.x >> 4; 
	int chunkY = worldBlockCoords.y >> 4;

	return ChunkCoords( chunkX, chunkY );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline LocalBlockCoords Chunk::GetLocalBlockCoordsFromWorldCoords( const WorldCoords& worldCoords )
{
	int x = (int) floorf( worldCoords.x ) & CHUNK_X_MASK;
	int y = (int) floorf( worldCoords.y ) & CHUNK_Y_MASK;
	int z = worldCoords.z > CHUNK_BLOCKS_Z ? (int)CHUNK_BLOCKS_Z : (int)worldCoords.z;

	return LocalBlockCoords( x, y, z );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline int Chunk::GetLocalXCoordFromBlockIndex( const BlockIndex& index )
{
	return index & CHUNK_X_MASK;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline int Chunk::GetLocalYCoordFromBlockIndex( const BlockIndex& index )
{
	return ( index & CHUNK_LAYER_MASK ) >> CHUNK_BLOCKS_X_EXPONENT;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline int Chunk::GetLocalZCoordFromBlockIndex( const BlockIndex& index )
{
	return index >> ( CHUNK_BLOCKS_X_EXPONENT + CHUNK_BLOCKS_Y_EXPONENT );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline WorldCoords Chunk::GetChunkWorldMinsFromChunkCoords( const ChunkCoords& chunkCoords )
{
	float worldMinX = (float) ( chunkCoords.x << 4 );
	float worldMinY = (float) ( chunkCoords.y << 4 );
	float worldMinZ = 0.0f;

	return WorldCoords( worldMinX, worldMinY, worldMinZ );
}

#endif


