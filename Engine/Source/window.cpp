#include "../Headers/window.h"
#include "../Headers/Graphics.h"
#include <cassert>

Window::Window(HINSTANCE instanceHandle, int show)
	:m_hInst(instanceHandle)
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = _HandleMsgSetup;
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

LRESULT Window::_HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//use create parameter passed in from CreateWindow() to store window class pointer at WinAPI side
	if (msg == WM_NCCREATE)
	{
		//extract ptr to window class from creation data
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = reinterpret_cast<Window*>(pCreate->lpCreateParams);
		//Sanity Check
		assert(pWnd != nullptr);
		// set WinAPI-managed user data to store ptr to window class
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		// set message proc to normal (non-setup) handler now that setup is finished
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::_HandleMsgThunk));
		//forward message to window class handler
		return pWnd->WndProc(hWnd, msg, wParam, lParam);
	}
	//if we get a message before the WM_NCCREATE message, handle with default handler
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT Window::_HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//retrieve ptr to window class
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	//forward message to window class handler
	return pWnd->WndProc(hWnd, msg, wParam, lParam);
}

bool Window::Run()
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
		/*if (!(lParam & 0x40000000) || kbd.AutorepeatIsEnabled())
		{*/
		kbd.OnKeyPressed(static_cast<unsigned char>(wParam));
		/*}*/
		if (wParam == VK_ESCAPE)
			PostQuitMessage(0); // Exit the application
			//DestroyWindow(hWnd); Destroys the open window it self
		if (wParam == VK_F5)
		{
			MessageBox(hWnd, L"boiiii", L"Suck a big dick", 0);
		}
		break;
	case WM_KEYUP:
		kbd.OnKeyReleased(static_cast<unsigned char>(wParam));
		break;
	case WM_MOUSEMOVE:
		mouse.m_x = LOWORD(lParam);
		mouse.m_y = HIWORD(lParam);
		break;
	case WM_LBUTTONDOWN:
		mouse.m_leftIsPressed = true;
		return 0;
	case WM_LBUTTONUP:
		mouse.m_leftIsPressed = false;
		return 0;
	case WM_RBUTTONDOWN:
		mouse.m_rightIsPressed = true;
		return 0;
	case WM_RBUTTONUP:
		mouse.m_rightIsPressed = false;
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}