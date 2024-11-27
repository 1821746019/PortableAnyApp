#pragma once
//#include <Windows.h>
struct HINSTANCE__;
typedef struct HINSTANCE__* HMODULE;
extern "C" __declspec(dllexport) int __stdcall DllMain(HMODULE hModule,
                                                       unsigned long dwReson,
                                                       void* lpReserved);