module;
#include <Windows.h>
#include <Psapi.h>
#include <toml++/toml.h>
// #include <spdlog/sp>
export module _;
import std;
 //import std.compat;
import selfInfo;

using namespace std;
namespace fs = filesystem;
#pragma comment(lib, "psapi.lib")

// 函数：创建并重定向控制台
void CreateAndRedirectConsole() {
  // 获取当前控制台窗口的句柄
  HWND hConsole = GetConsoleWindow();

  if (hConsole != nullptr) {
    // 当前程序已有控制台，检查是否被隐藏
    if (!IsWindowVisible(hConsole)) {
      // 如果控制台被隐藏，显示它
      ShowWindow(hConsole, SW_SHOW);
    }
  } else {
    // 当前程序没有控制台，分配一个新的控制台
    if (!AllocConsole()) {
      MessageBoxW(nullptr, L"无法分配控制台。", L"错误", MB_OK | MB_ICONERROR);
      return;
    }

    // 重定向标准输出到控制台
    FILE *fpOut;
    if (freopen_s(&fpOut, "CONOUT$", "w", stdout) != 0 || fpOut == nullptr) {
      MessageBoxW(nullptr, L"无法重定向标准输出。", L"错误",
                  MB_OK | MB_ICONERROR);
      return;
    }

    // 重定向标准输入到控制台
    FILE *fpIn;
    if (freopen_s(&fpIn, "CONIN$", "r", stdin) != 0 || fpIn == nullptr) {
      MessageBoxW(nullptr, L"无法重定向标准输入。", L"错误",
                  MB_OK | MB_ICONERROR);
      return;
    }

    // 重定向标准错误输出到控制台
    FILE *fpErr;
    if (freopen_s(&fpErr, "CONOUT$", "w", stderr) != 0 || fpErr == nullptr) {
      MessageBoxW(nullptr, L"无法重定向标准错误输出。", L"错误",
                  MB_OK | MB_ICONERROR);
      return;
    }
    std::ios::sync_with_stdio();
  }
}

bool isLShiftDown() {
  bool ret = false;
  constexpr auto TIMEOUT = chrono::milliseconds(200);
  std::chrono::milliseconds elapsed(0); // 正确初始化 elapsed
  chrono::milliseconds interval(15);
  while (elapsed < TIMEOUT) {
    if (GetAsyncKeyState(VK_LSHIFT) < 0) {
      ret = true;
      break;
    }
    this_thread::sleep_for(interval);
    elapsed += interval;
  }
  return ret;
}

