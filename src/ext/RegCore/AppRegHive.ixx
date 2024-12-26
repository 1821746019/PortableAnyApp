
module;

#include <ntdll.h>

export module AppRegHive;

import std;
import selfDir;

std::wstring GetKeyPath(HKEY hKey) {
  // 初始缓冲区大小
  ULONG bufferSize = 0;
  NtQueryKey(hKey, KeyNameInformation, nullptr, 0, &bufferSize);
  if (bufferSize == 0) {
    throw std::runtime_error("NtQueryKey 失败。\n");
  }

  // 使用std::unique_ptr管理缓冲区
  std::unique_ptr<UINT8[]> buffer(new UINT8[bufferSize]);
  if (!buffer) {
    throw std::runtime_error("内存分配失败。\n");
  }

  NTSTATUS status = NtQueryKey(hKey, KeyNameInformation, buffer.get(),
                               bufferSize, &bufferSize);
  if (status == 0) {  // STATUS_SUCCESS
    PKEY_NAME_INFORMATION keyNameInfo = (PKEY_NAME_INFORMATION)(buffer.get());
    return std::wstring(keyNameInfo->Name, keyNameInfo->NameLength / 2);
  } else {
    throw std::runtime_error(
        std::format("NtQueryKey 失败，状态码: 0x%X\n", status));
  }
}

export {
  HKEY getAppHiveRootKey() {
    static HKEY ret = [] {
      HKEY ret;
      auto status = RegLoadAppKeyW((selfDir() / "AppRegHive").c_str(), &ret,
                                   KEY_ALL_ACCESS, REG_PROCESS_APPKEY, 0);
      if (status != ERROR_SUCCESS) {
        throw std::runtime_error(
            "failed to load AppRegHive. Please check if it exists in the same "
            "dir of the dll");
      }
      return ret;
    }();
    return ret;
  };
  std::wstring getAppRegHiveRegPath() {
    static std::wstring ret = GetKeyPath(getAppHiveRootKey());
    return ret;
  };
  const std::unordered_map<HKEY, HKEY> rootKeyMap = [] {
    std::unordered_map<HKEY, HKEY> ret;
    HKEY hKeyMachine = nullptr, hKeyUser = nullptr, hKeyUsers = nullptr,
         hKeyClasses = nullptr, hKeyCurrentConfig = nullptr;
    //DWORD disposition;  // 接收键的创建/打开状态

    // 打开或创建 HKEY_LOCAL_MACHINE
    LSTATUS resultMachine =
        RegCreateKeyExW(getAppHiveRootKey(),
                        L"HKEY_LOCAL_MACHINE",    // 子键名称
                        0,                        // 保留，一般填0
                        nullptr,                  // 类名（可填空）
                        REG_OPTION_NON_VOLATILE,  // 非易失性
                        KEY_ALL_ACCESS,           // 全权限，方便读写
                        nullptr,                  // 安全属性
                        &hKeyMachine,             // 返回创建好的子键
                        nullptr                   // 不关心是否已存在
        );

    // 打开或创建 HKEY_CURRENT_USER
    LSTATUS resultUser =
        RegCreateKeyExW(getAppHiveRootKey(), L"HKEY_CURRENT_USER", 0, nullptr,
                        REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, nullptr,
                        &hKeyUser, nullptr);
    LSTATUS resultClasses =
        RegCreateKeyExW(getAppHiveRootKey(), L"HKEY_CLASSES_ROOT", 0, nullptr,
                        REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, nullptr,
                        &hKeyClasses, nullptr);
    LSTATUS resultUsers = RegCreateKeyExW(
        getAppHiveRootKey(), L"HKEY_USERS", 0, nullptr, REG_OPTION_NON_VOLATILE,
        KEY_ALL_ACCESS, nullptr, &hKeyUsers, nullptr);
    LSTATUS resultCurrentConfig =
        RegCreateKeyExW(getAppHiveRootKey(), L"HKEY_CURRENT_CONFIG", 0, nullptr,
                        REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, nullptr,
                        &hKeyCurrentConfig, nullptr);
    if (resultMachine != ERROR_SUCCESS || resultUser != ERROR_SUCCESS ||
        resultClasses != ERROR_SUCCESS || resultUsers != ERROR_SUCCESS ||
        resultCurrentConfig != ERROR_SUCCESS) {
      if (hKeyMachine)
        RegCloseKey(hKeyMachine);
      if (hKeyUser)
        RegCloseKey(hKeyUser);
      throw std::runtime_error("failed to open or create machine or user key");
    }

    ret[HKEY_LOCAL_MACHINE] = hKeyMachine;
    ret[HKEY_CURRENT_USER] = hKeyUser;
    ret[HKEY_CLASSES_ROOT] = hKeyClasses;
    ret[HKEY_USERS] = hKeyUsers;
    ret[HKEY_CURRENT_CONFIG] = hKeyCurrentConfig;
    return ret;
  }();
}