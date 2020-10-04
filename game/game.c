#include <windows.h>
#include <stdlib.h>
#include <time.h>

#define HEIGHT  535
#define WIDTH   510

#define MAX_OBSTICLES 4
#define MIN_OBSTICLES 3

#define WHITE (COLOR_WINDOW + 1)
#define BLACK (COLOR_WINDOW + 4)

short bonus     = 0;
short speed     = 20;
char overflow   = 0;
short offset    = 0;

short obs_size;
short rand_position;

RECT rect;
RECT returned;
RECT collision;
RECT *obsticles;
HANDLE mvObsHandle;

volatile char create_new_obsticles = 1;
volatile short j;
volatile short k;

void check_collision (HWND hw)
{
    for (j = 0; j < obs_size; j++)
    {
        IntersectRect(&collision, &rect, &obsticles[j]);

        if (!IsRectEmpty(&collision))
            PostMessage(hw, WM_DESTROY, 0, 0);
    }
}

void make_obsticles()
{
    if(obsticles)
        free(obsticles);

    obs_size = (rand() % MAX_OBSTICLES) + MIN_OBSTICLES;
    obsticles = calloc(obs_size, sizeof(RECT));

    for (k = 0; k < obs_size; k++)
    {
        rand_position = rand() % MAX_OBSTICLES;

        SetRect(&obsticles[k], 102 * rand_position, 0, 102 + (102 * rand_position), 100);
    }
}

DWORD WINAPI move_obsticles(LPVOID lparam)
{
    HWND hw = *(HWND*)lparam;
    HDC hdc = GetDC(hw);

    srand(time(NULL));
    
    make_obsticles();
    
    while(create_new_obsticles)
    {   
        hdc = GetDC(hw);

        if(obsticles[0].bottom >= HEIGHT)
        {
            for (j = 0; j < obs_size; j++)
            {
                FillRect(hdc, &obsticles[j], (HBRUSH) BLACK);
            }

            make_obsticles();
        }
        else
        {
            
            for (k = 0; k < obs_size; k++)
            {
                FillRect(hdc, &obsticles[k], (HBRUSH) BLACK);
                OffsetRect(&obsticles[k], 0, +20);
                check_collision(hw);
                FillRect(hdc, &obsticles[k], (HBRUSH) WHITE);
            }
            
        }

        ReleaseDC(hw, hdc);
        UpdateWindow(hw);

        Sleep(120);
    }

    return 0;
}

LRESULT CALLBACK WindowProcess(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    HDC hdc;
    PAINTSTRUCT ps;

    switch (msg)
    {
    case WM_CREATE:
        SetRect(&rect, 230, 445, 265, 485);
        break;

    case WM_DESTROY:
        SuspendThread(mvObsHandle);

        if (MessageBox(hwnd, "Game over", "Evade!", MB_OKCANCEL) == IDOK)
        {
            PostQuitMessage(0);
            DestroyWindow(hwnd);
            exit(0);
        }
        break;

    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);
        
        FillRect(hdc, &rect, (HBRUSH) WHITE);
        
        for (short i = 0; i < 5; i++)
        {
            FillRect(hdc, &obsticles[i], (HBRUSH) WHITE);
        }
        
        EndPaint(hwnd, &ps);
        break;

    case WM_KEYDOWN:
        hdc = GetDC(hwnd);
        FillRect(hdc, &rect, (HBRUSH) BLACK);

        bonus == 50 ? bonus = 0 : bonus;

        switch(wparam)
        {
            case VK_LEFT:
                offset = (rect.left - (speed+bonus)) <= 0 ? rect.left : speed+bonus;
                OffsetRect(&rect, -offset, 0);
                break;
            
            case VK_RIGHT:
                offset = (rect.right + (speed+bonus)) >= 500 ? (500 - rect.right) : speed+bonus;
                OffsetRect(&rect, +offset, 0);
                break;
        }

        bonus += 10;

        FillRect(hdc, &rect, (HBRUSH) WHITE);
        ReleaseDC(hwnd,hdc);

        break;
        
    default:
        return DefWindowProc (hwnd, msg, wparam, lparam);
        break;
    }

    return 0;
}

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE pInstance, LPSTR cmd, int showCmd)
{
    // Init app
    WNDCLASS wc = {0};
    const char CLASS_NAME[] ="Evade!";

    wc.lpszClassName    = CLASS_NAME;
    wc.lpfnWndProc      = WindowProcess;
    wc.hInstance        = hInstance;
    wc.hbrBackground    = (HBRUSH) BLACK;

    RegisterClass(&wc);

    // Create window
    HWND hwnd;

    hwnd = CreateWindow (CLASS_NAME, CLASS_NAME,
                        WS_OVERLAPPED | WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                        0, 0, WIDTH, HEIGHT, 0, 0, hInstance, 0);

    if (hwnd == NULL)
        return 0;

    ShowWindow(hwnd, showCmd);

    // Game loop
    MSG msg = {0};
    short running = 1;

    DWORD tID;
    mvObsHandle = CreateThread(NULL, 0, move_obsticles, &hwnd, 0, &tID);

    while (running)
    {
        while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                create_new_obsticles = 0;
                DWORD status = WaitForSingleObject(mvObsHandle, INFINITE);
                ExitThread(GetExitCodeThread(mvObsHandle, &status));

                running = 0;
            }
            
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return 0;
}