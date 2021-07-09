#include "WindowGL.h"
#include <gl\GL.h>
#include <gl\GLU.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	POINT position = { 100, 100 };
	POINT size = { 800, 600 };
	if (!window.Initialize(hInstance, position, size))
	{
		MessageBox(NULL, "Window initialization failed", "OpenGL app", MB_OK | MB_ICONERROR);
		return EXIT_FAILURE;
	}
	else return window.Run();
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return window.WndProc(hWnd, message, wParam, lParam);
}



LRESULT Window::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SIZE:
		RECT rect;
		GetClientRect(hWnd, &rect);
		userAreaWidth = rect.right - rect.left;
		userAreaHeight = rect.bottom - rect.top;
		break;
	default:
		return (DefWindowProc(hWnd, message, wParam, lParam));
	}
	return 0L;
}

bool Window::Initialize(HINSTANCE hApp, POINT position, POINT size)
{
	char name[] = "Adam Leczkowski OpenGL app";
	WNDCLASSEX wc;
	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; //style
	wc.lpfnWndProc = (WNDPROC)::WndProc; //window procedure
	wc.cbClsExtra = 0; //extra bytes reserved behind Window class
	wc.cbWndExtra = 0; //extra bytes reserved behind Window instance
	wc.hInstance = hApp;
	wc.hIcon = NULL;
	wc.hIconSm = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = name;

	//registration
	if (RegisterClassEx(&wc) == 0) return false;

	//window creation
	hWnd = CreateWindow(name, name, WS_OVERLAPPEDWINDOW, position.x, position.y, size.x, size.y,
		NULL, NULL, hApp, NULL);
	if (hWnd == NULL) return false;

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
	return true;
}

WPARAM Window::Run() 
{
	//main loop
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

bool WindowGL::SetPixelsFormat(HDC hDC) const
{
	PIXELFORMATDESCRIPTOR pixFormatDesc;
	ZeroMemory(&pixFormatDesc, sizeof(pixFormatDesc));
	pixFormatDesc.nVersion = 1;
	pixFormatDesc.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
	pixFormatDesc.iPixelType = PFD_TYPE_RGBA;
	pixFormatDesc.cColorBits = 32; //byte for every channel
	pixFormatDesc.cDepthBits = 32; //z-buffer
	pixFormatDesc.iLayerType = PFD_MAIN_PLANE;
	int pixelsFormat = ChoosePixelFormat(hDC, &pixFormatDesc);
	if (pixelsFormat == 0) return false;
	if (!SetPixelFormat(hDC, pixelsFormat, &pixFormatDesc)) return false;
	return true;
}

bool WindowGL::InitializeWGL(HWND hWnd)
{
	hDC = ::GetDC(hWnd);
	if (!SetPixelsFormat(hDC)) return false;

	hRC = wglCreateContext(hDC);
	if (hRC == NULL) return false;
	if (!wglMakeCurrent(hDC, hRC)) return false;
	return true;
}

void WindowGL::DeleteWGL()
{
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hRC);
	::ReleaseDC(hWnd, hDC);
}

LRESULT WindowGL::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	long result = Window::WndProc(hWnd, message, wParam, lParam);
	switch (message)
	{
	case WM_CREATE:
		if (!InitializeWGL(hWnd))
		{
			MessageBox(NULL, "Getting rendering context failed", "OpenGL app", MB_OK | MB_ICONERROR);
			return EXIT_FAILURE;
		}
		InfoTitle(hWnd);
		break;
	case WM_DESTROY:
		DeleteWGL();
		break;
	}
	return result;
}

void WindowGL::InfoTitle(HWND hWnd)
{
	char buffer[256];
	GetWindowText(hWnd, buffer, 256);
	const GLubyte* version = glGetString(GL_VERSION);
	strcat_s(buffer, " | OpenGL ");
	strcat_s(buffer, (char*)version);
	const GLubyte* vendor = glGetString(GL_VENDOR);
	strcat_s(buffer, " | ");
	strcat_s(buffer, (char*)vendor);
	const GLubyte* renderer = glGetString(GL_RENDERER);
	strcat_s(buffer, " | ");
	strcat_s(buffer, (char*)renderer);
	SetWindowText(hWnd, buffer);
}