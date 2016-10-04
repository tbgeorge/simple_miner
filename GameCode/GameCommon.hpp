//=================================================================================
// GameCommon.hpp
// Author: Tyler George
// Date  : April 16, 2015
//=================================================================================

#pragma once

#ifndef __included_GameCommon__
#define __included_GameCommon__

class World;

enum RenderingMode
{
	GL_BEGIN,
	GL_VERTEX_ARRAYS,
	GL_VBOS
};

extern bool g_makeAllBlocksWhite;
extern bool g_manuallyAdvanceLighting;
extern RenderingMode g_renderingMode;
extern World* g_theWorld;
extern bool g_loadBlockSounds;

#endif