#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#ifndef NDEBUG
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>

void DisplayError(LPTSTR lpszFunction) {
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    unsigned int last_error = GetLastError();

    FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            null, last_error,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR) &lpMsgBuf, 0, null);

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));

    if (FAILED( StringCchPrintf((LPTSTR)lpDisplayBuf, LocalSize(lpDisplayBuf) / sizeof(TCHAR),
                                TEXT("%s failed with error code %d as follows:\n%s"), lpszFunction, last_error, lpMsgBuf)))
        printf("FATAL ERROR: Unable to output error code.\n");

    _tprintf(TEXT((LPTSTR)"ERROR: %s\n"), (LPCTSTR)lpDisplayBuf);

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
}
#endif

#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))

WNDCLASSA window_class;
HWND window;
HDC win_dc;
BITMAPINFO info;
RECT win_rect;
RAWINPUT raw_inputs;
HRAWINPUT raw_input_handle;
RAWINPUTDEVICE raw_input_device;
UINT raw_input_size;
PUINT raw_input_size_ptr = (PUINT)(&raw_input_size);
UINT raw_input_header_size = sizeof(RAWINPUTHEADER);

u64 Win32_ticksPerSecond;
LARGE_INTEGER performance_counter;

void Win32_setWindowTitle(char* str) { SetWindowTextA(window, str); }
void Win32_setCursorVisibility(bool on) { ShowCursor(on); }
void Win32_setWindowCapture(bool on) { if (on) SetCapture(window); else ReleaseCapture(); }
u64 Win32_getTicks() {
    QueryPerformanceCounter(&performance_counter);
    return (u64)performance_counter.QuadPart;
}
void* Win32_getMemory(u64 size) {
    return VirtualAlloc((LPVOID)MEMORY_BASE, (SIZE_T)size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
}

inline UINT getRawInput(LPVOID data) {
    return GetRawInputData(raw_input_handle, RID_INPUT, data, raw_input_size_ptr, raw_input_header_size);
}
inline bool hasRawInput() {
    return getRawInput(0) == 0 && raw_input_size != 0;
}
inline bool hasRawMouseInput(LPARAM lParam) {
    raw_input_handle = (HRAWINPUT)(lParam);
    return (
        hasRawInput() &&
        getRawInput((LPVOID)&raw_inputs) == raw_input_size &&
        raw_inputs.header.dwType == RIM_TYPEMOUSE
    );
}

void Win32_closeFile(void *handle) { CloseHandle(handle); }
void* Win32_openFileForReading(const char* path) {
    HANDLE handle = CreateFile(path,           // file to open
                               GENERIC_READ,          // open for reading
                               FILE_SHARE_READ,       // share for reading
                               null,                  // default security
                               OPEN_EXISTING,         // existing file only
                               FILE_ATTRIBUTE_NORMAL, // normal file
                               null);                 // no attr. template
#ifndef NDEBUG
    if (handle == INVALID_HANDLE_VALUE) {
        DisplayError(TEXT((LPTSTR)"CreateFile"));
        _tprintf(TEXT("Terminal failure: unable to open file \"%s\" for read.\n"), path);
        return null;
    }
#endif
    return handle;
}
void* Win32_openFileForWriting(const char* path) {
    HANDLE handle = CreateFile(path,           // file to open
                               GENERIC_WRITE,          // open for writing
                               0,                      // do not share
                               null,                   // default security
                               OPEN_ALWAYS,            // create new or open existing
                               FILE_ATTRIBUTE_NORMAL,  // normal file
                               null);
#ifndef NDEBUG
    if (handle == INVALID_HANDLE_VALUE) {
        DisplayError(TEXT((LPTSTR)"CreateFile"));
        _tprintf(TEXT("Terminal failure: unable to open file \"%s\" for write.\n"), path);
        return null;
    }
#endif
    return handle;
}
bool Win32_readFromFile(LPVOID out, DWORD size, HANDLE handle) {
    DWORD bytes_read = 0;
    BOOL result = ReadFile(handle, out, size, &bytes_read, null);
#ifndef NDEBUG
    if (result == FALSE) {
        DisplayError(TEXT((LPTSTR)"ReadFile"));
        printf("Terminal failure: Unable to read from file.\n GetLastError=%08x\n", (unsigned int)GetLastError());
        CloseHandle(handle);
    }
#endif
    return result != FALSE;
}

bool Win32_writeToFile(LPVOID out, DWORD size, HANDLE handle) {
    DWORD bytes_written = 0;
    BOOL result = WriteFile(handle, out, size, &bytes_written, null);
#ifndef NDEBUG
    if (result == FALSE) {
        DisplayError(TEXT((LPTSTR)"WriteFile"));
        printf("Terminal failure: Unable to write from file.\n GetLastError=%08x\n", (unsigned int)GetLastError());
        CloseHandle(handle);
    }
#endif
    return result != FALSE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_DESTROY:
            app->is_running = false;
            PostQuitMessage(0);
            break;

        case WM_SIZE:
            GetClientRect(window, &win_rect);

            info.bmiHeader.biWidth = win_rect.right - win_rect.left;
            info.bmiHeader.biHeight = win_rect.top - win_rect.bottom;

            _windowResize((u16)info.bmiHeader.biWidth, (u16)-info.bmiHeader.biHeight);

            break;

        case WM_PAINT:
            SetDIBitsToDevice(win_dc,
                              0, 0, app->viewport.dimensions.width, app->viewport.dimensions.height,
                              0, 0, 0, app->viewport.dimensions.height,
                              app->window_content, &info, DIB_RGB_COLORS);

            ValidateRgn(window, null);
            break;

        case WM_SYSKEYDOWN:
        case WM_KEYDOWN:
            _keyChanged((u8)wParam, true);
            break;

        case WM_SYSKEYUP:
        case WM_KEYUP: _keyChanged((u8)wParam, false); break;

        case WM_MBUTTONUP:     _mouseButtonUp(  &app->controls.mouse.middle_button, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;
        case WM_MBUTTONDOWN:   _mouseButtonDown(&app->controls.mouse.middle_button, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;
        case WM_LBUTTONDOWN:   _mouseButtonDown(&app->controls.mouse.left_button,   GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;
        case WM_LBUTTONUP  :   _mouseButtonUp(  &app->controls.mouse.left_button,   GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;
        case WM_RBUTTONDOWN:   _mouseButtonDown(&app->controls.mouse.right_button,  GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;
        case WM_RBUTTONUP:     _mouseButtonUp(  &app->controls.mouse.right_button,  GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;
        case WM_LBUTTONDBLCLK: _mouseButtonDoubleClicked(&app->controls.mouse.left_button,   GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;
        case WM_RBUTTONDBLCLK: _mouseButtonDoubleClicked(&app->controls.mouse.right_button,  GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;
        case WM_MBUTTONDBLCLK: _mouseButtonDoubleClicked(&app->controls.mouse.middle_button, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;
        case WM_MOUSEWHEEL:    _mouseWheelScrolled((f32)(GET_WHEEL_DELTA_WPARAM(wParam)) / (f32)(WHEEL_DELTA)); break;
        case WM_MOUSEMOVE:
            _mouseMovementSet(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            _mousePositionSet(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            break;

        case WM_INPUT:
            if ((hasRawMouseInput(lParam)) && (
                raw_inputs.data.mouse.lLastX != 0 ||
                raw_inputs.data.mouse.lLastY != 0))
                _mouseRawMovementSet(
                        raw_inputs.data.mouse.lLastX,
                        raw_inputs.data.mouse.lLastY
                );

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow) {

    void* app_memory = GlobalAlloc(GPTR, sizeof(App));
    if (!app_memory)
        return -1;

    app = (App*)app_memory;

    void* window_content_memory = GlobalAlloc(GPTR, sizeof(u32) * MAX_WIDTH * MAX_HEIGHT);
    if (!window_content_memory)
        return -1;

    LARGE_INTEGER performance_frequency;
    QueryPerformanceFrequency(&performance_frequency);
    Win32_ticksPerSecond = (u64)performance_frequency.QuadPart;

    app->controls.key_map.space = VK_SPACE;
    app->controls.key_map.shift = VK_SHIFT;
    app->controls.key_map.ctrl  = VK_CONTROL;
    app->controls.key_map.alt   = VK_MENU;
    app->controls.key_map.tab   = VK_TAB;

    app->platform.ticks_per_second    = Win32_ticksPerSecond;
    app->platform.getTicks            = Win32_getTicks;
    app->platform.getMemory           = Win32_getMemory;
    app->platform.setWindowTitle      = Win32_setWindowTitle;
    app->platform.setWindowCapture    = Win32_setWindowCapture;
    app->platform.setCursorVisibility = Win32_setCursorVisibility;
    app->platform.closeFile           = Win32_closeFile;
    app->platform.openFileForReading  = Win32_openFileForReading;
    app->platform.openFileForWriting  = Win32_openFileForWriting;
    app->platform.readFromFile        = Win32_readFromFile;
    app->platform.writeToFile         = Win32_writeToFile;

    Defaults defaults;
    _initApp(&defaults, (u32*)window_content_memory);

    info.bmiHeader.biSize        = sizeof(info.bmiHeader);
    info.bmiHeader.biCompression = BI_RGB;
    info.bmiHeader.biBitCount    = 32;
    info.bmiHeader.biPlanes      = 1;

    window_class.lpszClassName  = "RnDer";
    window_class.hInstance      = hInstance;
    window_class.lpfnWndProc    = WndProc;
    window_class.style          = CS_OWNDC|CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS;
    window_class.hCursor        = LoadCursorA(null, IDC_ARROW);

    if (!RegisterClassA(&window_class)) return -1;

    win_rect.top = 0;
    win_rect.left = 0;
    win_rect.right  = defaults.width;
    win_rect.bottom = defaults.height;
    AdjustWindowRect(&win_rect, WS_OVERLAPPEDWINDOW, false);

    window = CreateWindowA(
            window_class.lpszClassName,
            defaults.title,
            WS_OVERLAPPEDWINDOW,

            CW_USEDEFAULT,
            CW_USEDEFAULT,
            win_rect.right - win_rect.left,
            win_rect.bottom - win_rect.top,

            null,
            null,
            hInstance,
            null
    );
    if (!window)
        return -1;

    raw_input_device.usUsagePage = 0x01;
    raw_input_device.usUsage = 0x02;
    if (!RegisterRawInputDevices(&raw_input_device, 1, sizeof(raw_input_device)))
        return -1;

    win_dc = GetDC(window);

    SetICMMode(win_dc, ICM_OFF);



    ShowWindow(window, nCmdShow);

    MSG message;
    while (app->is_running) {
        while (PeekMessageA(&message, null, 0, 0, PM_REMOVE)) {
            TranslateMessage(&message);
            DispatchMessageA(&message);
        }
        _windowRedraw();
        InvalidateRgn(window, null, false);
    }

    return 0;
}