#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void setText(HWND pHwnd, int width, int height, RECT rect);

void pkgInfo(HWND pHwnd);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Register the window class.
    const wchar_t CLASS_NAME[] = L"Sample Window Class";

    WNDCLASS wc = {};

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Create the window.

    HWND hwnd = CreateWindowEx(
            0,                              // Optional window styles.
            CLASS_NAME,                     // Window class
            L"Learn to Program Windows",    // Window text
            WS_OVERLAPPEDWINDOW,            // Window style

            // Size and position
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

            NULL,       // Parent window
            NULL,       // Menu
            hInstance,  // Instance handle
            NULL        // Additional application data
    );

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    // Run the message loop.

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int) msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_SIZING: {
            pkgInfo(hwnd);
            break;
        }
        case WM_PAINT: {
            RECT rect;
            GetClientRect(hwnd, &rect);
            int width = rect.right - rect.left;
            int height = rect.bottom - rect.top;
            setText(hwnd, width, height, rect);
            // 双缓冲绘制

            break;
        }
        case WM_SYSCOMMAND:{
            switch (wParam &0xFFF0) {
                case SC_MINIMIZE:{
                    ShowWindow(hwnd,SW_MAXIMIZE);
                    pkgInfo(hwnd);
                    return 0;
                }
                case SC_RESTORE:
                {
                    ShowWindow(hwnd,SW_RESTORE);
                    pkgInfo(hwnd);
                    return 0;
                }
            }
            break;
        }
            return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void pkgInfo(HWND hwnd)
{
    RECT rect;
    GetClientRect(hwnd, &rect);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    setText(hwnd, width, height, rect);
    InvalidateRect(hwnd, NULL, TRUE);
}
void setText(HWND hwnd, int width, int height, RECT rect) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    HDC memdc = CreateCompatibleDC(hdc);
    HBITMAP membitmap = CreateCompatibleBitmap(hdc, width, height);
    SelectObject(memdc, membitmap);

    // 在内存缓冲区中绘制内容
    HFONT hFont = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                             DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                             VARIABLE_PITCH, TEXT("Arial"));
    SelectObject(memdc, hFont);
    SetTextColor(memdc, RGB(0, 0, 255));
    SetBkMode(memdc, TRANSPARENT);
    DrawText(memdc, TEXT("居中文本"), -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // 将内存缓冲区中的内容绘制到屏幕上
    BitBlt(hdc, 0, 0, width, height, memdc, 0, 0, SRCCOPY);
    // 释放资源
    DeleteObject(hFont);
    DeleteObject(membitmap);
    DeleteDC(memdc);
    EndPaint(hwnd, &ps);
}