module;
#include <Windows.h>
export module RegHandler_Virtualize;
import std;
import RegHandler_B;
import func2hook.kernel.raw;
import AppRegHive;

export class RegHandler_Virtualize : public RegHandler_B {
 public:
  LSTATUS RegCreateKeyExInternalW(
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
    // 如果 hKey 是 rootKeyMap 的映射，尝试获取替代 hKey
    if (rootKeyMap.contains(hKey)) {
      HKEY hKey_new = rootKeyMap.at(hKey);
      LSTATUS status = RegCreateKeyExInternalW_raw(
          hKey_new, lpSubKey, Reserved, lpClass, dwOptions, samDesired, lpSecurityAttributes, phkResult,
          lpdwDisposition, lpReserved
      );

      if (status == ERROR_SUCCESS) {
        // 创建成功，直接返回
        return status;
      }
      // 如果不是 ERROR_SUCCESS 就 fallback
    }

    // 调用原始的 RegCreateKeyExInternalW_raw
    LSTATUS status = RegCreateKeyExInternalW_raw(
        hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired, lpSecurityAttributes, phkResult,
        lpdwDisposition, lpReserved
    );

    // 返回原始状态（如果没有遇到 NAME NOT FOUND）
    return status;
  }
  LSTATUS
  RegOpenKeyExW(HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult) override {
    if (rootKeyMap.contains(hKey)) {
      HKEY hKey_new = rootKeyMap.at(hKey);
      LSTATUS status = RegOpenKeyExW_raw(hKey_new, lpSubKey, ulOptions, samDesired, phkResult);
      if (status == ERROR_SUCCESS)
        return status;
    }
    return RegOpenKeyExW_raw(hKey, lpSubKey, ulOptions, samDesired, phkResult);
  }
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
    if (rootKeyMap.contains(hKey)) {
      HKEY hKey_new = rootKeyMap.at(hKey);
      LSTATUS status = RegQueryInfoKeyW_raw(
          hKey_new, lpClass, lpcClass, lpReserved, lpcSubKeys, lpcMaxSubKeyLen, lpcMaxClassLen, lpcValues,
          lpcMaxValueNameLen, lpcMaxValueLen, lpcbSecurityDescriptor, lpftLastWriteTime
      );
      if (status == ERROR_SUCCESS)
        return status;
    }
    return RegQueryInfoKeyW_raw(
        hKey, lpClass, lpcClass, lpReserved, lpcSubKeys, lpcMaxSubKeyLen, lpcMaxClassLen, lpcValues,
        lpcMaxValueNameLen, lpcMaxValueLen, lpcbSecurityDescriptor, lpftLastWriteTime
    );
  }
  LSTATUS RegQueryValueExW(
      HKEY hKey,
      LPCWSTR lpValueName,
      LPDWORD lpReserved,
      LPDWORD lpType,
      LPBYTE lpData,
      LPDWORD lpcbData
  ) override {
    if (rootKeyMap.contains(hKey)) {
      HKEY hKey_new = rootKeyMap.at(hKey);
      LSTATUS status = RegQueryValueExW_raw(hKey_new, lpValueName, lpReserved, lpType, lpData, lpcbData);
      if (status == ERROR_SUCCESS)
        return status;
    }
    return RegQueryValueExW_raw(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
  }
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
    if (rootKeyMap.contains(hKey)) {
      HKEY hKey_new = rootKeyMap.at(hKey);
      LSTATUS status = RegEnumKeyExW_raw(
          hKey_new, dwIndex, lpName, lpcName, lpReserved, lpClass, lpcClass, lpftLastWriteTime
      );
      if (status == ERROR_SUCCESS)
        return status;
    }
    return RegEnumKeyExW_raw(
        hKey, dwIndex, lpName, lpcName, lpReserved, lpClass, lpcClass, lpftLastWriteTime
    );
  }

  LSTATUS RegEnumValueW(
      HKEY hkey,
      DWORD dwIndex,
      LPWSTR lpValueName,
      LPDWORD lpcchValueName,
      LPDWORD lpReserved,
      LPDWORD lpType,
      LPBYTE lpData,
      LPDWORD lpcbData
  ) override {
    return RegEnumValueW_raw(
        hkey, dwIndex, lpValueName, lpcchValueName, lpReserved, lpType, lpData, lpcbData
    );
  }
  LSTATUS RegSetValueExW(
      HKEY hKey,
      LPCWSTR lpValueName,
      DWORD Reserved,
      DWORD dwType,
      const BYTE* lpData,
      DWORD cbData
  ) override {
    return RegSetValueExW_raw(hKey, lpValueName, Reserved, dwType, lpData, cbData);
  }
};