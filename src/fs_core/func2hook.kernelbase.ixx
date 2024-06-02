module;
#include "PathJudge.h"
#include <ntdll.h>
#include <stdexcept>
export module func2hook.kernelbase;
import fs_common;



using func_t = BOOL(WINAPI *)(LPCWSTR lpExistingFileName, LPCWSTR lpNewFileName, LPPROGRESS_ROUTINE lpProgressRoutine, LPVOID lpData, DWORD dwFlags, HANDLE hTransaction);
func_t MoveFileWithProgressTransactedW = []
	{
		auto h_module = LoadLibraryW(L"kernelbase.dll");
	if(!h_module)
	{
		auto msg = "Failed to load kernelbase.dll. There are some errors";
		MessageBoxA(nullptr, msg, "Error", MB_ICONERROR);
		throw std::runtime_error(msg);
	}
		return (func_t)GetProcAddress(h_module, "MoveFileWithProgressTransactedW");
	}();

export{
	decltype(MoveFileWithProgressTransactedW) MoveFileWithProgressTransactedW_raw = MoveFileWithProgressTransactedW;
	BOOL WINAPI MoveFileWithProgressTransactedW_mod(LPCWSTR lpExistingFileName, LPCWSTR lpNewFileName, LPPROGRESS_ROUTINE lpProgressRoutine, LPVOID lpData, DWORD dwFlags, HANDLE hTransaction)
	{

		wchar_t src_new[MAX_PATH+1], dst_new[MAX_PATH+1];
		auto path_judge = PathJudge::_ins_();
		if (path_judge->judgeAndRedirect(lpExistingFileName, src_new, (int)std::size(src_new)))
		{
			lpExistingFileName = src_new;
		}
		if (path_judge->judgeAndRedirect(lpNewFileName, dst_new, (int)std::size(dst_new)))
		{
			lpNewFileName = dst_new;
		}
		
		return MoveFileWithProgressTransactedW_raw(lpExistingFileName, lpNewFileName, lpProgressRoutine, lpData, dwFlags, hTransaction);
	}

}