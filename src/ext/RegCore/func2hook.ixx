module;
#include <ntdll.h>
export module func2hook;

import std;
import AppRegHive;
import my_converter.str;
import log;
import reg_common;

// 前缀不是\REGISTRY, 不作重定向
#define redirectIt                                                          \
  wchar_t path[int(MAX_PATH * 1.5)];                                        \
  wcscpy_s(path, getAppRegHiveRegPath().c_str());                           \
  wcscpy_s(path + getAppRegHiveRegPath().size(), MAX_PATH,                  \
           ObjectAttributes->ObjectName->Buffer + REG_PREFIX_LEN);          \
  OBJECT_ATTRIBUTES OA_new = *ObjectAttributes;                             \
  OA_new.ObjectName->Buffer = path;                                         \
  OA_new.ObjectName->Length =                                               \
      getAppRegHiveRegPath().size() + ObjectAttributes->ObjectName->Length; \
  OA_new.ObjectName->MaximumLength = OA_new.ObjectName->Length

export {

  decltype(&NtOpenKey) NtOpenKey_raw = &NtOpenKey;
  auto NTAPI NtOpenKey_mod(PHANDLE KeyHandle, ACCESS_MASK DesiredAccess,
                           POBJECT_ATTRIBUTES ObjectAttributes) {
    if (!ObjectAttributes->ObjectName->Buffer ||
        _wcsnicmp(REG_PREFIX, ObjectAttributes->ObjectName->Buffer,
                  REG_PREFIX_LEN) != 0)
      return NtOpenKey_raw(KeyHandle, DesiredAccess, ObjectAttributes);
    redirectIt;
    NTSTATUS status = NtOpenKey_raw(KeyHandle, DesiredAccess, &OA_new);
    if (status !=
        STATUS_SUCCESS) {  // 在AppRegHive里找不到, fallback to original
      status = NtOpenKey_raw(KeyHandle, DesiredAccess, ObjectAttributes);
    }
    return status;
  }
  decltype(&NtOpenKeyEx) NtOpenKeyEx_raw = &NtOpenKeyEx;
  auto NTAPI NtOpenKeyEx_mod(PHANDLE KeyHandle, ACCESS_MASK DesiredAccess,
                             POBJECT_ATTRIBUTES ObjectAttributes,
                             ULONG OpenOptions) {
    if (!ObjectAttributes->ObjectName->Buffer ||
        _wcsnicmp(REG_PREFIX, ObjectAttributes->ObjectName->Buffer,
                  REG_PREFIX_LEN) != 0)
      return NtOpenKeyEx_raw(KeyHandle, DesiredAccess, ObjectAttributes,
                             OpenOptions);
    redirectIt;
    NTSTATUS status =
        NtOpenKeyEx_raw(KeyHandle, DesiredAccess, &OA_new, OpenOptions);
    if (status != STATUS_SUCCESS) {
      status = NtOpenKeyEx_raw(KeyHandle, DesiredAccess, ObjectAttributes,
                               OpenOptions);
    }
    return status;
  }

  // decltype(&NtCreateKey) NtCreateKey_raw = &NtCreateKey;
  auto NtCreateKey_raw = (decltype(&NtCreateKey))GetProcAddress(
      LoadLibraryA("ntdll.dll"), "NtCreateKey");

  auto NTAPI NtCreateKey_mod(PHANDLE KeyHandle, ACCESS_MASK DesiredAccess,
                             POBJECT_ATTRIBUTES ObjectAttributes,
                             ULONG TitleIndex, PUNICODE_STRING Class,
                             ULONG CreateOptions, PULONG Disposition) {
    if (!ObjectAttributes->ObjectName->Buffer ||
        _wcsnicmp(REG_PREFIX, ObjectAttributes->ObjectName->Buffer,
                  REG_PREFIX_LEN) != 0)
      return NtCreateKey_raw(KeyHandle, DesiredAccess, ObjectAttributes,
                             TitleIndex, Class, CreateOptions, Disposition);
    redirectIt;
    NTSTATUS status =
        NtCreateKey_raw(KeyHandle, DesiredAccess, &OA_new, TitleIndex, Class,
                        CreateOptions, Disposition);
    if (status != STATUS_SUCCESS) {
      status = NtCreateKey_raw(KeyHandle, DesiredAccess, ObjectAttributes,
                               TitleIndex, Class, CreateOptions, Disposition);
    }
    return status;
  }
  auto NtQueryValueKey_raw = &NtQueryValueKey;

          auto NTAPI NtQueryValueKey_mod(
      HANDLE KeyHandle, PUNICODE_STRING ValueName,
      KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
      PVOID KeyValueInformation, ULONG Length, PULONG ResultLength) {
    if (!ValueName->Buffer ||
        _wcsnicmp(REG_PREFIX, ValueName->Buffer, REG_PREFIX_LEN) != 0)
      return NtQueryValueKey_raw(KeyHandle, ValueName, KeyValueInformationClass,
                                 KeyValueInformation, Length, ResultLength);
    //redirectIt;
    NTSTATUS status =
        NtQueryValueKey_raw(KeyHandle, ValueName, KeyValueInformationClass,
                            KeyValueInformation, Length, ResultLength);
    if (status != STATUS_SUCCESS) {
      status =
          NtQueryValueKey_raw(KeyHandle, ValueName, KeyValueInformationClass,
                              KeyValueInformation, Length, ResultLength);
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
