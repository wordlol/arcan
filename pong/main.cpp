//linker::system::subsystem  - Windows(/ SUBSYSTEM:WINDOWS)
//configuration::advanced::character set - not set
//linker::input::additional dependensies Msimg32.lib; Winmm.lib

#include "windows.h"
#include <vector>
#include <string>
using namespace std;


// секция данных игры  
typedef struct {
    float x, y, width, height, rad, dx, dy, speed;
    HBITMAP hBitmap;//хэндл к спрайту шарика 
} sprite;

sprite racket;//ракетка игрока

enum class itemID {
    axe,hemlet,sword
};

struct item_ {
    string name;
    sprite Sprite;
};

vector<item_> itemLib;


struct player_ {
    sprite hero_sprite;
    int life = 10;
    int current_location = 0;
    vector <item_> items;
};

player_ player;

struct location_ {
    HBITMAP hBitmap;
    int left_portal;
    int right_portal;
    vector <item_> items;
};

location_ loc[5];

struct {
    int score, balls;//количество набранных очков и оставшихся "жизней"
    bool action = false;//состояние - ожидание (игрок должен нажать пробел) или игра
} game;

struct {
    HWND hWnd;//хэндл окна
    HDC device_context, context;// два контекста устройства (для буферизации)
    int width, height;//сюда сохраним размеры окна которое создаст программа
} window;

HBITMAP hBack;// хэндл для фонового изображения



//cекция кода

