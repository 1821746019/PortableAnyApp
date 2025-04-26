module;
#include <ntdll.h>
export module I_RegCoreNt;

import std;
import Hooker;
import func2hook;
import ConfigMgr;
import selfInfo;
import RegHandlerMgr;
using namespace std;
void setHook() {
  //{&RtlInitUnicodeString_raw, &RtlInitUnicodeString_mod},
  //{&RtlInitUnicodeStringEx_raw, &RtlInitUnicodeStringEx_mod},
  DetoursHooker hooker;
  // hooker.endeque({

  //    {&NtOpenKey_raw, &NtOpenKey_mod},
  //    {&NtOpenKeyEx_raw, &NtOpenKeyEx_mod},
  //    {&NtCreateKey_raw, &NtCreateKey_mod},
  //});

  hooker.endeque({
      //{&BaseRegCreateKey_raw, &BaseRegCreateKey_mod},

  });
  auto i = RegEnumKeyExW_raw;
  hooker.setHook();
  // NtOpenKey(nullptr, 0, nullptr);
}

extern "C" __declspec(dllexport) BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved) {
  if (dwReason == DLL_PROCESS_ATTACH) {
#ifdef BS_DBG
    // AllocConsole();
    // freopen("CONOUT$", "w", stdout);
#endif
    DisableThreadLibraryCalls(hModule);
    try {
      ConfigMgr::_ins_(selfDir() / (std::string(BS_TARGET_NAME) + ".toml"));
      RegHandlerMgr::_ins_();
      //setHook();
    } catch (const std::exception& e) {
      MessageBoxA(nullptr, e.what(), "Exception occured", MB_ICONERROR);
    }
  }
  return TRUE;
}