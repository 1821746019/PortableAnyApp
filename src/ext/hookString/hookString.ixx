module;
#include <Windows.h>
export module hookString;

import std;
import Hooker;
import funcsToHook;
using namespace std;

bool setHook() {
  if (!initRawFunc())
    return false;
  MessageBeep(MB_ICONINFORMATION);
  DetoursHooker hooker;

  hooker.endeque({
      {&strlen_raw, &strlen_mod},
      {&wcslen_raw, &wcslen_mod},
      {&v8_String_NewFromUtf8_raw, &v8_String_NewFromUtf8_mod},
      {&v8_String_NewFromUtf8Literal_raw, &v8_String_NewFromUtf8Literal_mod},
  });
  hooker.setHook();

  return true;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReson, LPVOID lpReserved) {
  if (dwReson != DLL_PROCESS_ATTACH)
    return TRUE;

  DisableThreadLibraryCalls(hModule);
  // init the ConfigMgr and AtExit
  thread([] {
    try {
      while (GetAsyncKeyState(VK_F4) >= 0 || !setHook()) {
        Sleep(150);
        // MessageBeep(MB_ICONERROR);
      };
    } catch (const exception& e) {
      MessageBoxA(nullptr, e.what(), "Exception occured", MB_ICONERROR);
      exit(-1);
    }
  }).detach();

  return TRUE;
}