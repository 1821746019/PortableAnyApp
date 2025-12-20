module;
#include <Windows.h>
#include <ShlObj.h>
#include <DbgHelp.h>
#include <toml++/toml.hpp>
#pragma comment(lib, "dbghelp.lib")
export module UserDataDir_Modifier;
import std;
import Hooker;
import selfInfo;
import strUtils;
import my_converter.str;
using namespace std;
namespace fs = filesystem;
#define configMgr ConfigMgr::_ins_()

const wstring& defaultConfig =
    LR"(
# EXE_DIR is a bult-in variable
[AppData]
only_self_exe = true
all= '''${EXE_DIR}\..\Data'''
#Roaming = '''${EXE_DIR}\..\Data\Roaming'''
#Local = '''${EXE_DIR}\..\Data\Local'''
)";

std::wstring GetCallingModule(int skip_frame = 1) {
  // 初始化符号处理
  SymInitialize(GetCurrentProcess(), NULL, TRUE);

  // 准备上下文
  CONTEXT context = {};
  context.ContextFlags = CONTEXT_FULL;
  RtlCaptureContext(&context);

  // 准备栈帧
  STACKFRAME64 stackFrame = {};
#ifdef _M_IX86
  stackFrame.AddrPC.Offset = context.Eip;
  stackFrame.AddrPC.Mode = AddrModeFlat;
  stackFrame.AddrFrame.Offset = context.Ebp;
  stackFrame.AddrFrame.Mode = AddrModeFlat;
  stackFrame.AddrStack.Offset = context.Esp;
  stackFrame.AddrStack.Mode = AddrModeFlat;
#else
  stackFrame.AddrPC.Offset = context.Rip;
  stackFrame.AddrPC.Mode = AddrModeFlat;
  stackFrame.AddrFrame.Offset = context.Rbp;
  stackFrame.AddrFrame.Mode = AddrModeFlat;
  stackFrame.AddrStack.Offset = context.Rsp;
  stackFrame.AddrStack.Mode = AddrModeFlat;
#endif

  // 获取当前模块句柄（用于跳过自己的模块）
  HMODULE currentModule = NULL;
  {
    MEMORY_BASIC_INFORMATION mbi;
    VirtualQuery(GetCallingModule, &mbi, sizeof(mbi));
    currentModule = (HMODULE)mbi.AllocationBase;
  }

  // 查找调用栈
  for (int i = 0; i < 10; i++) {  // 限制深度避免无限循环
    if (!StackWalk64(
#ifdef _M_IX86
            IMAGE_FILE_MACHINE_I386,
#else
            IMAGE_FILE_MACHINE_AMD64,
#endif
            GetCurrentProcess(), GetCurrentThread(), &stackFrame, &context, NULL, NULL, NULL, NULL
        )) {
      break;
    }

    // 跳过第一个帧（是我们自己）
    if (i < skip_frame)
      continue;

    // 获取模块信息
    HMODULE frameModule = NULL;
    DWORD64 moduleBase = SymGetModuleBase64(GetCurrentProcess(), stackFrame.AddrPC.Offset);

    if (moduleBase) {
      frameModule = (HMODULE)moduleBase;

      // 如果模块不是当前模块，就是我们要找的调用者
      if (frameModule != currentModule) {
        WCHAR modulePath[MAX_PATH] = {0};
        if (GetModuleFileNameW(frameModule, modulePath, MAX_PATH)) {
          SymCleanup(GetCurrentProcess());
          WCHAR* fileName = wcsrchr(modulePath, L'\\');
          return modulePath;  // fileName ? (fileName + 1) : modulePath;
        }
      }
    }
  }

  SymCleanup(GetCurrentProcess());
  return L"未知模块";
}
bool isNeedRedirection() {
  std::wstring callerModulePath = GetCallingModule(2);  // 多了一层调用，skip_frame相应加1
  std::wstring exeDir = selfExeDir();
  if (_wcsnicmp(callerModulePath.data(), exeDir.data(), exeDir.size())) {  // 和exe同级的dll的调用，需重定向
    return true;
  }
  return false;
}
class ConfigMgr {
 public:
  wstring Roaming;
  wstring Local;
  bool only_self_exe = true;  // only self exe can be modified;  // only self exe can be modified
  wstring all;
  toml::table config_;
  wstring getFinalConfigContent() {
    wstring configContent;
    fs::path config_path = (selfDir() / fs::path(__FILE__).filename().replace_extension(".toml"));
    if (fs::exists(config_path)) {
      ifstream ifs(config_path);
      configContent = wstring(istreambuf_iterator<char>(ifs), istreambuf_iterator<char>());
    } else {
      cout << "Config file not found, using default config." << endl;
      configContent = defaultConfig;
    }
    // replace the built-in variable
    wstring exe_dir = selfExeDir().wstring();
    configContent = regex_replace(configContent, wregex(LR"(\$\{EXE_DIR\})"), exe_dir);
    return configContent;
  }

  void initConfigVar() {
    auto AppData_tbl = config_["AppData"];
    only_self_exe = AppData_tbl["only_self_exe"].value_or(true);
    all = AppData_tbl["all"].value_or(L"");
    Roaming = AppData_tbl["Roaming"].value_or(L"");
    Local = AppData_tbl["Local"].value_or(L"");
    if (Roaming.empty()) {
      throw runtime_error("Roaming path is empty");
    }
    // resolve path
    // Roaming = fs::absolute(Roaming).string();
    // Local = fs::absolute(Local).string();
  }
  ConfigMgr() {
    config_ = toml::parse(brv::strConvert(getFinalConfigContent()));
    initConfigVar();
  }
  inline static unique_ptr<ConfigMgr> ins_ = nullptr;

