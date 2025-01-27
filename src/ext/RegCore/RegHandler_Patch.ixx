module;
#include <Windows.h>

export module RegHandler_Patch;
import std;
import RegHandler_B;
import func2hook.kernel.raw;
import AppRegHive;
import reg_common;


export class RegHandler_Patch : public RegHandler_B {
 public:
  //--------------------------------------------------------------------------------
  // RegCreateKeyExInternalW
  //--------------------------------------------------------------------------------
  LSTATUS
  RegCreateKeyExInternalW(
      HKEY hKey,
      LPCWSTR lpSubKey,
      DWORD Reserved,
      LPWSTR lpClass,
      DWORD dwOptions,
      REGSAM samDesired,
      LPSECURITY_ATTRIBUTES lpSecurityAttributes,
      PHKEY phkResult,
      LPDWORD lpdwDisposition,
      LPVOID lpReserved
  ) override {
    // 直接调用底层 raw
    LSTATUS status = RegCreateKeyExInternalW_raw(
        hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired, lpSecurityAttributes, phkResult,
        lpdwDisposition, lpReserved
    );
    return status;
  }

  //--------------------------------------------------------------------------------
  // RegOpenKeyExW
  //--------------------------------------------------------------------------------
  LSTATUS RegOpenKeyExW(HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult)
      override {
    // 直接调用原始的 raw
    return RegOpenKeyExW_raw(hKey, lpSubKey, ulOptions, samDesired, phkResult);
  }

  //--------------------------------------------------------------------------------
  // RegQueryInfoKeyW
  //--------------------------------------------------------------------------------
  LSTATUS RegQueryInfoKeyW(
      HKEY hKey,
      LPWSTR lpClass,
      LPDWORD lpcClass,
      LPDWORD lpReserved,
      LPDWORD lpcSubKeys,
      LPDWORD lpcMaxSubKeyLen,
      LPDWORD lpcMaxClassLen,
      LPDWORD lpcValues,
      LPDWORD lpcMaxValueNameLen,
      LPDWORD lpcMaxValueLen,
      LPDWORD lpcbSecurityDescriptor,
      PFILETIME lpftLastWriteTime
  ) override {
    // 先尝试在 AppRegHive 中打开对应的子键
    std::wstring path_old = abstractNonRootFromAbsRegPath(GetKeyPath(hKey));

    HKEY hVirtualSubKey = nullptr;
    LSTATUS status = RegOpenKeyExW_raw(
        getAppHiveRootKey(),
        path_old.c_str(),  // 这里可以带"\\"分隔的多级路径
        0,
        KEY_READ,  // 根据需要可更改权限
        &hVirtualSubKey
    );

    if (status == ERROR_SUCCESS) {
      // 在虚拟子键上调用 QueryInfoKey
      status = RegQueryInfoKeyW_raw(
          hVirtualSubKey, lpClass, lpcClass, lpReserved, lpcSubKeys, lpcMaxSubKeyLen, lpcMaxClassLen,
          lpcValues, lpcMaxValueNameLen, lpcMaxValueLen, lpcbSecurityDescriptor, lpftLastWriteTime
      );
      RegCloseKey(hVirtualSubKey);

      if (status == ERROR_SUCCESS) {
        // 在虚拟表中成功取得信息，就直接返回
        return status;
      }
    }

    // 若打开虚拟表失败或者没找到值，则再尝试原始 hKey
    return RegQueryInfoKeyW_raw(
        hKey, lpClass, lpcClass, lpReserved, lpcSubKeys, lpcMaxSubKeyLen, lpcMaxClassLen, lpcValues,
        lpcMaxValueNameLen, lpcMaxValueLen, lpcbSecurityDescriptor, lpftLastWriteTime
    );
  }

