//#include "..\Headers\Window.h"

#include "..\Headers\Window.h"

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nCmdShow)
{
	Window wnd(hInst, nCmdShow);
	while (wnd.Run())
	{
	}
}