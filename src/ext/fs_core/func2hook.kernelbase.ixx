module;
#include <windows.h>

export module func2hook.kernelbase;
import std;
import fs_common;
import PathJudge;
import extraNativeFuncs;
import my_converter.str;

using func_t = BOOL(WINAPI)(
    LPCWSTR lpExistingFileName,
    LPCWSTR lpNewFileName,
    LPPROGRESS_ROUTINE lpProgressRoutine,
    LPVOID lpData,
    DWORD dwFlags,
    HANDLE hTransaction
);
func_t* pMoveFileWithProgressTransactedW = [] {
  auto h_module = LoadLibraryW(L"kernelbase.dll");
  if (!h_module) {
    auto msg = "Failed to load kernelbase.dll. There are some errors";
    MessageBoxA(nullptr, msg, "Error", MB_ICONERROR);
    throw std::runtime_error(msg);
  }
  return (func_t*)GetProcAddress(h_module, "MoveFileWithProgressTransactedW");
}();
// 提取路径函数
std::wstring ExtractPath(const std::wstring& input) {
  static std::wregex pattern(LR"(([a-zA-Z]:\\(?:[\w\s\-\.]+\\)*[\w\s\-\.]+))");  // 匹配路径
  std::wsmatch match;

  if (std::regex_search(input, match, pattern)) {
    return match.str(0);  // 返回匹配到的路径
  }

  return L"";  // 如果没有匹配到路径，返回空字符串
}
export {
  __int64 __fastcall CreateProcessInternalW_mod(
      void* hToken, const WCHAR* lpApplicationName, const WCHAR* lpCommandLine, WCHAR* lpProcessAttributes,
      WCHAR* lpThreadAttributes, int bInheritHandles, int dwCreationFlags, __int64 lpEnvironment,
      const WCHAR* lpCurrentDirectory, STARTUPINFOW* lpStartupInfo, PROCESS_INFORMATION* lpProcessInformation
  ) {
    // use regex to check if there are paths in lpApplicationName or lpCommandLine
    std::wstring pathAppName, pathCmdline;
    std::wsmatch matchAppName, matchCmdline;
    static std::wregex pattern(LR"(([a-zA-Z]:\\\S+|"[a-zA-Z]:\\.*?"))");
    if (!std::regex_search(std::wstring(lpApplicationName), matchAppName, pattern))
      wchar_t new_path[MAX_PATH + 1];
    auto path_judge = PathJudge::_ins_();
    if (path_judge->judgeAndRedirect(lpApplicationName, new_path, (int)std::size(new_path))) {
      lpApplicationName = new_path;
    }
    return CreateProcessInternalW_raw(
        hToken, lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles,
        dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation
    );
  }
  decltype(&CreateFileW) CreateFileW_raw = &CreateFileW;
  HANDLE WINAPI CreateFileW_mod(
      LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
      LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes,
      HANDLE hTemplateFile
  ) {
    wchar_t new_path[MAX_PATH + 1];
    auto path_judge = PathJudge::_ins_();
    if (path_judge->judgeAndRedirect(lpFileName, new_path, (int)std::size(new_path))) {
      lpFileName = new_path;
    }
    return CreateFileW_raw(
        lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition,
        dwFlagsAndAttributes, hTemplateFile
    );
  }
  // it init a unicode str inside and directly call the nt func, so detour it to
  // CreateFileW
  decltype(&CreateFileA) CreateFileA_raw = &CreateFileA;
  HANDLE WINAPI CreateFileA_mod(
      LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes,
      DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile
  ) {
    return CreateFileW(
        brv::strConvert(lpFileName).c_str(), dwDesiredAccess, dwShareMode, lpSecurityAttributes,
        dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile
    );
  }
  decltype(&CreateFile2) CreateFile2_raw = &CreateFile2;
  HANDLE WINAPI CreateFile2_mod(
      LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationDisposition,
      LPCREATEFILE2_EXTENDED_PARAMETERS pCreateExParams
  ) {
    wchar_t new_path[MAX_PATH + 1];
    auto path_judge = PathJudge::_ins_();
    if (path_judge->judgeAndRedirect(lpFileName, new_path, (int)std::size(new_path))) {
      lpFileName = new_path;
    }
    return CreateFile2_raw(lpFileName, dwDesiredAccess, dwShareMode, dwCreationDisposition, pCreateExParams);
  }
  decltype(&DeleteFileW) DeleteFileW_raw = &DeleteFileW;
  BOOL WINAPI DeleteFileW_mod(LPCWSTR lpFileName) {
    wchar_t new_path[MAX_PATH + 1];
    auto path_judge = PathJudge::_ins_();
    if (path_judge->judgeAndRedirect(lpFileName, new_path, (int)std::size(new_path))) {
      lpFileName = new_path;
    }
    return DeleteFileW_raw(lpFileName);
  }
  decltype(&CreateDirectoryW) CreateDirectoryW_raw = &CreateDirectoryW;
  BOOL CreateDirectoryW_mod(LPCWSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes) {
    wchar_t new_path[MAX_PATH + 1];
    auto path_judge = PathJudge::_ins_();
    if (path_judge->judgeAndRedirect(lpPathName, new_path, (int)std::size(new_path))) {
      lpPathName = new_path;
    }
    return CreateDirectoryW_raw(lpPathName, lpSecurityAttributes);
  }
  decltype(&CreateDirectoryExW) CreateDirectoryExW_raw = &CreateDirectoryExW;
  BOOL CreateDirectoryExW_mod(
      LPCWSTR lpTemplateDirectory, LPCWSTR lpNewDirectory, LPSECURITY_ATTRIBUTES lpSecurityAttributes
  ) {
    wchar_t templateDir_new[MAX_PATH + 1];
    wchar_t newDir_new[MAX_PATH + 1];

    auto path_judge = PathJudge::_ins_();
    if (path_judge->judgeAndRedirect(lpTemplateDirectory, templateDir_new, (int)std::size(templateDir_new))) {
      lpTemplateDirectory = templateDir_new;
    }
    if (path_judge->judgeAndRedirect(lpNewDirectory, newDir_new, (int)std::size(newDir_new))) {
      lpNewDirectory = newDir_new;
    }
    return CreateDirectoryExW_raw(lpTemplateDirectory, lpNewDirectory, lpSecurityAttributes);
  }

  decltype(pMoveFileWithProgressTransactedW) MoveFileWithProgressTransactedW_raw =
      pMoveFileWithProgressTransactedW;
  BOOL WINAPI MoveFileWithProgressTransactedW_mod(
      LPCWSTR lpExistingFileName, LPCWSTR lpNewFileName, LPPROGRESS_ROUTINE lpProgressRoutine, LPVOID lpData,
      DWORD dwFlags, HANDLE hTransaction
  ) {
    if (!lpExistingFileName || !lpNewFileName)
      return false;
    wchar_t src_new[MAX_PATH + 1], dst_new[MAX_PATH + 1];
    auto path_judge = PathJudge::_ins_();
    if (path_judge->judgeAndRedirect(lpExistingFileName, src_new, (int)std::size(src_new))) {
      lpExistingFileName = src_new;
    }
    if (path_judge->judgeAndRedirect(lpNewFileName, dst_new, (int)std::size(dst_new))) {
      lpNewFileName = dst_new;
    } else  // 判断 lpNewFileName 是否是recycle bin 的路径
    {
      constexpr wchar_t RECYCLE_BN_PREFIX[] = LR"(:\$RECYCLE.BIN\)";
      if (_wcsnicmp(lpNewFileName + 1, RECYCLE_BN_PREFIX, std::size(RECYCLE_BN_PREFIX) - 1 - 1) == 0) {
        wcscpy_s(dst_new, lpNewFileName);
        dst_new[0] = lpExistingFileName[0];
        lpNewFileName = dst_new;
      }
    }

    return MoveFileWithProgressTransactedW_raw(
        lpExistingFileName, lpNewFileName, lpProgressRoutine, lpData, dwFlags, hTransaction
    );
  }
  decltype(&FindFirstFileW) FindFirstFileW_raw = &FindFirstFileW;
  HANDLE WINAPI FindFirstFileW_mod(LPCWSTR lpFileName, LPWIN32_FIND_DATAW lpFindFileData) {
    wchar_t new_path[MAX_PATH + 1];
    auto path_judge = PathJudge::_ins_();
    if (path_judge->judgeAndRedirect(lpFileName, new_path, (int)std::size(new_path))) {
      lpFileName = new_path;
    }
    return FindFirstFileW_raw(lpFileName, lpFindFileData);
  }

  decltype(&FindFirstFileExW) FindFirstFileExW_raw = &FindFirstFileExW;
  HANDLE WINAPI FindFirstFileExW_mod(
      LPCWSTR lpFileName, FINDEX_INFO_LEVELS fInfoLevelId, LPVOID lpFindFileData, FINDEX_SEARCH_OPS fSearchOp,
      LPVOID lpSearchFilter, DWORD dwAdditionalFlags
  ) {
    wchar_t new_path[MAX_PATH + 1];
    auto path_judge = PathJudge::_ins_();
    if (path_judge->judgeAndRedirect(lpFileName, new_path, (int)std::size(new_path))) {
      lpFileName = new_path;
    }
    return FindFirstFileExW_raw(
        lpFileName, fInfoLevelId, lpFindFileData, fSearchOp, lpSearchFilter, dwAdditionalFlags
    );
  }
  decltype(&FindNextFileW) FindNextFileW_raw = &FindNextFileW;
  BOOL WINAPI FindNextFileW_mod(HANDLE hFindFile, LPWIN32_FIND_DATAW lpFindFileData) {
    wchar_t new_path[MAX_PATH + 1];
    auto path_judge = PathJudge::_ins_();
    if (path_judge->judgeAndRedirect(lpFindFileData->cFileName, new_path, (int)std::size(new_path))) {
      wcscpy_s(lpFindFileData->cFileName, std::size(lpFindFileData->cFileName), new_path);
    }
    return FindNextFileW_raw(hFindFile, lpFindFileData);
  }

  // decltype(&RemoveDirectoryW) RemoveDirectoryW_raw = &RemoveDirectoryW;
  // BOOL WINAPI RemoveDirectoryW_mod(LPCWSTR lpPathName) {
  //   wchar_t new_path[MAX_PATH + 1];
  //   auto path_judge = PathJudge::_ins_();
  //   if (path_judge->judgeAndRedirect(lpPathName, new_path,
  //                                    (int)std::size(new_path))) {
  //     lpPathName = new_path;
  //   }
  //   return RemoveDirectoryW_raw(lpPathName);
  // }
}