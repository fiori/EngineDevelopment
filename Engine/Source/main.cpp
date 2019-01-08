//#include "..\Headers\Window.h"

#include "..\Headers\Window.h"
#include "..\Headers\Graphics.h"
#include "../Headers/GameTimer.h"

//TODO: implement game timer like delta time
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nCmdShow)
{
	GameTimer g_Timer;
	Window wnd(hInst, nCmdShow);
	Graphics graphics(wnd);
	g_Timer.Reset();
	while (wnd.Run())
	{
		g_Timer.Tick();
		graphics.Input(g_Timer);
		graphics.Render();
	}
}