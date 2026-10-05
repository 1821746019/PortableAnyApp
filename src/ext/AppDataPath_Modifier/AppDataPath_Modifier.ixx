module;
#include <Windows.h>
#include <ShlObj.h>
#include <toml++/toml.hpp>
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
only_app_caller = true
all= '''${EXE_DIR}\..\Data'''
#Roaming = '''${EXE_DIR}\..\Data\Roaming'''
#Local = '''${EXE_DIR}\..\Data\Local'''
)";

// 常见业务运行时/框架 DLL 列表（即使位于系统共享目录，也代表应用本身的业务逻辑）
inline static const wstring_view kRuntimeModules[] = {
    L"coreclr.dll",
    L"clr.dll",
    L"hostpolicy.dll",
    L"hostfxr.dll",
    L"mono.dll",
    L"monosgen.dll",
    L"node.dll",
    L"v8.dll",
    L"jvm.dll",
};

bool isTrustedModule(HMODULE hModule, const wstring& exeDirWithSlash) {
  if (!hModule)
    return false;

  // 1. 检查是否就是主程序自身
  if (hModule == GetModuleHandleW(nullptr)) {
    return true;
  }

  wchar_t modPath[MAX_PATH] = {0};
  if (!GetModuleFileNameW(hModule, modPath, MAX_PATH)) {
    return false;
  }

  // 2. 检查是否位于 exe 同级或子目录下
  if (!exeDirWithSlash.empty() && _wcsnicmp(modPath, exeDirWithSlash.data(), exeDirWithSlash.size()) == 0) {
    return true;
  }

  // 3. 检查是否属于常见业务运行时 DLL
  const wchar_t* fileName = wcsrchr(modPath, L'\\');
  const wchar_t* baseName = fileName ? (fileName + 1) : modPath;
  for (const auto& rtName : kRuntimeModules) {
    if (_wcsicmp(baseName, rtName.data()) == 0) {
      return true;
    }
  }

  return false;
}

// 溯源整条调用栈，判断是否由主程序或其运行时触发
bool shouldRedirect() {
  void* backtrace[64] = {nullptr};
  // 捕获调用栈（跳过自身 shouldRedirect 帧，最多捕获 64 帧）
  USHORT captured = CaptureStackBackTrace(1, 64, backtrace, nullptr);

  wstring exeDir = selfExeDir().wstring();
  if (!exeDir.empty() && exeDir.back() != L'\\') {
    exeDir.push_back(L'\\');
  }

  for (USHORT i = 0; i < captured; ++i) {
    HMODULE hMod = nullptr;
    // 获取返回地址所属 PE 模块（轻量微秒级，天然线程安全）
    if (GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(backtrace[i]),
            &hMod
        ) && hMod) {
      if (isTrustedModule(hMod, exeDir)) {
        return true;  // 只要调用链路上出现主程序或其支持的运行时，即认定为需要重定向
      }
    }
    // 注：JIT 动态生成的代码内存帧没有对应 HMODULE，循环自然穿透并继续向上溯源
  }

  // 整条栈上均未发现主程序或其运行时，认定为非本程序/第三方注入组件
  return false;
}

class ConfigMgr {
 public:
  wstring Roaming;
  wstring Local;
  bool only_app_caller = true;  // only calls originating from app components/runtimes will be redirected
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
    only_app_caller = AppData_tbl["only_app_caller"].value_or(AppData_tbl["only_self_exe"].value_or(true));
    all = AppData_tbl["all"].value_or(L"");
    Roaming = AppData_tbl["Roaming"].value_or(L"");
    Local = AppData_tbl["Local"].value_or(L"");
    //if (Roaming.empty()) {
    //  throw runtime_error("Roaming path is empty");
    //}
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
  if (configMgr.only_app_caller && !shouldRedirect()) {
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
  if (configMgr.only_app_caller && !shouldRedirect()) {
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