 public:
  ConfigMgr(const ConfigMgr&) = delete;
  ConfigMgr& operator=(const ConfigMgr&) = delete;
  inline static ConfigMgr& _ins_() {
    if (!ins_) {
      ins_ = unique_ptr<ConfigMgr>(new ConfigMgr);
    }
    return *ins_;
  }
};
decltype(&SHGetFolderPathW) SHGetFolderPathW_raw = &SHGetFolderPathW;

HRESULT WINAPI SHGetFolderPathW_mod(HWND hwnd, int csidl, HANDLE hToken, DWORD dwFlags, LPWSTR pszPath) {
  int i = 0;
  if (configMgr.only_self_exe && isNeedRedirection()) {
    return SHGetFolderPathW_raw(hwnd, csidl, hToken, dwFlags, pszPath);
  }
  if (configMgr.all.size()) {
    HRESULT hr = SHGetFolderPathW_raw(hwnd, csidl, hToken, dwFlags, pszPath);
    if (!SUCCEEDED(hr))
      return hr;

    static const wstring& target = LR"(\AppData)";
    auto found = wcsistr(pszPath, target.data());
    if (found) {  //&& (found[target.size()] == L'\\' || found[target.size()] == 0)
      wstring dst_dir = (configMgr.all + (found + target.size()));
      wcscpy_s(pszPath, MAX_PATH, dst_dir.data());
    }
    return hr;
  } else if (configMgr.Roaming.size()) {
    HRESULT hr = SHGetFolderPathW_raw(hwnd, csidl, hToken, dwFlags, pszPath);
    if (!SUCCEEDED(hr))
      return hr;
    static const wstring& target = LR"(\AppData\Roaming)";
    auto found = wcsistr(pszPath, target.data());
    if (found && (found[target.size()] == L'\\' || found[target.size()] == 0)) {
      wcscpy_s(pszPath, MAX_PATH, configMgr.Roaming.data());
    }
    return hr;
  } else if (configMgr.Local.size()) {
    HRESULT hr = SHGetFolderPathW_raw(hwnd, csidl, hToken, dwFlags, pszPath);
    if (!SUCCEEDED(hr))
      return hr;
    static const wstring& target = LR"(\AppData\Local)";
    auto found = wcsistr(pszPath, target.data());
    if (found && (found[target.size()] == L'\\' || found[target.size()] == 0)) {
      wcscpy_s(pszPath, MAX_PATH, configMgr.Local.data());
    }
    return hr;
  }
  return SHGetFolderPathW_raw(hwnd, csidl, hToken, dwFlags, pszPath);
}
decltype(&SHGetKnownFolderPath) SHGetKnownFolderPath_raw =
    &SHGetKnownFolderPath;  // GetProcAddress(GetModuleHandleA("shell32.dll"), "SHGetKnownFolderPath");
// 辅助函数，用于替换 KnownFolder 路径
HRESULT ReplaceKnownFolderPath(
    PWSTR* ppszPath,
    const wstring& target,
    const wstring& newBasePath,
    bool checkAfter = true
) {
  auto found = wcsistr(*ppszPath, target.data());
  if (!found) {
    return S_OK;
  }

  // 检查后续字符（如果需要）
  if (checkAfter && !(found[target.size()] == L'\\' || found[target.size()] == 0)) {
    return S_OK;
  }

  // 保存后缀路径
  wstring suffix = found + target.size();
  // 构建新路径
  wstring newPath = newBasePath + suffix;

  // 保存旧指针
  PWSTR oldPath = *ppszPath;

  // 分配新内存
  size_t newSize = (newPath.length() + 1) * sizeof(wchar_t);
  *ppszPath = (PWSTR)CoTaskMemAlloc(newSize);
  if (!*ppszPath) {
    *ppszPath = oldPath;
    return E_OUTOFMEMORY;
  }

  // 复制新路径并释放旧内存
  wcscpy_s(*ppszPath, newPath.length() + 1, newPath.c_str());
  CoTaskMemFree(oldPath);

  return S_OK;
}

HRESULT WINAPI
SHGetKnownFolderPath_mod(REFKNOWNFOLDERID rfid, DWORD dwFlags, HANDLE hToken, PWSTR* ppszPath) {
  if (configMgr.only_self_exe && isNeedRedirection()) {
    return SHGetKnownFolderPath_raw(rfid, dwFlags, hToken, ppszPath);
  }

  HRESULT hr = SHGetKnownFolderPath_raw(rfid, dwFlags, hToken, ppszPath);
  if (!SUCCEEDED(hr))
    return hr;

  if (configMgr.all.size()) {
    static const wstring& target = LR"(\AppData)";
    hr = ReplaceKnownFolderPath(ppszPath, target, configMgr.all, false);  // 与原函数保持一致
  } else if (configMgr.Roaming.size()) {
    static const wstring& target = LR"(\AppData\Roaming)";
    hr = ReplaceKnownFolderPath(ppszPath, target, configMgr.Roaming);
  } else if (configMgr.Local.size()) {
    static const wstring& target = LR"(\AppData\Local)";
    hr = ReplaceKnownFolderPath(ppszPath, target, configMgr.Local);
  }

  return hr;
}

void setHook() {
  DetoursHooker hooker;
  hooker.endeque({
      {&SHGetFolderPathW_raw, &SHGetFolderPathW_mod},
      {&SHGetKnownFolderPath_raw, &SHGetKnownFolderPath_mod},

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