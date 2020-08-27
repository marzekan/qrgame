#include<windows.h>

LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow)
{
    WNDCLASSW windowClass = {0};

    windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW +3);
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.hInstance = hInst;
    windowClass.lpszClassName = L"myWindowClass";
    windowClass.lpfnWndProc = WindowProcedure;

    if (!RegisterClassW(&windowClass))
        return -1;

    CreateWindowW(L"myWindowClass", L"Game", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 500, 500, 500, 500, NULL, NULL, NULL, NULL);

    MSG msg = {0};

    while( GetMessage(&msg, NULL, NULL, NULL) )
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        DefWindowProcW(hWnd, msg, wp, lp);
        break;
    }
}