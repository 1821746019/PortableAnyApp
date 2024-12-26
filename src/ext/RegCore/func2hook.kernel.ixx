module;
#include <Windows.h>
export module func2hook.kernel;
import std;
import my_converter.str;
import AppRegHive;

std::unique_ptr<wchar_t[]> strConvert(const char* str, size_t bufSize = -1) {
  if (str == nullptr)
    return nullptr;
  std::wstring wstr = brv::strConvert(std::string_view(str));
  size_t bufSizeFinal = bufSize == -1 ? wstr.size() + 1 : bufSize;
  std::unique_ptr<wchar_t[]> ret(new wchar_t[bufSizeFinal]);
  std::wmemcpy(ret.get(), wstr.c_str(), bufSizeFinal);
  return ret;
}
export {
  // query from the AppRegHive first. If not found, query from the original
  // by examination, the type of last parameter of internal func is LPVOID
  LSTATUS WINAPI RegCreateKeyExInternalW(
      HKEY hKey, LPCWSTR lpSubKey, DWORD Reserved, LPWSTR lpClass,
      DWORD dwOptions, REGSAM samDesired,
      LPSECURITY_ATTRIBUTES lpSecurityAttributes, PHKEY phkResult,
      LPDWORD lpdwDisposition, LPVOID lpReserved);

  auto RegCreateKeyExInternalW_raw =
      (decltype(&RegCreateKeyExInternalW))GetProcAddress(
          LoadLibraryA("kernelbase.dll"), "RegCreateKeyExInternalW");
  auto WINAPI RegCreateKeyExInternalW_mod(
      HKEY hKey, LPCWSTR lpSubKey, DWORD Reserved, LPWSTR lpClass,
      DWORD dwOptions, REGSAM samDesired,
      const LPSECURITY_ATTRIBUTES lpSecurityAttributes, PHKEY phkResult,
      LPDWORD lpdwDisposition, LPVOID lpReserved) {
    // 如果 hKey 是 rootKeyMap 的映射，尝试获取替代 hKey
    if (rootKeyMap.contains(hKey)) {
      HKEY hKey_new = rootKeyMap.at(hKey);
      LSTATUS status = RegCreateKeyExInternalW_raw(
          hKey_new, lpSubKey, Reserved, lpClass, dwOptions, samDesired,
          lpSecurityAttributes,  // 强转去掉 const
          phkResult, lpdwDisposition, lpReserved);

      if (status == ERROR_SUCCESS) {
        // 创建成功，直接返回
        return status;
      }
      // 如果不是 ERROR_SUCCESS 就 fallback
    }

    // 调用原始的 RegCreateKeyExInternalW_raw
    LSTATUS status = RegCreateKeyExInternalW_raw(
        hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired,
        lpSecurityAttributes, phkResult, lpdwDisposition, lpReserved);

    // 返回原始状态（如果没有遇到 NAME NOT FOUND）
    return status;
  }

  LSTATUS WINAPI RegCreateKeyExInternalA(
      HKEY hKey, LPCSTR lpSubKey, DWORD Reserved, LPSTR lpClass,
      DWORD dwOptions, REGSAM samDesired,
      const LPSECURITY_ATTRIBUTES lpSecurityAttributes, PHKEY phkResult,
      LPDWORD lpdwDisposition, LPVOID lpReserved);
  auto RegCreateKeyExInternalA_raw =
      (decltype(&RegCreateKeyExInternalA))GetProcAddress(
          LoadLibraryA("kernelbase.dll"), "RegCreateKeyExInternalA");
  auto WINAPI RegCreateKeyExInternalA_mod(
      HKEY hKey, LPCSTR lpSubKey, DWORD Reserved, LPSTR lpClass,
      DWORD dwOptions, REGSAM samDesired,
      const LPSECURITY_ATTRIBUTES lpSecurityAttributes, PHKEY phkResult,
      LPDWORD lpdwDisposition, LPVOID lpReserved) {
    return RegCreateKeyExInternalW_mod(
        hKey, strConvert(lpSubKey).get(), Reserved, strConvert(lpClass).get(),
        dwOptions, samDesired, lpSecurityAttributes, phkResult, lpdwDisposition,
        lpReserved);
  }
  auto RegOpenKeyExW_raw = &RegOpenKeyExW;

  auto WINAPI RegOpenKeyExW_mod(HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions,
                                REGSAM samDesired, PHKEY phkResult) {
    if (rootKeyMap.contains(hKey)) {
      HKEY hKey_new = rootKeyMap.at(hKey);
      LSTATUS status = RegOpenKeyExW_raw(hKey_new, lpSubKey, ulOptions,
                                         samDesired, phkResult);
      if (status == ERROR_SUCCESS)
        return status;
    }
    return RegOpenKeyExW_raw(hKey, lpSubKey, ulOptions, samDesired, phkResult);
  }
  auto RegOpenKeyExA_raw = &RegOpenKeyExA;

  auto WINAPI RegOpenKeyExA_mod(HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions,
                                REGSAM samDesired, PHKEY phkResult) {
    return RegOpenKeyExW_mod(hKey, strConvert(lpSubKey).get(), ulOptions,
                             samDesired, phkResult);
  }

  auto RegQueryInfoKeyW_raw = &RegQueryInfoKeyW;

  auto WINAPI RegQueryInfoKeyW_mod(
      HKEY hKey, LPWSTR lpClass, LPDWORD lpcClass, LPDWORD lpReserved,
      LPDWORD lpcSubKeys, LPDWORD lpcMaxSubKeyLen, LPDWORD lpcMaxClassLen,
      LPDWORD lpcValues, LPDWORD lpcMaxValueNameLen, LPDWORD lpcMaxValueLen,
      LPDWORD lpcbSecurityDescriptor, PFILETIME lpftLastWriteTime) {
    if (rootKeyMap.contains(hKey)) {
      HKEY hKey_new = rootKeyMap.at(hKey);
      LSTATUS status = RegQueryInfoKeyW_raw(
          hKey_new, lpClass, lpcClass, lpReserved, lpcSubKeys, lpcMaxSubKeyLen,
          lpcMaxClassLen, lpcValues, lpcMaxValueNameLen, lpcMaxValueLen,
          lpcbSecurityDescriptor, lpftLastWriteTime);
      if (status == ERROR_SUCCESS)
        return status;
    }
    return RegQueryInfoKeyW_raw(hKey, lpClass, lpcClass, lpReserved, lpcSubKeys,
                                lpcMaxSubKeyLen, lpcMaxClassLen, lpcValues,
                                lpcMaxValueNameLen, lpcMaxValueLen,
                                lpcbSecurityDescriptor, lpftLastWriteTime);
  }

  auto RegQueryInfoKeyA_raw = &RegQueryInfoKeyA;

  auto WINAPI RegQueryInfoKeyA_mod(
      HKEY hKey, LPSTR lpClass, LPDWORD lpcchClass, LPDWORD lpReserved,
      LPDWORD lpcSubKeys, LPDWORD lpcbMaxSubKeyLen, LPDWORD lpcbMaxClassLen,
      LPDWORD lpcValues, LPDWORD lpcbMaxValueNameLen, LPDWORD lpcbMaxValueLen,
      LPDWORD lpcbSecurityDescriptor, PFILETIME lpftLastWriteTime) {
    return RegQueryInfoKeyW(
        hKey, strConvert(lpClass).get(), lpcchClass, lpReserved, lpcSubKeys,
        lpcbMaxSubKeyLen, lpcbMaxClassLen, lpcValues, lpcbMaxValueNameLen,
        lpcbMaxValueLen, lpcbSecurityDescriptor, lpftLastWriteTime);
  }
  auto RegQueryValueExW_raw = &RegQueryValueExW;
  auto WINAPI RegQueryValueExW_mod(HKEY hKey, LPCWSTR lpValueName,
                                   LPDWORD lpReserved, LPDWORD lpType,
                                   LPBYTE lpData, LPDWORD lpcbData) {
    if (rootKeyMap.contains(hKey)) {
      HKEY hKey_new = rootKeyMap.at(hKey);
      LSTATUS status = RegQueryValueExW_raw(hKey_new, lpValueName, lpReserved,
                                            lpType, lpData, lpcbData);
      if (status == ERROR_SUCCESS)
        return status;
    }
    return RegQueryValueExW_raw(hKey, lpValueName, lpReserved, lpType, lpData,
                                lpcbData);
  }

  auto RegQueryValueExA_raw = &RegQueryValueExA;
  auto WINAPI RegQueryValueExA_mod(HKEY hKey, LPCSTR lpValueName,
                                   LPDWORD lpReserved, LPDWORD lpType,
                                   LPBYTE lpData, LPDWORD lpcbData) {
    return RegQueryValueExW(hKey, strConvert(lpValueName).get(), lpReserved,
                            lpType, lpData, lpcbData);
  }

  auto RegEnumKeyExW_raw = (decltype(&RegEnumKeyExW))GetProcAddress(
      GetModuleHandleA("kernelbase.dll"), "RegEnumKeyExW");
  auto WINAPI RegEnumKeyExW_mod(HKEY hKey, DWORD dwIndex, LPWSTR lpName,
                                LPDWORD lpcName, LPDWORD lpReserved,
                                LPWSTR lpClass, LPDWORD lpcClass,
                                PFILETIME lpftLastWriteTime) {
    if (rootKeyMap.contains(hKey)) {
      HKEY hKey_new = rootKeyMap.at(hKey);
      LSTATUS status =
          RegEnumKeyExW_raw(hKey_new, dwIndex, lpName, lpcName, lpReserved,
                            lpClass, lpcClass, lpftLastWriteTime);
      if (status == ERROR_SUCCESS)
        return status;
    }
    return RegEnumKeyExW_raw(hKey, dwIndex, lpName, lpcName, lpReserved,
                             lpClass, lpcClass, lpftLastWriteTime);
  }

  auto RegEnumKeyExA_raw = (decltype(&RegEnumKeyExA))GetProcAddress(
      GetModuleHandleA("kernelbase.dll"), "RegEnumKeyExA");
  auto WINAPI RegEnumKeyExA_mod(HKEY hKey, DWORD dwIndex, LPSTR lpName,
                                LPDWORD lpcName, LPDWORD lpReserved,
                                LPSTR lpClass, LPDWORD lpcClass,
                                PFILETIME lpftLastWriteTime) {
    return RegEnumKeyExW(hKey, dwIndex, strConvert(lpName, *lpcName).get(),
                         lpcName, lpReserved,
                         strConvert(lpClass, lpcClass ? *lpcClass : -1).get(),
                         lpcClass, lpftLastWriteTime);
  }
}
