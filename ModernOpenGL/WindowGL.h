#pragma once
#ifndef OPENGL_H
#define OPENGL_H
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <cstdlib>

class Window
{
protected:
	HWND hWnd;
	long userAreaWidth;
	long userAreaHeight;

public:
	Window() :hWnd(NULL) {};
	LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	bool Initialize(HINSTANCE handle, POINT position, POINT size);
	WPARAM Run();
};

class WindowGL:public Window
{
private:
	HGLRC hRC; //rendering context handle
	HDC hDC; //GDI device private context
	bool SetPixelsFormat(HDC hDC) const;
	bool InitializeWGL(HWND hWnd);
	void DeleteWGL();

	void InfoTitle(HWND hWnd);
public:
	WindowGL() :Window(), hRC(NULL), hDC(NULL) {};
	LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
}window;

#endif // !OPENGL_H

