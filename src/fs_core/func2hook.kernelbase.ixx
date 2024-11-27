module;
#include <windows.h>

export module func2hook.kernelbase;
import std;
import fs_common;
import PathJudge;

using func_t = BOOL(WINAPI)(LPCWSTR lpExistingFileName,
                            LPCWSTR lpNewFileName,
                            LPPROGRESS_ROUTINE lpProgressRoutine,
                            LPVOID lpData,
                            DWORD dwFlags,
                            HANDLE hTransaction);
func_t* pMoveFileWithProgressTransactedW = [] {
  auto h_module = LoadLibraryW(L"kernelbase.dll");
  if (!h_module) {
    auto msg = "Failed to load kernelbase.dll. There are some errors";
    MessageBoxA(nullptr, msg, "Error", MB_ICONERROR);
    throw std::runtime_error(msg);
  }
  return (func_t*)GetProcAddress(h_module, "MoveFileWithProgressTransactedW");
}();

export {
  decltype(pMoveFileWithProgressTransactedW)
      MoveFileWithProgressTransactedW_raw = pMoveFileWithProgressTransactedW;
  BOOL WINAPI MoveFileWithProgressTransactedW_mod(
      LPCWSTR lpExistingFileName, LPCWSTR lpNewFileName,
      LPPROGRESS_ROUTINE lpProgressRoutine, LPVOID lpData, DWORD dwFlags,
      HANDLE hTransaction) {
    wchar_t src_new[MAX_PATH + 1], dst_new[MAX_PATH + 1];
    auto path_judge = PathJudge::_ins_();
    if (path_judge->judgeAndRedirect(lpExistingFileName, src_new,
                                     (int)std::size(src_new))) {
      lpExistingFileName = src_new;
    }
    if (path_judge->judgeAndRedirect(lpNewFileName, dst_new,
                                     (int)std::size(dst_new))) {
      lpNewFileName = dst_new;
    }

    return MoveFileWithProgressTransactedW_raw(lpExistingFileName,
                                               lpNewFileName, lpProgressRoutine,
                                               lpData, dwFlags, hTransaction);
  }
  decltype(&FindFirstFileExW) FindFirstFileExW_raw = &FindFirstFileExW;
  HANDLE WINAPI FindFirstFileExW_mod(
      LPCWSTR lpFileName, FINDEX_INFO_LEVELS fInfoLevelId,
      LPVOID lpFindFileData, FINDEX_SEARCH_OPS fSearchOp, LPVOID lpSearchFilter,
      DWORD dwAdditionalFlags) {
    wchar_t new_path[MAX_PATH + 1];
    auto path_judge = PathJudge::_ins_();
    if (path_judge->judgeAndRedirect(lpFileName, new_path,
                                     (int)std::size(new_path))) {
      lpFileName = new_path;
    }
    return FindFirstFileExW_raw(lpFileName, fInfoLevelId, lpFindFileData,
                                fSearchOp, lpSearchFilter, dwAdditionalFlags);
  }
  decltype(&FindNextFileW) FindNextFileW_raw = &FindNextFileW;
  BOOL WINAPI FindNextFileW_mod(HANDLE hFindFile,
                                LPWIN32_FIND_DATAW lpFindFileData) {
    wchar_t new_path[MAX_PATH + 1];
    auto path_judge = PathJudge::_ins_();
    if (path_judge->judgeAndRedirect(lpFindFileData->cFileName, new_path,
                                     (int)std::size(new_path))) {
      wcscpy_s(lpFindFileData->cFileName, std::size(lpFindFileData->cFileName),
               new_path);
    }
    return FindNextFileW_raw(hFindFile, lpFindFileData);
  }
}