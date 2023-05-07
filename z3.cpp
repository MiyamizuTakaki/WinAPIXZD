#ifndef UNICODE
#define UNICODE
#define IDM_FILE_EXIT 0
#endif

#include <windows.h>
#include <windowsx.h>
#include <gdiplus.h>
void SetMenus(HWND hwnd, HMENU hMenu);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void BitMapView(HWND hwnd,LPARAM lparam,wchar_t* fileName=NULL);
void DrawWinAgain(HWND hwnd,LPARAM lparam,wchar_t* colorname);
static COLORREF g_color = RGB(255, 255, 255);
static HDC global_dc = NULL;
static HBITMAP global_bitmap = NULL;
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPrevINstance, LPSTR lpCmdLine, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"WindowClass";
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hinstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);
    HWND hwnd = CreateWindowEx(0,CLASS_NAME, CLASS_NAME, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hinstance, NULL);
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            DrawWinAgain(hwnd,lParam,L"");
            BitMapView(hwnd,lParam,NULL);
            EndPaint(hwnd, &ps);
            break;
        }
        case WM_CREATE:{
            HMENU hMenu = CreateMenu();
            SetMenus(hwnd,hMenu);
            SetMenu(hwnd, hMenu);
            break;
        }
        case WM_CONTEXTMENU:{
            HMENU hMenu = CreatePopupMenu();
            SetMenus(hwnd,hMenu);
            POINT pt;
            if (GetCursorPos(&pt))
            {
                ScreenToClient(hwnd, &pt);
                pt.x = GET_X_LPARAM(lParam);
                pt.y = GET_Y_LPARAM(lParam);
                ClientToScreen(hwnd, &pt);
                RECT rect = {100,100,100,100};
                TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, &rect);

            }
            // 销毁菜单
            DestroyMenu(hMenu);
            break;
        }
        case WM_COMMAND:{
            switch (LOWORD(wParam))
            {
                case 1001://退出
                    DestroyWindow(hwnd);
                    break;
                case 1002://image1.bmp
                    BitMapView(hwnd,lParam,L"../resource/image1.bmp");
                    break;
                case 1003://image2.bmp
                    BitMapView(hwnd,lParam,L"../resource/image2.bmp");
                    break;
                case 1004://image3.bmp
                    BitMapView(hwnd,lParam,L"../resource/image3.bmp");
                    break;
                case 1005:
                    DrawWinAgain(hwnd,lParam,L"RED");
                    break;
                case 1006:
                    DrawWinAgain(hwnd,lParam,L"GREEN");
                    break;
                case 1007:
                    DrawWinAgain(hwnd,lParam,L"BLUE");
                    break;
                case 1008:
                    DestroyWindow(hwnd);
                    break;
                case 1009:
                    DestroyWindow(hwnd);
                    break;
                case 1010:
                    DestroyWindow(hwnd);
                    break;
                case 1011:
                    DestroyWindow(hwnd);
                    break;
            }
        }
        case WM_SIZE:{
            break;
        }
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}


void SetMenus(HWND hwnd, HMENU hMenu)
{
    HMENU hFileSubMenu = CreatePopupMenu();
    AppendMenu(hFileSubMenu, MF_STRING, 1001, L"退出");
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileSubMenu, L"文件");
    HMENU hImageSubMenu = CreatePopupMenu();
    AppendMenu(hImageSubMenu, MF_STRING, 1002, L"image1.bmp");
    AppendMenu(hImageSubMenu, MF_STRING, 1003, L"image2.bmp");
    AppendMenu(hImageSubMenu, MF_STRING, 1004, L"image3.bmp");
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hImageSubMenu, L"图片");
    HMENU hColorSubMenu = CreatePopupMenu();
    AppendMenu(hColorSubMenu, MF_STRING, 1005, L"red");
    AppendMenu(hColorSubMenu, MF_STRING, 1006, L"green");
    AppendMenu(hColorSubMenu, MF_STRING, 1007, L"blue");
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hColorSubMenu, L"颜色");
    AppendMenu(hMenu, MF_POPUP, 1008, L"关于");
}

void DrawWinAgain(HWND hwnd,LPARAM lparam,wchar_t* colorname){
    int xClient = LOWORD(lparam);
    int yCLient = HIWORD(lparam);
    if(colorname == NULL||colorname == L"WRITE"){
        g_color = RGB(255, 255, 255);
    }
    else if(colorname == L"RED"){
        g_color = RGB(255, 0, 0);
    }
    else if(colorname == L"GREEN"){
        g_color = RGB(0, 255, 0);
    }
    else if(colorname == L"BLUE"){
        g_color = RGB(0, 0, 255);
    }
    RECT rect;
    GetClientRect(hwnd, &rect);
    HDC hdc = GetDC(hwnd);
    HBRUSH hBrush = CreateSolidBrush(g_color);
    FillRect(hdc, &rect, hBrush);
    HBITMAP hbitmap = CreateCompatibleBitmap(global_dc, xClient, yCLient);
    SelectObject(hdc, hbitmap);
    DeleteObject(hBrush);
    ReleaseDC(hwnd, hdc);
}
void BitMapView(HWND hwnd,LPARAM lparam,wchar_t* fileName){
    DrawWinAgain(hwnd,lparam,L"");
    HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, fileName, IMAGE_BITMAP, 0, 0,LR_LOADFROMFILE);
    global_bitmap = hBitmap;
    HDC hdc = GetDC(hwnd);
    HDC hdcMem = CreateCompatibleDC(hdc);
    if(hBitmap == NULL)
        hBitmap = global_bitmap;
    HBITMAP hBitmapOld = (HBITMAP)SelectObject(hdcMem, hBitmap);
    BITMAP bm = {};
    GetObject(hBitmap, sizeof(bm), &bm);
    int width = bm.bmWidth;
    int height = bm.bmHeight;
    BitBlt(hdc, 0, 0, width, height, hdcMem, 0, 0, SRCCOPY);
    SelectObject(global_dc,hdc);
    SelectObject(hdcMem, hBitmapOld);
    DeleteDC(hdcMem);
    DeleteObject(hBitmap);
    ReleaseDC(hwnd, hdc);

}