//=================================================================================
//UserInterface.hpp
//Author: Tyler George
//Date  : April 20, 2015
//=================================================================================

#pragma once

#ifndef __included_UserInterface__
#define __included_UserInterface__

#include "Engine/Renderer/OpenGLRenderer.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "GameCode/BlockDefinition.hpp"

const BlockType g_blockTypesForUISlots[] = {
	BLOCK_TYPE_GRASS,
	BLOCK_TYPE_DIRT,
	BLOCK_TYPE_STONE,
	BLOCK_TYPE_GLASS,
	BLOCK_TYPE_OAK_WOOD,
	BLOCK_TYPE_OAK_WOOD_PLANKS,
	BLOCK_TYPE_SAND,
	BLOCK_TYPE_GLOWSTONE
};
const int NUM_PLACEABLE_BLOCK_TYPES = sizeof( g_blockTypesForUISlots ) / sizeof( g_blockTypesForUISlots[0] );


///---------------------------------------------------------------------------------
/// Constants
///---------------------------------------------------------------------------------
//const int NUM_PLACEABLE_BLOCK_TYPES = 8;

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
class UserInterface
{
public:
	///---------------------------------------------------------------------------------
	/// Contructor
	///---------------------------------------------------------------------------------
	UserInterface( const Vector2& displaySize );
	
	///---------------------------------------------------------------------------------
	/// Accessors
	///---------------------------------------------------------------------------------
	BlockType GetCurrentBlockType();

	///---------------------------------------------------------------------------------
	/// Update functions
	///---------------------------------------------------------------------------------
	void Update( double deltaSeconds, InputSystem* inputSystem );

	///---------------------------------------------------------------------------------
	/// Render functions
	///---------------------------------------------------------------------------------
	void Render( OpenGLRenderer* renderer ) const;


	///---------------------------------------------------------------------------------
	/// Static variables
	///---------------------------------------------------------------------------------
	static BlockType s_blockTypes[ NUM_PLACEABLE_BLOCK_TYPES ];

private:
	BlockType m_currentBlockType;
	int m_currentBlockTypeIndex;
	Vector2 m_displaySize;
};

#endif