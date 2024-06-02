#include <windows.h>
#include <iostream>

using namespace std;

import Hooker;
import procUtils;
import CmdlineMgr;
import hooked_funcs;
//import hooked_func;
void setHook()
{
	DetoursHooker hooker;
	hooker.endeque(
		{
		{&CreateProcessA_raw,CreateProcessA_mod},
		{ &CreateProcessW_raw,CreateProcessW_mod },
		{ &CreateProcessAsUserA_raw,CreateProcessAsUserA_mod },
		{ &CreateProcessAsUserW_raw,CreateProcessAsUserW_mod },

		}

	);
	hooker.setHook();

}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
	{
		
#ifdef BS_DBG
		//MessageBoxA(nullptr, GetCommandLineA(), "", 0);
		MessageBeep(MB_ICONINFORMATION);
#endif

		DisableThreadLibraryCalls(hModule);
		portableByCmdline(GetCommandLineA());
		setHook();
		break;
	}
	case DLL_PROCESS_DETACH:
	{
		//RemoveHook();
		break;
	}
	}
	return TRUE; // 返回TRUE表示初始化成功，返回FALSE可能导致DLL加载失败
}
