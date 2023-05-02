#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <gdiplus.h>
#include "resource.h"
#include <commdlg.h>
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void setWindow(HWND hwnd, HINSTANCE hinstance, RECT &rect, int width, int height);
void drawing(HWND hwnd,HINSTANCE hinstance,RECT &rect);
void printR(HWND hwnd,LPARAM lparam);

HINSTANCE hinstances;
static HDC static_hdc;
static HBITMAP hbitmap;
static int xClient, yCLient;
static HWND btn1;
static HWND btn2;
static HWND btn3;
static HWND btn4;
static HWND btn5;
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPrevINstance, LPSTR lpCmdLine, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"作业2";
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hinstance;
    wc.lpszClassName = CLASS_NAME;
    hinstances = hinstance;
    RegisterClass(&wc);
    HWND hwnd = CreateWindowEx(
            1,                              // Optional window styles.
            CLASS_NAME,                     // Window class
            CLASS_NAME,    // Window text
            WS_OVERLAPPEDWINDOW,            // Window style

            // Size and position
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

            NULL,       // Parent window
            NULL,       // Menu
            hinstance,  // Instance handle
            NULL        // Additional application data
    );

    if (hwnd == NULL) {
        return 0;
    }
    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int) msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static bool isRectanglePressed = false;
    static bool isTrianglePressed = false;
    static bool isRoundPressed = false;
    static bool isColourPressed = false;
    static bool isall = false;
    static COLORREF g_color = RGB(0, 0, 255);
    static bool isDrawing = false;
    static int startX = 0;             // 矩形起点的X坐标
    static int startY = 0;             // 矩形起点的Y坐标
    static int endX = 0;               // 矩形终点的X坐标
    static int endY = 0;               // 矩形终点的Y坐标


    switch (uMsg) {
        case WM_DESTROY: {
            DeleteObject(hbitmap);
            DeleteDC(static_hdc);
            PostQuitMessage(0);
            return 0;
        }
        case WM_SIZE: {
            printR(hwnd,lParam);
            break;
        }
        case WM_PAINT: {
            RECT rect;
            GetClientRect(hwnd, &rect);
            int width = rect.right - rect.left;
            int height = rect.bottom - rect.top;

            setWindow(hwnd, NULL, rect, width, height);
            break;
        }

        case WM_CREATE: {
            RECT rect;
            GetClientRect(hwnd, &rect);
            int width = rect.right - rect.left;
            int height = rect.bottom - rect.top;

            setWindow(hwnd, NULL, rect, width, height);
            break;
        }

        case WM_SYSCOMMAND:
            break;
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case ID_rectangle:
                    if (!isRectanglePressed) {
                        SendMessage(btn1, BM_SETSTATE, TRUE, 0);
                        isRectanglePressed = true;
                        isRoundPressed = false;
                        isTrianglePressed = false;
                    } else {
                        SendMessage(btn1, BM_SETSTATE, FALSE, 0);
                        isRectanglePressed = false;
                    }
                    //MessageBox(hwnd, L"按钮被点击了！", L"提示", MB_OK);
                    break;
                case ID_triangle:
                    if (!isTrianglePressed) {
                        SendMessage(btn3, BM_SETSTATE, TRUE, 0);
                        isTrianglePressed = true;
                        isRoundPressed = false;
                        isRectanglePressed = false;
                    } else {
                        SendMessage(btn3, BM_SETSTATE, FALSE, 0);
                        isTrianglePressed = false;
                    }
                    //MessageBox(hwnd, L"三角形！", L"提示", MB_OK);
                    break;
                case ID_round:
                    if (!isRoundPressed) {
                        SendMessage(btn2, BM_SETSTATE, TRUE, 0);
                        isRoundPressed = true;
                        isTrianglePressed = false;
                        isRectanglePressed = false;
                    } else {
                        SendMessage(btn2, BM_SETSTATE, FALSE, 0);
                        isRoundPressed = false;
                    }
                    //MessageBox(hwnd, L"圆形！", L"提示", MB_OK);
                    break;
                case ID_clour:
                {
                    CHOOSECOLOR cc = { 0 };
                    cc.lStructSize = sizeof(CHOOSECOLOR);
                    cc.hwndOwner = hwnd;
                    cc.Flags = CC_FULLOPEN | CC_RGBINIT;
                    cc.lpCustColors = (LPDWORD)malloc(sizeof(DWORD) * 16);
                    if (ChooseColor(&cc))
                    {
                        COLORREF color = cc.rgbResult;
                        g_color = color;
                        // 使用颜色
                    }
                    free(cc.lpCustColors);

                }
                    //MessageBox(hwnd, L"颜色！", L"提示", MB_OK);
                    break;
                case ID_all:
                    if(!isall){
                        SendMessage(btn5, BM_SETSTATE, TRUE, 0);
                        isall = true;
                    } else {
                        SendMessage(btn5, BM_SETSTATE, FALSE, 0);
                        isall = false;
                    }
            }
            break;
        case WM_LBUTTONDOWN:
            // 鼠标左键按下，开始绘制矩形
            isDrawing = true;
            startX = LOWORD(lParam);
            startY = HIWORD(lParam);
            break;
        case WM_MOUSEMOVE:
            // 鼠标移动，绘制矩形
            if (isDrawing)
            {
                endX = LOWORD(lParam);
                endY = HIWORD(lParam);

                HDC hdc = GetDC(hwnd);
                RECT rect = { startX, startY, endX, endY };
                DrawFocusRect(static_hdc, &rect);
                drawing(hwnd,NULL,rect);


            }
            break;
        case WM_LBUTTONUP:
            isDrawing = false;
            endX = LOWORD(lParam);
            endY = HIWORD(lParam);
            break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void printR(HWND hwnd,LPARAM lparam)
{
    xClient = LOWORD(lparam);
    yCLient = HIWORD(lparam);
    DeleteObject(hbitmap);
    DeleteDC(static_hdc);
    HDC hdc = GetDC(hwnd);
    static_hdc = CreateCompatibleDC(hdc);
    hbitmap = CreateCompatibleBitmap(hdc, xClient, yCLient);
    SelectObject(static_hdc, hbitmap);
    ReleaseDC(hwnd, hdc);
    // 使整个客户区失效，触发重绘
    InvalidateRect(hwnd, NULL, TRUE);
}
void setWindow(HWND hwnd, HINSTANCE hinstance, RECT &rect, int width, int height) {

    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    static_hdc = CreateCompatibleDC(hdc);
    hbitmap = CreateCompatibleBitmap(hdc, xClient, yCLient);
    SelectObject(static_hdc, hbitmap);
    HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
    auto setbrush = (HBRUSH)SelectObject(static_hdc, hBrush);
    FillRect(static_hdc, &rect, hBrush);
    SelectObject(static_hdc, setbrush);
    DeleteObject(setbrush);

    SelectObject(static_hdc,
                 btn1 = CreateWindowEx(0, L"BUTTON", L"矩形", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 10, 10, 100, 30, hwnd,
                                (HMENU) ID_rectangle, hinstances, nullptr));
    SelectObject(static_hdc,
                 btn2 = CreateWindowEx(0, L"BUTTON", L"圆形", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 130, 10, 100, 30, hwnd,
                                (HMENU) ID_round, hinstances, nullptr));
    SelectObject(static_hdc,
                 btn3 = CreateWindowEx(0, L"BUTTON", L"三角形", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 250, 10, 100, 30,
                                hwnd,
                                (HMENU) ID_triangle, hinstances, nullptr));
    SelectObject(static_hdc,
                 btn4 = CreateWindowEx(0, L"BUTTON", L"颜色", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 370, 10, 100, 30, hwnd,
                                (HMENU) ID_clour, hinstances, nullptr));
    SelectObject(static_hdc,
                 btn5 = CreateWindowEx(0, L"BUTTON", L"填充", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 490, 10, 100, 30, hwnd,
                                (HMENU) ID_all, hinstances, nullptr));
    SetTextColor(static_hdc, RGB(255, 255, 255));
    SetBkMode(static_hdc, TRANSPARENT);
    BitBlt(hdc, 0, 0, width, height, static_hdc, 0, 0, SRCCOPY);

    EndPaint(hwnd, &ps);
}
void drawing(HWND hwnd,HINSTANCE hinstance,RECT &rect){
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    static_hdc = CreateCompatibleDC(hdc);
    hbitmap = CreateCompatibleBitmap(hdc, xClient, yCLient);
    SelectObject(static_hdc, hbitmap);
    HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
    auto setbrush = (HBRUSH)SelectObject(static_hdc, hBrush);
    FillRect(static_hdc, &rect, hBrush);
    SelectObject(static_hdc, setbrush);
    DeleteObject(setbrush);
    Rectangle(static_hdc, rect.left,rect.top,rect.right,rect.bottom);
    EndPaint(hwnd, &ps);
}