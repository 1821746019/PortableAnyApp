module;
#include <ntdll.h>
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
    MachineGuid = config_["MachineGuid"].value_or(L"9f79a9d3-d029-4489-b59a-b382f2b14350");
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
decltype(&NtQueryValueKey) NtQueryValueKey_raw = &NtQueryValueKey;
decltype(&NtEnumerateValueKey) NtEnumerateValueKey_raw = &NtEnumerateValueKey;
NTSTATUS
NTAPI
NtQueryValueKey_mod(
    _In_ HANDLE KeyHandle,
    _In_ PUNICODE_STRING ValueName,
    _In_ KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
    _Out_ PVOID KeyValueInformation,
    _In_ ULONG Length,
    _Out_ PULONG ResultLength
) {
  // 检查是否查询的是"MachineGuid"
  bool isMachineGuid = false;
  if (ValueName != nullptr && ValueName->Buffer != nullptr) {
    if (_wcsnicmp(ValueName->Buffer, L"MachineGuid", ValueName->Length / sizeof(WCHAR)) == 0) {
      // 获取注册表键路径，检查是否是我们要修改的路径
      ULONG keyNameSize = 0;
      NTSTATUS status = NtQueryKey(KeyHandle, KeyNameInformation, nullptr, 0, &keyNameSize);

      if (status == STATUS_BUFFER_TOO_SMALL || status == STATUS_BUFFER_OVERFLOW) {
        std::vector<BYTE> buffer(keyNameSize);
        PKEY_NAME_INFORMATION keyNameInfo = reinterpret_cast<PKEY_NAME_INFORMATION>(buffer.data());

        status = NtQueryKey(KeyHandle, KeyNameInformation, keyNameInfo, keyNameSize, &keyNameSize);
        if (NT_SUCCESS(status)) {
          // 检查键路径是否包含Cryptography
          const WCHAR cryptoPath[] = L"\\REGISTRY\\MACHINE\\SOFTWARE\\MICROSOFT\\CRYPTOGRAPHY";

          std::wstring keyPath(keyNameInfo->Name, keyNameInfo->NameLength / sizeof(WCHAR));
          std::wstring keyPathUpper = keyPath;
          std::transform(keyPathUpper.begin(), keyPathUpper.end(), keyPathUpper.begin(), ::towupper);

          if (keyPathUpper.find(cryptoPath) != std::wstring::npos) {
            isMachineGuid = true;
          }
        }
      }
    }
  }

  // 如果不是目标键值或自定义GUID为空，则调用原始函数
  if (!isMachineGuid || ConfigMgr::_ins_().MachineGuid.empty()) {
    return NtQueryValueKey_raw(
        KeyHandle, ValueName, KeyValueInformationClass, KeyValueInformation, Length, ResultLength
    );
  }

  // 处理自定义的MachineGuid
  const std::wstring& customGuid = ConfigMgr::_ins_().MachineGuid;
  ULONG dataLength = (customGuid.length() + 1) * sizeof(WCHAR);  // 包含NULL终止符

  // 根据不同的信息类别直接构建返回结构
  switch (KeyValueInformationClass) {
    case KeyValuePartialInformation: {
      // 计算所需总大小
      ULONG requiredSize = FIELD_OFFSET(KEY_VALUE_PARTIAL_INFORMATION, Data) + dataLength;

      // 如果只是查询大小
      if (Length == 0) {
        *ResultLength = requiredSize;
        return STATUS_BUFFER_TOO_SMALL;
      }

      // 检查缓冲区是否足够
      if (Length < requiredSize) {
        *ResultLength = requiredSize;
        return STATUS_BUFFER_OVERFLOW;
      }

      // 填充结构
      PKEY_VALUE_PARTIAL_INFORMATION partialInfo =
          static_cast<PKEY_VALUE_PARTIAL_INFORMATION>(KeyValueInformation);
      partialInfo->TitleIndex = 0;
      partialInfo->Type = REG_SZ;
      partialInfo->DataLength = dataLength;
      memcpy(partialInfo->Data, customGuid.c_str(), dataLength);

      *ResultLength = requiredSize;
      return STATUS_SUCCESS;
    }

    case KeyValueFullInformation: {
      // 对于完整信息，我们需要考虑名称和数据的布局
      ULONG nameLength = ValueName->Length;

      // 计算数据偏移量（结构头 + 名称长度 + 对齐）
      ULONG dataOffset = FIELD_OFFSET(KEY_VALUE_FULL_INFORMATION, Name) + nameLength;
      // 对齐到4字节边界
      dataOffset = (dataOffset + 3) & ~3;

      // 计算所需总大小
      ULONG requiredSize = dataOffset + dataLength;

      // 如果只是查询大小
      if (Length == 0) {
        *ResultLength = requiredSize;
        return STATUS_BUFFER_TOO_SMALL;
      }

      // 检查缓冲区是否足够
      if (Length < requiredSize) {
        *ResultLength = requiredSize;
        return STATUS_BUFFER_OVERFLOW;
      }

      // 填充结构
      PKEY_VALUE_FULL_INFORMATION fullInfo = static_cast<PKEY_VALUE_FULL_INFORMATION>(KeyValueInformation);
      fullInfo->TitleIndex = 0;
      fullInfo->Type = REG_SZ;
      fullInfo->DataOffset = dataOffset;
      fullInfo->DataLength = dataLength;
      fullInfo->NameLength = nameLength;

      // 复制名称
      memcpy(fullInfo->Name, ValueName->Buffer, nameLength);

      // 复制数据（GUID）
      memcpy((BYTE*)fullInfo + dataOffset, customGuid.c_str(), dataLength);

      *ResultLength = requiredSize;
      return STATUS_SUCCESS;
    }

    default:
      // 对于其他信息类型，调用原始函数
      return NtQueryValueKey_raw(
          KeyHandle, ValueName, KeyValueInformationClass, KeyValueInformation, Length, ResultLength
      );
  }
}

