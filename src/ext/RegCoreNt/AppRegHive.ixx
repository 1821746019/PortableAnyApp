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

export {
  HKEY getCachedAppHiveRootKey() {
    // return AppHiveMgr::_ins_().hKey();

    static HKEY ret = [] {
      HKEY ret;
      fs::path hivePath = selfDir() / "AppRegHive";
      if (!fs::exists(hivePath)) {
        throw std::runtime_error("Failed to load AppRegHive: AppRegHive Not Found");
      }
      auto status = RegLoadAppKeyW(hivePath.c_str(), &ret, KEY_ALL_ACCESS, REG_PROCESS_APPKEY, 0);
      if (status != ERROR_SUCCESS) {
        throw std::runtime_error(
            "Failed to load AppRegHive. Please check if it has been loaded in other exe"
        );
      }
      return ret;
    }();
    return ret;
  };
  std::wstring getAppRegHiveRegPath() {
    static std::wstring ret = GetKeyPath(getCachedAppHiveRootKey());
    return ret;
  };
  const std::unordered_map<HKEY, HKEY> rootKeyMap = [] {
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
        getCachedAppHiveRootKey(), L"HKEY_USERS", 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, nullptr,
        &hKeyUsers, nullptr
    );
    LSTATUS resultCurrentConfig = RegCreateKeyExW(
        getCachedAppHiveRootKey(), L"HKEY_CURRENT_CONFIG", 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
        nullptr, &hKeyCurrentConfig, nullptr
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
  }();
}