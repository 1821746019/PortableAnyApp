module;
// #include <spdlog/spdlog.h>
#include <Windows.h>
#pragma comment(lib, "winmm.lib")
export module _;
import std;
using namespace std;
decltype(&CreateFileA) CreateFileA_raw = &CreateFileA;
decltype(&CreateFileA) CreateFileA_raw2 = CreateFileA;
extern "C"  int main() {

  cout << std::filesystem::current_path() << endl;

  //std::cout << filesystem:;
  //{
  //   filesystem::path dll_path =
  //       R"(D:\Users\Administrator\Desktop\myCode\_CMake\PortableAnyApp\build\x64-debug\src\ext\allocConsole.dll)";

  //   // putenv(format("LOADED_DLL={}", dll_path.string()).data());
  //   LoadLibraryW(dll_path.c_str());
  //   getchar();

  //   return 0;
  //}
  //      timeBeginPeriod(0);
  ////filesystem::path dll_path =
  ////
  ///R"(D:\Users\Administrator\Desktop\myCode\_CMake\PortableAnyApp\build\x64-debug\src\uniLauncher\extBoot.dll)";

  //// putenv(format("LOADED_DLL={}", dll_path.string()).data());
  // LoadLibraryW(dll_path.c_str());
  // system("cmd");
  // string line;
  // while (getline(cin, line)) {
  //   system(line.data());
  // }

  //// getchar();
  // getchar();
}