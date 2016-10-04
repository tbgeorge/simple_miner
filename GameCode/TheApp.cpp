//=================================================================================
// TheApp.cpp
// Author: Tyler George
// Date  : January 13, 2015
//=================================================================================

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "Engine/Utilities/Time.hpp"
#include "GameCode/TheApp.hpp"

AnimatedTexture* TheApp::s_textureAtlas = NULL;
//SoundSystem* TheApp::s_theSoundSystem = NULL;
const float TheApp::TEX_COORD_SIZE_PER_TILE = 0.03125f;

World* g_theWorld = NULL;

///---------------------------------------------------------------------------------
/// 
///---------------------------------------------------------------------------------
TheApp::TheApp() 
	: m_isRunning(true)
	, m_windowHandle( NULL )
	, m_renderer( NULL )
	, m_inputSystem( NULL )
	, m_debugModeEnabled( false )
	, m_frameNumber( 0 )
{
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
TheApp::~TheApp() 
{
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void TheApp::Startup( void* windowHandle ) 
{
    InitializeTimeSystem();
	srand( (unsigned int) time(NULL) );

	m_inputSystem = new InputSystem();
	if( m_inputSystem )
		m_inputSystem->Startup( windowHandle );

	m_inputSystem->ShowMousePointer( false );
	m_inputSystem->SetMousePosition( Vector2( 400.0f, 300.0f ) );

	//s_theSoundSystem = new SoundSystem();
	//if( s_theSoundSystem )
	//	s_theSoundSystem->Startup();

	m_windowHandle = windowHandle;
	m_renderer = new OpenGLRenderer();

	if( m_renderer )
	{
		m_renderer->Startup( (HWND)windowHandle );
		m_displaySize = Vector2( m_renderer->GetDisplayWidth(), m_renderer->GetDisplayHeight() );
	}

	if( !s_textureAtlas )
		s_textureAtlas = AnimatedTexture::CreateOrGetAnimatedTexture( "Data/Images/minecraftAtlas.png", 32, 32 );

	g_theWorld = new World( m_displaySize );
	g_theWorld->Startup();	


}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void TheApp::Run()
{
	while ( m_isRunning ) 
	{
		ProcessInput();
		Update();
		Render();
	}

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void TheApp::Shutdown() 
{
	if( m_renderer )
		m_renderer->Shutdown();

	//s_theSoundSystem->Shutdown();
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void TheApp::ProcessInput() 
{
	if( m_inputSystem )
	{
		m_inputSystem->Update();

		g_theWorld->ProcessInput( m_inputSystem );

		if( m_inputSystem->IsReadyToQuit() )
			m_isRunning = false;
		if( m_inputSystem->WasKeyJustReleased( VK_END ) )
			m_debugModeEnabled = !m_debugModeEnabled;

	}
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
bool TheApp::ProcessKeyDown( int keyData )
{
	UNUSED( keyData );
	return false;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
bool TheApp::ProcessKeyUp( int keyData )
{
	UNUSED( keyData );
	return false;
}


///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void TheApp::Update() 
{
	//if( s_theSoundSystem )
	//	s_theSoundSystem->Update();

	static double lastTimeSeconds = GetCurrentSeconds();
	double currentTimeSeconds = GetCurrentSeconds();
	double deltaSeconds	= currentTimeSeconds - lastTimeSeconds;
	
	if( deltaSeconds > 0.5 )
		deltaSeconds = 0.5;

	if( g_theWorld )
		g_theWorld->Update( m_inputSystem, deltaSeconds, m_debugModeEnabled );

	lastTimeSeconds = currentTimeSeconds;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void TheApp::Render()
{
	static double lastFrameTime = GetCurrentSeconds();
	double timeNow = GetCurrentSeconds();

// 	double timeSinceLastFrame = timeNow - lastFrameTime;

	lastFrameTime = timeNow;
	++ m_frameNumber;

	if( !m_renderer )
		return;

	float fieldOfViewDegreesVertical = 45.0f;
	float aspectRatio = ( 16.0f / 9.0f );
	float nearDepth = 0.1f;
	float farDepth = 1000.0f;

	m_renderer->Clear( Rgba::GREY );
	m_renderer->SetPerspective(fieldOfViewDegreesVertical, aspectRatio, nearDepth, farDepth );
	RenderWorld();

	//ConsolePrintf( "Frame #%i began at time=%.08f seconds (frametime = %.08f s).\n", m_frameNumber, timeNow, timeSinceLastFrame );
	m_renderer->SwapBuffers();
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void TheApp::RenderWorld() 
{
	g_theWorld->Draw( m_renderer, m_debugModeEnabled );
}