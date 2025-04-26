#include <windows.h>
#include <iostream>

using namespace std;

// import procUtils;
import CmdlineMgr;
//import hooked_funcs;
//import Hooker;
//void setHook() {
//  DetoursHooker hooker;
//  hooker.endeque({
//      {&CreateProcessInternalW_raw, CreateProcessInternalW_mod},
//
//  }
//
//  );
//  hooker.setHook();
//}
//
bool isParentPortabled() {
  const char* FLAG = "BS_PORTABLE";
  bool ret = getenv(FLAG) != nullptr;
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
      //  utools程序启动时指定--user-data-dir会导致无法正常启动，只能在创建子进程时下手。废弃，实测对子进程指定--user-data-dir无效，这命令行选项只能在主进程中指定。
      // setHook();
      break;
    }
    case DLL_PROCESS_DETACH: {
      // RemoveHook();
      break;
    }
    default:;
  }
  return TRUE;  // 返回TRUE表示初始化成功，返回FALSE可能导致DLL加载失败
}
