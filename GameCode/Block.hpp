//=================================================================================
// Block.hpp
// Author: Tyler George
// Date  : March 31, 2015
//=================================================================================

#pragma once
#ifndef __included_Block__
#define __included_Block__

#include <vector>
#include "GameCode/BlockDefinition.hpp"

///---------------------------------------------------------------------------------
/// Constants
///---------------------------------------------------------------------------------
const unsigned char BLOCK_IS_SKY_MASK = 0x10;
const unsigned char BLOCK_IS_DIRTY_MASK = 0x20;
const unsigned char BLOCK_LIGHT_LEVEL_MASK = 0x0F;

class Block
{

public:
	unsigned char m_blockType;

	///---------------------------------------------------------------------------------
	/// Accessors
	///---------------------------------------------------------------------------------
	int GetLightValue() const;
	bool IsDirty() const;
	bool IsSky() const;
	bool IsSolid() const;
	bool IsOpaque() const;
	bool IsVisible() const;
	bool IsTranslucent() const;
	bool FallsWithGravity() const;
	int GetInternalLightValue() const;

	//Sound* PlayRandomPlaceSound() const;
	//Sound* PlayRandomWalkSound() const;
	//Sound* PlayRandomBreakSound() const;

	///---------------------------------------------------------------------------------
	/// Mutators
	///---------------------------------------------------------------------------------
	void SetLightValue( int lightValue );

	void MarkAsDirty();
	void MarkAsSky();

	void MarkAsNotDirty();
	void MarkAsNotSky();

	void ClearAllFlags();

private:
	//---------------------------------------------------------------------------------
	// Bit meanings:
	// 7		- none
	// 6		- none
	// 5		- Is Dirty
	// 4		- Is Sky
	// 3 to 0	- Light Value
	//---------------------------------------------------------------------------------
	unsigned char m_lightingAndFlags;
};

///---------------------------------------------------------------------------------
/// Inline Function Implementations
///---------------------------------------------------------------------------------
///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline int Block::GetLightValue() const
{
	return (int) ( m_lightingAndFlags & BLOCK_LIGHT_LEVEL_MASK );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline bool Block::IsDirty() const
{
	return ( m_lightingAndFlags & BLOCK_IS_DIRTY_MASK ) != 0 ? true : false;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline bool Block::IsSky() const
{
	return ( m_lightingAndFlags & BLOCK_IS_SKY_MASK ) != 0 ? true : false;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline bool Block::IsSolid() const
{
	return BlockDefinition::s_blockDefinitions[ m_blockType ].m_isSolid;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline bool Block::IsOpaque() const
{
	return BlockDefinition::s_blockDefinitions[ m_blockType ].m_isOpaque;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline bool Block::IsVisible() const
{
	return BlockDefinition::s_blockDefinitions[ m_blockType ].m_isVisible;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline bool Block::IsTranslucent() const
{
	return BlockDefinition::s_blockDefinitions[ m_blockType ].m_isTranslucent;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline bool Block::FallsWithGravity() const
{
	return BlockDefinition::s_blockDefinitions[ m_blockType ].m_fallsWithGravity;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline int Block::GetInternalLightValue() const
{
	return BlockDefinition::s_blockDefinitions[ m_blockType ].m_illuminationLevel;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline void Block::SetLightValue( int value )
{
	//unsigned char lightValue = (unsigned char) value;
	m_lightingAndFlags &= 0xF0;
	m_lightingAndFlags |= value;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline void Block::MarkAsDirty()
{
	m_lightingAndFlags |= BLOCK_IS_DIRTY_MASK;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline void Block::MarkAsSky()
{
	m_lightingAndFlags |= BLOCK_IS_SKY_MASK;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline void Block::MarkAsNotDirty()
{
	m_lightingAndFlags &= ~BLOCK_IS_DIRTY_MASK;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline void Block::MarkAsNotSky()
{
	m_lightingAndFlags &= ~BLOCK_IS_SKY_MASK;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
inline void Block::ClearAllFlags()
{
	m_lightingAndFlags = 0x00;
}
#endif