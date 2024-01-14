#include <windows.h>
#include <tchar.h>
#include <string>
#include <iostream>

// The main window class name.
static TCHAR szWindowClass[] = _T("DesktopApp");

// The string that appears in the application's title bar.
static TCHAR szTitle[] = _T("Windows Desktop Guided Tour Application");

// Stored instance handle for use in Win32 API calls such as FindResource
HINSTANCE hInst;

// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(
        HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPSTR lpCmdLine,
        int nCmdShow) {
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(wcex.hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex)) {
        MessageBox(NULL,
                   _T("Call to RegisterClassEx failed!"),
                   _T("Windows Desktop Guided Tour"),
                   NULL);

        return 1;
    }

    // Store instance handle in our global variable
    hInst = hInstance;

    // The parameters to CreateWindowEx explained:
    // WS_EX_OVERLAPPEDWINDOW : An optional extended window style.
    // szWindowClass: the name of the application
    // szTitle: the text that appears in the title bar
    // WS_OVERLAPPEDWINDOW: the type of window to create
    // CW_USEDEFAULT, CW_USEDEFAULT: initial position (x, y)
    // 500, 100: initial size (width, length)
    // NULL: the parent of this window
    // NULL: this application does not have a menu bar
    // hInstance: the first parameter from WinMain
    // NULL: not used in this application
    HWND hWnd = CreateWindowEx(
            WS_EX_OVERLAPPEDWINDOW,
            szWindowClass,
            szTitle,
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT,
            500, 100,
            NULL,
            NULL,
            hInstance,
            NULL
    );

    if (!hWnd) {
        MessageBox(NULL,
                   _T("Call to CreateWindow failed!"),
                   _T("Windows Desktop Guided Tour"),
                   NULL);

        return 1;
    }

    HHOOK hhkLowLevelKybd = SetWindowsHookEx(WH_KEYBOARD_LL,
                                             LowLevelKeyboardProc,
                                             NULL,
                                             0);

    ShowWindow(hWnd,
               nCmdShow);
    UpdateWindow(hWnd);

    // Main message loop:
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int) msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    PAINTSTRUCT ps;
    HDC hdc;
    static std::string text = "hello world";

    switch (message) {
        case WM_PAINT: {
            hdc = BeginPaint(hWnd, &ps);

            TextOut(hdc,
                    5, 5,
                    text.c_str(), text.size());


            EndPaint(hWnd, &ps);
            break;
        }

        case WM_DESTROY: {
            PostQuitMessage(0);
            break;
        }

        case WM_KEYDOWN: {
            switch (LOWORD(wParam)) {
                case VK_CAPITAL: {
                    text = "caps";
                    break;
                }
                default:
                    text = "not caps";
                    break;
            }

            RECT ClientRect;
            GetClientRect(hWnd, &ClientRect);
            RedrawWindow(hWnd, &ClientRect, nullptr, RDW_INVALIDATE | RDW_UPDATENOW);

            return 1;
        }

        default: {
            break;
        }

    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        // Get hook struct
        auto p = (PKBDLLHOOKSTRUCT) lParam;
        
        switch (wParam) {
            case WM_KEYDOWN:
            case WM_SYSKEYDOWN:
                if (p->vkCode == VK_CAPITAL) {
                    INPUT inputs[4] = {};
                    ZeroMemory(inputs, sizeof inputs);

                    inputs[0].type = INPUT_KEYBOARD;
                    inputs[0].ki.wVk = VK_LWIN;

                    inputs[1].type = INPUT_KEYBOARD;
                    inputs[1].ki.wVk = VK_SPACE;

                    inputs[2].type = INPUT_KEYBOARD;
                    inputs[2].ki.wVk = VK_SPACE;
                    inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;

                    inputs[3].type = INPUT_KEYBOARD;
                    inputs[3].ki.wVk = VK_LWIN;
                    inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;

                    if (SendInput(4, inputs, sizeof(INPUT)) != 4) {
                        return CallNextHookEx(nullptr, nCode, wParam, lParam);
                    }
                    return 1;
                }
            case WM_KEYUP:
            case WM_SYSKEYUP:
                if (p->vkCode == VK_CAPITAL) {
                    return 1;
                }
        }
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}