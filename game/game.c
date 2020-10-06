#include <windows.h>
#include <stdlib.h>
#include <time.h>

#define HEIGHT 535
#define WIDTH 510

#define MAX_OBSTICLES 4
#define MIN_OBSTICLES 3

#define WHITE (COLOR_WINDOW + 1)
#define BLACK (COLOR_WINDOW + 4)

#define SLEEP_TIME 120

short obs_size;

RECT player;
RECT *obsticles;

HANDLE mvObsHandle;

void check_collision(HWND hw, RECT *collision)
{
    for (short i = 0; i < obs_size; i++)
    {
        IntersectRect(collision, &player, (RECT *)&obsticles[i]);

        if (!IsRectEmpty(collision))
        {
            PostMessage(hw, WM_DESTROY, 0, 0);
        }
    }
}

void make_obsticles(short *rand_position)
{
    if (obsticles)
        free(obsticles);

    obs_size = (rand() % MAX_OBSTICLES) + MIN_OBSTICLES;
    obsticles = calloc(obs_size, sizeof(RECT));

    for (short i = 0; i < obs_size; i++)
    {
        *rand_position = rand() % MAX_OBSTICLES;
        SetRect(&obsticles[i], 102 * (*rand_position), 0, 102 + (102 * (*rand_position)), 100);
    }
}

DWORD WINAPI move_obsticles(LPVOID lparam)
{
    HWND hw = *(HWND *)lparam;
    HDC hdc = GetDC(hw);
    RECT collision;
    short rand_position;

    srand(time(NULL));

    make_obsticles(&rand_position);

    while (1)
    {
        hdc = GetDC(hw);

        if (obsticles[0].bottom >= HEIGHT)
        {
            for (short i = 0; i < obs_size; i++)
            {
                FillRect(hdc, &obsticles[i], (HBRUSH)BLACK);
            }

            make_obsticles(&rand_position);
        }
        else
        {

            for (short i = 0; i < obs_size; i++)
            {
                FillRect(hdc, &obsticles[i], (HBRUSH)BLACK);
                OffsetRect(&obsticles[i], 0, +20);
                check_collision(hw, &collision);
                FillRect(hdc, &obsticles[i], (HBRUSH)WHITE);
            }
        }

        ReleaseDC(hw, hdc);
        UpdateWindow(hw);

        Sleep(SLEEP_TIME);
    }

    return 0;
}

LRESULT CALLBACK WindowProcess(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    HDC hdc;
    PAINTSTRUCT ps;

    short bonus = 0;
    short speed = 20;
    short offset = 0;

    switch (msg)
    {
    case WM_CREATE:
        SetRect(&player, 230, 445, 265, 485);
        break;

    case WM_DESTROY:
        SuspendThread(mvObsHandle);

        if (MessageBox(hwnd, "Game over", "Evade!", MB_OK) == IDOK)
        {
            PostQuitMessage(0);
            DestroyWindow(hwnd);
            exit(0);
        }
        break;

    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);

        FillRect(hdc, &player, (HBRUSH)WHITE);

        for (short i = 0; i < obs_size; i++)
        {
            FillRect(hdc, &obsticles[i], (HBRUSH)WHITE);
        }

        EndPaint(hwnd, &ps);
        break;

    case WM_KEYDOWN:
        hdc = GetDC(hwnd);
        FillRect(hdc, &player, (HBRUSH)BLACK);

        bonus == 50 ? bonus = 0 : bonus;

        switch (wparam)
        {
        case VK_LEFT:
            offset = (player.left - (speed + bonus)) <= 0 ? player.left : speed + bonus;
            OffsetRect(&player, -offset, 0);
            break;

        case VK_RIGHT:
            offset = (player.right + (speed + bonus)) >= 500 ? (500 - player.right) : speed + bonus;
            OffsetRect(&player, +offset, 0);
            break;
        }

        bonus += 10;

        FillRect(hdc, &player, (HBRUSH)WHITE);
        ReleaseDC(hwnd, hdc);

        break;

    default:
        return DefWindowProc(hwnd, msg, wparam, lparam);
        break;
    }

    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE pInstance, LPSTR cmd, int showCmd)
{
    // Init app
    WNDCLASS wc = {0};
    const char CLASS_NAME[] = "Evade!";

    wc.lpszClassName = CLASS_NAME;
    wc.lpfnWndProc = WindowProcess;
    wc.hInstance = hInstance;
    wc.hbrBackground = (HBRUSH)BLACK;

    RegisterClass(&wc);

    // Create window
    HWND hwnd;

    RECT desktop;
    HWND desktopHandle = GetDesktopWindow();
    GetWindowRect(desktopHandle, &desktop);

    hwnd = CreateWindow(CLASS_NAME, CLASS_NAME,
                        WS_OVERLAPPED | WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                        (desktop.right / 2) - HEIGHT / 2,
                        (desktop.bottom / 2) - WIDTH / 2,
                        WIDTH, HEIGHT, 0, 0, hInstance, 0);

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