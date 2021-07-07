#include "WindowGL.h"

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

bool Window::Initialize(HINSTANCE appHndl, POINT position, POINT size)
{
	char name[] = "Adam Leczkowski OpenGL app";
	WNDCLASSEX wc;
	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; //style
	wc.lpfnWndProc = (WNDPROC)::WndProc; //window procedure
	wc.cbClsExtra = 0; //extra bytes reserved behind Window class
	wc.cbWndExtra = 0; //extra bytes reserved behind Window instance
	wc.hInstance = appHndl;
	wc.hIcon = NULL;
	wc.hIconSm = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = name;

	//registration
	if (RegisterClassEx(&wc) == 0) return false;

	//window creation
	wndHndl = CreateWindow(name, name, WS_OVERLAPPEDWINDOW, position.x, position.y, size.x, size.y,
		NULL, NULL, appHndl, NULL);
	if (wndHndl == NULL) return false;

	ShowWindow(wndHndl, SW_SHOW);
	UpdateWindow(wndHndl);
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