//linker::system::subsystem  - Windows(/ SUBSYSTEM:WINDOWS)
//configuration::advanced::character set - not set
//linker::input::additional dependensies Msimg32.lib; Winmm.lib

#include "windows.h"
#include "math.h"

// ������ ������ ����  
typedef struct {
    float x, y, width, height, rad, dx, dy, speed;
    HBITMAP hBitmap;//����� � ������� ������ 
    bool active;
} sprite;

sprite racket;//������� ������
/*sprite enemy;*///������� ����������
sprite ball;//�����

const int horizont = 20;
const int vertical = 5;
sprite walls[horizont][vertical];

struct {
    int score, balls, x, xx, y, yy, z,w;//���������� ��������� ����� � ���������� "������"
    bool action = false;//��������� - �������� (����� ������ ������ ������) ��� ����
} game;

struct {
    HWND hWnd;//����� ����
    HDC device_context, context;// ��� ��������� ���������� (��� �����������)
    int width, height;//���� �������� ������� ���� ������� ������� ���������
} window;

HBITMAP hBack;// ����� ��� �������� �����������

//c����� ����

void InitGame()
{
    //� ���� ������ ��������� ������� � ������� ������� gdi
    //���� ������������� - ����� ������ ������ ����� � .exe 
    //��������� ������ LoadImageA ��������� � ������� ��������, ��������� �������� ����� ������������� � ������� ���� �������
    ball.hBitmap = (HBITMAP)LoadImageA(NULL, "ball.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    racket.hBitmap = (HBITMAP)LoadImageA(NULL, "racket.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    //enemy.hBitmap = (HBITMAP)LoadImageA(NULL, "racket_enemy.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    hBack = (HBITMAP)LoadImageA(NULL, "back.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    //------------------------------------------------------

   // racket.width = 300;
    racket.width = window.width; // **��� �����
    racket.height = 50;
    racket.speed = 30;//�������� ����������� �������
    racket.x = window.width / 2.;//������� ���������� ����
    racket.y = window.height - racket.height;//���� ���� ���� ������ - �� ������ �������

    //enemy.x = racket.x;//� ���������� �������� ������ � �� �� ����� ��� � ������

    ball.dy = (rand() % 65 + 35) / 100.;//��������� ������ ������ ������
    ball.dx = -(1 - ball.dy);//��������� ������ ������ ������
    ball.speed = 11;
    ball.rad = 20;
    ball.x = racket.x;//x ���������� ������ - �� ������� �������
    ball.y = racket.y - ball.rad;//����� ����� ������ �������

    game.score = 0;
    game.balls = 9;
    game.x = 0;
    game.y = 0;
    game.z = 0;

    //�������� �������� ������
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

void ProcessSound(const char* name)//������������ ���������� � ������� .wav, ���� ������ ������ � ��� �� ����� ��� � ���������
{
    //PlaySound(TEXT(name), NULL, SND_FILENAME | SND_ASYNC);//���������� name �������� ��� �����. ���� ASYNC ��������� ����������� ���� ���������� � ����������� ���������
}

void ShowScore()
{
    //�������� �������� � �������
    SetTextColor(window.context, RGB(191, 180, 180));
    SetBkColor(window.context, RGB(0, 0, 0));
    SetBkMode(window.context, TRANSPARENT);
    auto hFont = CreateFont(70, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 2, 0, "CALIBRI");
    auto hTmp = (HFONT)SelectObject(window.context, hFont);

    char txt[32];//����� ��� ������
    _itoa_s(game.score, txt, 10);//�������������� �������� ���������� � �����. ����� �������� � ���������� txt
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

    hMemDC = CreateCompatibleDC(hDC); // ������� �������� ������, ����������� � ���������� �����������
    hOldbm = (HBITMAP)SelectObject(hMemDC, hBitmapBall);// �������� ����������� bitmap � �������� ������

    if (hOldbm) // ���� �� ���� ������, ���������� ������
    {
        GetObject(hBitmapBall, sizeof(BITMAP), (LPSTR)&bm); // ���������� ������� �����������

        if (alpha)
        {
            TransparentBlt(window.context, x, y, x1, y1, hMemDC, 0, 0, x1, y1, RGB(0, 0, 0));//��� ������� ������� ����� ����� ��������������� ��� ����������
        }
        else
        {
            StretchBlt(hDC, x, y, x1, y1, hMemDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY); // ������ ����������� bitmap
        }

        SelectObject(hMemDC, hOldbm);// ��������������� �������� ������
    }

    DeleteDC(hMemDC); // ������� �������� ������
}

void ShowRacketAndBall()
{
    ShowBitmap(window.context, 0, 0, window.width, window.height, hBack);//������ ���
    ShowBitmap(window.context, racket.x - racket.width / 2., racket.y, racket.width, racket.height, racket.hBitmap);// ������� ������
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

    ShowBitmap(window.context, ball.x - ball.rad, ball.y - ball.rad, 2 * ball.rad, 2 * ball.rad, ball.hBitmap, true);// �����
}

void LimitRacket()
{
    racket.x = max(racket.x, racket.width / 2.);//���� ��������� ������ ���� ������� ������ ����, �������� �� ����
    racket.x = min(racket.x, window.width - racket.width / 2.);//���������� ��� ������� ����
}

void CheckWalls()
{
    if (ball.x < ball.rad || ball.x > window.width - ball.rad)
    {
        ball.dx *= -1;
        ProcessSound("bounce.wav");
    }
}

void CheckBricks()
{
   
    float X = ball.dx * ball.speed;
    float Y = ball.dy * ball.speed;
    float C = sqrt(X * X + Y * Y);
    float X_pixel;
    float Y_pixel;

    for (int iii = 0; iii < C*10; iii++)
    {
        int iterator_back = (C*10 - iii);

        X_pixel = iii * X / C + ball.x ;
        Y_pixel = iii * Y / C + ball.y ;
          
        SetPixel(window.context, X_pixel, Y_pixel, RGB(173, 3, 252));

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
                            
                            

                           
                            //ball.dx *= -1;
                        }
                        else
                        {
                            if (Y_pixel - walls[i][ii].y < walls[i][ii].y + walls[i][ii].height - Y_pixel) // ������
                            {
                                
                                    int posY_trace_in_brick;
                                    int posX_trace_in_brick;

                                    for (int Z = 0;Z < walls[i][ii].height;Z++)
                                    {
                                        for (int Zz = 0;Zz < walls[i][ii].width;Zz++)
                                        {
                                            if (walls[i][ii].x + Zz == X_pixel)
                                            {
                                                if (walls[i][ii].y + Z == Y_pixel)
                                                {

                                                    int posY_trace_in_brick = walls[i][ii].y + Z;
                                                    int posX_trace_in_brick = walls[i][ii].x + Zz;

                                                }
                                            }
                                        }
                                    }

                                int PointY_mirror = ball.y;
                                int PointX_mirror = X_pixel - (ball.x - X_pixel);
                                int C_M = sqrt(PointX_mirror * PointX_mirror + PointY_mirror * PointY_mirror);

                                int Mirror_pixelX = posX_trace_in_brick - iterator_back * PointX_mirror / C_M;
                                int Mirror_pixelY = posY_trace_in_brick - iterator_back * PointY_mirror / C_M;
                                SetPixel(window.context, Mirror_pixelX, Mirror_pixelY, RGB(75, 248, 42));
                                
                            }
                            else // �����
                            {

                                

                            }
                          /*  game.x = ball.x;
                            game.y = ball.y;*/
                                    
                            //ball.dy *= -1;
                        }
                           // game.z = Y_pixel; //�������� ���������
                         //return;
                    } 
                }
            }

    }
    


    /*for (int i = 0; i < horizont; i++)
    {
        for (int ii = 0; ii < vertical; ii++)
        {
            if (walls[i][ii].active == true &&
                ball.x > walls[i][ii].x &&
                ball.x < walls[i][ii].x + walls[i][ii].width &&
                ball.y > walls[i][ii].y &&
                ball.y < walls[i][ii].y + walls[i][ii].height
                )
            {
                
                ��� ��������� �� ����� ��������� �� ����
                int X_left = ball.x - walls[i][ii].x;
                int X_right = walls[i][ii].x + walls[i][ii].width - ball.x;
                int Y_up = ball.y - walls[i][ii].y;
                int Y_down = walls[i][ii].y + walls[i][ii].height - ball.y;

                int minX = min(X_left, X_right);
                int minY = min(Y_up, Y_down);

                if (minX < minY)
                {
                    ball.dx *= -1;
                }
                else
                {
                    ball.dy *= -1;
                }

                walls[i][ii].active = false;
                game.score++;
                return;
            }

        }
    }*/

    


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
    if (ball.y > window.height - ball.rad - racket.height)//����� ������� ����� ������� - ����������� �������
    {
        if (!tail && ball.x >= racket.x - racket.width / 2. - ball.rad && ball.x <= racket.x + racket.width / 2. + ball.rad)//����� �����, � �� �� � ������ ��������� ������
        {
            game.score++;//�� ������ ������� ���� ���� ����
            ball.speed += 5. / game.score;//�� ����������� ��������� - ���������� �������� ������
            ball.dy *= -1;//������
            racket.width -= 10. / game.score;//������������� ��������� ������ ������� - ��� ���������
            ProcessSound("bounce.wav");//������ ���� �������

        }
        else
        {//����� �� �����

            tail = true;//����� ������ ������ ���� �������

            if (ball.y - ball.rad > window.height)//���� ����� ���� �� ������� ����
            {
                game.balls--;//��������� ���������� "������"

                ProcessSound("fail.wav");//������ ����

                if (game.balls < 0) { //�������� ������� ��������� "������"

                    MessageBoxA(window.hWnd, "game over", "", MB_OK);//������� ��������� � ���������
                    InitGame();//������������������ ����
                }

                ball.dy = (rand() % 65 + 35) / 100.;//������ ����� ��������� ������ ��� ������
                ball.dx = -(1 - ball.dy);
                ball.x = racket.x;//�������������� ���������� ������ - ������ ��� �� �������
                ball.y = racket.y - ball.rad;
                game.action = false;//���������������� ����, ���� ����� �� ������ ������
                tail = false;
            }
        }
    }
}

