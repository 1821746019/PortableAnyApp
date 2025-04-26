module;
#include <Windows.h>
#include <toml++/toml.hpp>
export module MachineIdCustomizer;
import std;
import Hooker;
import selfInfo;
import strUtils;
import reg_common;
// import ConfigMgr;
namespace fs = std::filesystem;
using namespace std;
class ConfigMgr {
  string configContent_;
  toml::table config_;
  void initFinalConfigContent() {
    fs::path config_path = (selfDir() / (string(BS_TARGET_NAME) + (".toml")));
    if (!fs::exists(config_path)) {
      throw fs::filesystem_error(
          format("Please make sure the {} exists", config_path.string()).data(), config_path,
          make_error_code(errc::no_such_file_or_directory)
      );
    }
    ifstream ifs(config_path);
    configContent_ = string(istreambuf_iterator<char>(ifs), istreambuf_iterator<char>());
  }
  ConfigMgr() {
    initFinalConfigContent();
    config_ = toml::parse(configContent_);
    MachineGuid = config_["MachineGuid"].value_or(L"");
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
  wstring MachineGuid;
};

auto RegQueryValueExW_raw =
    (decltype(&RegQueryValueExW))GetProcAddress(GetModuleHandleA("kernelbase.dll"), "RegQueryValueExW");
auto RegQueryValueExA_raw =
    (decltype(&RegQueryValueExA))GetProcAddress(GetModuleHandleA("kernelbase.dll"), "RegQueryValueExA");
std::string WideMultiSzToAnsiMultiSz(const wchar_t* wMultiSz, size_t cchW) {
  // wMultiSz: 宽字符缓冲
  // cchW: 宽字符个数(包含最后的双'\0'吗？可根据实际情况传入)

  std::string result;
  if (!wMultiSz || cchW == 0)
    return result;

  // 遍历每个子串
  const wchar_t* pCur = wMultiSz;
  while (*pCur) {
    // 把当前子串转 ANSI
    std::string ansi = WideToAnsi(pCur);
    // 追加到result，添加一个 '\0' 分隔
    result.append(ansi.c_str(), ansi.size());
    result.push_back('\0');

    // 跳到下一个子串(以 L'\0' 为结束)
    pCur += wcslen(pCur) + 1;
  }
  // 最终再追加一个 '\0' 作为终止
  result.push_back('\0');
  return result;
}
LONG WINAPI RegQueryValueExA_mod(
    HKEY hKey,
    LPCSTR lpValueName,
    LPDWORD lpReserved,
    LPDWORD lpType,
    LPBYTE lpData,
    LPDWORD lpcbData
) {
  if (!lpValueName) {
    return ERROR_INVALID_PARAMETER;
  }

  // 1) 转换ValueName
  std::wstring wValName = AnsiToWide(lpValueName);

  // 2) 先探测所需大小
  DWORD dwType = 0;
  DWORD cbNeeded = 0;
  LONG ret = RegQueryValueExW(hKey, wValName.c_str(), lpReserved, &dwType, nullptr, &cbNeeded);

  if (ret != ERROR_SUCCESS && ret != ERROR_MORE_DATA) {
    return ret;
  }

  // 把探测到的type和size写回
  if (lpType)
    *lpType = dwType;
  if (lpcbData) {
    // 如果lpData==nullptr，调用方仅仅想获取大小和类型
    if (!lpData) {
      *lpcbData = cbNeeded;
      return ret;
    }
  } else {
    // 调用方不关心大小，但给了lpData，这样也能尝试一次
  }

  // 3) 分配临时缓冲区（cbNeeded 可能是0表示值为空字符串）
  std::vector<BYTE> tmpBuf(cbNeeded + 2);  // 给MULTI_SZ等多留点空间

  // 4) 再次调用W版获取数据
  DWORD dwType2 = 0;
  DWORD cbActual = cbNeeded;  // second call
  ret = RegQueryValueExW(hKey, wValName.c_str(), lpReserved, &dwType2, tmpBuf.data(), &cbActual);
  if (ret != ERROR_SUCCESS) {
    return ret;
  }

  // 再次写回type
  if (lpType) {
    *lpType = dwType2;
  }

  // 5) 根据类型判断
  if (dwType2 == REG_SZ || dwType2 == REG_EXPAND_SZ) {
    // tmpBuf里是一个以L'\0'终止的宽字符串
    // 转成ANSI
    LPCWSTR wData = reinterpret_cast<LPCWSTR>(tmpBuf.data());

    // 安全获取宽字符长度(可能包括终止符)
    size_t cchW = 0;
    if (cbActual >= 2) {
      // 宽字符个数 = cbActual / sizeof(WCHAR)，还要保证不越界
      cchW = cbActual / sizeof(WCHAR);
    }

    // 转ANSI(单字符串)
    int neededA = 0;
    if (cchW > 0) {
      // 不要用wcslen()盲取，因为REG_SZ 的数据不一定都写满?
      // 最安全是把末尾多余空间补0
      // 这里假设系统保证值结尾有\0
      neededA = WideCharToMultiByte(
          CP_ACP, 0, wData,
          (int)(cchW - 1),  // -1 去掉终止符？
          reinterpret_cast<LPSTR>(lpData), (lpcbData ? *lpcbData : 0), nullptr, nullptr
      );
    }

    if (neededA < 0 || (lpcbData && (DWORD)neededA >= *lpcbData)) {
      // 缓冲不够
      ret = ERROR_MORE_DATA;
    } else if (lpData) {
      // 写ANSI终止符
      lpData[neededA] = 0;
      if (lpcbData) {
        *lpcbData = neededA;
      }
    }
  } else if (dwType2 == REG_MULTI_SZ) {
    // tmpBuf里是一个多字符串(L'\0'分隔, 双 L'\0' 结尾)
    LPCWSTR wMulti = reinterpret_cast<LPCWSTR>(tmpBuf.data());
    size_t cchW = cbActual / sizeof(WCHAR);

    // 将“宽多字符串” 转成 “ANSI多字符串”
    // 形如: Wide1\0Wide2\0Wide3\0\0 => Ansi1\0Ansi2\0Ansi3\0\0
    std::string ansiMulti = WideMultiSzToAnsiMultiSz(wMulti, cchW);

    DWORD needed = (DWORD)ansiMulti.size();  // 包含最后的双 \0

    if (lpcbData && needed > *lpcbData) {
      // 缓冲不够
      ret = ERROR_MORE_DATA;
    } else {
      // 拷贝
      if (lpData) {
        memcpy(lpData, ansiMulti.data(), needed);
      }
      if (lpcbData) {
        *lpcbData = needed;
      }
    }
  } else {
    // 非字符串类(含REG_BINARY, REG_DWORD, REG_QWORD, etc.), 直接复制
    if (lpcbData && cbActual > *lpcbData) {
      ret = ERROR_MORE_DATA;
    } else {
      if (lpData) {
        memcpy(lpData, tmpBuf.data(), cbActual);
      }
      if (lpcbData) {
        *lpcbData = cbActual;
      }
    }
  }

  return ret;
}
LONG WINAPI RegQueryValueExW_mod(
    HKEY hKey,
    LPCWSTR lpValueName,
    LPDWORD lpReserved,
    LPDWORD lpType,
    LPBYTE lpData,
    LPDWORD lpcbData
) {
  wstring keyPath = GetUnifiedKeyPath(hKey);
  // 检查是否是加密相关的注册表路径，且查询的是MachineGuid值
  if (_wcsicmp(keyPath.data(), LR"(HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Cryptography)") == 0 &&
      lpValueName && _wcsicmp(lpValueName, L"MachineGuid") == 0) {
    // 获取自定义的MachineGuid
    const wstring& customGuid = ConfigMgr::_ins_().MachineGuid;

    // 设置返回的数据类型为REG_SZ（字符串）
    if (lpType) {
      *lpType = REG_SZ;
    }

    // 计算所需的缓冲区大小（包括终止的空字符）
    const DWORD requiredSize = static_cast<DWORD>((customGuid.length() + 1) * sizeof(wchar_t));

    // 如果调用者只是查询所需的缓冲区大小
    if (!lpData || (lpcbData && *lpcbData < requiredSize)) {
      if (lpcbData) {
        *lpcbData = requiredSize;
      }
      return ERROR_MORE_DATA;
    }

    // 复制自定义GUID到输出缓冲区
    if (lpData && lpcbData) {
      // 复制GUID（包括结尾的空字符）
      memcpy(lpData, customGuid.c_str(), requiredSize);
      // 更新实际写入的字节数
      *lpcbData = requiredSize;
      return ERROR_SUCCESS;
    }
  }

  // 对于其他键或值，使用原始函数处理
  return RegQueryValueExW_raw(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
}
void setHook() {
  DetoursHooker hooker;
  hooker.endeque({

      {&RegQueryValueExA_raw, &RegQueryValueExA_mod},
      {&RegQueryValueExW_raw, &RegQueryValueExW_mod},
  });

  hooker.setHook();
}

extern "C" __declspec(dllexport) BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved) {
  if (dwReason == DLL_PROCESS_ATTACH) {
#ifdef BS_DBG
    // AllocConsole();
    // freopen("CONOUT$", "w", stdout);
#endif
    DisableThreadLibraryCalls(hModule);
    try {
       ConfigMgr::_ins_();
      // RegHandlerMgr::_ins_();
        setHook();
    } catch (const std::exception& e) {
      MessageBoxA(nullptr, e.what(), "Exception occured", MB_ICONERROR);
      exit(-1);
    }
  }
  return TRUE;
}