#include "windows.h"

HWND hWnd;

int x = 555;
int y = 555;
int w = 100;
int h = 100;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    
    hWnd = CreateWindow("game", 0, WS_POPUP| WS_VISIBLE|WS_MAXIMIZE, 0, 0, 0, 0, 0, 0, 0, 0);
    UpdateWindow(hWnd);
    ShowWindow(hWnd, SW_SHOW);

    while (!GetAsyncKeyState(VK_ESCAPE))
    {
        //SetWindowPos(hWnd, NULL, x, y, w, h, 0);
        ShowWindow(hWnd, SW_SHOW);
        Sleep(16);//ждем 16 милисекунд (1/количество кадров в секунду)
       


    }

}
