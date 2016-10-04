//=================================================================================
// BlockDefinition.cpp
// Author: Tyler George
// Date  : March 31, 2015
//=================================================================================

#include "GameCode/BlockDefinition.hpp"
#include "GameCode/TheApp.hpp"
#include "GameCode/GameCommon.hpp"

///---------------------------------------------------------------------------------
/// Static variables
///---------------------------------------------------------------------------------
BlockDefinition BlockDefinition::s_blockDefinitions[ NUM_BLOCK_TYPES ];

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void BlockDefinition::InitializeBlockDefinitions()
{
	s_blockDefinitions[ BLOCK_TYPE_INVALID ].m_topTexCoordMins = TheApp::s_textureAtlas->CalcTextureCoordMinsAtSpriteCoordinates( IntVector2( 25, 18 ) );
	s_blockDefinitions[ BLOCK_TYPE_INVALID ].m_bottomTexCoordMins =		s_blockDefinitions[ BLOCK_TYPE_INVALID ].m_topTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_INVALID ].m_northTexCoordMins =		s_blockDefinitions[ BLOCK_TYPE_INVALID ].m_topTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_INVALID ].m_southTexCoordMins =		s_blockDefinitions[ BLOCK_TYPE_INVALID ].m_topTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_INVALID ].m_eastTexCoordMins =		s_blockDefinitions[ BLOCK_TYPE_INVALID ].m_topTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_INVALID ].m_westTexCoordMins =		s_blockDefinitions[ BLOCK_TYPE_INVALID ].m_topTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_INVALID ].m_topTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_INVALID ].m_bottomTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_INVALID ].m_northTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_INVALID ].m_southTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_INVALID ].m_eastTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_INVALID ].m_westTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_INVALID ].m_isSolid = true;
	s_blockDefinitions[ BLOCK_TYPE_INVALID ].m_isOpaque = true;
	s_blockDefinitions[ BLOCK_TYPE_INVALID ].m_fallsWithGravity = false;
	s_blockDefinitions[ BLOCK_TYPE_INVALID ].m_isVisible = true;
	s_blockDefinitions[ BLOCK_TYPE_INVALID ].m_isTranslucent = false;
	s_blockDefinitions[ BLOCK_TYPE_INVALID ].m_illuminationLevel = 0;


	s_blockDefinitions[ BLOCK_TYPE_AIR ].m_topTexCoordMins = TheApp::s_textureAtlas->CalcTextureCoordMinsAtSpriteCoordinates( IntVector2( 27, 18 ) );
	s_blockDefinitions[ BLOCK_TYPE_AIR ].m_bottomTexCoordMins =		s_blockDefinitions[ BLOCK_TYPE_AIR ].m_topTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_AIR ].m_northTexCoordMins =		s_blockDefinitions[ BLOCK_TYPE_AIR ].m_topTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_AIR ].m_southTexCoordMins =		s_blockDefinitions[ BLOCK_TYPE_AIR ].m_topTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_AIR ].m_eastTexCoordMins =		s_blockDefinitions[ BLOCK_TYPE_AIR ].m_topTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_AIR ].m_westTexCoordMins =		s_blockDefinitions[ BLOCK_TYPE_AIR ].m_topTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_AIR ].m_topTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_AIR ].m_bottomTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_AIR ].m_northTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_AIR ].m_southTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_AIR ].m_eastTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_AIR ].m_westTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_AIR ].m_isSolid = false;
	s_blockDefinitions[ BLOCK_TYPE_AIR ].m_isOpaque = false;
	s_blockDefinitions[ BLOCK_TYPE_AIR ].m_fallsWithGravity = false;
	s_blockDefinitions[ BLOCK_TYPE_AIR ].m_isVisible = false;
	s_blockDefinitions[ BLOCK_TYPE_AIR ].m_isTranslucent = false;
	s_blockDefinitions[ BLOCK_TYPE_AIR ].m_illuminationLevel = 0;


	s_blockDefinitions[ BLOCK_TYPE_GRASS ].m_topTexCoordMins = TheApp::s_textureAtlas->CalcTextureCoordMinsAtSpriteCoordinates( IntVector2( 15, 10 ) );
	s_blockDefinitions[ BLOCK_TYPE_GRASS ].m_bottomTexCoordMins = TheApp::s_textureAtlas->CalcTextureCoordMinsAtSpriteCoordinates( IntVector2( 8, 6 ) );
	s_blockDefinitions[ BLOCK_TYPE_GRASS ].m_northTexCoordMins = TheApp::s_textureAtlas->CalcTextureCoordMinsAtSpriteCoordinates( IntVector2( 12, 10 ) );
	s_blockDefinitions[ BLOCK_TYPE_GRASS ].m_southTexCoordMins =	s_blockDefinitions[ BLOCK_TYPE_GRASS ].m_northTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_GRASS ].m_eastTexCoordMins =		s_blockDefinitions[ BLOCK_TYPE_GRASS ].m_northTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_GRASS ].m_westTexCoordMins =		s_blockDefinitions[ BLOCK_TYPE_GRASS ].m_northTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_GRASS ].m_topTint = Rgba::GREEN;
	s_blockDefinitions[ BLOCK_TYPE_GRASS ].m_bottomTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_GRASS ].m_northTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_GRASS ].m_southTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_GRASS ].m_eastTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_GRASS ].m_westTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_GRASS ].m_isSolid = true;
	s_blockDefinitions[ BLOCK_TYPE_GRASS ].m_isOpaque = true;
	s_blockDefinitions[ BLOCK_TYPE_GRASS ].m_fallsWithGravity = false;
	s_blockDefinitions[ BLOCK_TYPE_GRASS ].m_isVisible = true;
	s_blockDefinitions[ BLOCK_TYPE_GRASS ].m_isTranslucent = false;
	s_blockDefinitions[ BLOCK_TYPE_GRASS ].m_illuminationLevel = 0;

	//if( g_loadBlockSounds )
	//{
	//	s_blockDefinitions[ BLOCK_TYPE_GRASS ].m_walkSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound3/step/grass1.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_GRASS ].m_walkSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound3/step/grass2.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_GRASS ].m_walkSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound3/step/grass3.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_GRASS ].m_walkSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound3/step/grass4.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_GRASS ].m_walkSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound3/step/grass5.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_GRASS ].m_walkSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound3/step/grass6.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_GRASS ].m_placeSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/grass1.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_GRASS ].m_placeSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/grass2.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_GRASS ].m_placeSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/grass3.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_GRASS ].m_placeSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/grass4.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_GRASS ].m_breakSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/grass1.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_GRASS ].m_breakSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/grass2.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_GRASS ].m_breakSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/grass3.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_GRASS ].m_breakSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/grass4.ogg", 2 ) );
	//	ConsolePrintf("Loading Sounds: %f \n", 12.5f );
	//}


	s_blockDefinitions[ BLOCK_TYPE_DIRT ].m_topTexCoordMins = TheApp::s_textureAtlas->CalcTextureCoordMinsAtSpriteCoordinates( IntVector2( 8, 6 ) );
	s_blockDefinitions[ BLOCK_TYPE_DIRT ].m_bottomTexCoordMins =	s_blockDefinitions[ BLOCK_TYPE_DIRT ].m_topTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_DIRT ].m_northTexCoordMins =		s_blockDefinitions[ BLOCK_TYPE_DIRT ].m_topTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_DIRT ].m_southTexCoordMins =		s_blockDefinitions[ BLOCK_TYPE_DIRT ].m_topTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_DIRT ].m_eastTexCoordMins =		s_blockDefinitions[ BLOCK_TYPE_DIRT ].m_topTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_DIRT ].m_westTexCoordMins =		s_blockDefinitions[ BLOCK_TYPE_DIRT ].m_topTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_DIRT ].m_topTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_DIRT ].m_bottomTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_DIRT ].m_northTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_DIRT ].m_southTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_DIRT ].m_eastTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_DIRT ].m_westTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_DIRT ].m_isSolid = true;
	s_blockDefinitions[ BLOCK_TYPE_DIRT ].m_isOpaque = true;
	s_blockDefinitions[ BLOCK_TYPE_DIRT ].m_fallsWithGravity = false;
	s_blockDefinitions[ BLOCK_TYPE_DIRT ].m_isVisible = true;
	s_blockDefinitions[ BLOCK_TYPE_DIRT ].m_isTranslucent = false;
	s_blockDefinitions[ BLOCK_TYPE_DIRT ].m_illuminationLevel = 0;

	//if( g_loadBlockSounds )
	//{
	//	s_blockDefinitions[ BLOCK_TYPE_DIRT ].m_walkSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound3/step/gravel1.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_DIRT ].m_walkSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound3/step/gravel2.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_DIRT ].m_walkSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound3/step/gravel3.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_DIRT ].m_walkSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound3/step/gravel4.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_DIRT ].m_placeSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/gravel1.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_DIRT ].m_placeSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/gravel2.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_DIRT ].m_placeSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/gravel3.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_DIRT ].m_placeSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/gravel4.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_DIRT ].m_breakSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/gravel1.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_DIRT ].m_breakSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/gravel2.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_DIRT ].m_breakSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/gravel3.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_DIRT ].m_breakSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/gravel4.ogg", 2 ) );
	//	ConsolePrintf("Loading Sounds: %f \n", 25.0f );
	//}


	s_blockDefinitions[ BLOCK_TYPE_STONE ].m_topTexCoordMins = TheApp::s_textureAtlas->CalcTextureCoordMinsAtSpriteCoordinates( IntVector2( 20, 9 ) );
	s_blockDefinitions[ BLOCK_TYPE_STONE ].m_bottomTexCoordMins =	s_blockDefinitions[ BLOCK_TYPE_STONE ].m_topTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_STONE ].m_northTexCoordMins =	s_blockDefinitions[ BLOCK_TYPE_STONE ].m_topTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_STONE ].m_southTexCoordMins =	s_blockDefinitions[ BLOCK_TYPE_STONE ].m_topTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_STONE ].m_eastTexCoordMins =		s_blockDefinitions[ BLOCK_TYPE_STONE ].m_topTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_STONE ].m_westTexCoordMins =		s_blockDefinitions[ BLOCK_TYPE_STONE ].m_topTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_STONE ].m_topTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_STONE ].m_bottomTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_STONE ].m_northTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_STONE ].m_southTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_STONE ].m_eastTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_STONE ].m_westTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_STONE ].m_isSolid = true;
	s_blockDefinitions[ BLOCK_TYPE_STONE ].m_isOpaque = true;
	s_blockDefinitions[ BLOCK_TYPE_STONE ].m_fallsWithGravity = false;
	s_blockDefinitions[ BLOCK_TYPE_STONE ].m_isVisible = true;
	s_blockDefinitions[ BLOCK_TYPE_STONE ].m_isTranslucent = false;
	s_blockDefinitions[ BLOCK_TYPE_STONE ].m_illuminationLevel = 0;

	//if( g_loadBlockSounds )
	//{
	//	s_blockDefinitions[ BLOCK_TYPE_STONE ].m_walkSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound3/step/stone1.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_STONE ].m_walkSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound3/step/stone2.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_STONE ].m_walkSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound3/step/stone3.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_STONE ].m_walkSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound3/step/stone4.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_STONE ].m_walkSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound3/step/stone5.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_STONE ].m_walkSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound3/step/stone6.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_STONE ].m_placeSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/stone1.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_STONE ].m_placeSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/stone2.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_STONE ].m_placeSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/stone3.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_STONE ].m_placeSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/stone4.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_STONE ].m_breakSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/stone1.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_STONE ].m_breakSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/stone2.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_STONE ].m_breakSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/stone3.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_STONE ].m_breakSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/stone4.ogg", 2 ) );
	//	ConsolePrintf("Loading Sounds: %f \n", 37.5f );
	//}


	s_blockDefinitions[ BLOCK_TYPE_GLASS ].m_topTexCoordMins = TheApp::s_textureAtlas->CalcTextureCoordMinsAtSpriteCoordinates( IntVector2( 7, 8 ) );
	s_blockDefinitions[ BLOCK_TYPE_GLASS ].m_bottomTexCoordMins =	s_blockDefinitions[ BLOCK_TYPE_GLASS ].m_topTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_GLASS ].m_northTexCoordMins =	s_blockDefinitions[ BLOCK_TYPE_GLASS ].m_topTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_GLASS ].m_southTexCoordMins =	s_blockDefinitions[ BLOCK_TYPE_GLASS ].m_topTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_GLASS ].m_eastTexCoordMins =		s_blockDefinitions[ BLOCK_TYPE_GLASS ].m_topTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_GLASS ].m_westTexCoordMins =		s_blockDefinitions[ BLOCK_TYPE_GLASS ].m_topTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_GLASS ].m_topTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_GLASS ].m_bottomTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_GLASS ].m_northTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_GLASS ].m_southTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_GLASS ].m_eastTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_GLASS ].m_westTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_GLASS ].m_isSolid = true;
	s_blockDefinitions[ BLOCK_TYPE_GLASS ].m_isOpaque = false;
	s_blockDefinitions[ BLOCK_TYPE_GLASS ].m_fallsWithGravity = false;
	s_blockDefinitions[ BLOCK_TYPE_GLASS ].m_isVisible = true;
	s_blockDefinitions[ BLOCK_TYPE_GLASS ].m_isTranslucent = false;
	s_blockDefinitions[ BLOCK_TYPE_GLASS ].m_illuminationLevel = 0;

	//if( g_loadBlockSounds )
	//{
	//	s_blockDefinitions[ BLOCK_TYPE_GLASS ].m_walkSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound3/step/stone1.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_GLASS ].m_walkSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound3/step/stone2.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_GLASS ].m_walkSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound3/step/stone3.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_GLASS ].m_walkSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound3/step/stone4.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_GLASS ].m_walkSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound3/step/stone5.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_GLASS ].m_walkSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound3/step/stone6.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_GLASS ].m_placeSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/stone1.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_GLASS ].m_placeSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/stone2.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_GLASS ].m_placeSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/stone3.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_GLASS ].m_placeSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/stone4.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_GLASS ].m_breakSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/glass1.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_GLASS ].m_breakSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/glass2.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_GLASS ].m_breakSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/glass3.ogg", 2 ) );
	//	ConsolePrintf("Loading Sounds: %f \n", 50.0f );
	//}


	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD ].m_topTexCoordMins = TheApp::s_textureAtlas->CalcTextureCoordMinsAtSpriteCoordinates( IntVector2( 4, 14 ) );
	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD ].m_bottomTexCoordMins =	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD ].m_topTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD ].m_northTexCoordMins =	TheApp::s_textureAtlas->CalcTextureCoordMinsAtSpriteCoordinates( IntVector2( 3, 14 ) );
	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD ].m_southTexCoordMins =		s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD ].m_northTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD ].m_eastTexCoordMins =		s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD ].m_northTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD ].m_westTexCoordMins =		s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD ].m_northTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD ].m_topTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD ].m_bottomTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD ].m_northTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD ].m_southTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD ].m_eastTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD ].m_westTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD ].m_isSolid = true;
	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD ].m_isOpaque = true;
	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD ].m_fallsWithGravity = false;
	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD ].m_isVisible = true;
	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD ].m_isTranslucent = false;
	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD ].m_illuminationLevel = 0;

	//if( g_loadBlockSounds )
	//{
	//	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD ].m_walkSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound3/step/wood1.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD ].m_walkSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound3/step/wood2.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD ].m_walkSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound3/step/wood3.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD ].m_walkSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound3/step/wood4.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD ].m_walkSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound3/step/wood5.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD ].m_walkSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound3/step/wood6.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD ].m_placeSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/wood1.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD ].m_placeSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/wood2.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD ].m_placeSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/wood3.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD ].m_placeSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/wood4.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD ].m_breakSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/wood1.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD ].m_breakSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/wood2.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD ].m_breakSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/wood3.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD ].m_breakSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/wood4.ogg", 2 ) );
	//	ConsolePrintf("Loading Sounds: %f \n", 62.5f );
	//}


	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD_PLANKS ].m_topTexCoordMins = TheApp::s_textureAtlas->CalcTextureCoordMinsAtSpriteCoordinates( IntVector2( 16, 4 ) );
	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD_PLANKS ].m_bottomTexCoordMins =		s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD_PLANKS ].m_topTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD_PLANKS ].m_northTexCoordMins =		s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD_PLANKS ].m_topTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD_PLANKS ].m_southTexCoordMins =		s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD_PLANKS ].m_topTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD_PLANKS ].m_eastTexCoordMins =		s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD_PLANKS ].m_topTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD_PLANKS ].m_westTexCoordMins =		s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD_PLANKS ].m_topTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD_PLANKS ].m_topTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD_PLANKS ].m_bottomTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD_PLANKS ].m_northTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD_PLANKS ].m_southTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD_PLANKS ].m_eastTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD_PLANKS ].m_westTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD_PLANKS ].m_isSolid = true;
	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD_PLANKS ].m_isOpaque = true;
	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD_PLANKS ].m_fallsWithGravity = false;
	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD_PLANKS ].m_isVisible = true;
	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD_PLANKS ].m_isTranslucent = false;
	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD_PLANKS ].m_illuminationLevel = 0;

	//if( g_loadBlockSounds )
	//{
	//	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD_PLANKS ].m_walkSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound3/step/wood1.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD_PLANKS ].m_walkSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound3/step/wood2.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD_PLANKS ].m_walkSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound3/step/wood3.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD_PLANKS ].m_walkSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound3/step/wood4.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD_PLANKS ].m_walkSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound3/step/wood5.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD_PLANKS ].m_walkSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound3/step/wood6.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD_PLANKS ].m_placeSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/wood1.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD_PLANKS ].m_placeSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/wood2.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD_PLANKS ].m_placeSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/wood3.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD_PLANKS ].m_placeSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/wood4.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD_PLANKS ].m_breakSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/wood1.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD_PLANKS ].m_breakSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/wood2.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD_PLANKS ].m_breakSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/wood3.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_OAK_WOOD_PLANKS ].m_breakSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/wood4.ogg", 2 ) );
	//	ConsolePrintf("Loading Sounds: %f \n", 75.0f );
	//}


	s_blockDefinitions[ BLOCK_TYPE_SAND ].m_topTexCoordMins = TheApp::s_textureAtlas->CalcTextureCoordMinsAtSpriteCoordinates( IntVector2( 19, 7 ) );
	s_blockDefinitions[ BLOCK_TYPE_SAND ].m_bottomTexCoordMins =	s_blockDefinitions[ BLOCK_TYPE_SAND ].m_topTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_SAND ].m_northTexCoordMins =		s_blockDefinitions[ BLOCK_TYPE_SAND ].m_topTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_SAND ].m_southTexCoordMins =		s_blockDefinitions[ BLOCK_TYPE_SAND ].m_topTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_SAND ].m_eastTexCoordMins =		s_blockDefinitions[ BLOCK_TYPE_SAND ].m_topTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_SAND ].m_westTexCoordMins =		s_blockDefinitions[ BLOCK_TYPE_SAND ].m_topTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_SAND ].m_topTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_SAND ].m_bottomTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_SAND ].m_northTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_SAND ].m_southTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_SAND ].m_eastTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_SAND ].m_westTint = Rgba();
	s_blockDefinitions[ BLOCK_TYPE_SAND ].m_isSolid = true;
	s_blockDefinitions[ BLOCK_TYPE_SAND ].m_isOpaque = true;
	s_blockDefinitions[ BLOCK_TYPE_SAND ].m_fallsWithGravity = true;
	s_blockDefinitions[ BLOCK_TYPE_SAND ].m_isVisible = true;
	s_blockDefinitions[ BLOCK_TYPE_SAND ].m_isTranslucent = false;
	s_blockDefinitions[ BLOCK_TYPE_SAND ].m_illuminationLevel = 0;

	//if( g_loadBlockSounds )
	//{
	//	s_blockDefinitions[ BLOCK_TYPE_SAND ].m_walkSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound3/step/sand1.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_SAND ].m_walkSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound3/step/sand2.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_SAND ].m_walkSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound3/step/sand3.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_SAND ].m_walkSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound3/step/sand4.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_SAND ].m_walkSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound3/step/sand5.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_SAND ].m_placeSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/gravel1.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_SAND ].m_placeSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/gravel2.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_SAND ].m_placeSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/gravel3.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_SAND ].m_placeSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/gravel4.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_SAND ].m_breakSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/gravel1.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_SAND ].m_breakSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/gravel2.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_SAND ].m_breakSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/gravel3.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_SAND ].m_breakSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/gravel4.ogg", 2 ) );
	//	ConsolePrintf("Loading Sounds: %f \n", 87.5f );
	//}


	s_blockDefinitions[ BLOCK_TYPE_WATER ].m_topTexCoordMins = TheApp::s_textureAtlas->CalcTextureCoordMinsAtSpriteCoordinates( IntVector2( 2, 0 ) );
	s_blockDefinitions[ BLOCK_TYPE_WATER ].m_bottomTexCoordMins =	s_blockDefinitions[ BLOCK_TYPE_WATER ].m_topTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_WATER ].m_northTexCoordMins =	s_blockDefinitions[ BLOCK_TYPE_WATER ].m_topTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_WATER ].m_southTexCoordMins =	s_blockDefinitions[ BLOCK_TYPE_WATER ].m_topTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_WATER ].m_eastTexCoordMins =		s_blockDefinitions[ BLOCK_TYPE_WATER ].m_topTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_WATER ].m_westTexCoordMins =		s_blockDefinitions[ BLOCK_TYPE_WATER ].m_topTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_WATER ].m_topTint =		Rgba(/*);//*/ 1.0f, 1.0f, 1.0f, 1.0f );// 0.7f );
	s_blockDefinitions[ BLOCK_TYPE_WATER ].m_bottomTint =	Rgba(/*);//*/ 1.0f, 1.0f, 1.0f, 1.0f );// 0.7f );
	s_blockDefinitions[ BLOCK_TYPE_WATER ].m_northTint =	Rgba(/*);//*/ 1.0f, 1.0f, 1.0f, 1.0f );// 0.7f );
	s_blockDefinitions[ BLOCK_TYPE_WATER ].m_southTint =	Rgba(/*);//*/ 1.0f, 1.0f, 1.0f, 1.0f );// 0.7f );
	s_blockDefinitions[ BLOCK_TYPE_WATER ].m_eastTint =		Rgba(/*);//*/ 1.0f, 1.0f, 1.0f, 1.0f );// 0.7f );
	s_blockDefinitions[ BLOCK_TYPE_WATER ].m_westTint =		Rgba(/*);//*/ 1.0f, 1.0f, 1.0f, 1.0f );// 0.7f );
	s_blockDefinitions[ BLOCK_TYPE_WATER ].m_isSolid = false;
	s_blockDefinitions[ BLOCK_TYPE_WATER ].m_isOpaque = false;
	s_blockDefinitions[ BLOCK_TYPE_WATER ].m_fallsWithGravity = true;
	s_blockDefinitions[ BLOCK_TYPE_WATER ].m_isVisible = true;
	s_blockDefinitions[ BLOCK_TYPE_WATER ].m_isTranslucent = true;
	s_blockDefinitions[ BLOCK_TYPE_WATER ].m_illuminationLevel = 0;

	s_blockDefinitions[ BLOCK_TYPE_GLOWSTONE ].m_topTexCoordMins = TheApp::s_textureAtlas->CalcTextureCoordMinsAtSpriteCoordinates( IntVector2( 9, 10 ) );
	s_blockDefinitions[ BLOCK_TYPE_GLOWSTONE ].m_bottomTexCoordMins =	s_blockDefinitions[ BLOCK_TYPE_GLOWSTONE ].m_topTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_GLOWSTONE ].m_northTexCoordMins =	s_blockDefinitions[ BLOCK_TYPE_GLOWSTONE ].m_topTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_GLOWSTONE ].m_southTexCoordMins =	s_blockDefinitions[ BLOCK_TYPE_GLOWSTONE ].m_topTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_GLOWSTONE ].m_eastTexCoordMins =		s_blockDefinitions[ BLOCK_TYPE_GLOWSTONE ].m_topTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_GLOWSTONE ].m_westTexCoordMins =		s_blockDefinitions[ BLOCK_TYPE_GLOWSTONE ].m_topTexCoordMins;
	s_blockDefinitions[ BLOCK_TYPE_GLOWSTONE ].m_topTint =		Rgba();
	s_blockDefinitions[ BLOCK_TYPE_GLOWSTONE ].m_bottomTint =	Rgba();
	s_blockDefinitions[ BLOCK_TYPE_GLOWSTONE ].m_northTint =	Rgba();
	s_blockDefinitions[ BLOCK_TYPE_GLOWSTONE ].m_southTint =	Rgba();
	s_blockDefinitions[ BLOCK_TYPE_GLOWSTONE ].m_eastTint =		Rgba();
	s_blockDefinitions[ BLOCK_TYPE_GLOWSTONE ].m_westTint =		Rgba();
	s_blockDefinitions[ BLOCK_TYPE_GLOWSTONE ].m_isSolid = true;
	s_blockDefinitions[ BLOCK_TYPE_GLOWSTONE ].m_isOpaque = true;
	s_blockDefinitions[ BLOCK_TYPE_GLOWSTONE ].m_fallsWithGravity = false;
	s_blockDefinitions[ BLOCK_TYPE_GLOWSTONE ].m_isVisible = true;
	s_blockDefinitions[ BLOCK_TYPE_GLOWSTONE ].m_isTranslucent = false;
	s_blockDefinitions[ BLOCK_TYPE_GLOWSTONE ].m_illuminationLevel = 15;

	//if( g_loadBlockSounds )
	//{
	//	s_blockDefinitions[ BLOCK_TYPE_GLOWSTONE ].m_walkSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound3/step/stone1.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_GLOWSTONE ].m_walkSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound3/step/stone2.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_GLOWSTONE ].m_walkSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound3/step/stone3.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_GLOWSTONE ].m_walkSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound3/step/stone4.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_GLOWSTONE ].m_walkSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound3/step/stone5.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_GLOWSTONE ].m_walkSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound3/step/stone6.ogg", 2 ) );
	//	//ConsolePrintf("walk sounds done");
	//	s_blockDefinitions[ BLOCK_TYPE_GLOWSTONE ].m_placeSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/stone1.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_GLOWSTONE ].m_placeSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/stone2.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_GLOWSTONE ].m_placeSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/stone3.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_GLOWSTONE ].m_placeSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/stone4.ogg", 2 ) );
	//	//ConsolePrintf("place sounds done");
	//	s_blockDefinitions[ BLOCK_TYPE_GLOWSTONE ].m_breakSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/glass1.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_GLOWSTONE ].m_breakSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/glass2.ogg", 2 ) );
	//	s_blockDefinitions[ BLOCK_TYPE_GLOWSTONE ].m_breakSounds.push_back( TheApp::s_theSoundSystem->LoadStreamingSound( "Data/Sounds/sound/glass3.ogg", 2 ) );
	//	ConsolePrintf("Loading Sounds: %f \n", 100.0f );
	//}


	Vector2 whiteBlockTextureCoords = TheApp::s_textureAtlas->CalcTextureCoordMinsAtSpriteCoordinates( IntVector2( 26, 18 ) );
	if( g_makeAllBlocksWhite )
	{
		for( int blockDefinitionIndex = 0; blockDefinitionIndex < NUM_BLOCK_TYPES; blockDefinitionIndex++ )
		{
			s_blockDefinitions[ blockDefinitionIndex ].m_topTexCoordMins = whiteBlockTextureCoords;
			s_blockDefinitions[ blockDefinitionIndex ].m_bottomTexCoordMins = whiteBlockTextureCoords;
			s_blockDefinitions[ blockDefinitionIndex ].m_northTexCoordMins = whiteBlockTextureCoords;
			s_blockDefinitions[ blockDefinitionIndex ].m_southTexCoordMins = whiteBlockTextureCoords;
			s_blockDefinitions[ blockDefinitionIndex ].m_eastTexCoordMins = whiteBlockTextureCoords;
			s_blockDefinitions[ blockDefinitionIndex ].m_westTexCoordMins = whiteBlockTextureCoords;
		}
	}
}