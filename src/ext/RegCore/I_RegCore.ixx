module;
#include <ntdll.h>
export module I_RegCore;

import std;
import Hooker;
import func2hook;
import func2hook.kernel;
import func2hook.kernel.raw;
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
      {&RegQueryInfoKeyW_raw, &RegQueryInfoKeyW_mod},
      {&RegQueryInfoKeyA_raw, &RegQueryInfoKeyA_mod},
      {&RegEnumKeyExW_raw, &RegEnumKeyExW_mod},
      {&RegEnumKeyExA_raw, &RegEnumKeyExA_mod},
      /*尚不清楚是否要hook，其内部实现未发现调用kernel层的函数而是直接调了nt层的NtQueryValueKey
       * 应该是和前面QueryInfo、EnumKey一样，要对根键(比如HKEY_LOCAL_MACHINE)Query或者Enum时才需要hook，
       * 实测无需hook能正常获取SOFTWARE\Microsoft\Cryptography\MachineGuid的值
       */
      //{&RegQueryValueExW_raw, &RegQueryValueExW_mod},
      //{&RegQueryValueExA_raw, &RegQueryValueExA_mod},
      {&RegOpenKeyExW_raw, &RegOpenKeyExW_mod},
      {&RegOpenKeyExA_raw, &RegOpenKeyExA_mod},
      {&RegCreateKeyExInternalW_raw, &RegCreateKeyExInternalW_mod},
      {&RegCreateKeyExInternalA_raw, &RegCreateKeyExInternalA_mod},
      //
      {&RegEnumValueW_raw, &RegEnumValueW_mod},
      {&RegEnumValueA_raw, &RegEnumValueA_mod},  // 莫非就是导致bug的原因, 之前用了W
      {&RegQueryValueExW_raw, &RegQueryValueExW_mod},
      {&RegQueryValueExA_raw, &RegQueryValueExA_mod},
      //{&RegSetValueExW_raw, &RegSetValueExW_mod},
      //{&RegSetValueExA_raw, &RegSetValueExA_mod},
  });
  auto i = RegEnumKeyExW_raw;
  hooker.setHook();
  // NtOpenKey(nullptr, 0, nullptr);
}

extern "C" __declspec(dllexport) BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved) {
//  if (dwReason == DLL_PROCESS_ATTACH) {
//#ifdef BS_DBG
//    // AllocConsole();
//    // freopen("CONOUT$", "w", stdout);
//#endif
//    DisableThreadLibraryCalls(hModule);
//    try {
//      ConfigMgr::_ins_(selfDir() / (std::string(BS_TARGET_NAME) + ".toml"));
//      RegHandlerMgr::_ins_();
//      //setHook();
//    } catch (const std::exception& e) {
//      MessageBoxA(nullptr, e.what(), "Exception occured", MB_ICONERROR);
//    }
//  }
  return TRUE;
}