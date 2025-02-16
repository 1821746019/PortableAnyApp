//#include <spdlog/spdlog.h>
#include <Windows.h>
//#include <cstdio>
import std;

#pragma comment(lib,"winmm.lib")
using namespace std;

std::string unlockFeatures(const std::string& val) {
  std::string result = val;

  // 预定义替换规则表（原始字符串字面量避免转义）
  const std::unordered_map<std::string, std::string> replacements = {
      // 解除功能限制
      {R"("is_expert": true)", R"("is_expert": false)"},
      {R"("disabled": true)", R"("disabled": false)"},

      // 升级账户类型
      {R"("id": "pro")", R"("id": "pro_premium_expert")"},
      {R"("id": "pro_realtime")", R"("id": "pro_premium_expert")"},
      {R"("id": "pro_premium")", R"("id": "pro_premium_expert")"},

      // 修改账户显示名称
      {R"("name": "Essential")", R"("name": "Ultimate")"},
      {R"("name": "Plus")", R"("name": "Ultimate")"},
      {R"("name": "Premium")", R"("name": "Ultimate")"},

      // 清除价格信息（演示部分典型字段）
      {R"("cost": 14.95,)", ""},
      {R"("cost_annual": 12.95,)", ""},
      {R"("cost_biennial": 9.95,)", ""}
  };

  // 顺序执行字符串替换
  for (const auto& [pattern, replacement] : replacements) {
    size_t pos = 0;
    while ((pos = result.find(pattern, pos)) != std::string::npos) {
      result.replace(pos, pattern.length(), replacement);
      pos += replacement.length();
    }
  }

  // 清理可能产生的JSON语法错误
  const std::string cleanup_patterns[] = {
      ",,", ",",  // 处理连续逗号
      ",}", "}",  // 对象尾部逗号
      ",]", "]"   // 数组尾部逗号
  };

  for (size_t i = 0; i < sizeof(cleanup_patterns) / sizeof(cleanup_patterns[0]); i += 2) {
    size_t pos = 0;
    while ((pos = result.find(cleanup_patterns[i], pos)) != std::string::npos) {
      result.replace(pos, cleanup_patterns[i].length(), cleanup_patterns[i + 1]);
    }
  }

  return result;
}

string loadText(const string& filePath) {
  ifstream ifs(filePath);
  return string(istreambuf_iterator<char>(ifs), istreambuf_iterator<char>());
} int main()
{
  string originalJson = loadText(R"(D:\_tmp\1.txt)");
  string modified = unlockFeatures(originalJson);
  cout << modified << endl;

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