//linker::system::subsystem  - Windows(/ SUBSYSTEM:WINDOWS)
//configuration::advanced::character set - not set
//linker::input::additional dependensies Msimg32.lib; Winmm.lib

#include "windows.h"
#include <vector>
#include "math.h"

struct {
    HWND hWnd;//хэндл окна
    HDC device_context, context;// два контекста устройства (для буферизации)
    int width, height;//сюда сохраним размеры окна которое создаст программа
} window;


void InitWindow()
{
    SetProcessDPIAware();
    window.hWnd = CreateWindow("edit", 0, WS_POPUP | WS_VISIBLE | WS_MAXIMIZE, 0, 0, 0, 0, 0, 0, 0, 0);

    RECT r;
    GetClientRect(window.hWnd, &r);
    window.device_context = GetDC(window.hWnd);//из хэндла окна достаем хэндл контекста устройства для рисования
    window.width = r.right - r.left;//определяем размеры и сохраняем
    window.height = r.bottom - r.top;
    window.context = CreateCompatibleDC(window.device_context);//второй буфер
    SelectObject(window.context, CreateCompatibleBitmap(window.device_context, window.width, window.height));//привязываем окно к контексту
    GetClientRect(window.hWnd, &r);

}

void drawBack()
{
    RECT rect;
    GetClientRect(window.hWnd, &rect);
    auto blackBrush = CreateSolidBrush(RGB(0, 0, 0));
    FillRect(window.context, &rect, blackBrush);
    DeleteObject(blackBrush);
}


struct Point3D
{
    float x;
    float y;
    float z;
};

void rotateZ(Point3D& p, float angle)
{
    float x1 = p.x * cos(angle) - p.y * sin(angle);
    float y1 = p.x * sin(angle) + p.y * cos(angle);
    p.x = x1;
    p.y = y1;
}

void rotateY(Point3D& p, float angle)
{
    float x1 = p.x * cos(angle) - p.z * sin(angle);
    float z1 = p.x * sin(angle) + p.z * cos(angle);
    p.x = x1;
    p.z = z1;
}

void rotateX(Point3D& p, float angle)
{
    float z1 = p.y * cos(angle) - p.z * sin(angle);
    float y1 = p.y * sin(angle) + p.z * cos(angle);
    p.z = z1;
    p.y = y1;
}

void project(Point3D& p)
{
    float dist = 5;
    float sx = p.x * dist / (p.z + dist);
    float sy = p.y * dist / (p.z + dist);
    p.x = sx; p.y = sy;
}

struct IndexLine
{
    int i[2];

};

struct indexTriangle
{
    int i[3];
};

std::vector<Point3D> vertecies;
std::vector<indexTriangle> indicies;

void DrawLine(Point3D p1, Point3D p2)
{
    float scale = window.width / 54.;
    p1.x = window.width / 2. + p1.x * scale;
    p1.y = window.height / 2. + p1.y * scale;
    p2.x = window.width / 2. + p2.x * scale;
    p2.y = window.height / 2. + p2.y * scale;

    MoveToEx(window.context, p1.x, p1.y, NULL);
    LineTo(window.context, p2.x, p2.y);
}

void drawTriangle(Point3D p1, Point3D p2, Point3D p3)
{
    float scale = .1*window.width / 4.;
    p1.x = window.width / 2. + p1.x * scale;
    p1.y = window.height / 2. + p1.y * scale;
    p2.x = window.width / 2. + p2.x * scale;
    p2.y = window.height / 2. + p2.y * scale;
    p3.x = window.width / 2. + p3.x * scale;
    p3.y = window.height / 2. + p3.y * scale;

    MoveToEx(window.context, p1.x, p1.y, NULL);
    LineTo(window.context, p2.x, p2.y);
    LineTo(window.context, p3.x, p3.y);
    LineTo(window.context, p1.x, p1.y);

}

void Show()
{
    drawBack();
    //SetPixel(window.context, 100, 100, RGB(255, 255, 255));
    HPEN hPenOld;
    HPEN hLinePen;
    COLORREF qLineColor;
    qLineColor = RGB(255, 0, 0);
    hLinePen = CreatePen(PS_SOLID, 7, qLineColor);
    hPenOld = (HPEN)SelectObject(window.context, hLinePen);

    float alpha = timeGetTime() * .001;

    for (int i = 0; i < indicies.size(); i++)
    {
        auto p1 = vertecies[indicies[i].i[0]];
        auto p2 = vertecies[indicies[i].i[1]];
        auto p3 = vertecies[indicies[i].i[2]];

        rotateX(p1, alpha);
        rotateX(p2, alpha);
        rotateX(p3, alpha);
        rotateY(p1, alpha);
        rotateY(p2, alpha);
        rotateY(p3, alpha);
        rotateZ(p1, alpha);
        rotateZ(p2, alpha);
        rotateZ(p3, alpha);
        project(p1);
        project(p2);
        project(p3);

        drawTriangle(p1, p2, p3);
    }


    SelectObject(window.context, hPenOld);
    DeleteObject(hLinePen);

}

void InitGame()
{
    vertecies.push_back({ -1, 1, -1 });
    vertecies.push_back({ 1, 1, -1 });
    vertecies.push_back({ 1, -1, -1 });
    vertecies.push_back({ -1, -1, -1 });
    vertecies.push_back({ -1, 1, 1 });
    vertecies.push_back({ 1, 1, 1 });
    vertecies.push_back({ 1, -1, 1 });
    vertecies.push_back({ -1, -1, 1 });

    indicies.push_back({ 0, 1, 3 });  //1
    indicies.push_back({ 1, 2, 3 });  //2
    indicies.push_back({ 1, 5, 2 });  //3
    indicies.push_back({ 5, 6, 2 });  //4
    indicies.push_back({ 5, 4, 6 });  //5
    indicies.push_back({ 4, 7, 6 });  //6
    indicies.push_back({ 4, 1, 0 });  //7
    indicies.push_back({ 4, 5, 1 });  //8
    indicies.push_back({ 4, 0, 7 });  //9
    indicies.push_back({ 0, 3, 7 });  //10
    indicies.push_back({ 3, 2, 6 });  //11
    indicies.push_back({ 3, 6, 7 });  //12
}


//#define pause

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    
    InitWindow();//здесь инициализируем все что нужно для рисования в окне
    InitGame();//здесь инициализируем переменные игры

  
    while (!GetAsyncKeyState(VK_ESCAPE))
    {
        Show();//рисуем фон, ракетку и шарик
        BitBlt(window.device_context, 0, 0, window.width, window.height, window.context, 0, 0, SRCCOPY);//копируем буфер в окно
        Sleep(16);//ждем 16 милисекунд (1/количество кадров в секунду)

#ifdef pause
        while (!GetAsyncKeyState('P'))
        {
            Sleep(16);
        }
#endif

    }

}
