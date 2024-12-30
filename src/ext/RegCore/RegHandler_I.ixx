module;
// Content: RegHandler.I interface definition
#include <Windows.h>
export module RegHandler_I;

export {
  class RegHandler_I {
   public:
    virtual ~RegHandler_I() = default;

    virtual LSTATUS WINAPI
    RegCreateKeyExInternalW(HKEY hKey,
                            LPCWSTR lpSubKey,
                            DWORD Reserved,
                            LPWSTR lpClass,
                            DWORD dwOptions,
                            REGSAM samDesired,
                            LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                            PHKEY phkResult,
                            LPDWORD lpdwDisposition,
                            LPVOID lpReserved) = 0;

    virtual LSTATUS WINAPI RegOpenKeyExW(HKEY hKey,
                                         LPCWSTR lpSubKey,
                                         DWORD ulOptions,
                                         REGSAM samDesired,
                                         PHKEY phkResult) = 0;
    virtual LSTATUS WINAPI RegQueryInfoKeyW(HKEY hKey,
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
                                            PFILETIME lpftLastWriteTime) = 0;
    virtual LSTATUS WINAPI RegQueryValueExW(HKEY hKey,
                                            LPCWSTR lpValueName,
                                            LPDWORD lpReserved,
                                            LPDWORD lpType,
                                            LPBYTE lpData,
                                            LPDWORD lpcbData) = 0;
    virtual LSTATUS WINAPI RegEnumKeyExW(HKEY hKey,
                                         DWORD dwIndex,
                                         LPWSTR lpName,
                                         LPDWORD lpcName,
                                         LPDWORD lpReserved,
                                         LPWSTR lpClass,
                                         LPDWORD lpcClass,
                                         PFILETIME lpftLastWriteTime) = 0;
  };
}