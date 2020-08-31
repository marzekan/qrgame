#include <Windows.h>
#include "utils.h"

<<<<<<< HEAD
LRESULT CALLBACK WindowProcess(HWND hwnd, UINT msg, WPARAM wparam, 
                                LPARAM lparam)
=======
#define WND_HEIGHT 500
#define WND_WIDTH  500

const char g_className[] = "myWindowClass";

LRESULT CALLBACK WndProc (HWND hwnd, UINT msg,
    WPARAM wParam, LPARAM lParam)
>>>>>>> cec2a24cc9074043d3a64ae2d8c592dc30741d0c
{
    switch (msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
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
    const char CLASS_NAME[] ="The Game";

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