//#include "..\Headers\Window.h"

#include "..\Headers\Window.h"
#include "..\Headers\Graphics.h"

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nCmdShow)
{
	Window wnd(hInst, nCmdShow);
	Graphics graphics(wnd);
	while (wnd.Run())
	{
		graphics.Render();
	}
}