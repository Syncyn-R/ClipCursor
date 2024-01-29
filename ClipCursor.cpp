#include "framework.h"
#include "ClipCursor.h"
#include <stdio.h>

char g_Enabled = 0;
char g_Disabled = 1;

HWND g_Minecraft_BE_HWND = 0;

RECT g_Minecraft_BE_Rect = { 0 };
RECT g_Cliped_Cursor_Rect = { 0 };

#define MAX_LOADSTRING 100

HINSTANCE hInst;
CHAR szTitle[MAX_LOADSTRING];
CHAR szWindowClass[MAX_LOADSTRING];

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_CLIPCURSOR, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLIPCURSOR));

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLIPCURSOR));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = 0;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance;

   HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      200, 200, 300, 300, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        {
            RegisterHotKey(hWnd, 0, MOD_CONTROL, VK_F8);
            MessageBox(hWnd, "软件已启动，启用/关闭快捷键:Ctrl+F8，使用时请勿关闭程序窗口", "提示", MB_OK);
        }
        break;
    case WM_HOTKEY:
        {
            if (wParam == 0)
            {
                MessageBeep(0);
                if (g_Enabled)
                {
                    g_Enabled = 0;
                    KillTimer(hWnd, 0);
                    ClipCursor(0);
                }
                else
                {
                    g_Enabled = 1;
                    SetTimer(hWnd, 0, 20, 0);
                }
            }
        }
        break;
    case WM_TIMER:
        {
            if(!IsWindow(g_Minecraft_BE_HWND)) g_Minecraft_BE_HWND = FindWindow(0, "Minecraft");
            HWND foregroundHWND = GetForegroundWindow();
            CURSORINFO currentCursorInfo = { sizeof(CURSORINFO) };
            GetCursorInfo(&currentCursorInfo);
            if ( !currentCursorInfo.flags && foregroundHWND == g_Minecraft_BE_HWND)
            {
                g_Disabled = 0;
                GetWindowRect(g_Minecraft_BE_HWND, &g_Minecraft_BE_Rect);
                g_Minecraft_BE_Rect.left += 8;
                g_Minecraft_BE_Rect.right -= 8;
                if (g_Minecraft_BE_Rect.top < 0) g_Minecraft_BE_Rect.top += 40;
                else  g_Minecraft_BE_Rect.top += 33;
                g_Minecraft_BE_Rect.bottom -= 8;
                ClipCursor(&g_Minecraft_BE_Rect);
            }
            else if(!g_Disabled)
            {
                g_Disabled = 1;
                ClipCursor(0);
            }
        }
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
    {
        g_Enabled = 0;
        KillTimer(hWnd, 0);
        ClipCursor(0);
        PostQuitMessage(0);
    }
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
