// esp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Offsets.hpp"
#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>

#define EnemyPen 0x000000FF
HBRUSH EnemyBrush = CreateSolidBrush(0x000000FF);

// get screen dimensions
int screenX = GetSystemMetrics(SM_CXSCREEN);
int screenY = GetSystemMetrics(SM_CYSCREEN);

DWORD GetProcId(const wchar_t* procName)
{
    DWORD procId = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnap != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32 procEntry;
        procEntry.dwSize = sizeof(procEntry);

        if (Process32First(hSnap, &procEntry))
        {
            do
            {
                if (!_wcsicmp(procEntry.szExeFile, procName))
                {
                    procId = procEntry.th32ProcessID;
                    break;
                }
            } while (Process32Next(hSnap, &procEntry));
        }
    }
    CloseHandle(hSnap);
    return procId;
}

uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName)
{
    uintptr_t modBaseAddr = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
    if (hSnap != INVALID_HANDLE_VALUE)
    {
        MODULEENTRY32 modEntry;
        modEntry.dwSize = sizeof(modEntry);
        if (Module32First(hSnap, &modEntry))
        {
            do
            {
                if (!_wcsicmp(modEntry.szModule, modName))
                {
                    modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
                    break;
                }
            } while (Module32Next(hSnap, &modEntry));
        }
    }
    CloseHandle(hSnap);
    return modBaseAddr;
}

uintptr_t moduleBase = GetModuleBaseAddress(GetProcId(L"csgo.exe"), L"client_panaroma.dll");
HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, GetProcId(L"csgo.exe"));
HDC hdc = GetDC(FindWindowA(NULL, "Counter-Strike: Global Offensive"));

template<typename T> T RPM(SIZE_T address) {
    T buffer;

    ReadProcessMemory(hProcess, (LPCVOID)address, &buffer, sizeof(T), NULL);

    return buffer;
}

struct view_matrix_t {
    float* operator[ ](int index) {
        return matrix[index];
    };

    float matrix[4][4];
};

struct Vector3
{
    float x, y, z;
};

// maths
Vector3 WorldToScreen(const Vector3 pos, view_matrix_t matrix)
{
    float _x = matrix[0][0] * pos.x + matrix[0][1] * pos.y + matrix[0][2] * pos.z + matrix[0][3];
    float _y = matrix[1][0] * pos.x + matrix[1][1] * pos.y + matrix[1][2] * pos.z + matrix[1][3];

    float w = matrix[3][0] * pos.x + matrix[3][1] * pos.y + matrix[3][2] * pos.z + matrix[3][3];

    float inv_w = 1.f / w;
    _x *= inv_w;
    _y *= inv_w;

    float x = screenX * .5f;
    float y = screenY * .5f;

    x += 0.5f * _x * screenX + 0.5f;
    y -= 0.5 * _y * screenY + 0.5f;

    return { x, y, w };
}

int main()
{
}

