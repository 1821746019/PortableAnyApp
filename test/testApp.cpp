extern "C" {
}
#include <cstdio>
#include <Windows.h>
import std;
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_string.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/fusion/include/adapt_struct.hpp>


using namespace std;

//static class MyGlobalInit
//{
//  lpp::LppDefaultAgent lppAgent;
//
//public:
//  MyGlobalInit()
//  {
//    // create a default agent, loading the Live++ agent from the given path, e.g. "ThirdParty/LivePP"
//    lppAgent = lpp::LppCreateDefaultAgent(nullptr, LR"(D:\environment\c++\LivePP)");
//
//    // bail out in case the agent is not valid
//    if (!lpp::LppIsValidDefaultAgent(&lppAgent))
//    {
//      throw runtime_error("Failed to create default agent");
//    }
//
//    // enable Live++ for all loaded modules
//    lppAgent.EnableModule(lpp::LppGetCurrentModulePath(),
//                          lpp::LPP_MODULES_OPTION_ALL_IMPORT_MODULES, nullptr, nullptr);
//  }
//
//  ~MyGlobalInit()
//  {
//    // destroy the Live++ agent
//    lpp::LppDestroyDefaultAgent(&lppAgent);
//  }
//} my_global_init;

void hotReloadTest()
{
	 int cnt = 1;
  for (int i = 0; i < 5; ++i)
  {
    println("{}", ++cnt);

  }
}

//int main()
//{
//  sizeof(void*);
//  (void)setvbuf(stdout,nullptr,_IONBF,0);
//  //LoadLibraryA("./kernel32.dll");
//  while (true)
//  {
//    hotReloadTest();
//    this_thread::sleep_for(chrono::milliseconds(500));
//  }
//  // hotReloadTest();
//}
// 定义数据结构
struct RegValue {
  std::string name;
  std::string value;
};

struct RegEntry {
  std::string key;
  std::vector<RegValue> values;
};

// 适配结构体
BOOST_FUSION_ADAPT_STRUCT(
  RegValue,
  (std::string, name)
  (std::string, value)
)

BOOST_FUSION_ADAPT_STRUCT(
  RegEntry,
  (std::string, key)
  (std::vector<RegValue>, values)
)

int main() {
  using namespace boost::spirit::qi;
  using boost::spirit::ascii::space;

  std::string input = R"(
Windows Registry Editor Version 5.00

[HKEY_CURRENT_USER\Software\Example]
"StringValue"="Example String"
"DWORDValue"=dword:00000001
)";

  RegEntry entry;
  auto it = input.begin();
  auto end = input.end();

  bool success = phrase_parse(
    it,
    end,
    // Grammar
    (
      // 版本声明
      lit("Windows Registry Editor Version") >> "5.00" >> eol
      >>
      // 键路径
      '[' >> +(char_ - ']') >> ']' >> eol
      >>
      // 键值对
      *(
        '"' >> +(char_ - '"') >> '"' >> '='
        >> (
          '"' >> *(char_ - '"') >> '"' |
          "dword:" >> +xdigit
          ) >> eol
        )
      ),
    // 解析时跳过空白
    space,
    entry
  );

  if (success && it == end) {
    std::cout << "解析成功！\n";
    std::cout << "键路径: " << entry.key << "\n";
    for (const auto& kv : entry.values) {
      std::cout << "键名: " << kv.name << "，值: " << kv.value << "\n";
    }
  }
  else {
    std::cout << "解析失败！\n";
  }

  return 0;
}