  //--------------------------------------------------------------------------------
  // RegQueryValueExW
  //--------------------------------------------------------------------------------
  LSTATUS RegQueryValueExW(
      HKEY hKey,
      LPCWSTR lpValueName,
      LPDWORD lpReserved,
      LPDWORD lpType,
      LPBYTE lpData,
      LPDWORD lpcbData
  ) override {
    std::wstring path_old = abstractNonRootFromAbsRegPath(GetKeyPath(hKey));

    // 先打开虚拟子键
    HKEY hVirtualSubKey = nullptr;
    LSTATUS status = RegOpenKeyExW_raw(getAppHiveRootKey(), path_old.c_str(), 0, KEY_READ, &hVirtualSubKey);

    if (status == ERROR_SUCCESS) {
      // 在虚拟子键上查询值 lpValueName（注意不再拼接 "\"）
      status = RegQueryValueExW_raw(hVirtualSubKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
      RegCloseKey(hVirtualSubKey);

      if (status == ERROR_SUCCESS) {
        // 找到了虚拟值就返回
        return status;
      }
    }

    // 如果在虚拟表里没找到，则回退到原始 hKey
    return RegQueryValueExW_raw(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
  }

  //--------------------------------------------------------------------------------
  // RegEnumKeyExW
  //--------------------------------------------------------------------------------
  LSTATUS RegEnumKeyExW(
      HKEY hKey,
      DWORD dwIndex,
      LPWSTR lpName,
      LPDWORD lpcName,
      LPDWORD lpReserved,
      LPWSTR lpClass,
      LPDWORD lpcClass,
      PFILETIME lpftLastWriteTime
  ) override {
    // 这里保留你原本的 patchEndIndex 逻辑，区别在于：先打开 AppRegHive
    // 子键再调用 RegEnumKeyExW_raw
    thread_local DWORD patchEndIndex = (DWORD)-1;

    std::wstring path_old = abstractNonRootFromAbsRegPath(GetKeyPath(hKey));

    if (patchEndIndex == (DWORD)-1) {
      // try to query from the AppRegHive first by opening the subkey
      HKEY hVirtualSubKey = nullptr;
      LSTATUS statusOpen =
          RegOpenKeyExW_raw(getAppHiveRootKey(), path_old.c_str(), 0, KEY_READ, &hVirtualSubKey);

      if (statusOpen == ERROR_SUCCESS) {
        // 在虚拟子键上枚举 dwIndex
        LSTATUS statusEnum = RegEnumKeyExW_raw(
            hVirtualSubKey, dwIndex, lpName, lpcName, lpReserved, lpClass, lpcClass, lpftLastWriteTime
        );
        RegCloseKey(hVirtualSubKey);

        if (statusEnum == ERROR_SUCCESS) {
          // 枚举成功，直接返回
          return statusEnum;
        } else if (statusEnum == ERROR_NO_MORE_ITEMS) {
          // 虚拟表已经到底，记录一下 patchEndIndex
          patchEndIndex = dwIndex;
        }
      }
      // 如果打开失败或者枚举失败/NO_MORE_ITEMS，我们就继续往下走到“真实
      // Key”部分
    }

    // 如果 patchEndIndex != -1，就说明虚拟部分已到结尾，需要对索引做偏移
    DWORD realIndex = dwIndex;
    if (patchEndIndex != (DWORD)-1) {
      realIndex = dwIndex - patchEndIndex;
    }

    LSTATUS statusReal =
        RegEnumKeyExW_raw(hKey, realIndex, lpName, lpcName, lpReserved, lpClass, lpcClass, lpftLastWriteTime);

    if (statusReal == ERROR_NO_MORE_ITEMS) {
      // 真实表也到底了，重置 patchEndIndex
      patchEndIndex = (DWORD)-1;
    }
    return statusReal;
  }

  //--------------------------------------------------------------------------------
  // RegEnumValueW
  //--------------------------------------------------------------------------------
  LSTATUS RegEnumValueW(
      HKEY hKey,
      DWORD dwIndex,
      LPWSTR lpValueName,
      LPDWORD lpcchValueName,
      LPDWORD lpReserved,
      LPDWORD lpType,
      LPBYTE lpData,
      LPDWORD lpcbData
  ) override {
    thread_local DWORD patchEndIndex = (DWORD)-1;

    std::wstring path_old = abstractNonRootFromAbsRegPath(GetKeyPath(hKey));

    if (patchEndIndex == (DWORD)-1) {
      // 先尝试枚举虚拟表
      HKEY hVirtualSubKey = nullptr;
      LSTATUS statusOpen =
          RegOpenKeyExW_raw(getAppHiveRootKey(), path_old.c_str(), 0, KEY_READ, &hVirtualSubKey);

      if (statusOpen == ERROR_SUCCESS) {
        LSTATUS statusEnum = RegEnumValueW_raw(
            hVirtualSubKey, dwIndex, lpValueName, lpcchValueName, lpReserved, lpType, lpData, lpcbData
        );
        RegCloseKey(hVirtualSubKey);

        if (statusEnum == ERROR_SUCCESS) {
          return statusEnum;
        } else if (statusEnum == ERROR_NO_MORE_ITEMS) {
          patchEndIndex = dwIndex;
        }
      }
      // 如果失败或无更多项目，则继续尝试真实表
    }

    DWORD realIndex = dwIndex;
    if (patchEndIndex != (DWORD)-1) {
      realIndex = dwIndex - patchEndIndex;
    }

    LSTATUS statusReal =
        RegEnumValueW_raw(hKey, realIndex, lpValueName, lpcchValueName, lpReserved, lpType, lpData, lpcbData);

    if (statusReal == ERROR_NO_MORE_ITEMS) {
      patchEndIndex = (DWORD)-1;
    }
    return statusReal;
  }
  //--------------------------------------------------------------------------------
  // RegSetValueExW
  //--------------------------------------------------------------------------------
  LSTATUS
  RegSetValueExW(
      HKEY hKey,
      LPCWSTR lpValueName,
      DWORD Reserved,
      DWORD dwType,
      const BYTE* lpData,
      DWORD cbData
  ) override {
    // 获取非根注册表路径
    std::wstring path_old = abstractNonRootFromAbsRegPath(GetKeyPath(hKey));

    HKEY hVKey = nullptr;
    DWORD dwDisposition = 0;

    // 尝试使用内部API打开或创建注册表项
    LSTATUS status = RegCreateKeyExInternalW_raw(
        getAppHiveRootKey(),      // 根键
        path_old.c_str(),         // 子键路径
        0,                        // 保留参数，通常为0
        nullptr,                  // lpClass, 这里假设为空
        REG_OPTION_NON_VOLATILE,  // 选项，非易失性键
        KEY_WRITE,                // 所需的访问权限
        nullptr,                  // 安全属性
        &hVKey,                   // 返回的键句柄
        &dwDisposition,           // 返回的创建结果标识
        nullptr                   // 保留参数，通常为空
    );

    // 如果打开/创建键失败，则返回错误
    if (status != ERROR_SUCCESS) {
      throw std::runtime_error("Failed to open/create virtual key");
      return status;
    }

    // 使用打开的键设置值
    status = RegSetValueExW_raw(hVKey, lpValueName, Reserved, dwType, lpData, cbData);

    // 关闭我们打开的键句柄
    RegCloseKey(hVKey);

    return status;
  }
};
