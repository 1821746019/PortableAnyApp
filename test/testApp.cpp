//#include <spdlog/spdlog.h>
#include <Windows.h>
//#include <cstdio>
import std;

#pragma comment(lib,"winmm.lib")
using namespace std;


int main()
{
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