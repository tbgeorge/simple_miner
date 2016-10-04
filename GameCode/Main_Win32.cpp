//=================================================================================
// Main_Win32.cpp
// Author: Tyler George
// Date  : January 13, 2015
//=================================================================================

#pragma once  // VS2010 bug

#define UNUSED(x) (void)(x)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "GameCode/TheApp.hpp"

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
TheApp* s_theApp = NULL;

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
LRESULT __stdcall GameMessageProcessingFunction( HWND windowHandle, UINT messageID, WPARAM wParam, LPARAM lParam ) 
{
	bool wasHandled = false;
	switch (messageID) 
	{
	case WM_KEYDOWN:
		if( wParam == VK_ESCAPE )
			PostQuitMessage( 0 );
		else
			wasHandled = s_theApp->ProcessKeyDown( (int) wParam );
		
		if( wasHandled )
			return 0;
		break;
		
	case WM_KEYUP:
		wasHandled = s_theApp->ProcessKeyUp( (int) wParam );

		if( wasHandled )
			return 0;
		break;

	default:
		break;
	}
	return DefWindowProc( windowHandle, messageID, wParam, lParam );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
HWND CreateAppWindow( HINSTANCE thisAppInstance, int showCommand ) 
{
	WNDCLASSEX windowDescription;
	memset( &windowDescription, 0, sizeof( windowDescription ) );

	windowDescription.cbSize = sizeof( WNDCLASSEX );
	windowDescription.hInstance	= thisAppInstance;
	windowDescription.lpszClassName = TEXT( "Simple Miner" );
	windowDescription.lpfnWndProc = GameMessageProcessingFunction;
	windowDescription.style = CS_HREDRAW | CS_OWNDC | CS_VREDRAW;

	/*ATOM returnValue = */RegisterClassEx( &windowDescription );

	LPCTSTR className = TEXT( "Simple Miner" );
	LPCTSTR windowName = TEXT( "Simple Miner" );
	DWORD windowsStyleFlags = WS_OVERLAPPEDWINDOW;
	DWORD windowsStyleFlagsEx = WS_EX_APPWINDOW;


	RECT desktopWindowRect;
	HWND desktopWindow = GetDesktopWindow();
	GetClientRect( desktopWindow, &desktopWindowRect );

	// MAKE SURE THIS WILL FIT!!

	RECT windowRect;
	windowRect.left = 100;
	windowRect.right = windowRect.left + 1600;
	windowRect.top = 100;
	windowRect.bottom = windowRect.top + 900;

	AdjustWindowRectEx( &windowRect, windowsStyleFlags, false, windowsStyleFlagsEx );
	
	HWND windowHandle = CreateWindow( className, windowName, windowsStyleFlags, windowRect.left, windowRect.top, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, NULL, NULL, thisAppInstance, NULL );

	if( windowHandle ) 
	{
		ShowWindow( windowHandle, showCommand);
		UpdateWindow( windowHandle );
	}

	return windowHandle;
}


///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
int __stdcall WinMain( HINSTANCE thisAppInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
	UNUSED( lpCmdLine );
	UNUSED( hPrevInstance );

	HWND myWindowHandle	= CreateAppWindow(thisAppInstance, nShowCmd );
	s_theApp = new TheApp();

	s_theApp->Startup( (void*)myWindowHandle );
	s_theApp->Run();
	s_theApp->Shutdown();

	
}