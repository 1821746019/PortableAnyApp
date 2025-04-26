// #include <spdlog/spdlog.h>
#include <Windows.h>
// #include <cstdio>
import std;

#pragma comment(lib, "winmm.lib")
using namespace std;

string loadText(const string& filePath) {

  ifstream ifs(filePath);
  return string(istreambuf_iterator<char>(ifs), istreambuf_iterator<char>());
}
int main() {
  string str = "Hello World";
  cout << strstr(str.data(), "Worl") << endl;
  cout << strstr(str.data(), "666") << endl;

  string originalJson = loadText(R"(D:\_tmp\1.txt)");

  getchar();
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