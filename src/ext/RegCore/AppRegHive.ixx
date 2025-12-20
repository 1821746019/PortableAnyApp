module;

#include <Windows.h>

export module AppRegHive;

import std;
import selfInfo;
import reg_common;
namespace fs = std::filesystem;

class AppHiveMgr {
  AppHiveMgr(const AppHiveMgr&) = delete;
  AppHiveMgr& operator=(const AppHiveMgr&) = delete;
  AppHiveMgr(AppHiveMgr&&) = delete;
  AppHiveMgr& operator=(AppHiveMgr&&) = delete;

  AppHiveMgr() {}
  HKEY hKey_ = nullptr;
  std::mutex mtx_;
  inline static std::unique_ptr<AppHiveMgr> ins_;
  void hotReloadAutomatically() {}

 public:
  HKEY hKey() {
    std::lock_guard<std::mutex> lock(mtx_);
    if (!hKey_) {
      // 复制到 系统temp/exe名+exe路径hash 避免原AppHive被系统锁住无法改动
      auto exePath = selfExePath();
      fs::path dstDir =
          fs::temp_directory_path() / (exePath.filename().wstring() + L"_" +
                                       std::to_wstring(std::hash<std::wstring>{}(exePath.wstring())));
      fs::path dstPath = dstDir / "AppRegHive";
      std::error_code ec;
      fs::create_directory(dstDir, ec);
      if (ec) {
        std::cerr << "create_directory failed: " << ec.message() << std::endl;
      }

      fs::copy_file(selfDir() / "AppRegHive", dstPath, fs::copy_options::overwrite_existing);

      auto status = RegLoadAppKeyW(dstPath.c_str(), &hKey_, KEY_ALL_ACCESS, REG_PROCESS_APPKEY, 0);
      if (status != ERROR_SUCCESS) {
        throw std::runtime_error(
            "failed to load AppRegHive. Please check if it exists in the same "
            "dir of the dll"
        );
      }
    }

    return hKey_;
  }
  static AppHiveMgr& _ins_() {
    if (!ins_) {
      ins_ = std::unique_ptr<AppHiveMgr>(new AppHiveMgr());
    }
    return *ins_;
  }
};
// 在调用RegLoadKeyW之前添加这段代码
BOOL AdjustPrivilege(LPCTSTR lpszPrivilege) {
  HANDLE hToken;
  TOKEN_PRIVILEGES tp;
  LUID luid;

  if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
    return FALSE;

  if (!LookupPrivilegeValue(NULL, lpszPrivilege, &luid)) {
    CloseHandle(hToken);
    return FALSE;
  }

  tp.PrivilegeCount = 1;
  tp.Privileges[0].Luid = luid;
  tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

  if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL)) {
    CloseHandle(hToken);
    return FALSE;
  }

  CloseHandle(hToken);
  return TRUE;
}