NTSTATUS
NTAPI
NtEnumerateValueKey_mod(
    _In_ HANDLE KeyHandle,
    _In_ ULONG Index,
    _In_ KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
    _Out_opt_ PVOID KeyValueInformation,
    _In_ ULONG Length,
    _Out_ PULONG ResultLength
) {
  // 先调用原始函数获取结果
  NTSTATUS status = NtEnumerateValueKey_raw(
      KeyHandle, Index, KeyValueInformationClass, KeyValueInformation, Length, ResultLength
  );

  // 检查是否成功获取值信息且有自定义GUID
  if (NT_SUCCESS(status) && !ConfigMgr::_ins_().MachineGuid.empty()) {
    bool isMachineGuid = false;
    bool isCryptographyKey = false;

    // 检查是否是Cryptography键下的枚举
    ULONG keyNameSize = 0;
    NTSTATUS keyStatus = NtQueryKey(KeyHandle, KeyNameInformation, nullptr, 0, &keyNameSize);

    if (keyStatus == STATUS_BUFFER_TOO_SMALL || keyStatus == STATUS_BUFFER_OVERFLOW) {
      std::vector<BYTE> buffer(keyNameSize);
      PKEY_NAME_INFORMATION keyNameInfo = reinterpret_cast<PKEY_NAME_INFORMATION>(buffer.data());

      keyStatus = NtQueryKey(KeyHandle, KeyNameInformation, keyNameInfo, keyNameSize, &keyNameSize);
      if (NT_SUCCESS(keyStatus)) {
        // 检查是否目标键路径
        const WCHAR cryptoPath[] = L"\\REGISTRY\\MACHINE\\SOFTWARE\\MICROSOFT\\CRYPTOGRAPHY";

        std::wstring keyPath(keyNameInfo->Name, keyNameInfo->NameLength / sizeof(WCHAR));
        std::wstring keyPathUpper = keyPath;
        std::transform(keyPathUpper.begin(), keyPathUpper.end(), keyPathUpper.begin(), ::towupper);

        if (keyPathUpper.find(cryptoPath) != std::wstring::npos) {
          isCryptographyKey = true;
        }
      }
    }

    // 如果是目标键路径，检查当前枚举的值是否为MachineGuid
    if (isCryptographyKey && KeyValueInformation != nullptr) {
      switch (KeyValueInformationClass) {
        case KeyValueBasicInformation: {
          PKEY_VALUE_BASIC_INFORMATION basicInfo =
              static_cast<PKEY_VALUE_BASIC_INFORMATION>(KeyValueInformation);
          if (basicInfo->NameLength >= 11 * sizeof(WCHAR)) {  // "MachineGuid"长度
            if (_wcsnicmp(basicInfo->Name, L"MachineGuid", 11) == 0) {
              isMachineGuid = true;
            }
          }
          break;
        }
        case KeyValueFullInformation: {
          PKEY_VALUE_FULL_INFORMATION fullInfo =
              static_cast<PKEY_VALUE_FULL_INFORMATION>(KeyValueInformation);
          if (fullInfo->NameLength >= 11 * sizeof(WCHAR)) {
            if (_wcsnicmp(fullInfo->Name, L"MachineGuid", 11) == 0) {
              isMachineGuid = true;

              // 处理替换值
              const std::wstring& customGuid = ConfigMgr::_ins_().MachineGuid;
              ULONG dataLength = (customGuid.length() + 1) * sizeof(WCHAR);

              // 检查缓冲区是否足够
              if (Length >= fullInfo->DataOffset + dataLength) {
                // 更新数据长度
                fullInfo->DataLength = dataLength;
                fullInfo->Type = REG_SZ;

                // 复制自定义GUID数据
                memcpy((BYTE*)fullInfo + fullInfo->DataOffset, customGuid.c_str(), dataLength);

                // 更新结果长度
                *ResultLength = fullInfo->DataOffset + dataLength;
              }
            }
          }
          break;
        }
        case KeyValuePartialInformation: {
          // Partial信息没有名称，我们需要额外查询以确定当前枚举的值
          UNICODE_STRING valueName;
          valueName.Buffer = NULL;
          valueName.Length = 0;
          valueName.MaximumLength = 0;

          // 先获取值名称大小
          ULONG nameLength = 0;
          NTSTATUS nameStatus =
              NtEnumerateValueKey(KeyHandle, Index, KeyValueBasicInformation, NULL, 0, &nameLength);

          if (nameStatus == STATUS_BUFFER_TOO_SMALL || nameStatus == STATUS_BUFFER_OVERFLOW) {
            std::vector<BYTE> nameBuffer(nameLength);
            PKEY_VALUE_BASIC_INFORMATION nameInfo =
                reinterpret_cast<PKEY_VALUE_BASIC_INFORMATION>(nameBuffer.data());

            nameStatus = NtEnumerateValueKey(
                KeyHandle, Index, KeyValueBasicInformation, nameInfo, nameLength, &nameLength
            );

            if (NT_SUCCESS(nameStatus)) {
              if (nameInfo->NameLength >= 11 * sizeof(WCHAR)) {
                if (_wcsnicmp(nameInfo->Name, L"MachineGuid", 11) == 0) {
                  isMachineGuid = true;

                  // 处理PartialInformation替换
                  PKEY_VALUE_PARTIAL_INFORMATION partialInfo =
                      static_cast<PKEY_VALUE_PARTIAL_INFORMATION>(KeyValueInformation);
                  const std::wstring& customGuid = ConfigMgr::_ins_().MachineGuid;
                  ULONG dataLength = (customGuid.length() + 1) * sizeof(WCHAR);

                  // 检查缓冲区是否足够
                  if (Length >= FIELD_OFFSET(KEY_VALUE_PARTIAL_INFORMATION, Data) + dataLength) {
                    // 更新数据长度和类型
                    partialInfo->DataLength = dataLength;
                    partialInfo->Type = REG_SZ;

                    // 复制自定义GUID数据
                    memcpy(partialInfo->Data, customGuid.c_str(), dataLength);

                    // 更新结果长度
                    *ResultLength = FIELD_OFFSET(KEY_VALUE_PARTIAL_INFORMATION, Data) + dataLength;
                  }
                }
              }
            }
          }
          break;
        }
      }
    }
  }

  return status;
}
void setHook() {
  DetoursHooker hooker;
  hooker.endeque({

      {&NtQueryValueKey_raw, &NtQueryValueKey_mod},
      {&NtEnumerateValueKey_raw, &NtEnumerateValueKey_mod},
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