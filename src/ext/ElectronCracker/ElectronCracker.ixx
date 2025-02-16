module;

#include <Windows.h>

#include <toml++/toml.hpp>

export module _;
import std;
import Hooker;
import selfInfo;
using namespace std;
namespace fs = filesystem;

string loadConfigFromDisk() {
  fs::path config_path = (selfDir() / fs::path(__FILE__).filename().replace_extension(".toml"));
  if (!fs::exists(config_path)) {
    throw fs::filesystem_error(
        format("Please make sure the {} exists", config_path.string()).data(), config_path,
        make_error_code(errc::no_such_file_or_directory)
    );
  }
  ifstream ifs(config_path);
  return string(istreambuf_iterator<char>(ifs), istreambuf_iterator<char>());
}

class ConfigMgr {
  static string initFinalConfigContent() {
    string configContent = loadConfigFromDisk();
    return configContent;
  }
  void parseConfig() {
    toml::table config = toml::parse(initFinalConfigContent());
    for (auto& e : *config["targetFileNames"].as_array()) {
      targetFileNames.emplace(e.value<wstring>().value());
    }
    for (auto& e : *config["suffixesWithoutExtName"].as_array()) {
      suffixesWithoutExtName.emplace(e.value<wstring>().value());
    }
    for (auto& e : *config["checkpoints"].as_array()) {
      checkpoints.emplace(e.value<int>().value());
    }
  }
  ConfigMgr() {
    initFinalConfigContent();
    parseConfig();
  }
  inline static unique_ptr<ConfigMgr> ins_ = nullptr;

 public:
  unordered_set<wstring> targetFileNames, suffixesWithoutExtName;
  unordered_set<int> checkpoints;
  ConfigMgr(const ConfigMgr&) = delete;
  ConfigMgr& operator=(const ConfigMgr&) = delete;
  ConfigMgr(ConfigMgr&&) = delete;
  ConfigMgr& operator=(ConfigMgr&&) = delete;
  static ConfigMgr& _ins_() {
    if (!ins_) {
      ins_ = unique_ptr<ConfigMgr>(new ConfigMgr);
    }
    return *ins_;
  }
};

decltype(&CreateFileW) CreateFileW_raw = &CreateFileW;

HANDLE WINAPI CreateFileW_mod(
    LPCWSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile
) {
  static unordered_map<wstring, int> fileNameToCnt;

  ConfigMgr& configMgr = ConfigMgr::_ins_();
  auto beforeStem = wcsrchr(lpFileName, L'\\');
  if (!beforeStem)
    return CreateFileW_raw(
        lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition,
        dwFlagsAndAttributes, hTemplateFile
    );
  // 先排除掉不需要处理的文件
  wstring fileName = beforeStem + 1;
  if (!configMgr.targetFileNames.contains(fileName))
    return CreateFileW_raw(
        lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition,
        dwFlagsAndAttributes, hTemplateFile
    );
  if (!configMgr.checkpoints.contains(++fileNameToCnt[fileName])) {
    // 命中，查找同级目录下的文件
    fs::path filePath = fs::path(lpFileName);
    for (auto& suffix : configMgr.suffixesWithoutExtName) {
      fs::path originalFile =
          filePath.parent_path() / (filePath.stem().wstring() + suffix + filePath.extension().wstring());
      if (fs::exists(originalFile)) {
        return CreateFileW_raw(
            originalFile.c_str(), dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition,
            dwFlagsAndAttributes, hTemplateFile
        );
      }
    }
  }
  return CreateFileW_raw(
      lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition,
      dwFlagsAndAttributes, hTemplateFile
  );
}

void setHook() {
  DetoursHooker hooker;
  hooker.endeque({
      {&CreateFileW_raw, &CreateFileW_mod},

  });
  hooker.setHook();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReson, LPVOID lpReserved) {
  if (dwReson != DLL_PROCESS_ATTACH)
    return TRUE;

  DisableThreadLibraryCalls(hModule);

  // init the ConfigMgr
  try {
    ConfigMgr::_ins_();
    setHook();
  } catch (const exception& e) {
    MessageBoxA(nullptr, e.what(), "Exception occured", MB_ICONERROR);
    exit(-1);
  }
  return TRUE;
}