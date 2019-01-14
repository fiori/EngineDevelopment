//#include "..\Headers\Window.h"

#include "..\Headers\Window.h"
#include "..\Headers\Graphics.h"
#include "../Headers/Game.h"

//TODO: implement game timer like delta time
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nCmdShow)
{
	
	//GameTimer g_Timer;
	Window wnd(hInst, L"Fiori Engine", nCmdShow);
	Graphics graphics(wnd);
	Game game(wnd, graphics);
	PlaySound(L"Assets/warm-acoustic-music.wav", NULL, SND_FILENAME | SND_LOOP | SND_ASYNC);
	//graphics.GameLoading();
	//g_Timer.Reset();
	while (wnd.Run())
	{
		game.Go();
	}
}