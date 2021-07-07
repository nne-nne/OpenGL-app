#pragma once
#ifndef OPENGL_H
#define OPENGL_H
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <cstdlib>

class Window
{
protected:
	HWND wndHndl;
	long userAreaWidth;
	long userAreaHeight;

public:
	Window() :wndHndl(NULL) {};
	LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	bool Initialize(HINSTANCE handle, POINT position, POINT size);
	WPARAM Run();
} window;

#endif // !OPENGL_H

