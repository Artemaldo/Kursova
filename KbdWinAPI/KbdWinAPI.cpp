#include <windows.h>
#include <tchar.h>
#include <mmsystem.h>
#define IOCTL_INCREMENT_KEY_FREQUENCY CTL_CODE(FILE_DEVICE_KEYBOARD, 0x801, METHOD_BUFFERED, FILE_WRITE_DATA)
#define IOCTL_GET_KEY_FREQUENCY CTL_CODE(FILE_DEVICE_KEYBOARD, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#pragma comment(lib,"winmm.lib")
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void LogMessage(HWND hLog, const wchar_t* message);
HANDLE hDevice = NULL;
TCHAR buffer[128] = L"";
ULONG frequency = 0;
ULONG lastfreq[12] = {};
bool enable = true;
bool advanced = false;
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
    MSG msg;
    HWND hWnd;
    TCHAR szWindowClass[] = L"MyClass";
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex)) {
        MessageBox(NULL, L"Cannot register class", L"Error", MB_OK);
        return 0;
    }

    hWnd = CreateWindow(
        szWindowClass, L"Kbd synthesiser app", WS_OVERLAPPEDWINDOW,
        400, 400, 600, 310, NULL, NULL, hInstance, NULL);

    if (!hWnd) {
        MessageBox(NULL, L"Failed to create window", L"Error", MB_OK);
        return 0;
    }

    hDevice = CreateFile(
        L"\\\\.\\KeyboardDriver", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

    if (hDevice == INVALID_HANDLE_VALUE) {
        MessageBox(NULL, L"Failed to open device", L"Error", MB_OK);
        return 0;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (hDevice != NULL) {
        CloseHandle(hDevice);
    }

    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HWND hSwitchOnOff, hLog, hAdvanced;

    switch (uMsg) {
    case WM_CREATE:
        hLog = CreateWindow(
            L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_READONLY | WS_VSCROLL,
            200, 40, 370, 220, hWnd, (HMENU)1, NULL, NULL);
        hSwitchOnOff = CreateWindow(
            L"BUTTON", L"Switch on/off", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            40, 50, 150, 30, hWnd, (HMENU)2, GetModuleHandle(NULL), NULL);
        hAdvanced = CreateWindow(
            L"BUTTON", L"Advanced info", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            40, 90, 150, 30, hWnd, (HMENU)3, GetModuleHandle(NULL), NULL);
        break;

    case WM_KEYDOWN:
        switch (LOWORD(wParam))
        {
        case 'Q':
            if (enable)
            {
                if (hDevice != NULL)
                {
                    DWORD bytesReturned;
                    char key = 0x10;

                    if (!DeviceIoControl(hDevice, IOCTL_INCREMENT_KEY_FREQUENCY, &key, sizeof(key), NULL, 0, &bytesReturned, NULL)) {
                        MessageBox(hWnd, L"Failed to send IOCTL_INCREMENT_KEY_FREQUENCY. Q button", L"Error", MB_OK);
                    }
                    if (!DeviceIoControl(hDevice, IOCTL_GET_KEY_FREQUENCY, &key, sizeof(key), &frequency, sizeof(frequency), &bytesReturned, NULL))
                    {
                        MessageBox(hWnd, L"Failed to send IOCTL_GET_KEY_FREQUENCY. Q button", L"Error", MB_OK);
                    }
                    else
                    {
                        if (advanced)
                        {
                            swprintf(buffer, 128, L"Key Q frequency: %lu", frequency);
                            LogMessage(hLog, buffer);
                        }
                        if (lastfreq[0] < frequency)
                        {
                            LogMessage(hLog, L"Q button has been pressed.'Do' note is now playing");
                            PlaySound(TEXT("sounds\\do.wav"), NULL, SND_FILENAME | SND_SYNC);
                        }
                        lastfreq[0] = frequency;
                    }
                }
            }
            break;
        case 'W':
            if (enable)
            {
                if (hDevice != NULL)
                {
                    DWORD bytesReturned;
                    char key = 0x11;

                    if (!DeviceIoControl(hDevice, IOCTL_INCREMENT_KEY_FREQUENCY, &key, sizeof(key), NULL, 0, &bytesReturned, NULL)) {
                        MessageBox(hWnd, L"Failed to send IOCTL_INCREMENT_KEY_FREQUENCY. W button", L"Error", MB_OK);
                    }
                    if (!DeviceIoControl(hDevice, IOCTL_GET_KEY_FREQUENCY, &key, sizeof(key), &frequency, sizeof(frequency), &bytesReturned, NULL))
                    {
                        MessageBox(hWnd, L"Failed to send IOCTL_GET_KEY_FREQUENCY. W button", L"Error", MB_OK);
                    }
                    else
                    {
                        if (advanced)
                        {
                            swprintf(buffer, 128, L"Key W frequency: %lu", frequency);
                            LogMessage(hLog, buffer);
                        }
                        if (lastfreq[1] < frequency)
                        {
                            LogMessage(hLog, L"W button has been pressed.'Re' note is now playing");
                            PlaySound(TEXT("sounds\\re.wav"), NULL, SND_FILENAME | SND_SYNC);
                        }
                        lastfreq[1] = frequency;
                    }
                }
            }
            break;
        case 'E':
            if (enable)
            {
                if (hDevice != NULL)
                {
                    DWORD bytesReturned;
                    char key = 0x12;

                    if (!DeviceIoControl(hDevice, IOCTL_INCREMENT_KEY_FREQUENCY, &key, sizeof(key), NULL, 0, &bytesReturned, NULL)) {
                        MessageBox(hWnd, L"Failed to send IOCTL_INCREMENT_KEY_FREQUENCY. E button", L"Error", MB_OK);
                    }
                    if (!DeviceIoControl(hDevice, IOCTL_GET_KEY_FREQUENCY, &key, sizeof(key), &frequency, sizeof(frequency), &bytesReturned, NULL))
                    {
                        MessageBox(hWnd, L"Failed to send IOCTL_GET_KEY_FREQUENCY. E button", L"Error", MB_OK);
                    }
                    else
                    {
                        if (advanced)
                        {
                            swprintf(buffer, 128, L"Key E frequency: %lu", frequency);
                            LogMessage(hLog, buffer);
                        }
                        if (lastfreq[2] < frequency)
                        {
                            LogMessage(hLog, L"E button has been pressed.'Mi' note is now playing");
                            PlaySound(TEXT("sounds\\mi.wav"), NULL, SND_FILENAME | SND_SYNC);
                        }
                        lastfreq[2] = frequency;
                    }
                }
            }
            break;
        case 'R':
            if (enable)
            {
                if (hDevice != NULL)
                {
                    DWORD bytesReturned;
                    char key = 0x13;

                    if (!DeviceIoControl(hDevice, IOCTL_INCREMENT_KEY_FREQUENCY, &key, sizeof(key), NULL, 0, &bytesReturned, NULL)) {
                        MessageBox(hWnd, L"Failed to send IOCTL_INCREMENT_KEY_FREQUENCY. R button", L"Error", MB_OK);
                    }
                    if (!DeviceIoControl(hDevice, IOCTL_GET_KEY_FREQUENCY, &key, sizeof(key), &frequency, sizeof(frequency), &bytesReturned, NULL))
                    {
                        MessageBox(hWnd, L"Failed to send IOCTL_GET_KEY_FREQUENCY. R button", L"Error", MB_OK);
                    }
                    else
                    {
                        if (advanced)
                        {
                            swprintf(buffer, 128, L"Key R frequency: %lu", frequency);
                            LogMessage(hLog, buffer);
                        }
                        if (lastfreq[3] < frequency)
                        {
                            LogMessage(hLog, L"R button has been pressed.'Fa' note is now playing");
                            PlaySound(TEXT("sounds\\fa.wav"), NULL, SND_FILENAME | SND_SYNC);
                        }
                        lastfreq[3] = frequency;
                    }
                }
            }
            break;
        case 'T':
            if (enable)
            {
                if (hDevice != NULL)
                {
                    DWORD bytesReturned;
                    char key = 0x14;

                    if (!DeviceIoControl(hDevice, IOCTL_INCREMENT_KEY_FREQUENCY, &key, sizeof(key), NULL, 0, &bytesReturned, NULL)) {
                        MessageBox(hWnd, L"Failed to send IOCTL_INCREMENT_KEY_FREQUENCY. T button", L"Error", MB_OK);
                    }
                    if (!DeviceIoControl(hDevice, IOCTL_GET_KEY_FREQUENCY, &key, sizeof(key), &frequency, sizeof(frequency), &bytesReturned, NULL))
                    {
                        MessageBox(hWnd, L"Failed to send IOCTL_GET_KEY_FREQUENCY. T button", L"Error", MB_OK);
                    }
                    else
                    {
                        if (advanced)
                        {
                            swprintf(buffer, 128, L"Key T frequency: %lu", frequency);
                            LogMessage(hLog, buffer);
                        }
                        if (lastfreq[4] < frequency)
                        {
                            LogMessage(hLog, L"T button has been pressed.'Sol' note is now playing");
                            PlaySound(TEXT("sounds\\sol.wav"), NULL, SND_FILENAME | SND_SYNC);
                        }
                        lastfreq[4] = frequency;
                    }
                }
            }
        case 'Y':
            if (enable)
            {
                if (hDevice != NULL)
                {
                    DWORD bytesReturned;
                    char key = 0x15;

                    if (!DeviceIoControl(hDevice, IOCTL_INCREMENT_KEY_FREQUENCY, &key, sizeof(key), NULL, 0, &bytesReturned, NULL)) {
                        MessageBox(hWnd, L"Failed to send IOCTL_INCREMENT_KEY_FREQUENCY. Y button", L"Error", MB_OK);
                    }
                    if (!DeviceIoControl(hDevice, IOCTL_GET_KEY_FREQUENCY, &key, sizeof(key), &frequency, sizeof(frequency), &bytesReturned, NULL))
                    {
                        MessageBox(hWnd, L"Failed to send IOCTL_GET_KEY_FREQUENCY. Y button", L"Error", MB_OK);
                    }
                    else
                    {
                        if (advanced)
                        {
                            swprintf(buffer, 128, L"Key Y frequency: %lu", frequency);
                            LogMessage(hLog, buffer);
                        }
                        if (lastfreq[5] < frequency)
                        {
                            LogMessage(hLog, L"Y button has been pressed.'Lya' note is now playing");
                            PlaySound(TEXT("sounds\\lya.wav"), NULL, SND_FILENAME | SND_SYNC);
                        }
                        lastfreq[5] = frequency;
                    }
                }
            }
            break;
        case 'U':
            if (enable)
            {
                if (hDevice != NULL)
                {
                    DWORD bytesReturned;
                    char key = 0x16;

                    if (!DeviceIoControl(hDevice, IOCTL_INCREMENT_KEY_FREQUENCY, &key, sizeof(key), NULL, 0, &bytesReturned, NULL)) {
                        MessageBox(hWnd, L"Failed to send IOCTL_INCREMENT_KEY_FREQUENCY. U button", L"Error", MB_OK);
                    }
                    if (!DeviceIoControl(hDevice, IOCTL_GET_KEY_FREQUENCY, &key, sizeof(key), &frequency, sizeof(frequency), &bytesReturned, NULL))
                    {
                        MessageBox(hWnd, L"Failed to send IOCTL_GET_KEY_FREQUENCY. U button", L"Error", MB_OK);
                    }
                    else
                    {
                        if (advanced)
                        {
                            swprintf(buffer, 128, L"Key U frequency: %lu", frequency);
                            LogMessage(hLog, buffer);
                        }
                        if (lastfreq[6] < frequency)
                        {
                            LogMessage(hLog, L"U button has been pressed.'Si' note is now playing");
                            PlaySound(TEXT("sounds\\si.wav"), NULL, SND_FILENAME | SND_SYNC);
                        }
                        lastfreq[6] = frequency;
                    }
                }
            }
            break;
        case 0x32:
            if (enable)
            {
                if (hDevice != NULL)
                {
                    DWORD bytesReturned;
                    char key = 0x3;

                    if (!DeviceIoControl(hDevice, IOCTL_INCREMENT_KEY_FREQUENCY, &key, sizeof(key), NULL, 0, &bytesReturned, NULL)) {
                        MessageBox(hWnd, L"Failed to send IOCTL_INCREMENT_KEY_FREQUENCY. 2 button", L"Error", MB_OK);
                    }
                    if (!DeviceIoControl(hDevice, IOCTL_GET_KEY_FREQUENCY, &key, sizeof(key), &frequency, sizeof(frequency), &bytesReturned, NULL))
                    {
                        MessageBox(hWnd, L"Failed to send IOCTL_GET_KEY_FREQUENCY. 2 button", L"Error", MB_OK);
                    }
                    else
                    {
                        if (advanced)
                        {
                            swprintf(buffer, 128, L"Key 2 frequency: %lu", frequency);
                            LogMessage(hLog, buffer);
                        }
                        if (lastfreq[7] < frequency)
                        {
                            LogMessage(hLog, L"2 button has been pressed.'Do-Re' note is now playing");
                            PlaySound(TEXT("sounds\\do-re.wav"), NULL, SND_FILENAME | SND_SYNC);
                        }
                        lastfreq[7] = frequency;
                    }
                }
            }
            break;
        case 0x33:
            if (enable)
            {
                if (hDevice != NULL)
                {
                    DWORD bytesReturned;
                    char key = 0x4;

                    if (!DeviceIoControl(hDevice, IOCTL_INCREMENT_KEY_FREQUENCY, &key, sizeof(key), NULL, 0, &bytesReturned, NULL)) {
                        MessageBox(hWnd, L"Failed to send IOCTL_INCREMENT_KEY_FREQUENCY. 3 button", L"Error", MB_OK);
                    }
                    if (!DeviceIoControl(hDevice, IOCTL_GET_KEY_FREQUENCY, &key, sizeof(key), &frequency, sizeof(frequency), &bytesReturned, NULL))
                    {
                        MessageBox(hWnd, L"Failed to send IOCTL_GET_KEY_FREQUENCY. 3 button", L"Error", MB_OK);
                    }
                    else
                    {
                        if (advanced)
                        {
                            swprintf(buffer, 128, L"Key 3 frequency: %lu", frequency);
                            LogMessage(hLog, buffer);
                        }
                        if (lastfreq[8] < frequency)
                        {
                            LogMessage(hLog, L"3 button has been pressed.'Re-Mi' note is now playing");
                            PlaySound(TEXT("sounds\\re-mi.wav"), NULL, SND_FILENAME | SND_SYNC);
                        }
                        lastfreq[8] = frequency;
                    }
                }
            }
            break;
        case 0x35:
            if (enable)
            {
                if (hDevice != NULL)
                {
                    DWORD bytesReturned;
                    char key = 0x6;

                    if (!DeviceIoControl(hDevice, IOCTL_INCREMENT_KEY_FREQUENCY, &key, sizeof(key), NULL, 0, &bytesReturned, NULL)) {
                        MessageBox(hWnd, L"Failed to send IOCTL_INCREMENT_KEY_FREQUENCY. 5 button", L"Error", MB_OK);
                    }
                    if (!DeviceIoControl(hDevice, IOCTL_GET_KEY_FREQUENCY, &key, sizeof(key), &frequency, sizeof(frequency), &bytesReturned, NULL))
                    {
                        MessageBox(hWnd, L"Failed to send IOCTL_GET_KEY_FREQUENCY. 5 button", L"Error", MB_OK);
                    }
                    else
                    {
                        if (advanced)
                        {
                            swprintf(buffer, 128, L"Key 5 frequency: %lu", frequency);
                            LogMessage(hLog, buffer);
                        }
                        if (lastfreq[9] < frequency)
                        {
                            LogMessage(hLog, L"5 button has been pressed.'Fa-Sol' note is now playing");
                            PlaySound(TEXT("sounds\\fa-sol.wav"), NULL, SND_FILENAME | SND_SYNC);
                        }
                        lastfreq[9] = frequency;
                    }
                }
            }
            break;
        case 0x36:
            if (enable)
            {
                if (hDevice != NULL)
                {
                    DWORD bytesReturned;
                    char key = 0x7;

                    if (!DeviceIoControl(hDevice, IOCTL_INCREMENT_KEY_FREQUENCY, &key, sizeof(key), NULL, 0, &bytesReturned, NULL)) {
                        MessageBox(hWnd, L"Failed to send IOCTL_INCREMENT_KEY_FREQUENCY. 6 button", L"Error", MB_OK);
                    }
                    if (!DeviceIoControl(hDevice, IOCTL_GET_KEY_FREQUENCY, &key, sizeof(key), &frequency, sizeof(frequency), &bytesReturned, NULL))
                    {
                        MessageBox(hWnd, L"Failed to send IOCTL_GET_KEY_FREQUENCY. 6 button", L"Error", MB_OK);
                    }
                    else
                    {
                        if (advanced)
                        {
                            swprintf(buffer, 128, L"Key 6 frequency: %lu", frequency);
                            LogMessage(hLog, buffer);
                        }
                        if (lastfreq[10] < frequency)
                        {
                            LogMessage(hLog, L"6 button has been pressed.'Sol-Lya' note is now playing");
                            PlaySound(TEXT("sounds\\sol-lya.wav"), NULL, SND_FILENAME | SND_SYNC);
                        }
                        lastfreq[10] = frequency;
                    }
                }
            }
            break;
        case 0x37:
            if (enable)
            {
                if (hDevice != NULL)
                {
                    DWORD bytesReturned;
                    char key = 0x8;

                    if (!DeviceIoControl(hDevice, IOCTL_INCREMENT_KEY_FREQUENCY, &key, sizeof(key), NULL, 0, &bytesReturned, NULL)) {
                        MessageBox(hWnd, L"Failed to send IOCTL_INCREMENT_KEY_FREQUENCY. 7 button", L"Error", MB_OK);
                    }
                    if (!DeviceIoControl(hDevice, IOCTL_GET_KEY_FREQUENCY, &key, sizeof(key), &frequency, sizeof(frequency), &bytesReturned, NULL))
                    {
                        MessageBox(hWnd, L"Failed to send IOCTL_GET_KEY_FREQUENCY. 7 button", L"Error", MB_OK);
                    }
                    else
                    {
                        if (advanced)
                        {
                            swprintf(buffer, 128, L"Key 7 frequency: %lu", frequency);
                            LogMessage(hLog, buffer);
                        }
                        if (lastfreq[11] < frequency)
                        {
                            LogMessage(hLog, L"7 button has been pressed.'Lya-Si' note is now playing");
                            PlaySound(TEXT("sounds\\lya-si.wav"), NULL, SND_FILENAME | SND_SYNC);
                        }
                        lastfreq[11] = frequency;
                    }
                }
            }
            break;
            break;
        }
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case 2:
            enable = !enable;
            if (enable)
                MessageBox(hWnd, L"Synthesiser functions were enabled", L"Settings info", MB_OK);
            else MessageBox(hWnd, L"Synthesiser functions were disabled", L"Settings info", MB_OK);
            break;
        case 3:
            advanced = !advanced;
            if (advanced)
                MessageBox(hWnd, L"Advanced logs were enabled", L"Settings info", MB_OK);
            else MessageBox(hWnd, L"Advanced logs were disabled", L"Settings info", MB_OK);
            break;
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    return 0;
}

void LogMessage(HWND hLog, const wchar_t* message) {
    int len = GetWindowTextLength(hLog);
    SendMessage(hLog, EM_SETSEL, len, len);
    SendMessage(hLog, EM_REPLACESEL, FALSE, (LPARAM)message);
    SendMessage(hLog, EM_REPLACESEL, FALSE, (LPARAM)L"\r\n");
}