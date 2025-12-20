module;
#include <ntdll.h>
export module func2hook;

import std;
import AppRegHive;
import my_converter.str;
import log;
import reg_common;
import RegGuardConfig;
import _common.redirect;
import strUtils;
#define regGuardConfig RegGuardConfig::_ins_()
// 前缀不是\REGISTRY, 不作重定向
#define redirectIt                                                                                          \
  wchar_t path[int(MAX_PATH * 1.5)];                                                                        \
  wcscpy_s(path, getAppRegHiveRegPath().c_str());                                                           \
  wcscpy_s(                                                                                                 \
      path + getAppRegHiveRegPath().size(), MAX_PATH, ObjectAttributes->ObjectName->Buffer + REG_PREFIX_LEN \
  );                                                                                                        \
  OBJECT_ATTRIBUTES OA_new = *ObjectAttributes;                                                             \
  OA_new.ObjectName->Buffer = path;                                                                         \
  OA_new.ObjectName->Length = getAppRegHiveRegPath().size() + ObjectAttributes->ObjectName->Length;         \
  OA_new.ObjectName->MaximumLength = OA_new.ObjectName->Length

bool isNeedBlocking(const std::wstring& path) {
  std::wstring toBlock = LR"(Software\Microsoft\Cryptography)";
  if (wcsistr(path.data(), toBlock.data())) {
    return true;
  }
  return false;
}
std::string TO_FIND = R"(9f79a9d3-d029-4489-b59a-b382f2b14351)";
// 检查数据中是否包含目标字符串
void CheckAndPrintIfContains(ULONG dataType, PVOID data, ULONG dataLength) {
  if (!data || dataLength == 0)
    return;

  // 只检查字符串类型的注册表值
  if (dataType == REG_SZ || dataType == REG_EXPAND_SZ || dataType == REG_MULTI_SZ) {
    try {
      if (dataType == REG_MULTI_SZ) {
        // 处理多字符串
        const wchar_t* currentStr = (const wchar_t*)data;
        const wchar_t* endPtr = (const wchar_t*)((BYTE*)data + dataLength);

        while (currentStr < endPtr && *currentStr != L'\0') {
          std::wstring wstr(currentStr);
          std::string str(wstr.begin(), wstr.end());

          if (str.find(TO_FIND) != std::string::npos) {
            std::wcout << L"[FOUND] Multi-string contains target: " << wstr << std::endl;
          }

          currentStr += wstr.length() + 1;  // 移动到下一个字符串
        }
      } else {
        // 处理单字符串 (REG_SZ, REG_EXPAND_SZ)
        size_t wcharCount = dataLength / sizeof(wchar_t);
        if (wcharCount > 0) {
          std::wstring wstr((const wchar_t*)data, wcharCount);

          // 移除可能的null终结符
          if (!wstr.empty() && wstr.back() == L'\0') {
            wstr.pop_back();
          }

          std::string str(wstr.begin(), wstr.end());

          if (str.find(TO_FIND) != std::string::npos) {
            std::wcout << L"[FOUND] String contains target: " << wstr << std::endl;
          }
        }
      }
    } catch (...) {
      // 忽略转换错误
    }
  }
}
export {
  decltype(&NtOpenKey) NtOpenKey_raw =
      (decltype(&NtOpenKey))GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtOpenKey");
  ;
  auto NTAPI NtOpenKey_mod(
      PHANDLE KeyHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes
  ) {
    if (!ObjectAttributes->ObjectName->Buffer || 0
        //_wcsnicmp(REG_PREFIX, ObjectAttributes->ObjectName->Buffer, REG_PREFIX_LEN) != 0
    )
      return NtOpenKey_raw(KeyHandle, DesiredAccess, ObjectAttributes);
    std::wstring path_old = abstractNonRootFromAbsRegPath(ObjectAttributes->ObjectName->Buffer);
    std::wstring path_old_full = path_old;
    bool isIncluded = isInIncluded(path_old_full);
    if (isNeedBlocking(ObjectAttributes->ObjectName->Buffer)) {
      // 返回打开失败
      return STATUS_ACCESS_DENIED;
    }
    if (isNeedRedirection(path_old_full)) {
      wchar_t path[int(MAX_PATH * 2)];
      wcscpy_s(path, (getAppRegHiveRegPath() + path_old_full).data());
      OBJECT_ATTRIBUTES OA_new = *ObjectAttributes;
      OA_new.ObjectName->Buffer = path;
      OA_new.ObjectName->Length = getAppRegHiveRegPath().size() + ObjectAttributes->ObjectName->Length;
      OA_new.ObjectName->MaximumLength = OA_new.ObjectName->Length;

      return NtOpenKey_raw(KeyHandle, DesiredAccess, &OA_new);
    }

    NTSTATUS status = NtOpenKey_raw(KeyHandle, DesiredAccess, ObjectAttributes);

    return status;
  }
  decltype(&NtOpenKeyEx) NtOpenKeyEx_raw =
      (decltype(&NtOpenKeyEx))GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtOpenKeyEx");
  ;
  auto NTAPI NtOpenKeyEx_mod(
      PHANDLE KeyHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, ULONG OpenOptions
  ) {
    if (!ObjectAttributes->ObjectName->Buffer || 0
        /*_wcsnicmp(REG_PREFIX, ObjectAttributes->ObjectName->Buffer, REG_PREFIX_LEN) != 0*/
    )
      return NtOpenKeyEx_raw(KeyHandle, DesiredAccess, ObjectAttributes, OpenOptions);
    std::wstring path_old = abstractNonRootFromAbsRegPath(ObjectAttributes->ObjectName->Buffer);
    std::wstring path_old_full = path_old;
    bool isIncluded = isInIncluded(path_old_full);
    if (isNeedBlocking(ObjectAttributes->ObjectName->Buffer)) {
      // 返回打开失败
      return STATUS_ACCESS_DENIED;
    }
    if (isNeedRedirection(path_old_full)) {
      wchar_t path[int(MAX_PATH * 2)];
      wcscpy_s(path, (getAppRegHiveRegPath() + path_old_full).data());
      OBJECT_ATTRIBUTES OA_new = *ObjectAttributes;
      OA_new.ObjectName->Buffer = path;
      OA_new.ObjectName->Length = getAppRegHiveRegPath().size() + ObjectAttributes->ObjectName->Length;
      OA_new.ObjectName->MaximumLength = OA_new.ObjectName->Length;

      return NtOpenKeyEx_raw(KeyHandle, DesiredAccess, &OA_new, OpenOptions);
    }

    NTSTATUS status = NtOpenKeyEx_raw(KeyHandle, DesiredAccess, ObjectAttributes, OpenOptions);

    return status;
  }

  // decltype(&NtCreateKey) NtCreateKey_raw = &NtCreateKey;
  auto NtCreateKey_raw = (decltype(&NtCreateKey))GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtCreateKey");

  auto NTAPI NtCreateKey_mod(
      PHANDLE KeyHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, ULONG TitleIndex,
      PUNICODE_STRING Class, ULONG CreateOptions, PULONG Disposition
  ) {
    if (!ObjectAttributes->ObjectName->Buffer || 0
        /*_wcsnicmp(REG_PREFIX, ObjectAttributes->ObjectName->Buffer, REG_PREFIX_LEN) != 0*/
    )
      return NtCreateKey_raw(
          KeyHandle, DesiredAccess, ObjectAttributes, TitleIndex, Class, CreateOptions, Disposition
      );
    std::wstring path_old = abstractNonRootFromAbsRegPath(ObjectAttributes->ObjectName->Buffer);
    std::wstring path_old_full = path_old;
    bool isIncluded = isInIncluded(path_old_full);
    if (isNeedBlocking(ObjectAttributes->ObjectName->Buffer)) {
      return STATUS_ACCESS_DENIED;
    }
    if (isNeedRedirection(path_old_full)) {
      wchar_t path[int(MAX_PATH * 2)];
      wcscpy_s(path, (getAppRegHiveRegPath() + path_old_full).data());
      OBJECT_ATTRIBUTES OA_new = *ObjectAttributes;
      OA_new.ObjectName->Buffer = path;
      OA_new.ObjectName->Length = getAppRegHiveRegPath().size() + ObjectAttributes->ObjectName->Length;
      OA_new.ObjectName->MaximumLength = OA_new.ObjectName->Length;

      return NtCreateKey_raw(
          KeyHandle, DesiredAccess, &OA_new, TitleIndex, Class, CreateOptions, Disposition
      );
    }

    NTSTATUS status = NtCreateKey_raw(
        KeyHandle, DesiredAccess, ObjectAttributes, TitleIndex, Class, CreateOptions, Disposition
    );

    return status;
  }

  auto NtQueryValueKey_raw = &NtQueryValueKey;
  auto NTAPI NtQueryValueKey_mod(
      HANDLE KeyHandle, PUNICODE_STRING ValueName, KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
      PVOID KeyValueInformation, ULONG Length, PULONG ResultLength
  ) {
    //if (!ValueName->Buffer || _wcsnicmp(REG_PREFIX, ValueName->Buffer, REG_PREFIX_LEN) != 0)
    //  return NtQueryValueKey_raw(
    //      KeyHandle, ValueName, KeyValueInformationClass, KeyValueInformation, Length, ResultLength
    //  );

    NTSTATUS status = NtQueryValueKey_raw(
        KeyHandle, ValueName, KeyValueInformationClass, KeyValueInformation, Length, ResultLength
    );

    // 如果成功获取到数据，检查内容
    if (status == STATUS_SUCCESS && KeyValueInformation && Length > 0) {
      switch (KeyValueInformationClass) {
        case KeyValuePartialInformation: {
          PKEY_VALUE_PARTIAL_INFORMATION partialInfo = (PKEY_VALUE_PARTIAL_INFORMATION)KeyValueInformation;
          if (Length >= sizeof(KEY_VALUE_PARTIAL_INFORMATION) && partialInfo->DataLength > 0) {
            CheckAndPrintIfContains(partialInfo->Type, partialInfo->Data, partialInfo->DataLength);
          }
          break;
        }
        case KeyValueFullInformation: {
          PKEY_VALUE_FULL_INFORMATION fullInfo = (PKEY_VALUE_FULL_INFORMATION)KeyValueInformation;
          if (Length >= sizeof(KEY_VALUE_FULL_INFORMATION) && fullInfo->DataLength > 0) {
            PVOID data = (BYTE*)KeyValueInformation + fullInfo->DataOffset;
            CheckAndPrintIfContains(fullInfo->Type, data, fullInfo->DataLength);
          }
          break;
        }
        default:
          // BasicInformation不包含数据，只有名称和类型
          break;
      }
    }

    if (status != STATUS_SUCCESS) {
      status = NtQueryValueKey_raw(
          KeyHandle, ValueName, KeyValueInformationClass, KeyValueInformation, Length, ResultLength
      );
    }
    return status;
  }

  auto NtEnumerateValueKey_raw = &NtEnumerateValueKey;
  auto NTAPI NtEnumerateValueKey_mod(
      HANDLE KeyHandle, ULONG Index, KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
      PVOID KeyValueInformation, ULONG Length, PULONG ResultLength
  ) {
    NTSTATUS status = NtEnumerateValueKey_raw(
        KeyHandle, Index, KeyValueInformationClass, KeyValueInformation, Length, ResultLength
    );

    // 如果成功枚举到数据，检查内容
    if (status == STATUS_SUCCESS && KeyValueInformation && Length > 0) {
      switch (KeyValueInformationClass) {
        case KeyValuePartialInformation: {
          PKEY_VALUE_PARTIAL_INFORMATION partialInfo = (PKEY_VALUE_PARTIAL_INFORMATION)KeyValueInformation;
          if (Length >= sizeof(KEY_VALUE_PARTIAL_INFORMATION) && partialInfo->DataLength > 0) {
            CheckAndPrintIfContains(partialInfo->Type, partialInfo->Data, partialInfo->DataLength);
          }
          break;
        }
        case KeyValueFullInformation: {
          PKEY_VALUE_FULL_INFORMATION fullInfo = (PKEY_VALUE_FULL_INFORMATION)KeyValueInformation;
          if (Length >= sizeof(KEY_VALUE_FULL_INFORMATION) && fullInfo->DataLength > 0) {
            PVOID data = (BYTE*)KeyValueInformation + fullInfo->DataOffset;
            CheckAndPrintIfContains(fullInfo->Type, data, fullInfo->DataLength);
          }
          break;
        }
        default:
          // BasicInformation不包含数据，只有名称和类型
          break;
      }
    }

    return status;
  }
  /*
     auto RtlInitUnicodeString_raw = &RtlInitUnicodeString;
    auto RtlInitUnicodeString_mod(PUNICODE_STRING DestinationString,
                                  PWSTR SourceString) {
      if (_wcsnicmp(REG_PREFIX, SourceString, REG_PREFIX_LEN) == 0) {
        wcscpy_s(DestinationString->Buffer, MAX_PATH,
    AppRegHiveRegPath.c_str()); wcscpy_s(DestinationString->Buffer +
    AppRegHiveRegPath.size(), MAX_PATH, SourceString + REG_PREFIX_LEN);
        DestinationString->Length =
            AppRegHiveRegPath.size() + wcslen(SourceString + REG_PREFIX_LEN) *
    2; DestinationString->MaximumLength = DestinationString->Length + 2; } else
    { RtlInitUnicodeString_raw(DestinationString, SourceString);
      }
    }
    NTSTATUS NTAPI RtlInitUnicodeStringEx(PUNICODE_STRING DestinationString,
                                          PCWSTR SourceString);
    auto RtlInitUnicodeStringEx_raw =
        (decltype(&RtlInitUnicodeStringEx))GetProcAddress(
            LoadLibraryA("ntdll.dll"), "RtlInitUnicodeStringEx");

    auto RtlInitUnicodeStringEx_mod(PUNICODE_STRING DestinationString,
                                    PCWSTR SourceString) {
      if (_wcsnicmp(REG_PREFIX, SourceString, REG_PREFIX_LEN) == 0) {
        wcscpy_s(DestinationString->Buffer, MAX_PATH,
    AppRegHiveRegPath.c_str()); wcscpy_s(DestinationString->Buffer +
    AppRegHiveRegPath.size(), MAX_PATH, SourceString + REG_PREFIX_LEN);
        DestinationString->Length =
            AppRegHiveRegPath.size() + wcslen(SourceString + REG_PREFIX_LEN) *
    2; DestinationString->MaximumLength = DestinationString->Length + 2; } else
    { RtlInitUnicodeStringEx_raw(DestinationString, SourceString);
      }
    }
    */
}