export {
  HKEY MountAppHive(const std::wstring& hivePath) {
    // 获取当前可执行文件路径
    std::wstring exePath = selfExePath();
    // 提取可执行文件名
    std::wstring exeName = std::filesystem::path(exePath).filename().wstring();

    std::wstring toSafeExeDir = selfExeDir().wstring();
    // 替换所有反斜杠为正斜杠
    for (size_t pos = toSafeExeDir.find(L'\\'); pos != std::wstring::npos;
         pos = toSafeExeDir.find(L'\\', pos + 1)) {
      toSafeExeDir.replace(pos, 1, L"/");
    }
    // 构建注册表路径
    std::wstring regKeyPath = exeName + L"_" + toSafeExeDir;

    // 检查是否已经挂载（先尝试打开注册表项）
    HKEY hKey = NULL;
    LONG result = RegOpenKeyExW(HKEY_USERS, regKeyPath.c_str(), 0, KEY_READ, &hKey);

    // 如果已挂载，则直接返回打开的hKey;
    if (result == ERROR_SUCCESS) {
      return hKey;
    }
    // 然后在MountAppHive函数中，在调用RegLoadKeyW前启用这些特权
    if (!AdjustPrivilege(SE_RESTORE_NAME) || !AdjustPrivilege(SE_BACKUP_NAME)) {
      std::wcerr << L"无法获取必要特权，最后错误: " << GetLastError() << std::endl;
      return NULL;
    }
    // 挂载注册表文件（需要管理员权限）
    // 创建挂载键防止不存在
    //HKEY _;
    //auto AppHiveKeyCreateResult = RegCreateKeyExW(
    //    HKEY_USERS, L"AppHive", 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, nullptr, &_,
    //    nullptr
    //);
    result = RegLoadKeyW(HKEY_USERS, regKeyPath.c_str(), hivePath.c_str());
    if (result != ERROR_SUCCESS) {
      std::wcerr << L"挂载注册表文件失败，错误码: " << result << std::endl;
      return NULL;
    }

    // 打开挂载的注册表项并返回句柄
    result = RegOpenKeyExW(HKEY_USERS, regKeyPath.c_str(), 0, KEY_ALL_ACCESS, &hKey);
    if (result != ERROR_SUCCESS) {
      std::wcerr << L"打开挂载的注册表项失败，错误码: " << result << std::endl;
      return NULL;
    }

    return hKey;
  }
  HKEY getCachedAppHiveRootKey() {
    // return AppHiveMgr::_ins_().hKey();

    static HKEY hiveRootKey = [] {
      HKEY ret;
      fs::path hivePath = selfDir() / "AppRegHive";
      ret = MountAppHive(hivePath);
      return ret;
    }();
    return hiveRootKey;
  };
  std::wstring getAppRegHiveRegPath() {
    static std::wstring ret = GetKeyPath(getCachedAppHiveRootKey());
    return ret;
  };
  auto getRootKeyMap() {
    std::unordered_map<HKEY, HKEY> ret;
    HKEY hKeyMachine = nullptr, hKeyUser = nullptr, hKeyUsers = nullptr, hKeyClasses = nullptr,
         hKeyCurrentConfig = nullptr;
    // DWORD disposition;  // 接收键的创建/打开状态

    // 打开或创建 HKEY_LOCAL_MACHINE
    LSTATUS resultMachine = RegCreateKeyExW(
        getCachedAppHiveRootKey(),
        L"HKEY_LOCAL_MACHINE",    // 子键名称
        0,                        // 保留，一般填0
        nullptr,                  // 类名（可填空）
        REG_OPTION_NON_VOLATILE,  // 非易失性
        KEY_ALL_ACCESS,           // 全权限，方便读写
        nullptr,                  // 安全属性
        &hKeyMachine,             // 返回创建好的子键
        nullptr                   // 不关心是否已存在
    );

    // 打开或创建 HKEY_CURRENT_USER
    LSTATUS resultUser = RegCreateKeyExW(
        getCachedAppHiveRootKey(), L"HKEY_CURRENT_USER", 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
        nullptr, &hKeyUser, nullptr
    );
    LSTATUS resultClasses = RegCreateKeyExW(
        getCachedAppHiveRootKey(), L"HKEY_CLASSES_ROOT", 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
        nullptr, &hKeyClasses, nullptr
    );
    LSTATUS resultUsers = RegCreateKeyExW(
        getCachedAppHiveRootKey(), L"HKEY_USERS", 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
        nullptr, &hKeyUsers, nullptr
    );
    LSTATUS resultCurrentConfig = RegCreateKeyExW(
        getCachedAppHiveRootKey(), L"HKEY_CURRENT_CONFIG", 0, nullptr, REG_OPTION_NON_VOLATILE,
        KEY_ALL_ACCESS, nullptr, &hKeyCurrentConfig, nullptr
    );
    if (resultMachine != ERROR_SUCCESS || resultUser != ERROR_SUCCESS || resultClasses != ERROR_SUCCESS ||
        resultUsers != ERROR_SUCCESS || resultCurrentConfig != ERROR_SUCCESS) {
      if (hKeyMachine)
        RegCloseKey(hKeyMachine);
      if (hKeyUser)
        RegCloseKey(hKeyUser);
      throw std::runtime_error("failed to open or create machine or user key");
    }

    ret[HKEY_LOCAL_MACHINE] = hKeyMachine;
    ret[HKEY_CURRENT_USER] = hKeyUser;
    ret[HKEY_CLASSES_ROOT] = hKeyClasses;
    ret[HKEY_USERS] = hKeyUsers;
    ret[HKEY_CURRENT_CONFIG] = hKeyCurrentConfig;
    return ret;
  }
  inline auto getCachedRootKeyMap() {
    static const std::unordered_map<HKEY, HKEY> rootKeyMap = getRootKeyMap();
    return rootKeyMap;
    ;
  }
}