class ConfigMgr {
  string configContent_;
  toml::table config_;
  wstring currExeName_ = [] {
    wstring buf(MAX_PATH, 0);
    GetModuleFileNameW(GetModuleHandleA(nullptr), buf.data(), buf.size());
    wstring ret = wcsrchr(buf.data(), '\\') + 1;
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
  void initFinalConfigContent() {
    fs::path config_path =
        (selfDir() / fs::path(__FILE__).filename().replace_extension(".toml"));
    ifstream ifs(config_path);

    if (!ifs)
      MessageBoxA(
          nullptr,
          format("Please make sure the {} exists", config_path.string()).data(),
          "File Not Found", MB_ICONERROR);
    configContent_ =
        string(istreambuf_iterator<char>(ifs), istreambuf_iterator<char>());
    preprocConfigContent();
  }

public:
  ConfigMgr() {
    initFinalConfigContent();
    config_ = toml::parse(configContent_);
  }
  bool isNeedAllocing(wstring_view cmdline) {
    // 获取配置
    bool isBlackListMode = config_["isBlacklistMode"].as_boolean();
    vector<wstring> excluded_list;
    for (auto &e : *(config_["excluded_list"].as_array())) {
      excluded_list.emplace_back(e.value<wstring>().value());
    }
    vector<wstring> included_list;
    for (auto &e : *config_["included_list"].as_array()) {
      included_list.emplace_back(e.value<wstring>().value());
    }

    // 定义正则表达式向量
    vector<wregex> excluded_re, included_re;

    // 转换函数，将字符串转换为 wregex
    auto transFunc = [&](wstring str) -> wregex {
      // 检查字符串是否以 '/' 开头和结尾
      if (str.starts_with(L"/") && str.ends_with(L"/")) {
        // 提取中间的内容
        str = str.substr(1, str.length() - 2);
        // /配置文件这里面应自行转义
        // size_t off = str.find(currExeName_);
        // wstring escaped =
        //    currExeName_.substr(0).replace(currExeName_.find(L"."), 1,
        //    LR"(\.)");
        ////转义PROCESS_NAME中的.为正则匹配服务 .任意单字符，可以是.
        // str=str.replace(off, currExeName_.size(),
        //             escaped);
      } else {
        // indicate that it is not a regex pattern
        //  替换所有的 '.' 为 '\.'
        size_t pos;
        // 替换所有/
        pos = 0;
        while ((pos = str.find(LR"(\)", pos)) != wstring::npos) {
          str.replace(pos, 1, LR"(\\)");
          pos += 2; // 跳过新替换的字符
        }
        pos = 0;
        while ((pos = str.find(L".", pos)) != wstring::npos) {
          str.replace(pos, 1, LR"(\.)");
          pos += 2; // 跳过新替换的字符
        }
      }
      // 创建并返回 wregex 对象
      return wregex(str);
    };
    // 转换 excluded_list 和 included_list 为正则表达式
    ranges::transform(excluded_list, back_inserter(excluded_re), transFunc);
    ranges::transform(included_list, back_inserter(included_re),
                      transFunc); // 修正这里，插入到 in_re

    // 匹配过程名是否在黑名单或白名单中
    bool match_excluded = ranges::any_of(excluded_re, [&](const wregex &re) {
      return regex_search(cmdline.begin(), cmdline.end(), re);
    });

    bool match_included = ranges::any_of(included_re, [&](const wregex &re) {
      return regex_search(cmdline.begin(), cmdline.end(), re);
    });

    // 合并黑名单和白名单的逻辑
    if (isBlackListMode) {
      // 黑名单模式:
      // 如果匹配到黑名单，或者不匹配任何白名单，则不需要加载
      // 括号内为true则匹配黑名单
      return !(!match_excluded && match_included);
    } else {
      // 白名单模式: 符合目标的才加载
      // 如果匹配到白名单，或者不匹配任何黑名单，则需要加载
      // 不被排除且匹配
      return !match_excluded && match_included;
    }
  }
};
void allocConsoleIfNeeded() {
  try {
    ConfigMgr configMgr;
    bool ret = configMgr.isNeedAllocing(GetCommandLineW()) // && isLShiftDown()
        ;
    if (ret) {
      CreateAndRedirectConsole();
    }
  } catch (const exception &e) {
    MessageBoxA(nullptr, e.what(), "Exception occurs", MB_ICONERROR);
  }
}
std::wstring GetForegroundWindowExePath() {
  HWND hwnd = GetForegroundWindow();
  DWORD processId;
  GetWindowThreadProcessId(hwnd, &processId);

  HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                FALSE, processId);
  if (hProcess == nullptr) {
    return L"";
  }

  wchar_t exePath[MAX_PATH];
  if (GetModuleFileNameExW(hProcess, nullptr, exePath, MAX_PATH) == 0) {
    CloseHandle(hProcess);
    return L"";
  }

  CloseHandle(hProcess);
  return exePath;
}
void initHotkeyToShowConsole() {
  wstring currExePath = [] {
    wchar_t buf[MAX_PATH];
    GetModuleFileNameW(nullptr, buf, size(buf));
    return wstring(buf);
  }();
  while (true) {
    this_thread::sleep_for(chrono::milliseconds(200));

    if (GetKeyState(VK_F11) >= 0 || GetForegroundWindowExePath() != currExePath)
      continue;

    CreateAndRedirectConsole();
  }
}
// static struct Init {
//   Init()
//   {
//
//   }
// } init;

BOOL DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved) {
  DisableThreadLibraryCalls(hModule);
  if (dwReason == DLL_PROCESS_ATTACH) {
    allocConsoleIfNeeded();

    //thread(initHotkeyToShowConsole).detach();
  } else if (dwReason == DLL_PROCESS_DETACH) {
  }
  return TRUE;
}
