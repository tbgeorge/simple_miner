//=================================================================================
// Chunk.cpp
// Author: Tyler George
// Date  : March 31, 2015
//=================================================================================

#include "GameCode/Chunk.hpp"
#include "GameCode/TheApp.hpp"
#include "Engine/Utilities/Console.hpp"
#include "GameCode/World.hpp"
#include "GameCode/GameCommon.hpp"

///---------------------------------------------------------------------------------
/// Statics 
///---------------------------------------------------------------------------------


///---------------------------------------------------------------------------------
/// Constants
///---------------------------------------------------------------------------------


///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Chunk::Chunk( const ChunkCoords& chunkCoords ) 
	: m_chunkCoords( chunkCoords )
	, m_chunkToNorth( NULL )
	, m_chunkToSouth( NULL )
	, m_chunkToEast( NULL )
	, m_chunkToWest( NULL )
	, m_vboIsDirty( false )
	, m_numVertexesInVBO( 0 )
	, m_vboID( 0 )
	, m_chunkOutdoorLightLevel( 0 )
	, m_distanceToCamera( 0 )
{
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Chunk::InitializeLighting( int outdoorLightLevel )
{
	m_chunkOutdoorLightLevel = outdoorLightLevel;

	for( int x = 0; x < CHUNK_BLOCKS_X; ++x)
	{
		for( int y = 0; y < CHUNK_BLOCKS_Y; ++y )
		{
			for( int z = CHUNK_BLOCKS_Z - 1; z > -1; --z )
			{
				LocalBlockCoords blockCoords = IntVector3( x, y, z );
				BlockIndex blockIndex = GetIndexFromLocalBlockCoords( blockCoords );
				Block& block = m_blocks[ blockIndex ];

				// top block of the column in the chunk
				if( z == CHUNK_BLOCKS_Z - 1 )
				{
					if( !block.IsOpaque() && !block.IsTranslucent() )
						block.MarkAsSky();
				}
				else if( m_blocks[ blockIndex + CHUNK_BLOCKS_PER_LAYER ].IsSky() && !block.IsOpaque() && !block.IsTranslucent() )
					block.MarkAsSky();
				else
					break;
			}
		}
	}

	for( BlockIndex blockIndex = 0; blockIndex < NUM_BLOCKS_PER_CHUNK; ++blockIndex )
	{
		Block& block = m_blocks[ blockIndex ];
		
		LocalBlockCoords blockCoords = GetLocalBlockCoordsFromIndex( blockIndex );
		WorldCoords blockWorldCoords = GetWorldCoordsFromLocalCoords( blockCoords );

		BlockData blockData( this, blockIndex );

		if( block.IsSky() )
		{
			block.SetLightValue( outdoorLightLevel );
			g_theWorld->MarkNSEWNonSkyNeighborsDirty( blockData );
			
			if( ( blockCoords.x == 0 || blockCoords.y == 0 || blockCoords.x == CHUNK_BLOCKS_X - 1 || blockCoords.y == CHUNK_BLOCKS_Y - 1 ) && !block.IsDirty())
			{
				World::SetBlockDirty( blockData );
			}
		}

		else if( block.IsTranslucent() )
			World::SetBlockDirty( blockData );

		else
		{
			int blockLightLevel = block.GetInternalLightValue();
			block.SetLightValue( blockLightLevel );
			if( blockLightLevel != 0 )
			{
				g_theWorld->MarkNeighborsDirty( blockData );
			}
		}
	}
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Chunk::GenerateFlatChunk( int maxStoneHeightInclusive, int maxDirtHeightInclusive )
{
	for( BlockIndex blockIndex = 0; blockIndex < NUM_BLOCKS_PER_CHUNK; ++blockIndex )
	{
		m_blocks[ blockIndex ].ClearAllFlags();

		LocalBlockCoords localBlockCoords = GetLocalBlockCoordsFromIndex( blockIndex );
		if( localBlockCoords.z <= maxStoneHeightInclusive )
			m_blocks[ blockIndex ].m_blockType = (unsigned char) BLOCK_TYPE_STONE;
		else if( localBlockCoords.z > maxStoneHeightInclusive && localBlockCoords.z <= maxDirtHeightInclusive )
			m_blocks[ blockIndex ].m_blockType = (unsigned char) BLOCK_TYPE_DIRT;
		else if( localBlockCoords.z == maxDirtHeightInclusive + 1 )
			m_blocks[ blockIndex ].m_blockType = (unsigned char) BLOCK_TYPE_GRASS;
		else
			m_blocks[ blockIndex ].m_blockType = (unsigned char) BLOCK_TYPE_AIR;
	}
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Chunk::GenerateStartingBlocks_2DPerlinNoise( int seed )
{
	//Compute ground heights
	const float BASE_GROUND_LEVEL = 64.0f;
	float groundHeightForEachColumn[ CHUNK_BLOCKS_PER_LAYER ];
	float dirtThicknessForEachColumn[ CHUNK_BLOCKS_PER_LAYER ];

	for( BlockIndex xyIndex = 0; xyIndex < CHUNK_BLOCKS_PER_LAYER; ++xyIndex )
	{
		WorldCoords blockWorldMins = GetBlockWorldMinsFromLayerIndex( xyIndex );
		float groundHeightPerlinNoiseAtXY = ComputePerlinNoiseValueAtPosition2D( Vector2( blockWorldMins.x, blockWorldMins.y ), 40, 3, 10, 0.5f, seed );
		float dirtThicknessPerlinNoiseAtXY = 15.0f + ComputePerlinNoiseValueAtPosition2D( Vector2( blockWorldMins.x, blockWorldMins.y ), 40, 2, 15, 0.5f, seed );
		groundHeightForEachColumn[ xyIndex ] = BASE_GROUND_LEVEL + groundHeightPerlinNoiseAtXY;
		dirtThicknessForEachColumn[ xyIndex ] = dirtThicknessPerlinNoiseAtXY;
	}

	// for loop through all blocks
	for( BlockIndex blockIndex = 0; blockIndex < NUM_BLOCKS_PER_CHUNK; ++blockIndex )
	{
		m_blocks[ blockIndex ].ClearAllFlags();

		WorldCoords blockCoords = GetWorldCoordsFromBlockIndex( blockIndex );
		BlockIndex xyIndex = blockIndex & CHUNK_LAYER_MASK;

		float groundHeightForColumn = groundHeightForEachColumn[ xyIndex ];
		float dirtThicknessForColumn = dirtThicknessForEachColumn[ xyIndex ];

		BlockType blockType = BLOCK_TYPE_AIR;
		
		if( blockCoords.z < groundHeightForColumn )
			blockType = BLOCK_TYPE_DIRT;
		if( blockCoords.z < groundHeightForColumn - dirtThicknessForColumn )
			blockType = BLOCK_TYPE_STONE;

		m_blocks[ blockIndex ].m_blockType = (unsigned char) blockType;
	}

	ReplaceTopLayerWithGrassSandOrWater();
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Chunk::ReplaceTopLayerWithGrassSandOrWater()
{
	for( BlockIndex blockIndex = 0; blockIndex < NUM_BLOCKS_PER_CHUNK; ++blockIndex )
	{
		LocalBlockCoords blockCoords = GetLocalBlockCoordsFromIndex( blockIndex );
		if( blockCoords.z + 1 == 128 )
			continue;

		BlockIndex index1 = GetIndexFromLocalBlockCoords( IntVector3 ( blockCoords.x, blockCoords.y, blockCoords.z + 1 ) );
		BlockIndex index2 = GetIndexFromLocalBlockCoords( IntVector3 ( blockCoords.x, blockCoords.y, blockCoords.z + 2 ) );


		if( m_blocks[ index1 ].m_blockType == BLOCK_TYPE_AIR )
		{
			if( m_blocks[ blockIndex ].m_blockType == BLOCK_TYPE_DIRT )
			{
				if( blockCoords.z < (int) SEA_LEVEL + 2 )
					m_blocks[ blockIndex ].m_blockType = BLOCK_TYPE_SAND;
				else
					m_blocks[ blockIndex ].m_blockType = BLOCK_TYPE_GRASS;
			}
		}

		if( m_blocks[ blockIndex ].m_blockType == BLOCK_TYPE_AIR && blockCoords.z <= (int) SEA_LEVEL )
			m_blocks[ blockIndex ].m_blockType = BLOCK_TYPE_WATER;

		if( blockCoords.z + 2 == 128 )
			continue;

		if( m_blocks[ index2 ].m_blockType == BLOCK_TYPE_AIR )
		{
			if( m_blocks[ blockIndex ].m_blockType == BLOCK_TYPE_DIRT )
			{
				if( blockCoords.z < (int) SEA_LEVEL + 2 )
					m_blocks[ blockIndex ].m_blockType = BLOCK_TYPE_SAND;
				else
					m_blocks[ blockIndex ].m_blockType = BLOCK_TYPE_GRASS;
			}
		}
	}
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Chunk::SetBlockType( BlockIndex blockIndex, BlockType type )
{
	m_blocks[ blockIndex ].m_blockType = (unsigned char) type;
	m_blocks[ blockIndex ].SetLightValue( m_blocks[ blockIndex ].GetInternalLightValue() );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Chunk::SetVBODirty()
{
	m_vboIsDirty = true;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Chunk::SetOutdoorLightLevel( int outdoorLightLevel )
{
	//ConsolePrintf( "Set outdoor lighting for chunk at %i,%i to %i\n", m_chunkCoords.x, m_chunkCoords.y, outdoorLightLevel );
	m_chunkOutdoorLightLevel = outdoorLightLevel;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Chunk::Update( double deltaSeconds )
{
	UNUSED( deltaSeconds );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Chunk::UpdateDistanceToCamera( const Vector3& cameraPos )
{
	Vector2 cameraPosXY( cameraPos.x, cameraPos.y );
	WorldCoords chunkWorldCoords = GetChunkWorldMinsFromChunkCoords( m_chunkCoords );
	Vector2 chunkPosXY( chunkWorldCoords.x, chunkWorldCoords.y );

	m_distanceToCamera = CalcDistance( cameraPosXY, chunkPosXY );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
bool Chunk::ShouldRenderChunk( const Vector3& cameraPos, const Vector3& cameraForward )
{
	WorldCoords chunkMins = GetChunkWorldMinsFromChunkCoords( m_chunkCoords );
	Vector3s chunkCorners;

	chunkCorners.push_back( chunkMins );
	chunkCorners.push_back( Vector3( chunkMins.x + CHUNK_BLOCKS_X, chunkMins.y, chunkMins.z ) );
	chunkCorners.push_back( Vector3( chunkMins.x, chunkMins.y + CHUNK_BLOCKS_Y, chunkMins.z ) );
	chunkCorners.push_back( Vector3( chunkMins.x + CHUNK_BLOCKS_X, chunkMins.y + CHUNK_BLOCKS_Y, chunkMins.z ) );
	
	chunkCorners.push_back( Vector3( chunkMins.x, chunkMins.y, chunkMins.z + CHUNK_BLOCKS_Z ) );
	chunkCorners.push_back( Vector3( chunkMins.x + CHUNK_BLOCKS_X, chunkMins.y, chunkMins.z + CHUNK_BLOCKS_Z ) );
	chunkCorners.push_back( Vector3( chunkMins.x, chunkMins.y + CHUNK_BLOCKS_Y, chunkMins.z + CHUNK_BLOCKS_Z ) );
	chunkCorners.push_back( Vector3( chunkMins.x + CHUNK_BLOCKS_X, chunkMins.y + CHUNK_BLOCKS_Y, chunkMins.z + CHUNK_BLOCKS_Z ) );

	bool allCornersBehind = true;

	for( Vector3s::const_iterator cornerIter = chunkCorners.begin(); cornerIter != chunkCorners.end(); ++cornerIter )
	{
		Vector3 chunkCorner = ( *cornerIter );
		Vector3 vectorToCornerFromCamera = chunkCorner - cameraPos;

		if( DotProduct( cameraForward, vectorToCornerFromCamera ) >= 0 )
		{
			allCornersBehind = false;
			break;
		}
	}

	if( allCornersBehind )
		return false;
	else
		return true;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Chunk::RenderWithGLBegin( OpenGLRenderer* renderer, bool highlightChunk ) 
{
	renderer->PushMatrix();
	renderer->SetModelViewTranslation( (float) ( m_chunkCoords.x * CHUNK_BLOCKS_X ), (float) ( m_chunkCoords.y * CHUNK_BLOCKS_Y ) );
	renderer->Enable( GL_TEXTURE_2D );
	renderer->Enable( GL_BLEND );
	renderer->BlendFunct( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	renderer->BindTexture( GL_TEXTURE_2D, TheApp::s_textureAtlas->GetPlatformHandle() );

	renderer->SetColor( Rgba() );
	if( highlightChunk )
		renderer->SetColor( Rgba::BLUE );

	renderer->Begin( GL_QUADS );
	for( BlockIndex blockIndex = 0; blockIndex < NUM_BLOCKS_PER_CHUNK; blockIndex++ )
	{
		this->DrawBlockAtIndex( blockIndex, renderer );
	}
	//renderer->Disable( GL_TEXTURE_2D );
	renderer->End();
	renderer->PopMatrix();

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Chunk::DrawBlockAtIndex( BlockIndex blockIndex, OpenGLRenderer* renderer )
{
	//renderer->Enable( GL_TEXTURE_2D );

	Block& block = m_blocks[ blockIndex ];
	BlockType type = (BlockType) block.m_blockType;

	if( type == BLOCK_TYPE_AIR )
		return;


	LocalBlockCoords blockCoords = GetLocalBlockCoordsFromIndex( blockIndex );
	Vector3 blockCoordsf = Vector3( (float) blockCoords.x, (float) blockCoords.y, (float) blockCoords.z );
	
	BlockDefinition& blockDefinition = BlockDefinition::s_blockDefinitions[ type ];

	Vector2 topMinTexCoords = blockDefinition.m_topTexCoordMins;
	Vector2 topMaxTexCoords = topMinTexCoords + Vector2( TheApp::TEX_COORD_SIZE_PER_TILE, TheApp::TEX_COORD_SIZE_PER_TILE );
	Vector2 bottomMinTexCoords = blockDefinition.m_bottomTexCoordMins;
	Vector2 bottomMaxTexCoords = bottomMinTexCoords + Vector2( TheApp::TEX_COORD_SIZE_PER_TILE, TheApp::TEX_COORD_SIZE_PER_TILE );
	Vector2 northMinTexCoords = blockDefinition.m_northTexCoordMins;
	Vector2 northMaxTexCoords = northMinTexCoords + Vector2( TheApp::TEX_COORD_SIZE_PER_TILE, TheApp::TEX_COORD_SIZE_PER_TILE );
	Vector2 southMinTexCoords = blockDefinition.m_southTexCoordMins;
	Vector2 southMaxTexCoords = southMinTexCoords + Vector2( TheApp::TEX_COORD_SIZE_PER_TILE, TheApp::TEX_COORD_SIZE_PER_TILE );
	Vector2 eastMinTexCoords = blockDefinition.m_eastTexCoordMins;
	Vector2 eastMaxTexCoords = eastMinTexCoords + Vector2( TheApp::TEX_COORD_SIZE_PER_TILE, TheApp::TEX_COORD_SIZE_PER_TILE );
	Vector2 westMinTexCoords = blockDefinition.m_westTexCoordMins;
	Vector2 westMaxTexCoords = westMinTexCoords + Vector2( TheApp::TEX_COORD_SIZE_PER_TILE, TheApp::TEX_COORD_SIZE_PER_TILE );


	renderer->SetModelViewTranslation( blockCoordsf.x, blockCoordsf.y, blockCoordsf.z );

	// Top
	Rgba topFaceColor;
	if( ShouldRenderFace( LocalBlockCoords( blockCoords.x, blockCoords.y, blockCoords.z + 1 ), topFaceColor ) )
	{
		renderer->SetColor( blockDefinition.m_topTint - (Rgba::WHITE - topFaceColor ) );
		renderer->TexCoord2f( Vector2( topMinTexCoords.x, topMaxTexCoords.y ) );
		renderer->Vertex3f( Vector3( blockCoordsf.x, blockCoordsf.y, blockCoordsf.z + 1.0f ) );
		renderer->TexCoord2f( Vector2( topMaxTexCoords.x, topMaxTexCoords.y ) );
		renderer->Vertex3f( Vector3( blockCoordsf.x + 1.0f, blockCoordsf.y, blockCoordsf.z + 1.0f ) );
		renderer->TexCoord2f( Vector2( topMaxTexCoords.x, topMinTexCoords.y ) );
		renderer->Vertex3f( Vector3( blockCoordsf.x + 1.0f, blockCoordsf.y + 1.0f, blockCoordsf.z + 1.0f ) );
		renderer->TexCoord2f( Vector2( topMinTexCoords.x, topMinTexCoords.y ) );
		renderer->Vertex3f( Vector3( blockCoordsf.x, blockCoordsf.y+ 1.0f, blockCoordsf.z + 1.0f ) );
	}


	// Bottom
	Rgba bottomFaceColor;
	if( ShouldRenderFace( LocalBlockCoords( blockCoords.x, blockCoords.y, blockCoords.z - 1 ), bottomFaceColor ) )
	{
		renderer->SetColor( blockDefinition.m_bottomTint - (Rgba::WHITE - bottomFaceColor ) );
		renderer->TexCoord2f( Vector2( bottomMinTexCoords.x, bottomMaxTexCoords.y ) );
		renderer->Vertex3f( Vector3( blockCoordsf.x + 1.0f, blockCoordsf.y, blockCoordsf.z ) );
		renderer->TexCoord2f( Vector2( bottomMaxTexCoords.x, bottomMaxTexCoords.y ) );
		renderer->Vertex3f( Vector3( blockCoordsf.x, blockCoordsf.y, blockCoordsf.z ) );
		renderer->TexCoord2f( Vector2( bottomMaxTexCoords.x, bottomMinTexCoords.y ) );
		renderer->Vertex3f( Vector3( blockCoordsf.x, blockCoordsf.y + 1.0f, blockCoordsf.z ) );
		renderer->TexCoord2f( Vector2( bottomMinTexCoords.x, bottomMinTexCoords.y ) );
		renderer->Vertex3f( Vector3( blockCoordsf.x + 1.0f, blockCoordsf.y+ 1.0f, blockCoordsf.z ) );
	}
	
	// North
	Rgba northFaceColor;
	if( ShouldRenderFace( LocalBlockCoords( blockCoords.x, blockCoords.y + 1, blockCoords.z ), northFaceColor ) )
	{
// 		if( ( type == BLOCK_TYPE_WATER && m_blocks[ blockNorth ].m_blockType == BLOCK_TYPE_AIR ) || type != BLOCK_TYPE_WATER )
// 		{
			renderer->SetColor( blockDefinition.m_northTint - (Rgba::WHITE - northFaceColor ) );
			renderer->TexCoord2f( Vector2( northMinTexCoords.x, northMaxTexCoords.y ) );
			renderer->Vertex3f( Vector3( blockCoordsf.x + 1.0f, blockCoordsf.y + 1.0f, blockCoordsf.z ) );
			renderer->TexCoord2f( Vector2( northMaxTexCoords.x, northMaxTexCoords.y ) );
			renderer->Vertex3f( Vector3( blockCoordsf.x, blockCoordsf.y + 1.0f, blockCoordsf.z ) );
			renderer->TexCoord2f( Vector2( northMaxTexCoords.x, northMinTexCoords.y ) );
			renderer->Vertex3f( Vector3( blockCoordsf.x, blockCoordsf.y + 1.0f, blockCoordsf.z + 1.0f ) );
			renderer->TexCoord2f( Vector2( northMinTexCoords.x, northMinTexCoords.y ) );
			renderer->Vertex3f( Vector3( blockCoordsf.x + 1.0f, blockCoordsf.y+ 1.0f, blockCoordsf.z + 1.0f ) );
		//}
	}

	// South
	Rgba southFaceColor;
	if( ShouldRenderFace( LocalBlockCoords( blockCoords.x, blockCoords.y - 1, blockCoords.z ), southFaceColor ) )
	{
// 		if( ( type == BLOCK_TYPE_WATER && m_blocks[ blockSouth ].m_blockType == BLOCK_TYPE_AIR ) || type != BLOCK_TYPE_WATER )
// 		{
			renderer->SetColor( blockDefinition.m_southTint - (Rgba::WHITE - southFaceColor ) );
			renderer->TexCoord2f( Vector2( southMinTexCoords.x, southMaxTexCoords.y ) );
			renderer->Vertex3f( Vector3( blockCoordsf.x, blockCoordsf.y, blockCoordsf.z ) );
			renderer->TexCoord2f( Vector2( southMaxTexCoords.x, southMaxTexCoords.y ) );
			renderer->Vertex3f( Vector3( blockCoordsf.x + 1.0f, blockCoordsf.y, blockCoordsf.z ) );
			renderer->TexCoord2f( Vector2( southMaxTexCoords.x, southMinTexCoords.y ) );
			renderer->Vertex3f( Vector3( blockCoordsf.x + 1.0f, blockCoordsf.y, blockCoordsf.z + 1.0f ) );
			renderer->TexCoord2f( Vector2( southMinTexCoords.x, southMinTexCoords.y ) );
			renderer->Vertex3f( Vector3( blockCoordsf.x, blockCoordsf.y, blockCoordsf.z + 1.0f ) );
		//}
	}

	// East
	Rgba eastFaceColor;
	if( ShouldRenderFace( LocalBlockCoords( blockCoords.x + 1, blockCoords.y, blockCoords.z ), eastFaceColor ) )
	{
// 		if( ( type == BLOCK_TYPE_WATER && m_blocks[ blockEast ].m_blockType == BLOCK_TYPE_AIR ) || type != BLOCK_TYPE_WATER )
// 		{
			renderer->SetColor( blockDefinition.m_eastTint - (Rgba::WHITE - eastFaceColor ) );
			renderer->TexCoord2f( Vector2( eastMinTexCoords.x, eastMaxTexCoords.y ) );
			renderer->Vertex3f( Vector3( blockCoordsf.x + 1.0f, blockCoordsf.y, blockCoordsf.z ) );
			renderer->TexCoord2f( Vector2( eastMaxTexCoords.x, eastMaxTexCoords.y ) );
			renderer->Vertex3f( Vector3( blockCoordsf.x + 1.0f, blockCoordsf.y + 1.0f, blockCoordsf.z ) );
			renderer->TexCoord2f( Vector2( eastMaxTexCoords.x, eastMinTexCoords.y ) );
			renderer->Vertex3f( Vector3( blockCoordsf.x + 1.0f, blockCoordsf.y + 1.0f, blockCoordsf.z + 1.0f ) );
			renderer->TexCoord2f( Vector2( eastMinTexCoords.x, eastMinTexCoords.y ) );
			renderer->Vertex3f( Vector3( blockCoordsf.x + 1.0f, blockCoordsf.y, blockCoordsf.z + 1.0f ) );
		//}
	}

	// West
	Rgba westFaceColor;
	if( ShouldRenderFace( LocalBlockCoords( blockCoords.x - 1, blockCoords.y, blockCoords.z ), westFaceColor ) )
	{
// 		if( ( type == BLOCK_TYPE_WATER && m_blocks[ blockWest ].m_blockType == BLOCK_TYPE_AIR ) || type != BLOCK_TYPE_WATER )
// 		{
			renderer->SetColor( blockDefinition.m_westTint - (Rgba::WHITE - westFaceColor ) );
			renderer->TexCoord2f( Vector2( westMinTexCoords.x, westMaxTexCoords.y ) );
			renderer->Vertex3f( Vector3( blockCoordsf.x, blockCoordsf.y + 1.0f, blockCoordsf.z ) );
			renderer->TexCoord2f( Vector2( westMaxTexCoords.x, westMaxTexCoords.y ) );
			renderer->Vertex3f( Vector3( blockCoordsf.x, blockCoordsf.y, blockCoordsf.z ) );
			renderer->TexCoord2f( Vector2( westMaxTexCoords.x, westMinTexCoords.y ) );
			renderer->Vertex3f( Vector3( blockCoordsf.x, blockCoordsf.y, blockCoordsf.z + 1.0f ) );
			renderer->TexCoord2f( Vector2( westMinTexCoords.x, westMinTexCoords.y ) );
			renderer->Vertex3f( Vector3( blockCoordsf.x, blockCoordsf.y+ 1.0f, blockCoordsf.z + 1.0f ) );
		//}
	}
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
bool Chunk::ShouldRenderFace( const LocalBlockCoords& blockToCheckLocalCoords, Rgba& faceColor )
{
	//BlockData blockToCheckData;

	if( blockToCheckLocalCoords.z == CHUNK_BLOCKS_Z )
	{
		faceColor = World::s_lightLevels[ OUTDOOR_LIGHT_VALUE ];
		return true;
	}
	
	BlockData blockToCheckData = World::GetBlockData( this, blockToCheckLocalCoords );

	if( !blockToCheckData.IsValid() )
		return false;
	
	else
	{
		Block& block = blockToCheckData.GetBlock();

		if( block.IsOpaque() )
			return false;

		faceColor = World::s_lightLevels[ block.GetLightValue() ];
		return true;
	}
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
bool Chunk::ShouldRenderFace( const BlockData& blockToCheckData, Rgba& faceColor )
{
	//BlockData blockToCheckData;
	LocalBlockCoords localBlockCoords = blockToCheckData.m_chunk->GetLocalBlockCoordsFromIndex( blockToCheckData.m_blockIndex );

	if( !blockToCheckData.IsValid() )
		return false;

	if( localBlockCoords.z == CHUNK_BLOCKS_Z )
	{
		faceColor = World::s_lightLevels[ OUTDOOR_LIGHT_VALUE ];
		return true;
	}

	else
	{
		Block& block = blockToCheckData.GetBlock();

		if( block.IsOpaque() )
			return false;

		faceColor = World::s_lightLevels[ block.GetLightValue() ];
		return true;
	}
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Chunk::RenderWithVAs( OpenGLRenderer* renderer )
{
	
	PopulateVertexArray( m_vertexes );

	renderer->Enable( GL_TEXTURE_2D );
	renderer->Enable( GL_BLEND );
	renderer->BlendFunct( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	renderer->BindTexture( GL_TEXTURE_2D, TheApp::s_textureAtlas->GetPlatformHandle() );

	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_COLOR_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );

	glVertexPointer(	3, GL_FLOAT, sizeof( Vertex3D_PUC ), &m_vertexes[0].position );	
	glColorPointer(		4, GL_UNSIGNED_BYTE, sizeof( Vertex3D_PUC ), &m_vertexes[0].color );	
	glTexCoordPointer(	2, GL_FLOAT, sizeof( Vertex3D_PUC ), &m_vertexes[0].uv );	

	glDrawArrays( GL_QUADS, 0, m_vertexes.size() );

	renderer->Disable( GL_TEXTURE_2D );
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_COLOR_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Chunk::PopulateVertexArray( PUC_Vertexes& out_vertexArray )
{
	out_vertexArray.clear();
	out_vertexArray.reserve( 10000 );

	for( BlockIndex blockIndex = 0; blockIndex < NUM_BLOCKS_PER_CHUNK; ++ blockIndex )
	{
		Block& block = m_blocks[ blockIndex ];
		if( block.IsVisible() && !block.IsTranslucent() )
		{
			AddBlockVertexes( blockIndex, out_vertexArray ); // Does push_back() up to 24 times (4 times per face)
		}
	}
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Chunk::AddBlockVertexes( BlockIndex blockIndex, PUC_Vertexes& out_vertexArray )
{
	BlockData blockData( this, blockIndex );

	if( blockData.GetBlock().m_blockType == BLOCK_TYPE_AIR )
		return;


	WorldCoords blockCoords = GetWorldCoordsFromBlockIndex( blockIndex );

	BlockDefinition& blockDefinition = BlockDefinition::s_blockDefinitions[ blockData.GetBlock().m_blockType ];

	Vector2 topMinTexCoords = blockDefinition.m_topTexCoordMins;
	Vector2 topMaxTexCoords = topMinTexCoords + Vector2( TheApp::TEX_COORD_SIZE_PER_TILE, TheApp::TEX_COORD_SIZE_PER_TILE );
	Vector2 bottomMinTexCoords = blockDefinition.m_bottomTexCoordMins;
	Vector2 bottomMaxTexCoords = bottomMinTexCoords + Vector2( TheApp::TEX_COORD_SIZE_PER_TILE, TheApp::TEX_COORD_SIZE_PER_TILE );
	Vector2 northMinTexCoords = blockDefinition.m_northTexCoordMins;
	Vector2 northMaxTexCoords = northMinTexCoords + Vector2( TheApp::TEX_COORD_SIZE_PER_TILE, TheApp::TEX_COORD_SIZE_PER_TILE );
	Vector2 southMinTexCoords = blockDefinition.m_southTexCoordMins;
	Vector2 southMaxTexCoords = southMinTexCoords + Vector2( TheApp::TEX_COORD_SIZE_PER_TILE, TheApp::TEX_COORD_SIZE_PER_TILE );
	Vector2 eastMinTexCoords = blockDefinition.m_eastTexCoordMins;
	Vector2 eastMaxTexCoords = eastMinTexCoords + Vector2( TheApp::TEX_COORD_SIZE_PER_TILE, TheApp::TEX_COORD_SIZE_PER_TILE );
	Vector2 westMinTexCoords = blockDefinition.m_westTexCoordMins;
	Vector2 westMaxTexCoords = westMinTexCoords + Vector2( TheApp::TEX_COORD_SIZE_PER_TILE, TheApp::TEX_COORD_SIZE_PER_TILE );


	Vertex3D_PUC vertex;

	// Top
	Rgba topFaceColor;
	if( ShouldRenderFace( blockData.GetTopNeighbor(), topFaceColor ) )
	{
		vertex.color = blockDefinition.m_topTint - (Rgba::WHITE - topFaceColor );

		vertex.uv = Vector2( topMinTexCoords.x, topMaxTexCoords.y );
		vertex.position = Vector3( blockCoords.x, blockCoords.y, blockCoords.z + 1.0f );
		out_vertexArray.push_back( vertex );

		vertex.uv = Vector2( topMaxTexCoords.x, topMaxTexCoords.y );
		vertex.position = Vector3( blockCoords.x + 1.0f, blockCoords.y, blockCoords.z + 1.0f );
		out_vertexArray.push_back( vertex );

		vertex.uv = Vector2( topMaxTexCoords.x, topMinTexCoords.y );
		vertex.position = Vector3( blockCoords.x + 1.0f, blockCoords.y + 1.0f, blockCoords.z + 1.0f );
		out_vertexArray.push_back( vertex );

		vertex.uv = Vector2( topMinTexCoords.x, topMinTexCoords.y );
		vertex.position = Vector3( blockCoords.x, blockCoords.y + 1.0f, blockCoords.z + 1.0f );
		out_vertexArray.push_back( vertex );
	}

	// Bottom
	Rgba bottomFaceColor;
	if( ShouldRenderFace( blockData.GetBottomNeighbor(), bottomFaceColor ) )
	{
		vertex.color = blockDefinition.m_bottomTint - (Rgba::WHITE - bottomFaceColor );

		vertex.uv = Vector2( bottomMinTexCoords.x, bottomMaxTexCoords.y );
		vertex.position = Vector3( blockCoords.x + 1.0f, blockCoords.y, blockCoords.z );
		out_vertexArray.push_back( vertex );

		vertex.uv = Vector2( bottomMaxTexCoords.x, bottomMaxTexCoords.y );
		vertex.position = Vector3( blockCoords.x, blockCoords.y, blockCoords.z );
		out_vertexArray.push_back( vertex );

		vertex.uv = Vector2( bottomMaxTexCoords.x, bottomMinTexCoords.y );
		vertex.position = Vector3( blockCoords.x, blockCoords.y + 1.0f, blockCoords.z );
		out_vertexArray.push_back( vertex );

		vertex.uv = Vector2( bottomMinTexCoords.x, bottomMinTexCoords.y );
		vertex.position = Vector3( blockCoords.x + 1.0f, blockCoords.y + 1.0f, blockCoords.z );
		out_vertexArray.push_back( vertex );
	}

	// North
	Rgba northFaceColor;
	if( ShouldRenderFace( blockData.GetNorthNeighbor(), northFaceColor ) )
	{
		vertex.color = blockDefinition.m_northTint - (Rgba::WHITE - northFaceColor );

		vertex.uv = Vector2( northMinTexCoords.x, northMaxTexCoords.y );
		vertex.position = Vector3( blockCoords.x + 1.0f, blockCoords.y + 1.0f, blockCoords.z );
		out_vertexArray.push_back( vertex );

		vertex.uv = Vector2( northMaxTexCoords.x, northMaxTexCoords.y );
		vertex.position = Vector3( blockCoords.x, blockCoords.y + 1.0f, blockCoords.z );
		out_vertexArray.push_back( vertex );

		vertex.uv = Vector2( northMaxTexCoords.x, northMinTexCoords.y );
		vertex.position = Vector3( blockCoords.x, blockCoords.y + 1.0f, blockCoords.z + 1.0f );
		out_vertexArray.push_back( vertex );

		vertex.uv = Vector2( northMinTexCoords.x, northMinTexCoords.y );
		vertex.position = Vector3( blockCoords.x + 1.0f, blockCoords.y + 1.0f, blockCoords.z + 1.0f );
		out_vertexArray.push_back( vertex );
	}

	// South
	Rgba southFaceColor;
	if( ShouldRenderFace( blockData.GetSouthNeighbor(), southFaceColor ) )
	{
		vertex.color = blockDefinition.m_southTint - (Rgba::WHITE - southFaceColor );

		vertex.uv = Vector2( southMinTexCoords.x, southMaxTexCoords.y );
		vertex.position = Vector3( blockCoords.x, blockCoords.y, blockCoords.z );
		out_vertexArray.push_back( vertex );

		vertex.uv = Vector2( southMaxTexCoords.x, southMaxTexCoords.y );
		vertex.position = Vector3( blockCoords.x + 1.0f, blockCoords.y, blockCoords.z );
		out_vertexArray.push_back( vertex );

		vertex.uv = Vector2( southMaxTexCoords.x, southMinTexCoords.y );
		vertex.position = Vector3( blockCoords.x + 1.0f, blockCoords.y, blockCoords.z + 1.0f );
		out_vertexArray.push_back( vertex );

		vertex.uv = Vector2( southMinTexCoords.x, southMinTexCoords.y );
		vertex.position = Vector3( blockCoords.x, blockCoords.y, blockCoords.z + 1.0f );
		out_vertexArray.push_back( vertex );
	}

	// East
	Rgba eastFaceColor;
	if( ShouldRenderFace( blockData.GetEastNeighbor(), eastFaceColor ) )
	{
		vertex.color = blockDefinition.m_eastTint - (Rgba::WHITE - eastFaceColor );

		vertex.uv = Vector2( eastMinTexCoords.x, eastMaxTexCoords.y );
		vertex.position = Vector3( blockCoords.x + 1.0f, blockCoords.y, blockCoords.z );
		out_vertexArray.push_back( vertex );

		vertex.uv = Vector2( eastMaxTexCoords.x, eastMaxTexCoords.y );
		vertex.position = Vector3( blockCoords.x + 1.0f, blockCoords.y + 1.0f, blockCoords.z );
		out_vertexArray.push_back( vertex );

		vertex.uv = Vector2( eastMaxTexCoords.x, eastMinTexCoords.y );
		vertex.position = Vector3( blockCoords.x + 1.0f, blockCoords.y + 1.0f, blockCoords.z + 1.0f );
		out_vertexArray.push_back( vertex );

		vertex.uv = Vector2( eastMinTexCoords.x, eastMinTexCoords.y );
		vertex.position = Vector3( blockCoords.x + 1.0f, blockCoords.y, blockCoords.z + 1.0f );
		out_vertexArray.push_back( vertex );
	}

	// West
	Rgba westFaceColor;
	if( ShouldRenderFace( blockData.GetWestNeighbor(), westFaceColor ) )
	{
		vertex.color = blockDefinition.m_westTint - (Rgba::WHITE - westFaceColor );

		vertex.uv = Vector2( westMinTexCoords.x, westMaxTexCoords.y );
		vertex.position = Vector3( blockCoords.x, blockCoords.y + 1.0f, blockCoords.z );
		out_vertexArray.push_back( vertex );

		vertex.uv = Vector2( westMaxTexCoords.x, westMaxTexCoords.y );
		vertex.position = Vector3( blockCoords.x, blockCoords.y, blockCoords.z );
		out_vertexArray.push_back( vertex );

		vertex.uv = Vector2( westMaxTexCoords.x, westMinTexCoords.y );
		vertex.position = Vector3( blockCoords.x, blockCoords.y, blockCoords.z + 1.0f );
		out_vertexArray.push_back( vertex );

		vertex.uv = Vector2( westMinTexCoords.x, westMinTexCoords.y );
		vertex.position = Vector3( blockCoords.x, blockCoords.y + 1.0f, blockCoords.z + 1.0f );
		out_vertexArray.push_back( vertex );
	}
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Chunk::GenerateVertexArrayAndVBO()
{
	PUC_Vertexes vertexArray;
	PopulateVertexArray( vertexArray );
	m_numVertexesInVBO = vertexArray.size();

	if( m_vboID == 0 )
	{
		glGenBuffers( 1, &m_vboID );
	}

	size_t vertexArrayNumBytes = sizeof( Vertex3D_PUC ) * vertexArray.size();
	glBindBuffer( GL_ARRAY_BUFFER, m_vboID );
	glBufferData( GL_ARRAY_BUFFER, vertexArrayNumBytes, vertexArray.data(), GL_DYNAMIC_DRAW );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	m_vboIsDirty = false;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Chunk::RenderWithVBOs( OpenGLRenderer* renderer )
{
	if( m_vboIsDirty )
		GenerateVertexArrayAndVBO();


	glPushMatrix();
	//glTranslatef( m_chunkCoords.x, m_chunkCoords.y, 0.0f );
	glBindBuffer( GL_ARRAY_BUFFER, m_vboID );

	renderer->Enable( GL_TEXTURE_2D );
	renderer->Enable( GL_BLEND );
	renderer->BlendFunct( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	renderer->BindTexture( GL_TEXTURE_2D, TheApp::s_textureAtlas->GetPlatformHandle() );

	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_COLOR_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );

	glVertexPointer(	3, GL_FLOAT, sizeof( Vertex3D_PUC ), (const GLvoid*) offsetof( Vertex3D_PUC, position ) );	
	glColorPointer(		4, GL_UNSIGNED_BYTE, sizeof( Vertex3D_PUC ), (const GLvoid*) offsetof( Vertex3D_PUC, color ) );	
	glTexCoordPointer(	2, GL_FLOAT, sizeof( Vertex3D_PUC ), (const GLvoid*) offsetof( Vertex3D_PUC, uv ) );	

	glDrawArrays( GL_QUADS, 0, m_numVertexesInVBO );

	renderer->Disable( GL_TEXTURE_2D );
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_COLOR_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );

	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glPopMatrix();
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Chunk::DeleteVBO()
{
	glDeleteBuffers( 1, &m_vboID );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
bool TranslucentBlockSortingFunction( const BlockData& i, const BlockData& j )
{ 
	return ( i.m_distanceToCamera > j.m_distanceToCamera ); 
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Chunk::GenerateAndRenderTranslucentFaceVertexArray( const WorldCoords& cameraPosition )
{
	m_translucentFaceVertexes.clear();
	m_translucentFaceVertexes.reserve( 5000 );

	BlockDataList translucentBlockList;

	for( BlockIndex blockIndex = 0; blockIndex < NUM_BLOCKS_PER_CHUNK; ++blockIndex )
	{
		Block& block = GetBlockAtIndex( blockIndex );
		if( block.IsTranslucent() )
		{
			BlockData blockData( this, blockIndex );
			blockData.m_distanceToCamera = CalcDistance( blockData.GetWorldCoords(), cameraPosition );

			translucentBlockList.push_back( blockData );
		}
	}

	std::sort( translucentBlockList.begin(), translucentBlockList.end(), TranslucentBlockSortingFunction );

	for( BlockDataList::const_iterator blockDataIter = translucentBlockList.begin(); blockDataIter != translucentBlockList.end(); ++blockDataIter )
	{
		BlockData blockData = ( *blockDataIter );
		AddTranslucentVertexes( blockData, m_translucentFaceVertexes );
	}
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Chunk::AddTranslucentVertexes(const BlockData& blockData, PUC_Vertexes& out_translucentFaceVertexes)
{
	WorldCoords blockCoords = blockData.GetWorldCoords();

	BlockDefinition& blockDefinition = BlockDefinition::s_blockDefinitions[ blockData.GetBlock().m_blockType ];

	Vector2 topMinTexCoords = blockDefinition.m_topTexCoordMins;
	Vector2 topMaxTexCoords = topMinTexCoords + Vector2( TheApp::TEX_COORD_SIZE_PER_TILE, TheApp::TEX_COORD_SIZE_PER_TILE );
	Vector2 bottomMinTexCoords = blockDefinition.m_bottomTexCoordMins;
	Vector2 bottomMaxTexCoords = bottomMinTexCoords + Vector2( TheApp::TEX_COORD_SIZE_PER_TILE, TheApp::TEX_COORD_SIZE_PER_TILE );
	Vector2 northMinTexCoords = blockDefinition.m_northTexCoordMins;
	Vector2 northMaxTexCoords = northMinTexCoords + Vector2( TheApp::TEX_COORD_SIZE_PER_TILE, TheApp::TEX_COORD_SIZE_PER_TILE );
	Vector2 southMinTexCoords = blockDefinition.m_southTexCoordMins;
	Vector2 southMaxTexCoords = southMinTexCoords + Vector2( TheApp::TEX_COORD_SIZE_PER_TILE, TheApp::TEX_COORD_SIZE_PER_TILE );
	Vector2 eastMinTexCoords = blockDefinition.m_eastTexCoordMins;
	Vector2 eastMaxTexCoords = eastMinTexCoords + Vector2( TheApp::TEX_COORD_SIZE_PER_TILE, TheApp::TEX_COORD_SIZE_PER_TILE );
	Vector2 westMinTexCoords = blockDefinition.m_westTexCoordMins;
	Vector2 westMaxTexCoords = westMinTexCoords + Vector2( TheApp::TEX_COORD_SIZE_PER_TILE, TheApp::TEX_COORD_SIZE_PER_TILE );


	Vertex3D_PUC vertex;

	// Top
	Rgba topFaceColor;
	if( ShouldRenderTranslucentFace( blockData, blockData.GetTopNeighbor(), topFaceColor ) )
	{
		vertex.color = blockDefinition.m_topTint - (Rgba::WHITE - topFaceColor );

		vertex.uv = Vector2( topMinTexCoords.x, topMaxTexCoords.y );
		vertex.position = Vector3( blockCoords.x, blockCoords.y, blockCoords.z + 1.0f );
		out_translucentFaceVertexes.push_back( vertex );

		vertex.uv = Vector2( topMaxTexCoords.x, topMaxTexCoords.y );
		vertex.position = Vector3( blockCoords.x + 1.0f, blockCoords.y, blockCoords.z + 1.0f );
		out_translucentFaceVertexes.push_back( vertex );

		vertex.uv = Vector2( topMaxTexCoords.x, topMinTexCoords.y );
		vertex.position = Vector3( blockCoords.x + 1.0f, blockCoords.y + 1.0f, blockCoords.z + 1.0f );
		out_translucentFaceVertexes.push_back( vertex );

		vertex.uv = Vector2( topMinTexCoords.x, topMinTexCoords.y );
		vertex.position = Vector3( blockCoords.x, blockCoords.y + 1.0f, blockCoords.z + 1.0f );
		out_translucentFaceVertexes.push_back( vertex );
	}

	// Bottom
	Rgba bottomFaceColor;
	if( ShouldRenderTranslucentFace( blockData, blockData.GetBottomNeighbor(), bottomFaceColor ) )
	{
		vertex.color = blockDefinition.m_bottomTint - (Rgba::WHITE - bottomFaceColor );

		vertex.uv = Vector2( bottomMinTexCoords.x, bottomMaxTexCoords.y );
		vertex.position = Vector3( blockCoords.x + 1.0f, blockCoords.y, blockCoords.z );
		out_translucentFaceVertexes.push_back( vertex );

		vertex.uv = Vector2( bottomMaxTexCoords.x, bottomMaxTexCoords.y );
		vertex.position = Vector3( blockCoords.x, blockCoords.y, blockCoords.z );
		out_translucentFaceVertexes.push_back( vertex );

		vertex.uv = Vector2( bottomMaxTexCoords.x, bottomMinTexCoords.y );
		vertex.position = Vector3( blockCoords.x, blockCoords.y + 1.0f, blockCoords.z );
		out_translucentFaceVertexes.push_back( vertex );

		vertex.uv = Vector2( bottomMinTexCoords.x, bottomMinTexCoords.y );
		vertex.position = Vector3( blockCoords.x + 1.0f, blockCoords.y + 1.0f, blockCoords.z );
		out_translucentFaceVertexes.push_back( vertex );
	}

	// North
	Rgba northFaceColor;
	if( ShouldRenderTranslucentFace( blockData, blockData.GetNorthNeighbor(), northFaceColor ) )
	{
		vertex.color = blockDefinition.m_northTint - (Rgba::WHITE - northFaceColor );

		vertex.uv = Vector2( northMinTexCoords.x, northMaxTexCoords.y );
		vertex.position = Vector3( blockCoords.x + 1.0f, blockCoords.y + 1.0f, blockCoords.z );
		out_translucentFaceVertexes.push_back( vertex );

		vertex.uv = Vector2( northMaxTexCoords.x, northMaxTexCoords.y );
		vertex.position = Vector3( blockCoords.x, blockCoords.y + 1.0f, blockCoords.z );
		out_translucentFaceVertexes.push_back( vertex );

		vertex.uv = Vector2( northMaxTexCoords.x, northMinTexCoords.y );
		vertex.position = Vector3( blockCoords.x, blockCoords.y + 1.0f, blockCoords.z + 1.0f );
		out_translucentFaceVertexes.push_back( vertex );

		vertex.uv = Vector2( northMinTexCoords.x, northMinTexCoords.y );
		vertex.position = Vector3( blockCoords.x + 1.0f, blockCoords.y + 1.0f, blockCoords.z + 1.0f );
		out_translucentFaceVertexes.push_back( vertex );
	}

	// South
	Rgba southFaceColor;
	if( ShouldRenderTranslucentFace( blockData, blockData.GetSouthNeighbor(), southFaceColor ) )
	{
		vertex.color = blockDefinition.m_southTint - (Rgba::WHITE - southFaceColor );

		vertex.uv = Vector2( southMinTexCoords.x, southMaxTexCoords.y );
		vertex.position = Vector3( blockCoords.x, blockCoords.y, blockCoords.z );
		out_translucentFaceVertexes.push_back( vertex );

		vertex.uv = Vector2( southMaxTexCoords.x, southMaxTexCoords.y );
		vertex.position = Vector3( blockCoords.x + 1.0f, blockCoords.y, blockCoords.z );
		out_translucentFaceVertexes.push_back( vertex );

		vertex.uv = Vector2( southMaxTexCoords.x, southMinTexCoords.y );
		vertex.position = Vector3( blockCoords.x + 1.0f, blockCoords.y, blockCoords.z + 1.0f );
		out_translucentFaceVertexes.push_back( vertex );

		vertex.uv = Vector2( southMinTexCoords.x, southMinTexCoords.y );
		vertex.position = Vector3( blockCoords.x, blockCoords.y, blockCoords.z + 1.0f );
		out_translucentFaceVertexes.push_back( vertex );
	}

	// East
	Rgba eastFaceColor;
	if( ShouldRenderTranslucentFace( blockData, blockData.GetEastNeighbor(), eastFaceColor ) )
	{
		vertex.color = blockDefinition.m_eastTint - (Rgba::WHITE - eastFaceColor );

		vertex.uv = Vector2( eastMinTexCoords.x, eastMaxTexCoords.y );
		vertex.position = Vector3( blockCoords.x + 1.0f, blockCoords.y, blockCoords.z );
		out_translucentFaceVertexes.push_back( vertex );

		vertex.uv = Vector2( eastMaxTexCoords.x, eastMaxTexCoords.y );
		vertex.position = Vector3( blockCoords.x + 1.0f, blockCoords.y + 1.0f, blockCoords.z );
		out_translucentFaceVertexes.push_back( vertex );

		vertex.uv = Vector2( eastMaxTexCoords.x, eastMinTexCoords.y );
		vertex.position = Vector3( blockCoords.x + 1.0f, blockCoords.y + 1.0f, blockCoords.z + 1.0f );
		out_translucentFaceVertexes.push_back( vertex );

		vertex.uv = Vector2( eastMinTexCoords.x, eastMinTexCoords.y );
		vertex.position = Vector3( blockCoords.x + 1.0f, blockCoords.y, blockCoords.z + 1.0f );
		out_translucentFaceVertexes.push_back( vertex );
	}

	// West
	Rgba westFaceColor;
	if( ShouldRenderTranslucentFace( blockData, blockData.GetWestNeighbor(), westFaceColor ) )
	{
		vertex.color = blockDefinition.m_westTint - (Rgba::WHITE - westFaceColor );

		vertex.uv = Vector2( westMinTexCoords.x, westMaxTexCoords.y );
		vertex.position = Vector3( blockCoords.x, blockCoords.y + 1.0f, blockCoords.z );
		out_translucentFaceVertexes.push_back( vertex );

		vertex.uv = Vector2( westMaxTexCoords.x, westMaxTexCoords.y );
		vertex.position = Vector3( blockCoords.x, blockCoords.y, blockCoords.z );
		out_translucentFaceVertexes.push_back( vertex );

		vertex.uv = Vector2( westMaxTexCoords.x, westMinTexCoords.y );
		vertex.position = Vector3( blockCoords.x, blockCoords.y, blockCoords.z + 1.0f );
		out_translucentFaceVertexes.push_back( vertex );

		vertex.uv = Vector2( westMinTexCoords.x, westMinTexCoords.y );
		vertex.position = Vector3( blockCoords.x, blockCoords.y + 1.0f, blockCoords.z + 1.0f );
		out_translucentFaceVertexes.push_back( vertex );
	}
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
bool Chunk::ShouldRenderTranslucentFace( const BlockData& blockData, const BlockData& blockToCheckData, Rgba& out_faceColor )
{
	LocalBlockCoords localBlockCoords = blockToCheckData.m_chunk->GetLocalBlockCoordsFromIndex( blockToCheckData.m_blockIndex );

	if( !blockToCheckData.IsValid() )
		return false;

	if( localBlockCoords.z == CHUNK_BLOCKS_Z )
	{
		out_faceColor = World::s_lightLevels[ World::s_outdoorLightValue ];
		return true;
	}

	else
	{
		Block& blockToCheck = blockToCheckData.GetBlock();

		if( blockToCheck.IsOpaque() )
			return false;

		if( blockToCheck.m_blockType == blockData.GetBlock().m_blockType )
			return false;

		out_faceColor = World::s_lightLevels[ blockToCheck.GetLightValue() ];
		return true;
	}
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Chunk::RenderTranslucentFaces( const WorldCoords& cameraPosition, OpenGLRenderer* renderer, bool debugModeEnabled )
{
	UNUSED( debugModeEnabled );
	GenerateAndRenderTranslucentFaceVertexArray( cameraPosition );


	if( m_translucentFaceVertexes.size() == 0 )
		return;

	glDepthMask( GL_FALSE );
	renderer->Enable( GL_TEXTURE_2D );
	renderer->Enable( GL_BLEND );
	renderer->BlendFunct( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	renderer->BindTexture( GL_TEXTURE_2D, TheApp::s_textureAtlas->GetPlatformHandle() );

	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_COLOR_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );

	glVertexPointer(	3, GL_FLOAT, sizeof( Vertex3D_PUC ), &m_translucentFaceVertexes[0].position );	
	glColorPointer(		4, GL_UNSIGNED_BYTE, sizeof( Vertex3D_PUC ), &m_translucentFaceVertexes[0].color );	
	glTexCoordPointer(	2, GL_FLOAT, sizeof( Vertex3D_PUC ), &m_translucentFaceVertexes[0].uv );	

	glDrawArrays( GL_QUADS, 0, m_translucentFaceVertexes.size() );

	renderer->Disable( GL_TEXTURE_2D );
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_COLOR_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	glDepthMask( GL_TRUE );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Chunk::SaveChunkToDisk() const  
{
	std::string chunkFilePath = GetFilePathToChunk();
	size_t rleBufferSize = 0;
	unsigned char* rleBuffer = CreateRLEBuffer( rleBufferSize );
	WriteBufferToFile( chunkFilePath, rleBuffer, rleBufferSize );
	delete rleBuffer;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
bool Chunk::LoadChunkFromDisk()
{
	std::string chunkFilePath = GetFilePathToChunk();
	size_t bufferSize;
    
	unsigned char* buffer = LoadBinaryFileToNewBuffer( chunkFilePath, bufferSize );
	if( buffer != NULL )
	{
		PopulateFromRLEBuffer( buffer, bufferSize );
		return true;
	}
	return false;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
unsigned char* Chunk::CreateRLEBuffer( size_t& rleBufferSize ) const 
{
	// start RLE entry with 1st block
	unsigned char currentBlockType = m_blocks[0].m_blockType;
	unsigned short currentBlockCount = 1;

	std::vector< unsigned char > bufferVec;
	bufferVec.reserve( 4096 );

	// iterate second block and beyond
	for( BlockIndex blockIndex = 1; blockIndex < NUM_BLOCKS_PER_CHUNK; ++blockIndex )
	{
		const Block& block = m_blocks[ blockIndex ]; 
		if( block.m_blockType != currentBlockType )
		{
			AppendRLEEntryToBuffer( currentBlockType, currentBlockCount, bufferVec );
			currentBlockCount = 0;
			currentBlockType = block.m_blockType;
		}
		currentBlockCount++;
	}

	// append one last time to get the last entry
	AppendRLEEntryToBuffer( currentBlockType, currentBlockCount, bufferVec );

	unsigned char* buffer = new unsigned char[ bufferVec.size() ];
	memcpy( buffer, bufferVec.data(), bufferVec.size() );
	rleBufferSize = bufferVec.size();
	return buffer;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
std::string Chunk::GetFilePathToChunk() const 
{
	std::stringstream ss;
	ss << "Data/Chunks/Chunk" << m_chunkCoords.x << "," << m_chunkCoords.y << ".chunk";
	return ss.str();
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Chunk::AppendRLEEntryToBuffer( const unsigned char& blockType, const unsigned short& blockCount, std::vector< unsigned char >& buffer ) const
{
	buffer.push_back( blockType );
	buffer.push_back( (unsigned char) ( blockCount >> 8 ) );
	buffer.push_back( (unsigned char) ( blockCount ) );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Chunk::PopulateFromRLEBuffer( unsigned char* buffer, const size_t& bufferSize )
{
	std::vector<unsigned char> bufferCheck;
	bufferCheck.reserve( bufferSize );
	for( unsigned int i = 0; i < bufferSize; ++i )
		bufferCheck.push_back( buffer[i] );
	
	int bufferIndex = 0;
	unsigned char currentBlockType = buffer[bufferIndex++];
	unsigned short numBlocksOfCurrentType = (unsigned short) buffer[bufferIndex++];
	unsigned short numBlocksOfCurrentTypeHigh = numBlocksOfCurrentType << 8;
	unsigned short numBlocksOfCurrentTypeLow = (unsigned short) buffer[bufferIndex++];
	numBlocksOfCurrentType = numBlocksOfCurrentTypeHigh | numBlocksOfCurrentTypeLow;

	for( BlockIndex blockIndex = 0; blockIndex < NUM_BLOCKS_PER_CHUNK; ++blockIndex )
	{
		m_blocks[ blockIndex ].ClearAllFlags();

		m_blocks[blockIndex].m_blockType = currentBlockType;
		numBlocksOfCurrentType--;

		if( numBlocksOfCurrentType == 0 )
		{
			currentBlockType = buffer[bufferIndex++];
			numBlocksOfCurrentType = (unsigned short) buffer[bufferIndex++];
			numBlocksOfCurrentTypeHigh = numBlocksOfCurrentType << 8;
			numBlocksOfCurrentTypeLow = (unsigned short) buffer[bufferIndex++];
			numBlocksOfCurrentType = numBlocksOfCurrentTypeHigh | numBlocksOfCurrentTypeLow;
		}
	}
}