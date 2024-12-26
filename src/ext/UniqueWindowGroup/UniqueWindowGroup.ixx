module;
#include <Windows.h>

export module _;

import std;
import Hooker;
import func2hook;
using namespace std;

void setHook() {
  DetoursHooker hooker;
  hooker.endeque({{&SetCurrentProcessExplicitAppUserModelID_raw,
                   &SetCurrentProcessExplicitAppUserModelID_mod}});
  hooker.setHook();
}

BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD ul_reason_for_call,
                      LPVOID lpReserved) {
  if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
    DisableThreadLibraryCalls(hModule);
    setHook();
  }
  return TRUE;
}