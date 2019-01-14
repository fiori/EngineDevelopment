#include "../Headers/window.h"
#include "../Headers/Graphics.h"
#include <cassert>
#include <sstream>

Window::Window(HINSTANCE instanceHandle, std::wstring WindowName, int show)
	:m_hInst(instanceHandle), m_MainWndCaption(WindowName)
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
		m_MainWndCaption.c_str(),
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
	ShowCursor(false);

	m_Timer.Reset();
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
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
void Window::CalculateFrameStats()
{
	// Code computes the average frames per second, and also
	// the average time it takes to render one frame. The stats
	// are appended to the window caption bar.

	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	//Compute averages over one second period.
	if ((m_Timer.GameTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1

		std::wostringstream outs;
		outs.precision(6);
		outs << m_MainWndCaption << L" "
			<< L"FPS: " << fps << L" ";
		SetWindowText(m_MainWnd, outs.str().c_str());

		//Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}

	/*std::wostringstream outs;
	outs.precision(6);
	outs << m_Timer.GameTime() << L" (ms)";
	SetWindowText(m_MainWnd, outs.str().c_str());*/
}