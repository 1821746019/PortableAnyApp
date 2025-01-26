#include <Windows.h>
#include <spdlog/sinks/win_eventlog_sink.h>
#include <spdlog/spdlog.h>
// #include <filesystem>
#include <toml++/toml.h>

#include <filesystem>
import std;

import spdlogger;
using namespace std;
using winEventLogger::logger;
namespace fs = std::filesystem;

static fs::path selfDir;
static fs::path selfPath;
void dbgThrow(const string& what) {
#if BS_DBG
  throw runtime_error(what);
#endif
}
class ConfigMgr {
  string configContent_;
  toml::table config_;
  string currExeName_ = [] {
    string buf(MAX_PATH, 0);
    GetModuleFileNameA(GetModuleHandleA(nullptr), buf.data(), buf.size());
    string ret = strrchr(buf.data(), '\\') + 1;
    return ret;
  }();

  void preprocConfigContent() {
    string varName = "TOP_PROCESS_NAME";
    string toFind = "${" + varName + "}";
    string varValue = getenv(varName.data());
    size_t off = configContent_.find(toFind);
    if (off == -1)
      return;
    configContent_.replace(off, size(toFind), varValue);
  }

  fs::path getConfigFilePath() {
    fs::path configFileName =
        fs::path(__FILE__).filename().replace_extension(".toml");
    fs::path currDir = selfDir;
    fs::path config_path;
    while (true) {
      config_path = (currDir / configFileName);
      if (exists(config_path)) {
        // dbgThrow(format("config_path is {}", config_path.string()));
        break;
      }
      bool isInHome =
          fs::exists(currDir / "App") && fs::is_directory(currDir / "App");
      if (isInHome) {
        // 停止查找
        throw fs::filesystem_error(
            format("{} not found in the ancestor dir of the {}",
                   configFileName.string(), currExeName_),
            config_path,
            std::make_error_code(std::errc::no_such_file_or_directory));
      }
      currDir = currDir.parent_path();  // 找不到就回退一级目录
    }
    return config_path;
  }

  void initFinalConfigContent() {
    fs::path config_path = getConfigFilePath();
    ifstream ifs(config_path);
    if (!ifs) {
      throw fs::filesystem_error(
          format("Please make sure the {} exists", config_path.string()).data(),
          config_path, make_error_code(errc::no_such_file_or_directory));
    }

    configContent_ =
        string(istreambuf_iterator<char>(ifs), istreambuf_iterator<char>());
    preprocConfigContent();
  }

  vector<wregex> dllReList_;

  void initDllReList() {
    auto p = config_["processName2dllList"].as_array();
    auto processName2dllList = *p;
    // 转换函数，将字符串转换为 wregex
    auto transFunc = [&](wstring str) -> wregex {
      // 检查字符串是否以 '/' 开头和结尾
      if (str.starts_with(L"/") && str.ends_with(L"/")) {
        // 提取中间的内容
        str = str.substr(1, str.length() - 2);
      } else {
        // indicate that it is not a regex pattern
        //  替换所有的 '.' 为 '\.'
        size_t pos;
        // 替换所有/
        pos = 0;
        while ((pos = str.find(LR"(\)", pos)) != wstring::npos) {
          str.replace(pos, 1, LR"(\\)");
          pos += 2;  // 跳过新替换的字符
        }
        pos = 0;
        while ((pos = str.find(L".", pos)) != wstring::npos) {
          str.replace(pos, 1, LR"(\.)");
          pos += 2;  // 跳过新替换的字符
        }
      }

      // 创建并返回 wregex 对象
      return wregex(str);
    };
    for (auto& r : processName2dllList) {
      // auto type = old2new.type();
      auto tbl = *r.as_table();
      auto processName = (tbl["processName"].as_string()->get());
      if (processName != currExeName_ && processName != "/.*/")
        continue;

      auto dllList = (tbl["dllList"].as_array());
      for (auto& e : *dllList) {
        wregex re = transFunc(*e.value<wstring>());

        dllReList_.emplace_back(re);
      }
      dllList->for_each([&](auto& e) {});
      // ranges::transform(*dllList, back_inserter(dllListRe_), transFunc);
    }
    if (dllReList_.empty()) {
    }
  }

