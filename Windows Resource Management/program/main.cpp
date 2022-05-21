#include <shlwapi.h>
#include <windows.h>
#include "../library/library.h"
#pragma comment(lib, "library.lib")
#pragma comment(lib, "shlwapi.lib")
#define BUFSIZE 128
char BUF[BUFSIZE];
volatile library_info libInfo;

// ������� ���������, �������������� ��������� ��� ���� ���������
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    PAINTSTRUCT ps;
    HDC dc;

    switch (uMsg) {
    case WM_PAINT: // ����������� ����
        dc = BeginPaint(hWnd, &ps); // ������������� ���������
        wnsprintf(BUF, BUFSIZE - 1, // ����� ���������� � �����
            "Number of functional keys: %d",
            libInfo.funcKeysCount); // ���������� �������������� ������5
        TextOut(dc, 200, 50, BUF, lstrlen(BUF)); // ����� �� ����� (x=200, y=50)
        wnsprintf(BUF, BUFSIZE - 1,
            "L3 cache length: %d",
            (char)(libInfo.cpuidInfo)); // ����� ������ ����
        TextOut(dc, 200, 70, BUF, lstrlen(BUF));
        EndPaint(hWnd, &ps); // ���������� ���������
        break;
    case WM_DESTROY: // ����������� ���� (������������ �������� ExitProcess)
        PostQuitMessage(EXIT_SUCCESS); // ���������� ������
        break;
    default: // ��������� ����� ������ ��������� ����������� �������
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    return 0;
}

// �������, ������������� � ������ ���������� ������
DWORD WINAPI ThreadFunc(void* lParam) {
    GetInfo((library_info*)lParam);
    return 0;
}

// ����� ����� ������������ ����������
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR CmdLine, int nCmdShow) {
    HANDLE hThread; // ���������� ������
    DWORD IDThread; // ������������� ������
    WNDCLASS wcl; // ����� ����
    HWND hWnd; // ���������� ����
    ATOM regReturn; // ��������, ������������ �������� RegisterClass
    MSG msg; // ���������

    // ������� ���������� ����
    wcl.hInstance = hInstance; // ����������
    wcl.lpszClassName = "KeyCounterWindow"; // �������� ������
    wcl.lpfnWndProc = WndProc; // �������-���������� ���������
    wcl.style = CS_VREDRAW | CS_HREDRAW; // �����
    wcl.hIcon = NULL; // ���������� �����������
    wcl.hCursor = NULL; // ���������� �������
    wcl.lpszMenuName = NULL; // ���������� ����
    wcl.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // ���� ����
    regReturn = RegisterClass(&wcl); // ����������� ������

    // �������� ����
    hWnd = CreateWindow(
        "KeyCounterWindow", // ������������� �������� ������
        "Function Keys Counter", // ��������� ����
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, // ����� ����
        0, 0, // ���������� ������ �������� ���� ���� ��� ������ �� �����
        600, 200, // ������ � ������ ����
        HWND_DESKTOP, // ������� ���� ��� ������������ ����
        NULL, // �������� ����
        hInstance, // ��������� ����������
        NULL); // ��������� �� ������ �������� ����

    // �������� ������
    hThread = CreateThread(
        NULL, // ������ ������������ �����������
        0, // ��������� ������ �����
        (LPTHREAD_START_ROUTINE)ThreadFunc, // ��������� �� �������
        (LPVOID)&libInfo, // ����������, ������������ ������
        0, // ���� ������������� ������� ������
        &IDThread); // ����������, � ������� ����� ������� ������������� ������

    // ��������� ������ ��� ����������� ������, �������� ���� ��� ������
    if (!regReturn || !hWnd || !hThread)
        return GetLastError();

    WaitForSingleObject(hThread, INFINITE); // ����������� �������� ���������� ������
    CloseHandle(hThread); // �������� ����������� ������
    ShowWindow(hWnd, SW_SHOWNORMAL); // ����������� ����
    UpdateWindow(hWnd); // ���������� ����

    // ����, �������������� ���������
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg); // �������������� � ASCII
        DispatchMessage(&msg); // �������� ��������� � ������� ���������
    }

    ExitProcess(msg.wParam); // ���������� �������� � ��� �������
}
