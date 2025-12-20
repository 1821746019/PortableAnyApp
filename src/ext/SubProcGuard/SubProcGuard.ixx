module;

#include <Windows.h>
#include <toml++/toml.hpp>

export module _;
import std;
import Hooker;
import selfInfo;
import extraNativeFuncs;
import my_converter.str;
using namespace std;
auto defaultConfig =
    LR"(
[[to_replace]]
src = '' # 默认不启用正则，//包裹则启用正则表达式：例子 /^HKLM/
dst = ''
)";

namespace fs = filesystem;

class ConfigMgr {
 private:
  vector<pair<wregex, wstring>> to_replace_;
  toml::table config_;

  // 解析src字段，判断是否为正则表达式
  pair<wregex, bool> parseSrcPattern(const wstring& src) {
    if (src.length() >= 3 && src.front() == L'/' && src.back() == L'/') {
      // 去除首尾的斜杠，创建正则表达式
      wstring pattern = src.substr(1, src.length() - 2);
      try {
        return {wregex(pattern), true};
      } catch (const std::regex_error& e) {
        wcout << L"Invalid regex pattern: " << pattern << L", treating as literal string" << endl;
        // 如果正则表达式无效，将其转义为字面量
        wstring escaped = regex_replace(pattern, wregex(LR"([\.\^\$\*\+\?\(\)\[\]\{\}\\|])"), LR"(\$&)");
        return {wregex(escaped), false};
      }
    } else {
      // 普通字符串，转义特殊字符后创建正则表达式进行精确匹配
      wstring escaped = regex_replace(src, wregex(LR"([\.\^\$\*\+\?\(\)\[\]\{\}\\|])"), LR"(\$&)");
      return {wregex(escaped), false};
    }
  }

  wstring getFinalConfigContent() {
    wstring configContent;
    fs::path config_path = (selfDir() / fs::path(__FILE__).filename().replace_extension(".toml"));

    if (fs::exists(config_path)) {
      // 使用宽字符流读取文件
      wifstream wifs(config_path);
      wifs.imbue(locale(locale(), new codecvt_utf8<wchar_t>));
      configContent = wstring((istreambuf_iterator<wchar_t>(wifs)), istreambuf_iterator<wchar_t>());
    } else {
      cout << "Config file not found, using default config." << endl;
      configContent = defaultConfig;
    }

    // 替换内置变量
    wstring exe_dir = selfExeDir().wstring();
    configContent = regex_replace(configContent, wregex(LR"(\$\{EXE_DIR\})"), exe_dir);
    return configContent;
  }

  void initConfigVar() {
    try {
      auto to_replace_arr = *config_["to_replace"].as_array();
      for (auto& item : to_replace_arr) {
        auto item_tbl = *item.as_table();
        wstring src_str = item_tbl["src"].value_or<wstring>(L"");
        wstring dst_str = item_tbl["dst"].value_or<wstring>(L"");

        auto [pattern, is_regex] = parseSrcPattern(src_str);
        to_replace_.emplace_back(pair{pattern, dst_str});
        wcout << L"Added replacement rule: " << src_str << (is_regex ? L" (regex)" : L" (literal)") << L" -> "
              << dst_str << endl;
      }

    } catch (const exception& e) {
      cout << "Error parsing config: " << e.what() << endl;
    }
  }

 public:
  ConfigMgr(const ConfigMgr&) = delete;
  ConfigMgr& operator=(const ConfigMgr&) = delete;

  ConfigMgr() {
    try {
      wstring configContent = getFinalConfigContent();
      // 转换回多字节字符串给toml解析器
      string configStr = brv::strConvert(configContent, 65001);
      config_ = toml::parse(configStr);
      initConfigVar();
    } catch (const exception& e) {
      cout << "Failed to initialize config: " << e.what() << endl;
    }
  }

  // 获取替换规则的只读访问
  auto& to_replace() const { return to_replace_; }

  // 应用所有替换规则到给定字符串
  wstring applyReplacements(const wstring& input) const {
    wstring result = input;
    for (const auto& [pattern, replacement] : to_replace_) {
      try {
        result = regex_replace(result, pattern, replacement);
      } catch (const std::regex_error& e) {
        wcout << L"Regex replacement error: " << e.what() << endl;
      }
    }
    return result;
  }

 private:
  inline static unique_ptr<ConfigMgr> ins_ = nullptr;
  inline static mutex ins_mutex_;

 public:
  inline static ConfigMgr& _ins_() {
    if (!ins_) {
      lock_guard<mutex> lock(ins_mutex_);
      if (!ins_) {  // 双重检查锁定
        ins_ = unique_ptr<ConfigMgr>(new ConfigMgr);
      }
    }
    return *ins_;
  }
};

#define config_mgr ConfigMgr::_ins_()

__int64 __fastcall CreateProcessInternalW_mod(
    void* hToken,
    const WCHAR* lpApplicationName,
    const WCHAR* lpCommandLine,
    WCHAR* lpProcessAttributes,
    WCHAR* lpThreadAttributes,
    int bInheritHandles,
    int dwCreationFlags,
    __int64 lpEnvironment,
    const WCHAR* lpCurrentDirectory,
    STARTUPINFOW* lpStartupInfo,
    PROCESS_INFORMATION* lpProcessInformation
) {
  wstring cmdline = lpCommandLine ? lpCommandLine : L"";
  if (cmdline.contains(L"HKEY_LOCAL_MACHINE")) {
    cout << "found it" << '\n';
  }
  // 应用所有替换规则
  wstring modified_cmdline = config_mgr.applyReplacements(cmdline);

  return CreateProcessInternalW_raw(
      hToken, lpApplicationName,
      modified_cmdline.c_str(),  // 使用c_str()而不是data()更安全
      lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment,
      lpCurrentDirectory, lpStartupInfo, lpProcessInformation
  );
}

void setHook() {
  DetoursHooker hooker;
  hooker.endeque({
      {&CreateProcessInternalW_raw, &CreateProcessInternalW_mod},
  });
  hooker.setHook();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReson, LPVOID lpReserved) {
  if (dwReson != DLL_PROCESS_ATTACH)
    return TRUE;

  DisableThreadLibraryCalls(hModule);

  // init the ConfigMgr
  try {
    setHook();
  } catch (const exception& e) {
    MessageBoxA(nullptr, e.what(), "Exception occured", MB_ICONERROR);
    exit(-1);
  }
  return TRUE;
}