#include "windows.h"

int x = 0;
int y = 0;
int w = 100;
int h = 100;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    
    HWND hWnd = CreateWindow("game", 0, WS_VISIBLE, x, y, w, h, 0, 0, 0, 0);
    UpdateWindow(hWnd);
    ShowWindow(hWnd, SW_SHOW);

    while (!GetAsyncKeyState(VK_ESCAPE))
    {
        ShowWindow(hWnd, SW_SHOW);
        Sleep(16);//ждем 16 милисекунд (1/количество кадров в секунду)
       


    }

}
