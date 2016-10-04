//=================================================================================
// TheApp.hpp
// Author: Tyler George
// Date  : January 13, 2015
//=================================================================================

#pragma once

#include "Engine/Renderer/OpenGLRenderer.hpp"
#include "Engine/Renderer/AnimatedTexture.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Sound/SoundSystem.hpp"
#include "GameCode/World.hpp"

#define UNUSED(x) (void)(x)

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
class TheApp 
{
public:

	///---------------------------------------------------------------------------------
	/// Constructor/Destructor
	///---------------------------------------------------------------------------------
	TheApp();
	~TheApp();

	///---------------------------------------------------------------------------------
	/// Initialization Functions
	///---------------------------------------------------------------------------------
	void Startup( void* windowHandle );
	void Shutdown(); 
	void Run();

	///---------------------------------------------------------------------------------
	/// Update Functions
	///---------------------------------------------------------------------------------
	void ProcessInput();
	bool ProcessKeyDown( int keyData );
	bool ProcessKeyUp( int keyData );
	void Update();

	void Stop();
	
	///---------------------------------------------------------------------------------
	/// Render Functions
	///---------------------------------------------------------------------------------
	void Render();
	void RenderWorld();

	///---------------------------------------------------------------------------------
	/// Public static variables
	///---------------------------------------------------------------------------------
	static AnimatedTexture* s_textureAtlas;
	//static SoundSystem* s_theSoundSystem;
	static const float TEX_COORD_SIZE_PER_TILE;

private:
	void*			m_windowHandle;
	OpenGLRenderer*	m_renderer;
	Vector2			m_displaySize;
	bool			m_isRunning;
	InputSystem*	m_inputSystem;
	bool			m_debugModeEnabled;
	int				m_frameNumber;
};