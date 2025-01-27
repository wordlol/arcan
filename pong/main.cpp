#include "windows.h"
#include "math.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    float x[3], y[3], dx[3], dy[3];

    for (int j = 0; j < 3; j++)
    {
        x[j] = rand() % 2000;
        y[j] = rand() % 1000;
        dx[j] = 0;
        dy[j] = 0;
    }

    HWND hWnd[3];

    for (int i = 0; i < 3; i++)
    {
        hWnd[i] = CreateWindow("edit", 0, WS_POPUP | WS_VISIBLE, x[i], y[i], 30, 30, 0, 0, 0, 0);
        UpdateWindow(hWnd[i]);
    }

    while (!GetAsyncKeyState(VK_ESCAPE))
    {
        POINT mouse;
        GetCursorPos(&mouse);

        for (int i = 0; i < 3; i++)
        {
            float deltaX = mouse.x - x[i];
            float deltaY = mouse.y - y[i];
            float len = sqrt(deltaX * deltaX + deltaY * deltaY);

            if (len < 15) 
            {
                MessageBox(hWnd[i], "game over", "!", 0);

                for (int j = 0; j < 3; j++)
                {
                    x[j] = rand() % 1000;
                    y[j] = rand() % 1000;
                }
            }

            float mix = (5-i*i)*.01;
            dx[i] = (pow(i+1,5) + 20) * deltaX / len * mix + dx[i] * (1-mix);
            dy[i] = (pow(i+1,5) + 20) * deltaY / len * mix + dy[i] * (1 - mix);


            x[i] += dx[i];
            y[i] += dy[i];

            SetWindowPos(hWnd[i], NULL, x[i], y[i], 0, 0, SWP_NOSIZE);

        }

        Sleep(16);//ждем 16 милисекунд (1/количество кадров в секунду)
    }
}
