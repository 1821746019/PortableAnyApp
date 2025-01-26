module;
#include <Windows.h>
export module func2hook.kernel.raw;
// query from the AppRegHive first. If not found, query from the original
// by examination, the type of last parameter of internal func is LPVOID
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
                        LPVOID lpReserved);
LSTATUS WINAPI
RegCreateKeyExInternalA(HKEY hKey,
                        LPCSTR lpSubKey,
                        DWORD Reserved,
                        LPSTR lpClass,
                        DWORD dwOptions,
                        REGSAM samDesired,
                        const LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                        PHKEY phkResult,
                        LPDWORD lpdwDisposition,
                        LPVOID lpReserved);
export {
  auto RegCreateKeyExInternalW_raw =
      (decltype(&RegCreateKeyExInternalW))GetProcAddress(
          GetModuleHandleA("kernelbase.dll"), "RegCreateKeyExInternalW");

  auto RegCreateKeyExInternalA_raw =
      (decltype(&RegCreateKeyExInternalA))GetProcAddress(
          GetModuleHandleA("kernelbase.dll"), "RegCreateKeyExInternalA");
  auto RegOpenKeyExW_raw = (decltype(&RegOpenKeyExW))GetProcAddress(
      GetModuleHandleA("kernelbase.dll"), "RegOpenKeyExW");
  auto RegOpenKeyExA_raw = (decltype(&RegOpenKeyExA))GetProcAddress(
      GetModuleHandleA("kernelbase.dll"), "RegOpenKeyExA");
  auto RegQueryInfoKeyW_raw = (decltype(&RegQueryInfoKeyW))GetProcAddress(
      GetModuleHandleA("kernelbase.dll"), "RegQueryInfoKeyW");
  auto RegQueryInfoKeyA_raw = (decltype(&RegQueryInfoKeyA))GetProcAddress(
      GetModuleHandleA("kernelbase.dll"), "RegQueryInfoKeyA");
  auto RegQueryValueExW_raw = (decltype(&RegQueryValueExW))GetProcAddress(
      GetModuleHandleA("kernelbase.dll"), "RegQueryValueExW");
  auto RegQueryValueExA_raw = (decltype(&RegQueryValueExA))GetProcAddress(
      GetModuleHandleA("kernelbase.dll"), "RegQueryValueExA");
  auto RegEnumKeyExW_raw = (decltype(&RegEnumKeyExW))GetProcAddress(
      GetModuleHandleA("kernelbase.dll"), "RegEnumKeyExW");
  auto RegEnumKeyExA_raw = (decltype(&RegEnumKeyExA))GetProcAddress(
      GetModuleHandleA("kernelbase.dll"), "RegEnumKeyExA");
  auto RegEnumValueW_raw = (decltype(&RegEnumValueW))GetProcAddress(
      GetModuleHandleA("kernelbase.dll"), "RegEnumValueW");
  auto RegEnumValueA_raw = (decltype(&RegEnumValueA))GetProcAddress(
      GetModuleHandleA("kernelbase.dll"), "RegEnumValueA");
  auto RegSetValueExW_raw = (decltype(&RegSetValueExW))GetProcAddress(
      GetModuleHandleA("kernelbase.dll"), "RegSetValueExW");
  auto RegSetValueExA_raw = (decltype(&RegSetValueExA))GetProcAddress(
      GetModuleHandleA("kernelbase.dll"), "RegSetValueExA");
}