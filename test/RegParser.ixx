module;
#include <Windows.h>

export module RegParser;

import std;

using namespace std;

class RegParser {
 public:
  RegParser(const wstring& regContetn) {}
};

HKEY getCachedAppHiveRootKey() {
  static HKEY ret = [] {
    HKEY ret;
    auto status = RegLoadAppKeyW(
        LR"(D:\Program\OSEnhance\RegistryFinder_2.59.0_64bit_Green\AppRegHive)",
        &ret, KEY_ALL_ACCESS, REG_PROCESS_APPKEY, 0);
    if (status != ERROR_SUCCESS) {
      throw std::runtime_error(
          "failed to load AppRegHive. Please check if it exists in the same "
          "dir of the dll");
    }
    return ret;
  }();
  return ret;
};
extern "C" int main() {
  try {
    // 1. 获取 AppHive 句柄
    HKEY hAppHive = getCachedAppHiveRootKey();
    HKEY hMachine = nullptr;
    // 2. 测试创建一个名为 "TestKey" 的子键
    LONG rootStatus = RegCreateKeyExW(hAppHive,
                                  L"HKEY_LOCAL_MACHINE",  // 子键名称
                                  0,           // 保留，一般填0
                                  nullptr,     // 类名（可填空）
                                  REG_OPTION_NON_VOLATILE,  // 非易失性
                                  KEY_ALL_ACCESS,  // 全权限，方便读写
                                  nullptr,         // 安全属性
                                      &hMachine,  // 返回创建好的子键
                                  nullptr          // 不关心是否已存在
    );
    HKEY hTestKey = nullptr;
    LONG status = RegCreateKeyExW(hMachine,
                             L"Software\\SogouInput",            // 子键名称
                             0,                        // 保留，一般填0
                             nullptr,                  // 类名（可填空）
                             REG_OPTION_NON_VOLATILE,  // 非易失性
                             KEY_ALL_ACCESS,  // 全权限，方便读写
                             nullptr,         // 安全属性
                             &hTestKey,       // 返回创建好的子键
                             nullptr          // 不关心是否已存在
    );
    if (status != ERROR_SUCCESS) {
      throw std::runtime_error("RegCreateKeyExW 创建键失败，请检查权限。");
    }

    // 3. 往这个子键写入一个简单字符串值做验证
    const wchar_t* data = L"Hello from AppHive";
    status = RegSetValueExW(
        hTestKey,
        L"MyValue",  // 值名称
        0,           // 保留
        REG_SZ,      // 字符串类型
        reinterpret_cast<const BYTE*>(data),
        static_cast<DWORD>((wcslen(data) + 1) * sizeof(wchar_t)));

    if (status != ERROR_SUCCESS) {
      RegCloseKey(hTestKey);
      throw std::runtime_error("RegSetValueExW 写值失败，请检查权限。");
    }

    // 4. 清理句柄
    RegCloseKey(hTestKey);

    std::cout << "测试成功，AppHive 可写！" << std::endl;
    return 0;  // 表示成功

  } catch (const std::exception& ex) {
    std::cerr << "测试失败: " << ex.what() << std::endl;
    return 1;  // 表示失败
  }
}