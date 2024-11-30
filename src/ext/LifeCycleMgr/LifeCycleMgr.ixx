module;
#include <Windows.h>
#include <toml++/toml.hpp>
export module _;

import std;
import selfDir;
using namespace std;
namespace fs = std::filesystem;

bool execFile(const fs::path& filePath,
              bool isAsync,
              bool shouldShowWindow = false) {
  // Prepare the startup info and process information structures
  STARTUPINFO si = {0};
  PROCESS_INFORMATION pi = {0};

  // Initialize the STARTUPINFO structure
  si.cb = sizeof(STARTUPINFO);

  // if (filePath.extension() == ".bat")
  if (shouldShowWindow) {
    si.dwFlags = STARTF_USESHOWWINDOW;  // You can adjust this flag to control
    // window visibility
    si.wShowWindow =
        SW_SHOWNORMAL;  // Show the window normally (can be  adjusted)

  } else {
    // hide the console window for .bat files
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
  }
  // Create a process to execute the file
  BOOL success = CreateProcessA(
      nullptr,  // No module name (use command line)
      const_cast<char*>(filePath.string().c_str()),  // Command line
      nullptr,             // Process handle not inheritable
      nullptr,             // Thread handle not inheritable
      FALSE,               // Set handle inheritance to FALSE
      CREATE_NEW_CONSOLE,  // Create a new console for the process
      nullptr,             // Use parent's environment block
      nullptr,             // Use parent's starting directory
      &si,                 // Pointer to STARTUPINFO structure
      &pi                  // Pointer to PROCESS_INFORMATION structure
  );

  if (!success) {
    std::cerr << "Failed to create process! Error code: " << GetLastError()
              << std::endl;
    return false;
  }

  // If running asynchronously, do not wait for the process to finish
  if (!isAsync) {
    // Wait for the process to finish
    WaitForSingleObject(pi.hProcess, INFINITE);
  }

  // Close the process and thread handles
  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);

  return true;
}

class ConfigMgr {
  string configContent_;
  toml::table config_;
  set<wstring> domainBlockList_;
  void initFinalConfigContent() {
    fs::path config_path =
        (selfDir / fs::path(__FILE__).filename().replace_extension(".toml"));
    if (!fs::exists(config_path)) {
      throw fs::filesystem_error(
          format("Please make sure the {} exists", config_path.string()).data(),
          config_path, make_error_code(errc::no_such_file_or_directory));
    }
    ifstream ifs(config_path);
    configContent_ =
        string(istreambuf_iterator<char>(ifs), istreambuf_iterator<char>());
  }
  void updateDomainBlockList() {
    domainBlockList_.clear();
    for (auto& e : *config_["domainBlockList"].as_array()) {
      domainBlockList_.emplace(e.value<wstring>().value());
    }
  }
  ConfigMgr() {
    initFinalConfigContent();
    config_ = toml::parse(configContent_);
  }
  inline static unique_ptr<ConfigMgr> ins_ = nullptr;

 public:
  ConfigMgr(const ConfigMgr&) = delete;
  ConfigMgr& operator=(const ConfigMgr&) = delete;
  static ConfigMgr& _ins_() {
    if (!ins_) {
      ins_ = unique_ptr<ConfigMgr>(new ConfigMgr);
    }
    return *ins_;
  }
  const toml::table& config() { return config_; }
};

class AtExit {
  // cannot use deque for functional objects because of mem allocator diff
  // between deque and functional, fallback to ptr deque
  inline static deque<unique_ptr<function<void()>>> funcList_;
  // static void atExitHandler() {
  //   for (unique_ptr<function<void()>>& e : funcList_)
  //     (*e)();
  // }

  AtExit() {
    std::atexit([] {
      for (unique_ptr<function<void()>>& e : funcList_)
        (*e)();
    });
  }

  inline static unique_ptr<AtExit> ins_ = nullptr;

 public:
  static AtExit& _ins_() {
    if (!ins_) {
      ins_ = unique_ptr<AtExit>(new AtExit);
    }
    return *ins_;
  }
  AtExit& appendFunc(unique_ptr<function<void()>> func) {
    funcList_.emplace_back(move(func));
    return *this;
  }
  AtExit(const AtExit&) = delete;
  AtExit& operator=(const AtExit&) = delete;
};
void execWithConfig(const toml::table& config) {
  fs::path baseDir = selfDir;
  // proc for beforeRunning
  auto beforeRunningObjList = *config["beforeRunning"].as_array();
  for (auto& obj : beforeRunningObjList) {
    toml::table objTable = *obj.as_table();
    vector<fs::path> finalFileList;
    toml::array fileList = *objTable["fileList"].as_array();
    for (auto& filePath : fileList) {
      finalFileList.emplace_back(baseDir / filePath.value<wstring>().value());
    }
    toml::array dirList = *objTable["dirList"].as_array();
    // scan the dir and append the fileList
    for (auto& dirPath : dirList) {
      fs::path dir = baseDir / dirPath.value<wstring>().value();
      for (auto& e : fs::directory_iterator(dir)) {
        finalFileList.emplace_back(e.path());
      }
    }
    bool isAsync = objTable["isAsync"].value_or(true);
    bool shouldShowWindow = objTable["shouldShowWindow"].value_or(false);
    for (auto& filePath : finalFileList) {
      execFile(filePath, isAsync, shouldShowWindow);
    }
    // execFile(filePath.value<wstring>().value(), isAsync);
  }
  // proc for afterRunning
  auto afterRunningObjList = *config["afterRunning"].as_array();
  for (auto& obj : afterRunningObjList) {
    toml::table objTable = *obj.as_table();
    vector<fs::path> finalFileList;
    toml::array fileList = *objTable["fileList"].as_array();
    for (auto& filePath : fileList) {
      finalFileList.emplace_back(baseDir / filePath.value<wstring>().value());
    }
    toml::array dirList = *objTable["dirList"].as_array();
    // scan the dir and append the fileList
    for (auto& dirPath : dirList) {
      fs::path dir = baseDir / dirPath.value<wstring>().value();
      for (auto& e : fs::directory_iterator(dir)) {
        finalFileList.emplace_back(e.path());
      }
    }
    bool isAsync = objTable["isAsync"].value_or(true);
    bool shouldShowWindow = objTable["shouldShowWindow"].value_or(false);
    for (auto& filePath : finalFileList) {
      AtExit::_ins_().appendFunc(make_unique<function<void()>>(
          [=] { execFile(filePath, isAsync, shouldShowWindow); }));
    }
    // execFile(filePath.value<wstring>().value(), isAsync);
  }
}
BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReson, LPVOID lpReserved) {
  if (dwReson != DLL_PROCESS_ATTACH)
    return TRUE;

  DisableThreadLibraryCalls(hModule);

  try {
    // init the ConfigMgr and AtExit
    ConfigMgr::_ins_();
    execWithConfig(ConfigMgr::_ins_().config());
    AtExit::_ins_();
    // setHook();
  } catch (const exception& e) {
    MessageBoxA(nullptr, e.what(), "Exception occured", MB_ICONERROR);
    exit(-1);
  }
  return TRUE;
}