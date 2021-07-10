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

bool Window::Initialize(HINSTANCE hApp, POINT position, POINT size, bool fullScreen, bool resChange)
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

	//window settings
	DWORD windowStyle = WS_OVERLAPPEDWINDOW;

	if (fullScreen)
	{
		windowStyle = WS_POPUP;
		position.x = 0;
		position.y = 0;
		if (resChange)
		{
			size.x = 1024;
			size.y = 768;
			if (!ChangeResolution(size.x, size.y)) return false;
		}
		else
		{
			RECT screenSize;
			GetWindowRect(GetDesktopWindow(), &screenSize);
			size.x = screenSize.right - screenSize.left;
			size.y = screenSize.bottom - screenSize.top;
		}
	}

	//window creation
	hWnd = CreateWindow(name, name, windowStyle, position.x, position.y, size.x, size.y,
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
		SceneSetup();
		InfoTitle(hWnd);
		break;
	case WM_SIZE:
		SceneSetup();
		break;
	case WM_PAINT:
		DrawScene();
		ValidateRect(hWnd, NULL);
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

void WindowGL::SceneSetup(bool isometric)
{
	glViewport(0, 0, userAreaWidth, userAreaHeight);
	glMatrixMode(GL_PROJECTION); //switch to projection matrix
	glLoadIdentity();
	float ratio = userAreaHeight / (float)userAreaWidth;
	if (!isometric) //frustum clipping
		glFrustum(-1.0f, 1.0f, ratio * -1.0f, ratio * 1.0f, 1.0f, 10.0f);
	else
		glOrtho(-1.0f, 1.0f, ratio * -1.0f, ratio * 1.0f, 1.0f, 10.0f);
	glMatrixMode(GL_MODELVIEW); //back to model view matrix
	glEnable(GL_DEPTH_TEST); //active z-buffer
}

void WindowGL::DrawScene()
{
	const float x0 = 1.0f;
	const float y0 = 1.0f;
	const float z0 = 1.0f;

	//preparing buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clearing frame buffer and depth buffer
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //background
	glLoadIdentity(); //model-view matrix = identity matrix

	glTranslatef(1.0f, 0.0f, -3.0f); //move eye from centre of scene
	glRotatef(35.26f, 1.0f, 0.0f, 0.0f);
	glRotatef(45.0f, 0.0f, 1.0f, 0.0f);
	/*gluLookAt(
		-1.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f);*/
	

	glBegin(GL_TRIANGLES); //drawing triangle
	glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
	glVertex3f(-x0, -y0, 0.0f);
	glColor4f(0.0f, 1.0f, 1.0f, 1.0f);
	glVertex3f(x0, -y0, 0.0f);
	glColor4f(1.0f, 0.0f, 1.0f, 1.0f);
	glVertex3f(0, y0, 0.0f);
	glEnd();

	SwapBuffers(hDC); //from buffer to screen
}

bool Window::ChangeResolution(long width, long height, long colorDepth) const
{
	DEVMODE dmScreenSettings;
	memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
	dmScreenSettings.dmSize = sizeof(dmScreenSettings);
	dmScreenSettings.dmPelsWidth = width;
	dmScreenSettings.dmPelsHeight = height;
	dmScreenSettings.dmBitsPerPel = colorDepth;
	dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
	return ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL;
}