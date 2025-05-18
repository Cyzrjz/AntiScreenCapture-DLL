// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include <windows.h>
#include <thread>

// 选择保护模式：
// WDA_MONITOR         = 0x00000001  （Windows7+，截屏时黑屏）
// WDA_EXCLUDEFROMCAPTURE = 0x00000011 （Windows10 2004+，截屏与窗口框架均隐藏）
static const DWORD affinityMode = WDA_MONITOR;

void AntiCapture() {
    DWORD pid = GetCurrentProcessId();

    //枚举所有所有顶层窗口句柄
    EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL{ 
        DWORD wnpid = 0;
        GetWindowThreadProcessId(hwnd, &wnpid);

        //如果当前窗口的进程ID和DLL所在进程ID一致，并且窗口可见，则设置窗口的显示模式
        if (wnpid == (DWORD)lParam && IsWindowVisible(hwnd))
        {
            SetWindowDisplayAffinity(hwnd, affinityMode);
        }
        return TRUE;
        }, (LPARAM)pid);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{

    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(hModule);
            std::thread(AntiCapture).detach();
            break;
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
