//linker::system::subsystem  - Windows(/ SUBSYSTEM:WINDOWS)
//configuration::advanced::character set - not set
//linker::input::additional dependensies Msimg32.lib; Winmm.lib

#include "windows.h"
#include "math.h"
#define _USE_MATH_DEFINES
#include <cmath>


// секция данных игры  
typedef struct {
    float x, y, width, height, rad, dx, dy, speed;
    HBITMAP hBitmap;//хэндл к спрайту шарика 
    bool active;
} sprite;

sprite racket;//ракетка игрока
/*sprite enemy;*///ракетка противника
sprite ball;//шарик

const int horizont = 20;
const int vertical = 5;
sprite walls[horizont][vertical];

struct {
    int score, balls, x, xx, y, yy, z,w;//количество набранных очков и оставшихся "жизней"
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
    //в этой секции загружаем спрайты с помощью функций gdi
    //пути относительные - файлы должны лежать рядом с .exe 
    //результат работы LoadImageA сохраняет в хэндлах битмапов, рисование спрайтов будет произовдиться с помощью этих хэндлов
    ball.hBitmap = (HBITMAP)LoadImageA(NULL, "ball.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    racket.hBitmap = (HBITMAP)LoadImageA(NULL, "racket.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    //enemy.hBitmap = (HBITMAP)LoadImageA(NULL, "racket_enemy.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    hBack = (HBITMAP)LoadImageA(NULL, "back.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    //------------------------------------------------------

   // racket.width = 300;
    racket.width = window.width; // **для теста
    racket.height = 50;
    racket.speed = 30;//скорость перемещения ракетки
    racket.x = window.width / 2.;//ракетка посередине окна
    racket.y = window.height - racket.height;//чуть выше низа экрана - на высоту ракетки

    //enemy.x = racket.x;//х координату оппонета ставим в ту же точку что и игрока

    ball.dy = (rand() % 65 + 35) / 100.;//формируем вектор полета шарика
    ball.dx = -(1 - ball.dy);//формируем вектор полета шарика
    ball.speed = 11;
    ball.rad = 20;
    ball.x = racket.x;//x координата шарика - на середие ракетки
    ball.y = racket.y - ball.rad;//шарик лежит сверху ракетки

    game.score = 0;
    game.balls = 0;
    game.x = 0;
    game.y = 0;
    game.z = 0;

    //создание спрайтов блоков
    //enemy
   

    for (int i = 0; i < horizont; i++)
    {
        for (int ii = 0; ii < vertical; ii++)
        {
            walls[i][ii].hBitmap = racket.hBitmap;
            walls[i][ii].width = window.width / horizont;
            walls[i][ii].height = window.height / (vertical * 3);
            walls[i][ii].x = i * walls[i][ii].width;
            walls[i][ii].y = ii * walls[i][ii].height + window.height / 3;
            walls[i][ii].active = true;

        }
    }

}

void ProcessSound(const char* name)//проигрывание аудиофайла в формате .wav, файл должен лежать в той же папке где и программа
{
    //PlaySound(TEXT(name), NULL, SND_FILENAME | SND_ASYNC);//переменная name содежрит имя файла. флаг ASYNC позволяет проигрывать звук паралельно с исполнением программы
}

void ShowScore()
{
    //поиграем шрифтами и цветами
    SetTextColor(window.context, RGB(191, 180, 180));
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

    _itoa_s(game.z, txt, 10);
    TextOutA(window.context, 10, 250, "Side", 5);
    TextOutA(window.context, 200, 250, (LPCSTR)txt, strlen(txt));

    _itoa_s(game.x, txt, 10);
    TextOutA(window.context, 200, 150, (LPCSTR)txt, strlen(txt));

    _itoa_s(game.y, txt, 10);
    TextOutA(window.context, 200, 200, (LPCSTR)txt, strlen(txt));

    _itoa_s(game.xx, txt, 10);
    TextOutA(window.context, 400, 150, (LPCSTR)txt, strlen(txt));

    _itoa_s(game.yy, txt, 10);
    TextOutA(window.context, 400, 200, (LPCSTR)txt, strlen(txt));

}

void ShowDevtool()
{

    SetTextColor(window.context, RGB(250, 65, 65));
    SetBkColor(window.context, RGB(0, 0, 0));
    SetBkMode(window.context, TRANSPARENT);
    auto hFont = CreateFont(30, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 2, 0, "CALIBRI");
    auto hTmp = (HFONT)SelectObject(window.context, hFont);
    char txt[32];

    POINT p;
    if (GetCursorPos(&p))
    {
        game.y = window.height - p.y;
        game.x = window.width - p.x;
        game.yy = p.y;
        game.xx = p.x;

        for (int i = 0;i < window.width;i++)
        {
            _itoa_s(game.z, txt, 10);
            TextOutA(window.context, i, p.y - 15, "-", 1);

            _itoa_s(game.y, txt, 10);
            TextOutA(window.context, p.x - 70, p.y+10, (LPCSTR)txt, strlen(txt));

            _itoa_s(game.yy, txt, 10);
            TextOutA(window.context, p.x+10, p.y+10, (LPCSTR)txt, strlen(txt));
        }
        for (int i = 0;i < window.height;i++)
        {
            _itoa_s(game.w, txt, 10);
            TextOutA(window.context, p.x - 5, i, "|", 1);

            _itoa_s(game.x, txt, 10);
            TextOutA(window.context, p.x - 70, p.y - 50, (LPCSTR)txt, strlen(txt));

            _itoa_s(game.xx, txt, 10);
            TextOutA(window.context, p.x+10, p.y - 50, (LPCSTR)txt, strlen(txt));
        }

    }

}

void ProcessInput()
{
    if (GetAsyncKeyState(VK_LEFT)) racket.x -= racket.speed;
    if (GetAsyncKeyState(VK_RIGHT)) racket.x += racket.speed;

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
    ShowBitmap(window.context, 0, 0, window.width, window.height, hBack);//задний фон
    ShowBitmap(window.context, racket.x - racket.width / 2., racket.y, racket.width, racket.height, racket.hBitmap);// ракетка игрока
    for (int i = 0; i < horizont; i++)
    {
        for (int ii = 0; ii < vertical; ii++)
        {
            if (walls[i][ii].active == true)
            {
                ShowBitmap(window.context, walls[i][ii].x, walls[i][ii].y, walls[i][ii].width, walls[i][ii].height, walls[i][ii].hBitmap);
            }
        }
    }

    ShowBitmap(window.context, ball.x - ball.rad, ball.y - ball.rad, 2 * ball.rad, 2 * ball.rad, ball.hBitmap, true);// шарик
}

void LimitRacket()
{
    racket.x = max(racket.x, racket.width / 2.);//если коодината левого угла ракетки меньше нуля, присвоим ей ноль
    racket.x = min(racket.x, window.width - racket.width / 2.);//аналогично для правого угла
}

void CheckWalls()
{
    if (ball.x < ball.rad || ball.x > window.width - ball.rad)
    {
        ball.dx *= -1;
        ProcessSound("bounce.wav");
    }
}


void Paint_line(
    float start_x, float start_y,
    float iterator,
    float x1,  float y1, 
    float x2, float y2, 
    float *Poxel_x, 
    float *Poxel_y, 
    int R , int G , int B)
{
    float dx = x2 - x1;
    float dy = y2 - y1;
    float C;
    C = sqrt(pow(dx, 2) + pow(dy, 2));
    
    (*Poxel_x) = iterator * dx / C + start_x;
    (*Poxel_y) = iterator * dy / C + start_y;
    SetPixel(window.context, *Poxel_x, *Poxel_y, RGB(R, G, B));
}

void Circle(int x, int y, int R, int x_pixel, int y_pixel)
{
    int x1;
    int y1;
    for (float i = 0.; i <= 360; i++)
    {
            x1 = R * cos(i) + x;
            y1 = R * sin(i) + y;

            int x2_round = R * cos(i) + x_pixel;
            int y2_round = R * sin(i) + y_pixel;

            if (x1 == x_pixel &&
                y1 == y_pixel
                ) // нахождении точки пересечения от луча до края шара
            {
             SetPixel(window.context, x1, y1, RGB(255, 255, 255)); // отрисока точки на радиусе шара и луча
             
             float x2 = R * cos(i + M_PI/2) + x;
             float y2 = R * sin(i + M_PI/2) + y;

             float x3_round = R * cos(i + M_PI / 2) + x_pixel;
             float y3_round = R * sin(i + M_PI / 2) + y_pixel;

             SetPixel(window.context, x2, y2, RGB(255, 0, 255)); //точки 90 градусов

             SetPixel(window.context, x3_round, y3_round, RGB(255, 0, 255)); //точки 90 градусов от точки луча

             for (float ii = 0.; ii < R*2; ii++)
             {

              float Poxel_x1;
              float Poxel_y1;
              Paint_line(
                  x2, y2, 
                  ii, 
                  x2, y2, 
                  x, y, 
                  &Poxel_x1, &Poxel_y1, 
                  255,255,255); // отрисовка линии под 90 градусов от точки x1y1

              float Poxel_x2;
              float Poxel_y2;
              Paint_line(
                  x3_round, y3_round,
                  ii, 
                  x3_round, y3_round,
                  x_pixel, y_pixel,
                  &Poxel_x2, &Poxel_y2,
                  255,0,255); // отрисовка линии от точки x3x3 до точки x_p y_p


              for (float iii = 0.; iii < R*6; iii++)
              {
                  float Poxel_x3;
                  float Poxel_y3;
                  Paint_line(
                      Poxel_x1, Poxel_y1,
                      iii,
                      Poxel_x1, Poxel_y1,
                      Poxel_x2, Poxel_y2,
                      &Poxel_x3, &Poxel_y3,
                      0, 255, 0); // отрисовка линии от точки x3x3 до точки x_p y_p
              }




                 
              }
            }
    }
       
}


void CheckBricks()
{
    float bx = ball.x;
    float by = ball.y;
    float X = ball.dx * ball.speed;
    float Y = ball.dy * ball.speed;
    float C = sqrt(X * X + Y * Y);
    float X_pixel;
    float Y_pixel;
    int Multi_size = 10;
    
 
    for (int iii = 0; iii < C * Multi_size; iii++)
    {
        
        X_pixel = iii * X / C + bx;
        Y_pixel = iii * Y / C + by;

        SetPixel(window.context, X_pixel, Y_pixel, RGB(255, 255, 252));
        Circle(bx, by, ball.rad, X_pixel, Y_pixel);
        for (int i = 0; i < horizont; i++)
            {
                for (int ii = 0; ii < vertical; ii++)
                {
                    if (walls[i][ii].active &&
                        X_pixel > walls[i][ii].x &&
                        X_pixel < walls[i][ii].x + walls[i][ii].width &&
                        Y_pixel > walls[i][ii].y &&
                        Y_pixel < walls[i][ii].y + walls[i][ii].height
                        )
                    {
                        int minX = min(X_pixel - walls[i][ii].x, walls[i][ii].x + walls[i][ii].width - X_pixel);
                        int minY = min(Y_pixel - walls[i][ii].y, walls[i][ii].y + walls[i][ii].height - Y_pixel);

                        
                        if (minX < minY)
                        {
                            bx -= -(X_pixel - bx)*2;
                            X *= -1;     
                            //ball.dx *= -1;
                        }
                        else
                        {
                             by -= -(Y_pixel - by)*2;
                             Y *= -1;
                             // ball.dy *= -1;
                        }
                        
                    } 

                }
            }

    }
    
}

void CheckRoof()
{
    if (ball.y < ball.rad)
    {
        ball.dy *= -1;
        ProcessSound("bounce.wav");
    }
}

bool tail = false;

void CheckFloor()
{
    if (ball.y > window.height - ball.rad - racket.height)//шарик пересек линию отскока - горизонталь ракетки
    {
        if (!tail && ball.x >= racket.x - racket.width / 2. - ball.rad && ball.x <= racket.x + racket.width / 2. + ball.rad)//шарик отбит, и мы не в режиме обработки хвоста
        {
            game.score++;//за каждое отбитие даем одно очко
            ball.speed += 5. / game.score;//но увеличиваем сложность - прибавляем скорости шарику
            ball.dy *= -1;//отскок
            racket.width -= 10. / game.score;//дополнительно уменьшаем ширину ракетки - для сложности
            ProcessSound("bounce.wav");//играем звук отскока

        }
        else
        {//шарик не отбит

            tail = true;//дадим шарику упасть ниже ракетки

            if (ball.y - ball.rad > window.height)//если шарик ушел за пределы окна
            {
                game.balls--;//уменьшаем количество "жизней"

                ProcessSound("fail.wav");//играем звук

                if (game.balls < 0) { //проверка условия окончания "жизней"

                    MessageBoxA(window.hWnd, "game over", "", MB_OK);//выводим сообщение о проигрыше
                    InitGame();//переинициализируем игру
                }

                ball.dy = (rand() % 65 + 35) / 100.;//задаем новый случайный вектор для шарика
                ball.dx = -(1 - ball.dy);
                ball.x = racket.x;//инициализируем координаты шарика - ставим его на ракетку
                ball.y = racket.y - ball.rad;
                game.action = false;//приостанавливаем игру, пока игрок не нажмет пробел
                tail = false;
            }
        }
    }
}

void ProcessRoom()
{
    //обрабатываем стены, потолок и пол. принцип - угол падения равен углу отражения, а значит, для отскока мы можем просто инвертировать часть вектора движения шарика
    CheckWalls();
    CheckRoof();
    CheckFloor();
    CheckBricks();
}

void ProcessBall()
{
    if (game.action)
    {
        //если игра в активном режиме - перемещаем шарик
        ball.x += ball.dx * ball.speed;
        ball.y += ball.dy * ball.speed;
    }
    else
    {
        //иначе - шарик "приклеен" к ракетке
      //  ball.x = racket.x;
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

    // mciSendString(TEXT("play ..\\Debug\\music.mp3 repeat"), NULL, 0, NULL);
    ShowCursor(NULL);




     
    while (!GetAsyncKeyState(VK_ESCAPE))
    {
        
        POINT p;
        GetCursorPos(&p);
        ScreenToClient(window.hWnd, &p);
        ball.x = p.x;
        ball.y = p.y;
        
        ShowRacketAndBall();//рисуем фон, ракетку и шарик
        ShowScore();//рисуем очик и жизни

       //ShowDevtool(); //отрисовка дополнительной информации


        ProcessInput();//опрос клавиатуры
        LimitRacket();//проверяем, чтобы ракетка не убежала за экран
        ProcessBall();//перемещаем шарик
        ProcessRoom();//обрабатываем отскоки от стен и каретки, попадание шарика в картетку

        BitBlt(window.device_context, 0, 0, window.width, window.height, window.context, 0, 0, SRCCOPY);//копируем буфер в окно
        Sleep(16);//ждем 16 милисекунд (1/количество кадров в секунду)


    }
}