 public:
  ConfigMgr() {
    initFinalConfigContent();
    config_ = toml::parse(configContent_);
    initDllReList();
  }

  bool isNeedLoadingDll(const fs::path& dllPath) {
    bool ret = false;
    if (ranges::any_of(dllReList_, [&](const wregex& re) {
          wstring_view toMatch = dllPath.c_str();
          return regex_search(toMatch.begin(), toMatch.end(), re);
        })) {
      ret = true;
    }
    return ret;
  }
};

void loadExtDll() {
  vector<fs::path> dllPath_list;

  fs::path currDir = selfDir;
  fs::path ExtDir;
  constexpr auto ExtDirName = "Ext";
  while (true) {
    ExtDir = (currDir / ExtDirName);
    if (exists(ExtDir))
      break;
    bool isInHome =
        fs::exists(currDir / "App") && fs::is_directory(currDir / "App");
    if (isInHome) {
      // 停止查找
      logger().warn(format("{} not found in the ancestor dir of the {}",
                         ExtDirName, currDir.string()));
    }
    currDir = currDir.parent_path();  // 找不到就回退一级目录
  }

  for (auto& e : fs::recursive_directory_iterator(ExtDir)) {
    // _开头的dll或其所在的目录为_开头则不加载
    if (e.is_regular_file() && e.path().extension() == ".dll" &&
        !e.path().filename().string().starts_with("_") &&
        !e.path().parent_path().filename().wstring().starts_with(L"_")) {
      dllPath_list.emplace_back(e);
    }
  }
  _putenv(format("EXT_BOOT_PATH={}", selfPath.string()).data());
  bool is32 = sizeof(void*) == 4;
  if (is32) {
    // 32位系统下，加载 x86 版本的 dll故只加载以.x86.dll 结尾的 dll
    dllPath_list.erase(
        ranges::remove_if(dllPath_list,
                          [](const fs::path& e) -> bool {
                            return !e.wstring().ends_with(L".x86.dll");
                          })
            .begin(),
        dllPath_list.end());
  }

  ConfigMgr configMgr;
  for (auto& e : dllPath_list) {
    if (!configMgr.isNeedLoadingDll(e))
      continue;

    LoadLibraryW(e.c_str());  // x86 compat
  }
}
BOOL DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved) {
  if (dwReason == DLL_PROCESS_ATTACH) {
#ifdef BS_DBG
    std::wcout << selfDir.wstring() << L'\n';
#endif
    try {
      loadExtDll();
    } catch (const exception& e) {
      MessageBoxA(nullptr, e.what(), "Exception occured", MB_ICONERROR);
      exit(-1);
    }
  } else if (dwReason == DLL_PROCESS_DETACH) {
  }
  //  // 返回false表示dll加载失败，将会卸载该dll
  //  return FALSE;
  return TRUE;
}
void initEnvVarForChildren() {
  if (getenv("TOP_PROCESS_NAME"))
    return;
  // the first process loading extBoot.dll recognized as TOP_PROCESS
  _putenv(format("TOP_PROCESS_NAME={}",
                 [] {
                   string buf(MAX_PATH, 0);
                   GetModuleFileNameA(GetModuleHandleA(nullptr), buf.data(),
                                      buf.size());
                   string ret = buf.data() + buf.rfind("\\") + 1;
                   return ret;
                 }())
              .data());
}
static struct Init {
  Init() {
    HMODULE hModule = nullptr;

    // 使用 GetModuleHandleExW 获取当前模块句柄
    if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           reinterpret_cast<LPCWSTR>(&DllMain), &hModule)) {
      wchar_t buf[MAX_PATH];
      GetModuleFileNameW(hModule, buf, size(buf));
      selfPath = buf;
      selfDir = fs::path(buf).parent_path();
      initEnvVarForChildren();
    }
  }
} init;
