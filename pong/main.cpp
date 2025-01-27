#include "windows.h"
#include "math.h"

HWND hWnd;

float x = 0;
float y = 0;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    
    hWnd = CreateWindow("edit", 0, WS_POPUP | WS_VISIBLE, x, y, 30, 30, 0, 0, 0, 0);
    UpdateWindow(hWnd);

    while (!GetAsyncKeyState(VK_ESCAPE))
    {
        POINT p;
        GetCursorPos(&p);
        float dx = p.x - x;
        float dy = p.y - y;
        float len = sqrt(dx * dx + dy * dy);
        
        if (len < 15) {
            MessageBox(hWnd, "game over", "!", 0);
            x = 0;
            y = 0;
        }

        len *= .1;
        dx /= len;
        dy /= len;
        x += dx;
        y += dy;
        SetWindowPos(hWnd, NULL, x, y, 0, 0, SWP_NOSIZE);
        Sleep(16);//ждем 16 милисекунд (1/количество кадров в секунду)
    }
}
