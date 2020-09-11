#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include "utils.h"

#define HEIGHT          535
#define WIDTH           505
#define OBSTICLES       5
#define MAX_OBSTICLES   3
#define MIN_OBSTICLES   1
#define WHITE           COLOR_WINDOW
#define BLACK           (COLOR_WINDOW + 4)

RECT rect;
short bonus   = 0;
short speed   = 20;
char overflow = 0;
short offset  = 0;

RECT obsticles[OBSTICLES];
DWORD tID;
HANDLE mvObsHandle;

// kt - kill thread. Functions that closes thread handle.
void kt (HANDLE *h)
{
    if(h != NULL)
    {
        WaitForSingleObject(h, INFINITE);
        CloseHandle(h);
    }
}

void make_obsticles()
{ 
    /*  
        Right branch
        Generates random number between [MAX_OBSTICLES, MIN_OBSTICLES] 
        e.g. right = 3
        ___________________
        |     |     |     | 
        |_____|_____|_____|
    */
    short right = (short) (rand() % MAX_OBSTICLES + MIN_OBSTICLES);

    for (short i = 0; i < right; i++)
    {
        SetRect(&obsticles[i], 102*i, 0, 102+(102*i), 100);
    }

    /* 
        Left branch
        This part of code should generate remaining part of obsticles
        leaving 1 gap, size of 1 obsticle for sqare to move through, but 
        it does not work :(
        e.g. if right = 3 then left = 1
        ___________________      ______
        |     |     |     |      |     |
        |_____|_____|_____|      |_____|
    */
    short left = OBSTICLES - MIN_OBSTICLES - right; 

    for (short i = 1; i <= left; i++)
    {
        SetRect(&obsticles[OBSTICLES - i], 102*i, 0, 102+(102*i), 100);
    }
}

DWORD WINAPI move_obsticles (LPVOID lparam)
{
    HWND hw = *(HWND*)lparam;
    
    while(1)
    {
        if(obsticles[1].bottom >= HEIGHT)
        {
            make_obsticles();
        }
        else
        {
            for (short i = 0; i < OBSTICLES; i++)
            {
                
                InvalidateRect(hw, &obsticles[i], TRUE);
                SetRect(&obsticles[i], obsticles[i].left, obsticles[i].top + 50, obsticles[i].right, obsticles[i].bottom + 50);
            }
        }

        UpdateWindow(hw);
        Sleep(500);
    }

    return 0;
}

LRESULT CALLBACK WindowProcess(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    HDC hdc;
    PAINTSTRUCT ps;
    HBRUSH hbrush = (HBRUSH) WHITE;
    HBRUSH bckgnd = (HBRUSH) BLACK;

    switch (msg)
    {
    case WM_CREATE:
        SetRect(&rect, 230, 445, 265, 485);
        make_obsticles();
        break;

    case WM_DESTROY:
        kt(&mvObsHandle);
        PostQuitMessage(0);
        break;

    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);
        
        FillRect(hdc, &rect, hbrush);
        
        for (short i = 0; i < 5; i++)
        {
            FillRect(hdc, &obsticles[i], hbrush);
        }
        
        EndPaint(hwnd, &ps);
        break;

    case WM_KEYDOWN:
        hdc = GetDC(hwnd);
        FillRect(hdc, &rect, bckgnd);

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

        FillRect(hdc, &rect, hbrush);
        ReleaseDC(hwnd,hdc);
        break;

    case WM_QUIT:
        kt(&mvObsHandle);
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
    srand((unsigned int)time(NULL));

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

    mvObsHandle = CreateThread(NULL, 0, move_obsticles, &hwnd, 0, &tID);

    while (running)
    {
        while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                running = 0;
            
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    kt(&mvObsHandle);

    return 0;
}