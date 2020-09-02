#include <windows.h>
#include "utils.h"

RECT rect;
int bonus = 0;
int speed = 15;

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
        SetRect(&rect, 215, 400, 275, 450);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);
        FillRect(hdc, &rect, hbrush);
        EndPaint(hwnd, &ps);
        break;


    case WM_KEYDOWN:
        hdc = GetDC(hwnd);
        FillRect(hdc, &rect, bckgnd);

        if (bonus==30)
        {
            bonus = 0;
        }
        

        switch(wparam)
        {
            case VK_LEFT:
                OffsetRect(&rect, -(speed+bonus), 0);
                bonus += 10;
                break;
            
            case VK_RIGHT:
                OffsetRect(&rect, +(speed+bonus), 0);
                bonus += 10;
                break;
        }

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
                        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                        0, 0, 500, 500, 0, 0, hInstance, 0);

    if (hwnd == NULL)
        return 0;

    ShowWindow(hwnd, showCmd);

    // Game loop
    MSG msg = {0};
    int running = 1;

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