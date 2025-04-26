module;
#include <Windows.h>
#include <toml++/toml.hpp>

export module funcsToHook;
import std;
import fs_related;
import my_converter.str;
export {
  class ConfigMgr {
    static std::string initFinalConfigContent() {
      std::string configContent = loadConfigFromDisk(
          std::filesystem::path(__FILE__).parent_path().filename().replace_extension(".toml")
      );
      return configContent;
    }
    void parseConfig() {
      toml::table config = toml::parse(initFinalConfigContent());
      for (auto& e : *config["list"].as_array()) {
        list.emplace_back(e.value<std::string>().value());
        list_w.emplace_back(e.value<std::wstring>().value());
      }
    }
    ConfigMgr() {
      initFinalConfigContent();
      parseConfig();
    }
    inline static std::unique_ptr<ConfigMgr> ins_ = nullptr;

   public:
    std::vector<std::string> list;
    std::vector<std::wstring> list_w;

    ConfigMgr(const ConfigMgr&) = delete;
    ConfigMgr& operator=(const ConfigMgr&) = delete;
    ConfigMgr(ConfigMgr&&) = delete;
    ConfigMgr& operator=(ConfigMgr&&) = delete;
    static ConfigMgr& _ins_() {
      if (!ins_) {
        ins_ = std::unique_ptr<ConfigMgr>(new ConfigMgr);
      }
      return *ins_;
    }
  };
#define configMgr ConfigMgr::_ins_()
  void __fastcall v8_String_NewFromUtf8(
      void* result, void* v8_isolate, const char* data, int32_t type, int a5
  );

  decltype(&v8_String_NewFromUtf8) v8_String_NewFromUtf8_raw;
  void __fastcall v8_String_NewFromUtf8_mod(
      void* result, void* v8_isolate, const char* data, int32_t type, int a5
  ) {
    if (std::ranges::any_of(configMgr.list, [&](const std::string& e) { return strstr(data, e.c_str()); })) {
      DebugBreak();
      MessageBoxA(nullptr, ("似乎发现了目标:\n" + std::string(data)).data(), "Warning", MB_ICONWARNING);
      // 将整个字符串追加写入文件
      auto filePath = R"(D:\_tmp\hookString.log)";
      appendToFile(filePath, data);
    }
    v8_String_NewFromUtf8_raw(result, v8_isolate, data, type, a5);
  }
  void* __fastcall v8_String_NewFromUtf8Literal(
      void* result, void* v8_isolate, const char* literal, int32_t type, int a5
  );

  decltype(&v8_String_NewFromUtf8Literal) v8_String_NewFromUtf8Literal_raw;
  void* __fastcall v8_String_NewFromUtf8Literal_mod(
      void* result, void* v8_isolate, const char* literal, int32_t type, int a5
  ) {
    if (std::ranges::any_of(configMgr.list, [&](const std::string& e) {
          return strstr(literal, e.c_str());
        })) {
      DebugBreak();
      MessageBoxA(nullptr, ("似乎发现了目标:\n" + std::string(literal)).data(), "Warning", MB_ICONWARNING);
      // 将整个字符串追加写入文件
      auto filePath = R"(D:\_tmp\hookString.log)";
      appendToFile(filePath, literal);
    }
    return v8_String_NewFromUtf8Literal_raw(result, v8_isolate, literal, type, a5);
  }
  size_t __cdecl strlen(const char* Str);
  decltype(&strlen) strlen_raw;
  size_t __cdecl strlen_mod(const char* Str) {
    if (std::ranges::any_of(configMgr.list, [&](const std::string& e) { return strstr(Str, e.c_str()); })) {
      DebugBreak();
      MessageBoxA(nullptr, ("似乎发现了目标:\n" + std::string(Str)).data(), "Warning", MB_ICONWARNING);
      // 将整个字符串追加写入文件
      auto filePath = R"(D:\_tmp\hookString.log)";
      appendToFile(filePath, Str);
    }
    return strlen_raw(Str);
  }
  __int64 __fastcall wcslen_v8(const wchar_t* string, double _XMM1_8, double _XMM2_8);
  decltype(&wcslen_v8) wcslen_raw;
  __int64 __fastcall wcslen_mod(const wchar_t* string, double _XMM1_8, double _XMM2_8) {
    if (std::ranges::any_of(configMgr.list_w, [&](const std::wstring& e) {
          return wcsstr(string, e.data());
        })) {
      DebugBreak();
      MessageBoxW(nullptr, (L"似乎发现了目标:\n" + std::wstring(string)).data(), L"Warning", MB_ICONWARNING);
      // 将整个字符串追加写入文件
      auto filePath = R"(D:\_tmp\hookString.log)";
      appendToFile(filePath, brv::strConvert(string));
    }
    return wcslen_raw(string,_XMM1_8,_XMM2_8);
  }

  bool initRawFunc() {
    uint8_t* hModule = (uint8_t*)GetModuleHandleA("chrome.dll");
    if (!hModule)
      return false;

    v8_String_NewFromUtf8_raw = (decltype(&v8_String_NewFromUtf8))(hModule + 0x4D38E30);
    v8_String_NewFromUtf8Literal_raw = (decltype(&v8_String_NewFromUtf8Literal))(hModule + 0x10D8930);
    strlen_raw = (decltype(&strlen))(hModule + 0x5627060);
    wcslen_raw = (decltype(&wcslen_v8))(hModule + 0x55EBFD8);
    // if (strlen_raw("666") != 3) {
    //   MessageBoxA(nullptr, std::format("initRawFunc ret: {}", strlen_raw("666")).data(), "", 0);

    //  throw std::runtime_error("strlen_raw error");
    //}
    return true;
  }
}