void ProcessRoom()
{
    //������������ �����, ������� � ���. ������� - ���� ������� ����� ���� ���������, � ������, ��� ������� �� ����� ������ ������������� ����� ������� �������� ������
    CheckWalls();
    CheckRoof();
    CheckFloor();
    CheckBricks();
}

void ProcessBall()
{
    if (game.action)
    {
        //���� ���� � �������� ������ - ���������� �����
        ball.x += ball.dx * ball.speed;
        ball.y += ball.dy * ball.speed;
    }
    else
    {
        //����� - ����� "��������" � �������
      //  ball.x = racket.x;
    }
}

void InitWindow()
{
    SetProcessDPIAware();
    window.hWnd = CreateWindow("edit", 0, WS_POPUP | WS_VISIBLE | WS_MAXIMIZE, 0, 0, 0, 0, 0, 0, 0, 0);

    RECT r;
    GetClientRect(window.hWnd, &r);
    window.device_context = GetDC(window.hWnd);//�� ������ ���� ������� ����� ��������� ���������� ��� ���������
    window.width = r.right - r.left;//���������� ������� � ���������
    window.height = r.bottom - r.top;
    window.context = CreateCompatibleDC(window.device_context);//������ �����
    SelectObject(window.context, CreateCompatibleBitmap(window.device_context, window.width, window.height));//����������� ���� � ���������
    GetClientRect(window.hWnd, &r);

}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{

    InitWindow();//����� �������������� ��� ��� ����� ��� ��������� � ����
    InitGame();//����� �������������� ���������� ����

    // mciSendString(TEXT("play ..\\Debug\\music.mp3 repeat"), NULL, 0, NULL);
    ShowCursor(NULL);




     
    while (!GetAsyncKeyState(VK_ESCAPE))
    {
        POINT p;
        GetCursorPos(&p);
        ScreenToClient(window.hWnd, &p);
        ball.x = p.x;
        ball.y = p.y;

        ShowRacketAndBall();//������ ���, ������� � �����
        ShowScore();//������ ���� � �����

       //ShowDevtool(); //��������� �������������� ����������


        ProcessInput();//����� ����������
        LimitRacket();//���������, ����� ������� �� ������� �� �����
        ProcessBall();//���������� �����
        ProcessRoom();//������������ ������� �� ���� � �������, ��������� ������ � ��������

        BitBlt(window.device_context, 0, 0, window.width, window.height, window.context, 0, 0, SRCCOPY);//�������� ����� � ����
        Sleep(16);//���� 16 ���������� (1/���������� ������ � �������)


    }
}
