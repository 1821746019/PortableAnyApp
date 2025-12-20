module;

#include <Windows.h>
// #include <combaseapi.h>
//  #include <toml++/toml.hpp>

export module _;
import std;
import Hooker;
import fs_related;
using namespace std;
namespace fs = filesystem;

decltype(&CoCreateInstance) CoCreateInstance_raw =
    (decltype(&CoCreateInstance))GetProcAddress(GetModuleHandleA("combase.dll"), "CoCreateInstance");
decltype(&CoCreateInstanceEx) CoCreateInstanceEx_raw =
    (decltype(&CoCreateInstanceEx))GetProcAddress(GetModuleHandleA("combase.dll"), "CoCreateInstance");
HRESULT __stdcall CoCreateInstance_mod(
    REFCLSID rclsid,
    LPUNKNOWN pUnkOuter,
    DWORD dwClsContext,
    REFIID riid,
    LPVOID FAR* ppv
) {
  // 直接返回失败
  return E_FAIL;
}
HRESULT __stdcall CoCreateInstanceEx_mod(
    const IID* const Clsid,
    IUnknown* punkOuter,
    DWORD dwClsCtx,
    COSERVERINFO* pServerInfo,
    DWORD dwCount,
    MULTI_QI* pResults
) {
  // 直接返回失败
  return E_FAIL;
}
void setHook() {
  DetoursHooker hooker;
  hooker.endeque({
      {&CoCreateInstance_raw, &CoCreateInstance_mod},
      {&CoCreateInstanceEx_raw, &CoCreateInstanceEx_mod},
  });
  hooker.setHook();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReson, LPVOID lpReserved) {
  if (dwReson != DLL_PROCESS_ATTACH)
    return TRUE;

  DisableThreadLibraryCalls(hModule);

  // init the ConfigMgr
  try {
    setHook();
  } catch (const exception& e) {
    MessageBoxA(nullptr, e.what(), "Exception occured", MB_ICONERROR);
    exit(-1);
  }
  return TRUE;
}