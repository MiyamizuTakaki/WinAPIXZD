#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <gdiplus.h>
#include "resource.h"
#include <commdlg.h>
#include <vector>
#include <ntdef.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void setWindow(HWND hwnd, HINSTANCE hinstance, RECT &rect, int width, int height);

void drawing(HWND hwnd, HINSTANCE hinstance, RECT &rect);

void printR(HWND hwnd, LPARAM lparam);

void viewPhoto(HWND hwnd, HINSTANCE hinstance,BITMAP bmp,HBITMAP hBmp,BYTE* pPixels);
std::vector<POINT> points;

HINSTANCE hinstances;
static HDC static_hdc;
static HBITMAP hbitmap;
static int xClient, yCLient;
static HWND btn1;
static HWND btn2;
static HWND btn3;
static HWND btn4;
static HWND btn5;
static HWND btn6;
static CHOOSECOLOR cc = {0};
OPENFILENAME ofn;
WCHAR szFileName[MAX_PATH] = L"";

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
            printR(hwnd, lParam);
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
                case ID_clour: {

                    cc.lStructSize = sizeof(CHOOSECOLOR);
                    cc.hwndOwner = hwnd;
                    cc.Flags = CC_FULLOPEN | CC_RGBINIT;
                    cc.lpCustColors = (LPDWORD) malloc(sizeof(DWORD) * 16);
                    if (ChooseColor(&cc)) {
                        COLORREF color = cc.rgbResult;
                        g_color = color;
                        // 使用颜色
                    }
                    free(cc.lpCustColors);

                }
                    //MessageBox(hwnd, L"颜色！", L"提示", MB_OK);
                    break;
                case ID_all:
                    if (!isall) {
                        SendMessage(btn5, BM_SETSTATE, TRUE, 0);
                        isall = true;
                    } else {
                        SendMessage(btn5, BM_SETSTATE, FALSE, 0);
                        isall = false;
                    }
                case ID_file:
                {
                    ZeroMemory(&ofn,sizeof(OPENFILENAME));
                    ofn.lStructSize = sizeof(OPENFILENAME);
                    ofn.hwndOwner = hwnd;
                    ofn.lpstrFilter = L"Image Files (*.bmp)\0*.bmp\0All Files (*.*)\0*.*\0"; // 文件类型过滤器
                    ofn.nFilterIndex = 1;
                    ofn.lpstrFile = szFileName;
                    ofn.nMaxFile = sizeof(szFileName);
                    ofn.lpstrFileTitle = NULL;
                    ofn.nMaxFileTitle = 0;
                    ofn.lpstrInitialDir = NULL;
                    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
                    if (GetOpenFileName(&ofn) == TRUE)
                    {
                        // 用户选择了文件，文件名保存在 szFileName 中
                        MessageBox(NULL, szFileName, L"File Selected", MB_OK);
                        HBITMAP hBmp = (HBITMAP)LoadImage(NULL, szFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
                        BITMAP bmp;
                        GetObject(hBmp, sizeof(BITMAP), &bmp);
                        HDC hdc = GetDC(hwnd); // 获取窗口设备上下文
                        HDC memdc = CreateCompatibleDC(hdc); // 创建与当前显示设备兼容的设备上下文

                        SelectObject(memdc, hBmp); // 选择位图对象到设备上下文中

                        BitBlt(hdc, 150, 150, bmp.bmWidth, bmp.bmHeight, memdc, 0, 0, SRCCOPY); // 将设备上下文中的图像绘制到窗口设备上下文中

                        DeleteObject(hBmp); // 删除位图对象
                        DeleteDC(memdc); // 删除设备上下文
                        ReleaseDC(hwnd, hdc); // 释放设备上下文

                    }
                    break;
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
            if (isDrawing) {
                HDC hdc = GetDC(hwnd);
                endX = LOWORD(lParam);
                endY = HIWORD(lParam);


                if (isRectanglePressed) {

                    HPEN hPen = CreatePen(PS_SOLID, 2, g_color);
                    SelectObject(hdc, hPen);
                    RECT rect = {startX, startY, endX - 1, endY - 1};
                    HBRUSH hBrush;
                    if (isall)
                        hBrush = CreateSolidBrush(g_color);
                    else
                        hBrush = CreateSolidBrush(RGB(255, 255, 255));
                    FillRect(hdc, &rect, hBrush);
                    Rectangle(hdc, startX, startY, endX, endY);
                    FillRect(hdc, &rect, hBrush);
                    DeleteObject(hBrush);
                    // 释放画笔
                    DeleteObject(hPen);

                    drawing(hwnd, NULL, rect);
                }
                if (isTrianglePressed) {
                    HPEN hPen = CreatePen(PS_SOLID, 1, g_color);
                    HPEN hOldPen = (HPEN) SelectObject(hdc, hPen);
// 创建画刷对象
                    RECT rect = {startX, startY, endX - 1, endY - 1};
                    HBRUSH hBrush;
                    if (isall)
                        hBrush = CreateSolidBrush(g_color);
                    else
                        hBrush = CreateSolidBrush(RGB(255, 255, 255));
                    // 填充矩形
                    HBRUSH hOldBrush = (HBRUSH) SelectObject(hdc, hBrush);

                    POINT points[3] = {{startX,                       startY},
                                       {endX,                         endY},
                                       {startX + (endX - startX) / 2, startY}};
//{startX, startY}, {endX, endY},{startX+(endX-startX)/2, startY}
// 绘制三角形
                    Polygon(hdc, points, 3);

// 恢复原始的画笔和画刷对象
                    SelectObject(hdc, hOldPen);
                    DeleteObject(hPen);
                    SelectObject(hdc, hOldBrush);
                    DeleteObject(hBrush);

                    //drawing(hwnd,NULL,rect);
                    // 重绘窗口
                    break;
                }
                if (isRoundPressed) {
                    HBRUSH hBrush;
                    if (isall)
                        hBrush = CreateSolidBrush(g_color);
                    else
                        hBrush = CreateSolidBrush(RGB(255, 255, 255));
                    HPEN hPen = CreatePen(PS_SOLID, 2, g_color);
                    SelectObject(hdc, hPen);
                    RECT rect = {startX, startY, endX, endY};
                    FillRect(hdc, &rect, hBrush);
                    HBRUSH hOldBrush = (HBRUSH) SelectObject(hdc, hBrush);
                    FillRect(hdc, &rect, CreateSolidBrush(RGB(255, 255, 255)));

                    int centerX = (startX + endX) / 2;
                    int centerY = (startY + endY) / 2;
                    int main;
                    if (abs(startX - endX) < abs(startY - endY))
                        main = abs(startX - endX);
                    else
                        main = abs(startY - endY);
                    int radius = main / 2;
                    // 绘制圆形

                    Ellipse(hdc, centerX - radius, centerY - radius, centerX + radius, centerY + radius);

                    DeleteObject(hBrush);

                    // 释放画笔
                    DeleteObject(hPen);

                    drawing(hwnd, NULL, rect);
                }
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

void printR(HWND hwnd, LPARAM lparam) {
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
    auto setbrush = (HBRUSH) SelectObject(static_hdc, hBrush);
    FillRect(static_hdc, &rect, hBrush);
    SelectObject(static_hdc, setbrush);
    DeleteObject(setbrush);

    SelectObject(static_hdc,
                 btn1 = CreateWindowEx(0, L"BUTTON", L"矩形", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 10, 10, 100, 30,
                                       hwnd,
                                       (HMENU) ID_rectangle, hinstances, nullptr));
    SelectObject(static_hdc,
                 btn2 = CreateWindowEx(0, L"BUTTON", L"圆形", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 130, 10, 100,
                                       30, hwnd,
                                       (HMENU) ID_round, hinstances, nullptr));
    SelectObject(static_hdc,
                 btn3 = CreateWindowEx(0, L"BUTTON", L"三角形", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 250, 10, 100,
                                       30,
                                       hwnd,
                                       (HMENU) ID_triangle, hinstances, nullptr));
    SelectObject(static_hdc,
                 btn4 = CreateWindowEx(0, L"BUTTON", L"颜色", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 370, 10, 100,
                                       30, hwnd,
                                       (HMENU) ID_clour, hinstances, nullptr));
    SelectObject(static_hdc,
                 btn5 = CreateWindowEx(0, L"BUTTON", L"填充", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 490, 10, 100,
                                       30, hwnd,
                                       (HMENU) ID_all, hinstances, nullptr));
    SelectObject(static_hdc,
                 btn6 = CreateWindowEx(0, L"BUTTON", L"打开文件", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 610, 10,
                                       100, 30, hwnd, (HMENU) ID_file, hinstances, nullptr));
    SetTextColor(static_hdc, RGB(255, 255, 255));
    SetBkMode(static_hdc, TRANSPARENT);
    BitBlt(hdc, 0, 0, width, height, static_hdc, 0, 0, SRCCOPY);

    EndPaint(hwnd, &ps);
}

void drawing(HWND hwnd, HINSTANCE hinstance, RECT &rect) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    static_hdc = CreateCompatibleDC(hdc);
    hbitmap = CreateCompatibleBitmap(hdc, xClient, yCLient);
    SelectObject(static_hdc, hbitmap);
    HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
    auto setbrush = (HBRUSH) SelectObject(static_hdc, hBrush);
    FillRect(static_hdc, &rect, hBrush);
    SelectObject(static_hdc, setbrush);
    DeleteObject(setbrush);
    Rectangle(static_hdc, rect.left, rect.top, rect.right, rect.bottom);
    EndPaint(hwnd, &ps);
}
void viewPhoto(HWND hwnd, HINSTANCE hinstance,BITMAP bmp,HBITMAP hBmp,BYTE* pPixels) {
    HDC hdc = GetDC(hwnd); // 获取窗口设备上下文
    HDC memdc = CreateCompatibleDC(hdc); // 创建与当前显示设备兼容的设备上下文

    BITMAPINFO bmpInfo = { 0 };
    bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfo.bmiHeader.biWidth = bmp.bmWidth;
    bmpInfo.bmiHeader.biHeight = bmp.bmHeight;
    bmpInfo.bmiHeader.biPlanes = 1;
    bmpInfo.bmiHeader.biBitCount = 32;
    bmpInfo.bmiHeader.biCompression = BI_RGB;

    BYTE* pPixels32 = new BYTE[bmp.bmWidthBytes * bmp.bmHeight * 4];
    memset(pPixels32, 0, bmp.bmWidthBytes * bmp.bmHeight * 4);

    GetDIBits(hdc, hBmp, 0, bmp.bmHeight, pPixels32, &bmpInfo, DIB_RGB_COLORS);

    BYTE* pPixels24 = new BYTE[bmp.bmWidthBytes * bmp.bmHeight];
    for (int i = 0; i < bmp.bmWidthBytes * bmp.bmHeight; i += 3)
    {
        pPixels24[i] = pPixels32[i + 2];
        pPixels24[i + 1] = pPixels32[i + 1];
        pPixels24[i + 2] = pPixels32[i];
    }

    HBITMAP hBitmap = CreateBitmap(bmp.bmWidth, bmp.bmHeight, bmp.bmPlanes, bmp.bmBitsPixel, pPixels24);

    BitBlt(hdc, 0, 0, bmp.bmWidth, bmp.bmHeight, memdc, 0, 0, SRCCOPY);

    DeleteObject(hBitmap);
    DeleteDC(memdc);
    ReleaseDC(hwnd, hdc);
    HDC hdc1 = GetDC(hwnd); // 获取窗口设备上下文
    HDC memdc1 = CreateCompatibleDC(hdc1); // 创建与当前显示设备兼容的设备上下文
    HBITMAP hBitmap1 = CreateBitmap(bmp.bmWidth, bmp.bmHeight, bmp.bmPlanes, bmp.bmBitsPixel, pPixels); // 创建位图对象
    HGDIOBJ oldbmp = SelectObject(memdc, hBitmap);
    BitBlt(hdc1, 0, 0, bmp.bmWidth, bmp.bmHeight, memdc1, 0, 0, SRCCOPY);
    SelectObject(memdc1, oldbmp);
    DeleteObject(hBitmap1);
    DeleteDC(memdc1);
    ReleaseDC(hwnd, hdc1);

}