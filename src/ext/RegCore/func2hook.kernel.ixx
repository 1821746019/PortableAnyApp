module;
#include <Windows.h>
export module func2hook.kernel;
import std;
import my_converter.str;
import ConfigMgr;
import RegHandler_I;
import RegHandlerMgr;
import func2hook.kernel.raw;
#define cfgMgr ConfigMgr::_ins_()
#define regHandle RegHandlerMgr::_ins_().RegHandler()
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
  auto WINAPI RegCreateKeyExInternalW_mod(
      HKEY hKey, LPCWSTR lpSubKey, DWORD Reserved, LPWSTR lpClass,
      DWORD dwOptions, REGSAM samDesired,
      const LPSECURITY_ATTRIBUTES lpSecurityAttributes, PHKEY phkResult,
      LPDWORD lpdwDisposition, LPVOID lpReserved) {
    return regHandle.RegCreateKeyExInternalW(
        hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired,
        lpSecurityAttributes, phkResult, lpdwDisposition, lpReserved);
  }

  auto WINAPI RegOpenKeyExW_mod(HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions,
                                REGSAM samDesired, PHKEY phkResult) {
    return regHandle.RegOpenKeyExW(hKey, lpSubKey, ulOptions, samDesired,
                                   phkResult);
  }

  auto WINAPI RegQueryInfoKeyW_mod(
      HKEY hKey, LPWSTR lpClass, LPDWORD lpcClass, LPDWORD lpReserved,
      LPDWORD lpcSubKeys, LPDWORD lpcMaxSubKeyLen, LPDWORD lpcMaxClassLen,
      LPDWORD lpcValues, LPDWORD lpcMaxValueNameLen, LPDWORD lpcMaxValueLen,
      LPDWORD lpcbSecurityDescriptor, PFILETIME lpftLastWriteTime) {
    return regHandle.RegQueryInfoKeyW(
        hKey, lpClass, lpcClass, lpReserved, lpcSubKeys, lpcMaxSubKeyLen,
        lpcMaxClassLen, lpcValues, lpcMaxValueNameLen, lpcMaxValueLen,
        lpcbSecurityDescriptor, lpftLastWriteTime);
  }

  auto WINAPI RegQueryValueExW_mod(HKEY hKey, LPCWSTR lpValueName,
                                   LPDWORD lpReserved, LPDWORD lpType,
                                   LPBYTE lpData, LPDWORD lpcbData) {
    return regHandle.RegQueryValueExW(hKey, lpValueName, lpReserved, lpType,
                                      lpData, lpcbData);
  }

  auto WINAPI RegEnumKeyExW_mod(HKEY hKey, DWORD dwIndex, LPWSTR lpName,
                                LPDWORD lpcName, LPDWORD lpReserved,
                                LPWSTR lpClass, LPDWORD lpcClass,
                                PFILETIME lpftLastWriteTime) {
    return regHandle.RegEnumKeyExW(hKey, dwIndex, lpName, lpcName, lpReserved,
                                   lpClass, lpcClass, lpftLastWriteTime);
  }

  // for the ANSI func, convert the input to unicode and call the unicode func

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
  auto WINAPI RegOpenKeyExA_mod(HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions,
                                REGSAM samDesired, PHKEY phkResult) {
    return RegOpenKeyExW_mod(hKey, strConvert(lpSubKey).get(), ulOptions,
                             samDesired, phkResult);
  }
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
  auto WINAPI RegEnumKeyExA_mod(HKEY hKey, DWORD dwIndex, LPSTR lpName,
                                LPDWORD lpcName, LPDWORD lpReserved,
                                LPSTR lpClass, LPDWORD lpcClass,
                                PFILETIME lpftLastWriteTime) {
    return RegEnumKeyExW(hKey, dwIndex, strConvert(lpName, *lpcName).get(),
                         lpcName, lpReserved,
                         strConvert(lpClass, lpcClass ? *lpcClass : -1).get(),
                         lpcClass, lpftLastWriteTime);
  }
  auto WINAPI RegQueryValueExA_mod(HKEY hKey, LPCSTR lpValueName,
                                   LPDWORD lpReserved, LPDWORD lpType,
                                   LPBYTE lpData, LPDWORD lpcbData) {
    return RegQueryValueExW(hKey, strConvert(lpValueName).get(), lpReserved,
                            lpType, lpData, lpcbData);
  }
}
