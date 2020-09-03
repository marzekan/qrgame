#include <windows.h>
#include "utils.h"

RECT rect;
short bonus = 0;
short speed = 20;
char overflow = 0;
short offset = 0;

RECT obsticles[5];

void make_obsticles()
{
    for (short i = 0; i < 5; i++)
    {
        SetRect(&obsticles[i], 102*i, 0, 102+(102*i), 100);
    }
    
}

LRESULT CALLBACK WindowProcess(HWND hwnd, UINT msg, WPARAM wparam, 
                                LPARAM lparam)
{
    HDC hdc;
    PAINTSTRUCT ps;
    HBRUSH hbrush = (HBRUSH) COLOR_WINDOW;
    HBRUSH bckgnd = (HBRUSH) (COLOR_WINDOW+4);
    

    switch (msg)
    {
    case WM_CREATE:
        SetRect(&rect, 230, 445, 265, 485);
        make_obsticles();
        break;

    case WM_DESTROY:
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
        
    
    default:
        return DefWindowProc (hwnd, msg, wparam, lparam);
        break;
    }

    return 0;
}

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE pInstance,
                    LPSTR cmd, int showCmd)
{
    // Init app
    WNDCLASS wc = {0};
    const char CLASS_NAME[] ="Evade!";

    wc.lpszClassName    = CLASS_NAME;
    wc.lpfnWndProc      = WindowProcess;
    wc.hInstance        = hInstance;
    wc.hbrBackground    = (HBRUSH) (COLOR_WINDOW + 4);

    RegisterClass(&wc);

    // Create window
    HWND hwnd;

    hwnd = CreateWindow (CLASS_NAME, CLASS_NAME,
                        WS_OVERLAPPED | WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                        0, 0, 510, 535, 0, 0, hInstance, 0);

    if (hwnd == NULL)
        return 0;

    ShowWindow(hwnd, showCmd);

    // Game loop
    MSG msg = {0};
    short running = 1;

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

    return 0;
}