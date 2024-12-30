module;
#include <Windows.h>
export module RegHandler_Patch;
import std;
import RegHandler_I;
import func2hook.kernel.raw;
import AppRegHive;
import reg_common;
export class RegHandler_Patch : public RegHandler_I {
 public:
  LSTATUS WINAPI
  RegCreateKeyExInternalW(HKEY hKey,
                          LPCWSTR lpSubKey,
                          DWORD Reserved,
                          LPWSTR lpClass,
                          DWORD dwOptions,
                          REGSAM samDesired,
                          LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                          PHKEY phkResult,
                          LPDWORD lpdwDisposition,
                          LPVOID lpReserved) override {
    // 调用原始的 RegCreateKeyExInternalW_raw
    LSTATUS status = RegCreateKeyExInternalW_raw(
        hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired,
        lpSecurityAttributes, phkResult, lpdwDisposition, lpReserved);

    // 返回原始状态（如果没有遇到 NAME NOT FOUND）
    return status;
  }
  LSTATUS WINAPI RegOpenKeyExW(HKEY hKey,
                               LPCWSTR lpSubKey,
                               DWORD ulOptions,
                               REGSAM samDesired,
                               PHKEY phkResult) override {
    return RegOpenKeyExW_raw(hKey, lpSubKey, ulOptions, samDesired, phkResult);
  }
  LSTATUS WINAPI RegQueryInfoKeyW(HKEY hKey,
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
                                  PFILETIME lpftLastWriteTime) override {
    return RegQueryInfoKeyW_raw(hKey, lpClass, lpcClass, lpReserved, lpcSubKeys,
                                lpcMaxSubKeyLen, lpcMaxClassLen, lpcValues,
                                lpcMaxValueNameLen, lpcMaxValueLen,
                                lpcbSecurityDescriptor, lpftLastWriteTime);
  }
  LSTATUS WINAPI RegQueryValueExW(HKEY hKey,
                                  LPCWSTR lpValueName,
                                  LPDWORD lpReserved,
                                  LPDWORD lpType,
                                  LPBYTE lpData,
                                  LPDWORD lpcbData) override {
    // Query from the AppRegHive first. If not found, query from the original
    std::wstring path_old = GetKeyPath(hKey);
    LSTATUS status = RegQueryValueExW_raw(
        getAppHiveRootKey(), (path_old + L"\\" + lpValueName).data(),
        lpReserved, lpType, lpData, lpcbData);
    if (status != ERROR_SUCCESS) {
      status = RegQueryValueExW_raw(hKey, lpValueName, lpReserved, lpType,
                                    lpData, lpcbData);
    }
    return status;
  }
  LSTATUS WINAPI RegEnumKeyExW(HKEY hKey,
                               DWORD dwIndex,
                               LPWSTR lpName,
                               LPDWORD lpcName,
                               LPDWORD lpReserved,
                               LPWSTR lpClass,
                               LPDWORD lpcClass,
                               PFILETIME lpftLastWriteTime) override {

    return RegEnumKeyExW_raw(hKey, dwIndex, lpName, lpcName, lpReserved,
                             lpClass, lpcClass, lpftLastWriteTime);
  }
};