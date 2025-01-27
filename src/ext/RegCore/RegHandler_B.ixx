module;
// Content: RegHandler.I interface definition
#include <Windows.h>
export module RegHandler_B;
import func2hook.kernel.raw;
export {
  class RegHandler_B {
   public:
    virtual ~RegHandler_B() =
        default;  // 消除IDE警告(定义虚析构函数，以确保使用基类指针删除子类对象时，能正确调用子类的析构函数)
    // direct call to raw for all the functions
    virtual LSTATUS RegCreateKeyExInternalW(
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
    ) {
      return RegCreateKeyExInternalW_raw(
          hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired, lpSecurityAttributes, phkResult,
          lpdwDisposition, lpReserved
      );
    }

    virtual LSTATUS
    RegOpenKeyExW(HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult) {
      return RegOpenKeyExW_raw(hKey, lpSubKey, ulOptions, samDesired, phkResult);
    }
    virtual LSTATUS RegQueryInfoKeyW(
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
    ) {
      return RegQueryInfoKeyW_raw(
          hKey, lpClass, lpcClass, lpReserved, lpcSubKeys, lpcMaxSubKeyLen, lpcMaxClassLen, lpcValues,
          lpcMaxValueNameLen, lpcMaxValueLen, lpcbSecurityDescriptor, lpftLastWriteTime
      );
    }
    virtual LSTATUS RegQueryValueExW(
        HKEY hKey,
        LPCWSTR lpValueName,
        LPDWORD lpReserved,
        LPDWORD lpType,
        LPBYTE lpData,
        LPDWORD lpcbData
    ) {
      return RegQueryValueExW_raw(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
    }
    virtual LSTATUS RegEnumKeyExW(
        HKEY hKey,
        DWORD dwIndex,
        LPWSTR lpName,
        LPDWORD lpcName,
        LPDWORD lpReserved,
        LPWSTR lpClass,
        LPDWORD lpcClass,
        PFILETIME lpftLastWriteTime
    ) {
      return RegEnumKeyExW_raw(hKey, dwIndex, lpName, lpcName, lpReserved, lpClass, lpcClass, lpftLastWriteTime);
    }
    virtual LSTATUS RegEnumValueW(
        HKEY hkey,
        DWORD dwIndex,
        LPWSTR lpValueName,
        LPDWORD lpcchValueName,
        LPDWORD lpReserved,
        LPDWORD lpType,
        LPBYTE lpData,
        LPDWORD lpcbData
    ) {
      return RegEnumValueW_raw(hkey, dwIndex, lpValueName, lpcchValueName, lpReserved, lpType, lpData, lpcbData);
    }
    virtual LSTATUS RegSetValueExW(
        HKEY hKey,
        LPCWSTR lpValueName,
        DWORD Reserved,
        DWORD dwType,
        const BYTE* lpData,
        DWORD cbData
    ) {
      return RegSetValueExW_raw(hKey, lpValueName, Reserved, dwType, lpData, cbData);
    }
  };
}
