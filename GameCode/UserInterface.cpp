//=================================================================================
//UserInterface.cpp
//Author: Tyler George
//Date  : April 20, 2015
//=================================================================================

#include "GameCode/UserInterface.hpp"
#include "GameCode/World.hpp"
#include "GameCode/TheApp.hpp"




///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
UserInterface::UserInterface( const Vector2& displaySize )
	: m_currentBlockTypeIndex( 0 )
	, m_currentBlockType( g_blockTypesForUISlots[ 0 ] )
	, m_displaySize( displaySize )
{

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
BlockType UserInterface::GetCurrentBlockType() 
{
	return m_currentBlockType;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void UserInterface::Update( double deltaSeconds, InputSystem* inputSystem )
{
	UNUSED( deltaSeconds );

	for( int uiSlotIndex = 0; uiSlotIndex < NUM_PLACEABLE_BLOCK_TYPES; ++uiSlotIndex )
	{
		if( inputSystem->WasKeyJustReleased( '0' + ( uiSlotIndex + 1 ) ) )
		{
			m_currentBlockTypeIndex = uiSlotIndex;
		}
	}

	int numWheelClicks = inputSystem->GetWheelClicks();

	
	m_currentBlockTypeIndex += -numWheelClicks;
	if( m_currentBlockTypeIndex >= NUM_PLACEABLE_BLOCK_TYPES )
		m_currentBlockTypeIndex = 0;
	if( m_currentBlockTypeIndex < 0 )
		m_currentBlockTypeIndex = NUM_PLACEABLE_BLOCK_TYPES - 1;
	

	m_currentBlockType = g_blockTypesForUISlots[ m_currentBlockTypeIndex ];

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void UserInterface::Render( OpenGLRenderer* renderer ) const
{
	if( !renderer )
		return;

	const float maxBlockWidth = 64.0f;
	const float paddingBetweenBlocks = 20.0f;
	const float marginSize = 20.0f;

	int numPaddings = NUM_PLACEABLE_BLOCK_TYPES - 1;
	float totalWidth = m_displaySize.x;

	float widthForPadding = (float) numPaddings * paddingBetweenBlocks;
	float widthForMargins = 2.0f * marginSize;
	float widthForBlocks = totalWidth - ( widthForPadding + widthForMargins );

	float blockWidth = widthForBlocks / NUM_PLACEABLE_BLOCK_TYPES;

	if( blockWidth > maxBlockWidth )
		blockWidth = maxBlockWidth;

	float interfaceBarWidth = ( blockWidth * NUM_PLACEABLE_BLOCK_TYPES ) + widthForPadding;

	float renderingStartPosX = ( m_displaySize.x * 0.5f ) - ( interfaceBarWidth * 0.5f );

	Vector2s blockTypesTexCoordMins;
	Vector2s blockTypesTexCoordMaxs;
	RgbaList blockTypesColors;

	for( int uiSlotIndex = 0; uiSlotIndex < NUM_PLACEABLE_BLOCK_TYPES; ++uiSlotIndex )
	{
		BlockType blockTypeIndex = g_blockTypesForUISlots[ uiSlotIndex ];

		Vector2 blockTypeTexCoordMins = BlockDefinition::s_blockDefinitions[ blockTypeIndex ].m_topTexCoordMins;
		Vector2 blockTypeTexCoordMaxs = Vector2( blockTypeTexCoordMins.x + TheApp::TEX_COORD_SIZE_PER_TILE, blockTypeTexCoordMins.y + TheApp::TEX_COORD_SIZE_PER_TILE );
		Rgba blockTypeColor = BlockDefinition::s_blockDefinitions[ blockTypeIndex ].m_topTint;

		blockTypesTexCoordMins.push_back( blockTypeTexCoordMins );
		blockTypesTexCoordMaxs.push_back( blockTypeTexCoordMaxs );
		blockTypesColors.push_back( blockTypeColor );
	}

	renderer->SetOrthographicView( m_displaySize.x, m_displaySize.y );
	
	Vector2s vertices;
	vertices.push_back( Vector2( 0.0f, 0.0f ) );
	vertices.push_back( Vector2( blockWidth, 0.0f ) );
	vertices.push_back( Vector2( blockWidth, blockWidth ) );
	vertices.push_back( Vector2( 0.0f, blockWidth ) );

	Vector2s texCoords;
	
	float renderPosX = renderingStartPosX;
	float renderPosY = marginSize;

	for( int uiSlotIndex = 0; uiSlotIndex < NUM_PLACEABLE_BLOCK_TYPES; ++uiSlotIndex )
	{
		Rgba blockTypeColor = blockTypesColors[ uiSlotIndex ];
		Vector2 blockTypeTexCoordMins = blockTypesTexCoordMins[ uiSlotIndex ];
		Vector2 blockTypeTexCoordMaxs = blockTypesTexCoordMaxs[ uiSlotIndex ];
		texCoords.push_back( Vector2( blockTypeTexCoordMins.x, blockTypeTexCoordMaxs.y ) );
		texCoords.push_back( Vector2( blockTypeTexCoordMaxs.x, blockTypeTexCoordMaxs.y ) );
		texCoords.push_back( Vector2( blockTypeTexCoordMaxs.x, blockTypeTexCoordMins.y ) );
		texCoords.push_back( Vector2( blockTypeTexCoordMins.x, blockTypeTexCoordMins.y ) );

		renderer->PushMatrix();
		renderer->SetModelViewTranslation( renderPosX, renderPosY );
		renderer->DrawTexturedQuad2D( *TheApp::s_textureAtlas, vertices, texCoords, blockTypeColor ); 
		

		Rgba outlineColor = Rgba::BLACK;
		if( g_blockTypesForUISlots[ uiSlotIndex ] == m_currentBlockType )
			outlineColor = Rgba::WHITE;

		renderer->DrawPolygon2D( vertices, outlineColor, 5.0f );


		renderer->PopMatrix();
		texCoords.clear();
		renderPosX += ( blockWidth + paddingBetweenBlocks );
	}

	vertices.clear();
	vertices.push_back( Vector2( -0.5f, 0.0f ) );
	vertices.push_back( Vector2( 0.5f, 0.0f ) );
	vertices.push_back( Vector2( 0.5f, 5.0f ) );
	vertices.push_back( Vector2( -0.5f, 5.0f ) );

	glBlendFunc( GL_ONE_MINUS_DST_COLOR, GL_ZERO );
	renderer->PushMatrix();
	renderer->SetModelViewTranslation( m_displaySize.x * 0.5f, m_displaySize.y * 0.5f );
	renderer->SetModelViewScale( 3.0f, 3.0f );
	renderer->DrawFilledPolygon2D( vertices, Rgba::WHITE );
	renderer->SetModelViewRotation( 90.0f, 0.0f, 0.0f, 1.0f );
	renderer->DrawFilledPolygon2D( vertices, Rgba::WHITE );
	renderer->SetModelViewRotation( 90.0f, 0.0f, 0.0f, 1.0f );
	renderer->DrawFilledPolygon2D( vertices, Rgba::WHITE );
	renderer->SetModelViewRotation( 90.0f, 0.0f, 0.0f, 1.0f );
	renderer->DrawFilledPolygon2D( vertices, Rgba::WHITE );
	renderer->PopMatrix();
}