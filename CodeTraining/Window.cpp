#include "Window.h"

Window::Window(HINSTANCE hInstance, int show)
	:m_hInstance(hInstance)
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hInstance;
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = L"Fiori Engine";
	wc.lpszClassName = m_ClassName;

	if (!RegisterClass(&wc))
	{
		MessageBox(m_hWnd, L"Failed registering class", 0, 0);
	}

	m_hWnd = CreateWindow(
		m_ClassName,
		L"Fiori Engine",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		nullptr,
		nullptr,
		m_hInstance,
		this);
	if (m_hWnd == nullptr)
	{
		MessageBox(m_hWnd, L"Create window Failed", 0, 0);
	}

	ShowWindow(m_hWnd, show);
	UpdateWindow(m_hWnd);
}

LRESULT Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int Window::Run()
{
	MSG msg;
	while (PeekMessage(&msg, m_hWnd, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if (msg.message == WM_QUIT)
		{
			return false;
		}
	}
	return true;
}

Window::~Window()
{
	UnregisterClass(m_ClassName, m_hInstance);
}