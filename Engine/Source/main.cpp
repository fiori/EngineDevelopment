//#include "..\Headers\Window.h"

#include "..\Headers\Window.h"
#include "..\Headers\Graphics.h"

//TODO: implement game timer like delta time
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nCmdShow)
{
	//GameTimer g_Timer;
	Window wnd(hInst, L"Fiori Engine", nCmdShow);
	Graphics graphics(wnd);
	graphics.GameLoading();
	//g_Timer.Reset();
	while (wnd.Run())
	{
		//g_Timer.Tick();
		graphics.Input();
		graphics.UpdateModel();
		graphics.Render();
	}
}