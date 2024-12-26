module;

// #include <spdlog/spdlog.h>

#include <ntdll.h>

// #include <w>
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "ntdll.lib")

export module _;
import std;
// import std.compat;
using namespace std;

extern "C" int main() {

  unique_ptr<wchar_t>(nullptr);

  getchar();
  /*64bit 0xffffffff80000002
   *32bit 0x80000002
   */

#ifdef _WIN64
  LoadLibraryA(
      R"(D:\Users\Administrator\Desktop\myCode\_CMake\PortableAnyApp\install\x64\Ext\RegCore.dll)");
#else
  LoadLibraryA(
      R"(D:\Users\Administrator\Desktop\myCode\_CMake\PortableAnyApp\install\x86\Ext\RegCore.x86.dll)");
#endif
  while (true) {
    HKEY hkey;
    // auto appReg = RegLoadAppKeyW(
    //     LR"(D:\Users\Administrator\Desktop\myCode\_CMake\PortableAnyApp\install\AppRegHive)",
    //     &hkey, KEY_ALL_ACCESS, 0, 0);
    winreg::RegKey key(HKEY_LOCAL_MACHINE,
                       LR"(SOFTWARE\Microsoft\Cryptography)");
    // winreg::RegKey key(HKEY_LOCAL_MACHINE);

    // winreg::RegKey key(hkey);

    for (auto& v : key.EnumSubKeys()) {
      wcout << v << L'\n';
    }
    for (auto& v : key.EnumValues()) {
      wcout << v.first << " : " << v.second << L'\n';
      if (v.second == REG_SZ) {
        wcout << key.GetStringValue(v.first) << L'\n';
      }
    }
    getchar();
  }

  // std::cout << filesystem:;
  //{
  //    filesystem::path dll_path =
  //        R"(D:\Users\Administrator\Desktop\myCode\_CMake\PortableAnyApp\build\x64-debug\src\ext\allocConsole.dll)";

  //   // putenv(format("LOADED_DLL={}", dll_path.string()).data());
  //   LoadLibraryW(dll_path.c_str());
  //   getchar();

  //   return 0;
  //}
  //      timeBeginPeriod(0);
  ////filesystem::path dll_path =
  ////
  /// R"(D:\Users\Administrator\Desktop\myCode\_CMake\PortableAnyApp\build\x64-debug\src\uniLauncher\extBoot.dll)";

  //// putenv(format("LOADED_DLL={}", dll_path.string()).data());
  // LoadLibraryW(dll_path.c_str());
  // system("cmd");
  // string line;
  // while (getline(cin, line)) {
  //   system(line.data());
  // }

  //// getchar();
}