void InitGame()
{
    
    item_ i;

    i.name = "axe";
    i.Sprite.x = 1150;
    i.Sprite.y = window.height - 50;;
    i.Sprite.height = 50;
    i.Sprite.width = 50;
    i.Sprite.hBitmap = (HBITMAP)LoadImageA(NULL, "axe.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    itemLib.push_back(i);

    i.name = "hemlet";
    i.Sprite.x = 150;
    i.Sprite.y = window.height - 50;;
    i.Sprite.height = 50;
    i.Sprite.width = 50;
    i.Sprite.hBitmap = (HBITMAP)LoadImageA(NULL, "hemlet.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    itemLib.push_back(i);

    i.name = "sword";
    i.Sprite.x = 2150;
    i.Sprite.y = window.height - 50;;
    i.Sprite.height = 50;
    i.Sprite.width = 50;
    i.Sprite.hBitmap = (HBITMAP)LoadImageA(NULL, "sword.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    itemLib.push_back(i);

    loc[0].items.push_back(itemLib[(int)itemID::axe]);
    loc[0].items.push_back(itemLib[(int)itemID::sword]);
    loc[0].items.push_back(itemLib[(int)itemID::hemlet]);



    loc[0].hBitmap = (HBITMAP)LoadImageA(NULL, "loc0.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    loc[0].left_portal = 2;
    loc[0].right_portal = 1;

    loc[1].hBitmap = (HBITMAP)LoadImageA(NULL, "loc1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    loc[1].left_portal = 0;
    loc[1].right_portal = 2;

    loc[2].hBitmap = (HBITMAP)LoadImageA(NULL, "loc2.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    loc[2].left_portal = 1;
    loc[2].right_portal = 0;

    player.current_location = 0;
    player.hero_sprite.x = 0;
    player.hero_sprite.y = window.height-50;
    player.hero_sprite.width = 50;
    player.hero_sprite.height = 50;
    player.hero_sprite.hBitmap = (HBITMAP)LoadImageA(NULL, "racket.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    
    game.score = 0;
    game.balls = 9;

   
}

void ProcessSound(const char* name)//проигрывание аудиофайла в формате .wav, файл должен лежать в той же папке где и программа
{
    //PlaySound(TEXT(name), NULL, SND_FILENAME | SND_ASYNC);//переменная name содежрит имя файла. флаг ASYNC позволяет проигрывать звук паралельно с исполнением программы
}

void ShowScore()
{
    //поиграем шрифтами и цветами
    SetTextColor(window.context, RGB(160, 160, 160));
    SetBkColor(window.context, RGB(0, 0, 0));
    SetBkMode(window.context, TRANSPARENT);
    auto hFont = CreateFont(70, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 2, 0, "CALIBRI");
    auto hTmp = (HFONT)SelectObject(window.context, hFont);

    char txt[32];//буфер для текста
    _itoa_s(game.score, txt, 10);//преобразование числовой переменной в текст. текст окажется в переменной txt
    TextOutA(window.context, 10, 10, "Score", 5);
    TextOutA(window.context, 200, 10, (LPCSTR)txt, strlen(txt));

    _itoa_s(game.balls, txt, 10);
    TextOutA(window.context, 10, 100, "Balls", 5);
    TextOutA(window.context, 200, 100, (LPCSTR)txt, strlen(txt));
}

void ProcessInput()
{
    if (GetAsyncKeyState(VK_LEFT)) player.hero_sprite.x -= 30;
    if (GetAsyncKeyState(VK_RIGHT)) player.hero_sprite.x += 30;

    if (!game.action && GetAsyncKeyState(VK_SPACE))
    {
        game.action = true;
        ProcessSound("bounce.wav");
    }
}

void ShowBitmap(HDC hDC, int x, int y, int x1, int y1, HBITMAP hBitmapBall, bool alpha = false)
{
    HBITMAP hbm, hOldbm;
    HDC hMemDC;
    BITMAP bm;

    hMemDC = CreateCompatibleDC(hDC); // Создаем контекст памяти, совместимый с контекстом отображения
    hOldbm = (HBITMAP)SelectObject(hMemDC, hBitmapBall);// Выбираем изображение bitmap в контекст памяти

    if (hOldbm) // Если не было ошибок, продолжаем работу
    {
        GetObject(hBitmapBall, sizeof(BITMAP), (LPSTR)&bm); // Определяем размеры изображения

        if (alpha)
        {
            TransparentBlt(window.context, x, y, x1, y1, hMemDC, 0, 0, x1, y1, RGB(0, 0, 0));//все пиксели черного цвета будут интепретированы как прозрачные
        }
        else
        {
            StretchBlt(hDC, x, y, x1, y1, hMemDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY); // Рисуем изображение bitmap
        }

        SelectObject(hMemDC, hOldbm);// Восстанавливаем контекст памяти
    }

    DeleteDC(hMemDC); // Удаляем контекст памяти
}

void ShowRacketAndBall()
{
    ShowBitmap(window.context, 0, 0, window.width, window.height, loc[player.current_location].hBitmap);//задний фон

    for (int i = 0; i < loc[player.current_location].items.size(); i++) {
        auto item = loc[player.current_location].items[i].Sprite;
        ShowBitmap(window.context, item.x, item.y, item.width, item.height, item.hBitmap);
    }


    ShowBitmap(window.context, player.hero_sprite.x, player.hero_sprite.y, player.hero_sprite.width, player.hero_sprite.height, player.hero_sprite.hBitmap );// ракетка игрока
   
}

void LimitRacket()
{
    //racket.x = max(racket.x, racket.width / 2.);//если коодината левого угла ракетки меньше нуля, присвоим ей ноль
    //racket.x = min(racket.x, window.width - racket.width / 2.);//аналогично для правого угла
    if (player.hero_sprite.x <= 0) {
        player.current_location = loc[player.current_location].left_portal;
        player.hero_sprite.x = window.width - player.hero_sprite.width;
    }
    if (player.hero_sprite.x >= window.width) {
        player.current_location = loc[player.current_location].right_portal;
        player.hero_sprite.x = 0;
    }
}


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

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    
    InitWindow();//здесь инициализируем все что нужно для рисования в окне
    InitGame();//здесь инициализируем переменные игры

    //mciSendString(TEXT("play ..\\Debug\\music.mp3 repeat"), NULL, 0, NULL);
    ShowCursor(NULL);
    
    while (!GetAsyncKeyState(VK_ESCAPE))
    {
        ShowRacketAndBall();//рисуем фон, ракетку и шарик
        ShowScore();//рисуем очик и жизни
        BitBlt(window.device_context, 0, 0, window.width, window.height, window.context, 0, 0, SRCCOPY);//копируем буфер в окно
        Sleep(16);//ждем 16 милисекунд (1/количество кадров в секунду)

        ProcessInput();//опрос клавиатуры
        LimitRacket();//проверяем, чтобы ракетка не убежала за экран
    }

}
