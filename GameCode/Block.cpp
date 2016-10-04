//=================================================================================
// Block.cpp
// Author: Tyler George
// Date  : March 31, 2015
//=================================================================================

#include "GameCode/Block.hpp"
#include "GameCode/TheApp.hpp"

/////---------------------------------------------------------------------------------
/////
/////---------------------------------------------------------------------------------
//Sound* Block::PlayRandomPlaceSound() const 
//{
//	SoundIDs placeSounds = BlockDefinition::s_blockDefinitions[ m_blockType ].GetPlaceSounds();
//	
//	if( placeSounds.size() == 0 )
//		return NULL;
//
//	int randPlaceSound = GetRandomIntLessThan( placeSounds.size() );
//
//	return TheApp::s_theSoundSystem->PlayStreamingSound( placeSounds[ randPlaceSound ] );
//}
//
/////---------------------------------------------------------------------------------
/////
/////---------------------------------------------------------------------------------
//Sound* Block::PlayRandomWalkSound() const 
//{
//	SoundIDs walkSounds = BlockDefinition::s_blockDefinitions[ m_blockType ].GetWalkSounds();
//
//	if( walkSounds.size() == 0 )
//		return NULL;
//
//	int randWalkSound = GetRandomIntLessThan( walkSounds.size() );
//
//	return TheApp::s_theSoundSystem->PlayStreamingSound( walkSounds[ randWalkSound ] );
//}
//
/////---------------------------------------------------------------------------------
/////
/////---------------------------------------------------------------------------------
//Sound* Block::PlayRandomBreakSound() const 
//{
//	SoundIDs breakSounds = BlockDefinition::s_blockDefinitions[ m_blockType ].GetBreakSounds();
//
//	if( breakSounds.size() == 0 )
//		return NULL;
//
//	int randBreakSound = GetRandomIntLessThan( breakSounds.size() );
//
//	return TheApp::s_theSoundSystem->PlayStreamingSound( breakSounds[ randBreakSound ] );
//}