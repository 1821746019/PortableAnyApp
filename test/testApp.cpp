// #include <spdlog/spdlog.h>
#include <Windows.h>
#include <ntdll.h>
// #include <cstdio>
import std;
import reg_common;
#pragma comment(lib, "winmm.lib")
using namespace std;

string loadText(const string& filePath) {
  ifstream ifs(filePath);
  return string(istreambuf_iterator<char>(ifs), istreambuf_iterator<char>());
}
HKEY getCachedAppHiveRootKey() {
  // return AppHiveMgr::_ins_().hKey();

  static HKEY ret = [] {
    HKEY ret;
    auto status =
        RegLoadAppKeyW(LR"(D:\_tmp\RegisterFinder\MachineGUID_AppRegHive)", &ret, KEY_ALL_ACCESS, REG_PROCESS_APPKEY, 0);
    println("{}", (void*)ret);

    if (status != ERROR_SUCCESS) {
      throw std::runtime_error(
          "failed to load AppRegHive. Please check if it exists in the same "
          "dir of the dll"
      );
    }
    getchar();
    return ret;
  }();
  return ret;
};
wstring getAppRegHiveRegPath() {
  static wstring ret = GetKeyPath(getCachedAppHiveRootKey());
  return ret;
};
int main() {
  HKEY curr = (HKEY)0xf0;
  HKEY result = nullptr;
  RegOpenKeyW(curr, L"HKEY_LOCAL_MACHINE", &result);

  getchar();
  HANDLE hKey = nullptr;
  OBJECT_ATTRIBUTES objAttr;
  UNICODE_STRING keyName;
  wstring appRegHiveRegPath = getAppRegHiveRegPath();
  RtlInitUnicodeString(
      &keyName, appRegHiveRegPath.data()
  );
  InitializeObjectAttributes(&objAttr, &keyName, OBJ_CASE_INSENSITIVE, NULL, NULL);
  //NTSTATUS status = NtCreateKey(&hKey, KEY_READ, &objAttr);


  wstring keyPath = GetKeyPath((HKEY)hKey);
  wcout << keyPath << endl;
  // NtOpenKey()
  //{
  //   filesystem::path dll_path =
  //       R"(D:\Users\Administrator\Desktop\myCode\_CMake\PortableAnyApp\build\x64-debug\src\ext\allocConsole.dll)";

  //   // putenv(format("LOADED_DLL={}", dll_path.string()).data());
  //   LoadLibraryW(dll_path.c_str());
  //   getchar();

  //   return 0;
  //}
  timeBeginPeriod(0);
  filesystem::path dll_path =
      R"(D:\Users\Administrator\Desktop\myCode\_CMake\PortableAnyApp\build\x64-debug\src\uniLauncher\extBoot.dll)";

  // putenv(format("LOADED_DLL={}", dll_path.string()).data());
  LoadLibraryW(dll_path.c_str());
  system("cmd");
  string line;
  while (getline(cin, line)) {
    system(line.data());
  }

  // getchar();
  getchar();
}