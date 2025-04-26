export module I_fs_core;

import std;
import Hooker;
import func2hook;
import func2hook.kernelbase;
import func2hook.maybe;
import VarInterpolationMgr;
import ConfigMgr;
import fs_common;
import PathJudge;

export {
  void setHook();
  void fs_core_init(const std::wstring& config_path);
  void fs_core_uninit();
}

void setHook() {
  DetoursHooker hooker;

  hooker.endeque({
      {&NtCreateFile_raw, &NtCreateFile_mod},
      {&NtCreatePort_raw, &NtCreatePort_mod},
      {&NtCreateNamedPipeFile_raw, &NtCreateNamedPipeFile_mod},
      {&NtOpenFile_raw, &NtOpenFile_mod},
      {&NtDeleteFile_raw, &NtDeleteFile_mod},
      {&NtQueryFullAttributesFile_raw, &NtQueryFullAttributesFile_mod},
      {&NtQueryAttributesFile_raw, &NtQueryAttributesFile_mod},
      {&NtOpenSymbolicLinkObject_raw, &NtOpenSymbolicLinkObject_mod},
      //{&NtDeviceIoControlFile_raw, &NtDeviceIoControlFile_mod},
      //{ &NtQueryDirectoryFile_raw, &NtQueryDirectoryFile_mod },
      //{ &NtSetInformationFile_raw, &NtSetInformationFile_mod },
      {&MoveFileWithProgressTransactedW_raw,
       &MoveFileWithProgressTransactedW_mod},
      {&FindFirstFileW_raw, &FindFirstFileW_mod},
      {&FindFirstFileExW_raw, &FindFirstFileExW_mod},
      {&FindNextFileW_raw, &FindNextFileW_mod},
      //{&CreateFileW_raw, &CreateFileW_mod},
      //{&CreateFile2_raw, &CreateFile2_mod},
      //{&CreateFileA_raw, &CreateFileA_mod},
      //{&CreateDirectoryW_raw, &CreateDirectoryW_mod},
      //{&CreateDirectoryExW_raw, &CreateDirectoryExW_mod},
      // improve the compatibility for win11, need to hook the following
      // functions
      // DeleteFileW will call NtOpenFile firstly, so no need to hook it
      //{&DeleteFileW_raw, &DeleteFileW_mod},

  });

  // hooker.endeque({&GetNamedSecurityInfoW_raw, &GetNamedSecurityInfoW_mod});
  hooker.setHook();
}

void fs_core_init(const std::wstring& config_path) {
  VarInterpolationMgr macro_mgr({{L"USER_HOME", getUserHomePath()}});
  ConfigMgr config_mgr(&macro_mgr, config_path);
  PathJudge::_init_(config_mgr.fsConfig());
  setHook();
}

void fs_core_uninit() {}