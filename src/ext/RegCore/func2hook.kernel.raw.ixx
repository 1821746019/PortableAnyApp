module;
#include <Windows.h>
export module func2hook.kernel.raw;
// query from the AppRegHive first. If not found, query from the original
// by examination, the type of last parameter of internal func is LPVOID
LSTATUS WINAPI RegCreateKeyExInternalW(
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
);
LSTATUS WINAPI RegCreateKeyExInternalA(
    HKEY hKey,
    LPCSTR lpSubKey,
    DWORD Reserved,
    LPSTR lpClass,
    DWORD dwOptions,
    REGSAM samDesired,
    const LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    PHKEY phkResult,
    LPDWORD lpdwDisposition,
    LPVOID lpReserved
);
export {
  auto RegCreateKeyExInternalW_raw = (decltype(&RegCreateKeyExInternalW)
  )GetProcAddress(GetModuleHandleA("kernelbase.dll"), "RegCreateKeyExInternalW");

  auto RegCreateKeyExInternalA_raw = (decltype(&RegCreateKeyExInternalA)
  )GetProcAddress(GetModuleHandleA("kernelbase.dll"), "RegCreateKeyExInternalA");
  auto RegOpenKeyExW_raw = &RegOpenKeyExW;
  auto RegOpenKeyExA_raw = &RegOpenKeyExA;
  auto RegQueryInfoKeyW_raw = &RegQueryInfoKeyW;
  auto RegQueryInfoKeyA_raw = &RegQueryInfoKeyA;
  auto RegQueryValueExW_raw = &RegQueryValueExW;
  auto RegQueryValueExA_raw = &RegQueryValueExA;
  auto RegEnumKeyExW_raw = &RegEnumKeyExW;
  auto RegEnumKeyExA_raw = &RegEnumKeyExA;
  auto RegEnumValueW_raw = &RegEnumValueW;
  auto RegEnumValueA_raw = &RegEnumValueA;
  auto RegSetValueExW_raw = &RegSetValueExW;
  auto RegSetValueExA_raw = &RegSetValueExA;
}