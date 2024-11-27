#include <windows.h>
#include <iostream>

using namespace std;


//import procUtils;
import CmdlineMgr;
//import hooked_funcs;
// import hooked_func;
// void setHook() {
//  DetoursHooker hooker;
//  hooker.endeque({
//      {&CreateProcessInternalW_raw, CreateProcessInternalW_mod},
//
//  }
//
//  );
//  hooker.setHook();
//}

bool isParentPortabled() {
  bool ret = getenv("BS_PORTABLE") != nullptr;
  return ret;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved) {
  switch (dwReason) {
    case DLL_PROCESS_ATTACH: {
      DisableThreadLibraryCalls(hModule);
#ifdef BS_DBG
      // MessageBoxA(nullptr, GetCommandLineA(), "", 0);
      MessageBeep(MB_ICONINFORMATION);
#endif
      if (!isParentPortabled()) {
        _putenv("BS_PORTABLE=1");
        portableByCmdline(GetCommandLineA());
      }
      //setHook();
      break;
    }
    case DLL_PROCESS_DETACH: {
      // RemoveHook();
      break;
    }
  default: ;
  }
  return TRUE;  // 返回TRUE表示初始化成功，返回FALSE可能导致DLL加载失败
}
