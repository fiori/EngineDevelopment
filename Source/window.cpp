#include "../Headers/window.h"

Window::Window(HINSTANCE instanceHandle, int show)
	:m_hInst(instanceHandle)
{
	WNDCLASS wc;

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hInst;
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = m_ClassName;

	if (!RegisterClass(&wc))
	{
		MessageBox(nullptr, L"RegisterClass Failed", nullptr, 0);
	}

	m_MainWnd = CreateWindow(
		m_ClassName,
		L"Fiori Engine",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		nullptr,
		nullptr,
		m_hInst,
		this);
	if (m_MainWnd == nullptr)
	{
		MessageBox(nullptr, L"Create Window Failed", nullptr, 0);
	}
	ShowWindow(m_MainWnd, show);
	UpdateWindow(m_MainWnd);
}

Window::~Window()
{
	UnregisterClass(m_ClassName, m_hInst);
}

int Window::Run()
{
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
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

LRESULT Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY: //when clicking on the X on the window
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			PostQuitMessage(0); // Exit the application
			//DestroyWindow(hWnd); Destroys the open window it self
		if (wParam == VK_F5)
		{
			MessageBox(hWnd, L"boiiii", L"Suck a big dick", 0);
		}
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}