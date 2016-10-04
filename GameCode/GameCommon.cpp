//=================================================================================
// GameCommon.cpp
// Author: Tyler George
// Date  : April 16, 2015
//=================================================================================

#include "GameCode/GameCommon.hpp"

///---------------------------------------------------------------------------------
/// Globals
///---------------------------------------------------------------------------------
bool g_makeAllBlocksWhite = false;
bool g_manuallyAdvanceLighting = false;
bool g_loadBlockSounds = true;
RenderingMode g_renderingMode = GL_VBOS; // Options: GL_BEGIN || GL_VERTEX_ARRAYS || GL_VBOS