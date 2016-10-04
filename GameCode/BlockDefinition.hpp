//=================================================================================
// BlockDefinition.hpp
// Author: Tyler George
// Date  : March 31, 2015
//=================================================================================

#pragma once
#ifndef __included_BlockDefinition__
#define __included_BlockDefinition__

#include <vector>
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Sound/SoundSystem.hpp"
#include "Engine/Utilities/Rgba.hpp"

///---------------------------------------------------------------------------------
/// Enums
///---------------------------------------------------------------------------------
enum BlockType {
	BLOCK_TYPE_AIR,
	BLOCK_TYPE_GRASS,
	BLOCK_TYPE_DIRT,
	BLOCK_TYPE_STONE,
	BLOCK_TYPE_GLASS,
	BLOCK_TYPE_OAK_WOOD,
	BLOCK_TYPE_OAK_WOOD_PLANKS,
	BLOCK_TYPE_SAND,
	BLOCK_TYPE_WATER,
	BLOCK_TYPE_GLOWSTONE,
	BLOCK_TYPE_INVALID,
	NUM_BLOCK_TYPES

};

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
class BlockDefinition
{
public:
	BlockDefinition();
	~BlockDefinition() {}

public:
	static void InitializeBlockDefinitions();
	//SoundIDs GetPlaceSounds() const { return m_placeSounds; }
	//SoundIDs GetWalkSounds() const { return m_walkSounds; }
	//SoundIDs GetBreakSounds() const { return m_breakSounds; }

public:
	static BlockDefinition s_blockDefinitions[ NUM_BLOCK_TYPES ];

public:
	Vector2	m_topTexCoordMins;
	Vector2	m_bottomTexCoordMins;
	Vector2	m_northTexCoordMins;
	Vector2	m_southTexCoordMins;
	Vector2	m_eastTexCoordMins;
	Vector2	m_westTexCoordMins;

	Rgba m_topTint;
	Rgba m_bottomTint;
	Rgba m_northTint;
	Rgba m_southTint;
	Rgba m_eastTint;
	Rgba m_westTint;

	bool	m_isSolid;
	bool	m_isOpaque;
	bool	m_fallsWithGravity;
	bool	m_isVisible;
	bool	m_isTranslucent;

	unsigned char	m_illuminationLevel;

 	//SoundIDs	m_walkSounds;
 	//SoundIDs	m_placeSounds;
 	//SoundIDs	m_breakSounds;

};

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline BlockDefinition::BlockDefinition()
	: m_isSolid( false )
	, m_isOpaque( false )
	, m_fallsWithGravity( false )
	, m_isVisible( false )
	, m_isTranslucent( false )
{

}

#endif