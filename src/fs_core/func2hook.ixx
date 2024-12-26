module;
#include <stdint.h>
#include <ntdll.h>

// #include <NTLib.h>
export module func2hook;

import PathJudge;

import std;
// 确保ObjectAttributes->ObjectName->Buffer的内存在此作用域内有效
// UNICODE_STRING效果同上
#define procRedirect()                                                     \
  std::wstring new_path_buffer;                                            \
  UNICODE_STRING new_path;                                                 \
  auto is_redirected = judgeAndRedirect(ObjectAttributes, new_path_buffer, \
                                        new_path, __FUNCTION__)

// bool judgeAndRedirect(POBJECT_ATTRIBUTES old_PA, UNICODE_STRING* _new_path,
// const char* func_name = nullptr)
//{
//	wchar_t buffer[MAX_PATH];
//	bool is_redirected =
// path_judge->judgeAndRedirect(old_PA->ObjectName->Buffer, buffer,
//(int)std::size(buffer)); 	return is_redirected;
// }
//  初始化 UNICODE_STRING
void InitUnicodeString(PUNICODE_STRING DestinationString, PCWSTR SourceString) {
  if (SourceString) {
    DestinationString->Length = (USHORT)wcslen(SourceString) * sizeof(WCHAR);
    DestinationString->MaximumLength =
        DestinationString->Length + sizeof(WCHAR);
    DestinationString->Buffer = (PWSTR)SourceString;
  } else {
    DestinationString->Length = 0;
    DestinationString->MaximumLength = 0;
    DestinationString->Buffer = NULL;
  }
}

bool doRedirect(const POBJECT_ATTRIBUTES OA,
                POBJECT_ATTRIBUTES _objAtrr_new,
                UNICODE_STRING* _filePath,
                wchar_t* _buffer,
                int len) {
  PathJudge* path_judge = PathJudge::_ins_();
  if (!path_judge)
    return false;
  if (!OA || !(OA->ObjectName) || !OA->ObjectName->Buffer)
    return false;

  bool ret = false;
  if (path_judge->judgeAndRedirect(OA->ObjectName->Buffer, _buffer, len)) {
    // gen a new OBJECT_ATTRIBUTE;
    // 初始化 UNICODE_STRING
    InitUnicodeString(_filePath, _buffer);

    // 初始化 OBJECT_ATTRIBUTES
    InitializeObjectAttributes(
        _objAtrr_new,  // ObjectAttributes
        _filePath,     // ObjectName
        OA->Attributes, OA->RootDirectory,
        OA->SecurityDescriptor  // TODO:
                                // OBJ_CASE_INSENSITIVE,  // Attributes
                                // NULL,                  // RootDirectory
                                // NULL                   // SecurityDescriptor
    );
    ret = true;
  }
  return ret;
}
#define redirectIt                                                  \
  OBJECT_ATTRIBUTES objAtrr_new;                                    \
  UNICODE_STRING filePath;                                          \
  wchar_t buffer[MAX_PATH + 1];                                     \
  if (doRedirect(ObjectAttributes, &objAtrr_new, &filePath, buffer, \
                 (int)std::size(buffer))) {                         \
    ObjectAttributes = &objAtrr_new;                                \
  };

export {
  decltype(&NtCreateFile) NtCreateFile_raw = &NtCreateFile;

  auto NTAPI NtCreateFile_mod(
      PHANDLE FileHandle, ACCESS_MASK DesiredAccess,
      POBJECT_ATTRIBUTES ObjectAttributes, PIO_STATUS_BLOCK IoStatusBlock,
      PLARGE_INTEGER AllocationSize, ULONG FileAttributes, ULONG ShareAccess,
      ULONG CreateDisposition, ULONG CreateOptions, PVOID EaBuffer,
      ULONG EaLength) {
    redirectIt;

    auto ret = NtCreateFile_raw(FileHandle, DesiredAccess, ObjectAttributes,
                                IoStatusBlock, AllocationSize, FileAttributes,
                                ShareAccess, CreateDisposition, CreateOptions,
                                EaBuffer, EaLength);

    return ret;
  };
  decltype(&NtOpenFile) NtOpenFile_raw = &NtOpenFile;

  auto NTAPI NtOpenFile_mod(PHANDLE FileHandle, ACCESS_MASK DesiredAccess,
                            POBJECT_ATTRIBUTES ObjectAttributes,
                            PIO_STATUS_BLOCK IoStatusBlock, ULONG ShareAccess,
                            ULONG OpenOptions) {
    redirectIt;

    NTSTATUS ret = NtOpenFile_raw(FileHandle, DesiredAccess, ObjectAttributes,
                                  IoStatusBlock, ShareAccess, OpenOptions);

    return ret;
  };
  decltype(&NtOpenSymbolicLinkObject) NtOpenSymbolicLinkObject_raw =
      &NtOpenSymbolicLinkObject;

  auto NTAPI NtOpenSymbolicLinkObject_mod(PHANDLE LinkHandle,
                                          ACCESS_MASK DesiredAccess,
                                          POBJECT_ATTRIBUTES ObjectAttributes) {
    redirectIt;

    NTSTATUS ret = NtOpenSymbolicLinkObject_raw(LinkHandle, DesiredAccess,
                                                ObjectAttributes);

    return ret;
  };
  decltype(&NtDeleteFile) NtDeleteFile_raw = &NtDeleteFile;

  auto NTAPI NtDeleteFile_mod(POBJECT_ATTRIBUTES ObjectAttributes) {
    redirectIt;
    return NtDeleteFile_raw(ObjectAttributes);
  };
  // decltype(&NtDeviceIoControlFile)

  decltype(&NtQueryAttributesFile) NtQueryAttributesFile_raw =
      &NtQueryAttributesFile;

  auto NTAPI NtQueryAttributesFile_mod(
      POBJECT_ATTRIBUTES ObjectAttributes,
      PFILE_BASIC_INFORMATION FileInformation) {
    redirectIt;

    return NtQueryAttributesFile_raw(ObjectAttributes, FileInformation);
  };
  decltype(&NtQueryFullAttributesFile) NtQueryFullAttributesFile_raw =
      &NtQueryFullAttributesFile;
  auto NTAPI NtQueryFullAttributesFile_mod(
      POBJECT_ATTRIBUTES ObjectAttributes,
      PFILE_NETWORK_OPEN_INFORMATION FileInformation) {
    redirectIt;
    return NtQueryFullAttributesFile_raw(ObjectAttributes, FileInformation);
  }

  NTSTATUS NtCreatePort(PHANDLE PortHandle, POBJECT_ATTRIBUTES ObjectAttributes,
                        ULONG MaxConnectionInfoLength, ULONG MaxMessageLength,
                        ULONG MaxPoolUsage);
  decltype(&NtCreatePort) NtCreatePort_raw = [] {
    auto ret = (decltype(&NtCreatePort))GetProcAddress(
        LoadLibraryA("ntdll.dll"), "NtCreatePort");
    return ret;
  }();
  auto NTAPI NtCreatePort_mod(PHANDLE PortHandle,
                              POBJECT_ATTRIBUTES ObjectAttributes,
                              ULONG MaxConnectionInfoLength,
                              ULONG MaxMessageLength, ULONG MaxPoolUsage) {
    redirectIt;
    return NtCreatePort_raw(PortHandle, ObjectAttributes,
                            MaxConnectionInfoLength, MaxMessageLength,
                            MaxPoolUsage);
  }
  decltype(&NtCreateNamedPipeFile) NtCreateNamedPipeFile_raw =
      &NtCreateNamedPipeFile;
  auto NTAPI NtCreateNamedPipeFile_mod(
      PHANDLE FileHandle, ULONG DesiredAccess,
      POBJECT_ATTRIBUTES ObjectAttributes, PIO_STATUS_BLOCK IoStatusBlock,
      ULONG ShareAccess, ULONG CreateDisposition, ULONG CreateOptions,
      ULONG NamedPipeType, ULONG ReadMode, ULONG CompletionMode,
      ULONG MaximumInstances, ULONG InboundQuota, ULONG OutboundQuota,
      PLARGE_INTEGER DefaultTimeout) {
    redirectIt;
    return NtCreateNamedPipeFile_raw(
        FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, ShareAccess,
        CreateDisposition, CreateOptions, NamedPipeType, ReadMode,
        CompletionMode, MaximumInstances, InboundQuota, OutboundQuota,
        DefaultTimeout);
  }
  // decltype(&NtSetInformationFile) NtSetInformationFile_raw =
  //     &NtSetInformationFile;
  // struct FILE_LINK_INFORMATION {
  //   BOOLEAN ReplaceIfExists;
  //   HANDLE RootDirectory;
  //   ULONG FileNameLength;
  //   WCHAR FileName[1];
  // };
  // NTSTATUS NTAPI NtSetInformationFile_mod(HANDLE FileHandle,
  //     PIO_STATUS_BLOCK IoStatusBlock, PVOID FileInformation, ULONG Length,
  //     FILE_INFORMATION_CLASS FileInformationClass) {
  //   FILE_RENAME_INFO* FRI_new = nullptr;
  //   FILE_LINK_INFORMATION* FLI_new = nullptr;
  //   wchar_t path_old[MAX_PATH + 1];
  //   wchar_t path_new[MAX_PATH + 1];

  //  // rename file
  //  if (FileInformationClass == FileRenameInformation) {
  //    FILE_RENAME_INFO* FRI = (FILE_RENAME_INFO*)FileInformation;
  //    memcpy(path_old, FRI->FileName, FRI->FileNameLength);
  //    uint8_t* p = (uint8_t*)path_old;
  //    p[FRI->FileNameLength] = 0;
  //    p[FRI->FileNameLength + 1] = 0;
  //    if (PathJudge::_ins_()->judgeAndRedirect(
  //            path_old, path_new, (int)std::size(path_new))) {
  //      auto path_len = wcslen(path_new);
  //      uint32_t path_byte_num = uint32_t(path_len + 1) * sizeof(wchar_t);

  //      FRI_new = (FILE_RENAME_INFO*)new uint8_t[sizeof(FILE_RENAME_INFO) +
  //                                               path_byte_num];
  //      *FRI_new = *FRI;
  //      FRI_new->FileNameLength = path_byte_num;
  //      memcpy(FRI_new->FileName, path_new, path_byte_num);
  //      FRI_new->FileName[path_len] = 0;  // add a '\0' at the end
  //    }
  //  } else if (FileInformationClass == FileLinkInformation) {
  //    FILE_LINK_INFORMATION* FLI = (FILE_LINK_INFORMATION*)FileInformation;
  //    memcpy(path_old, FLI->FileName, FLI->FileNameLength);
  //    uint8_t* p = (uint8_t*)path_old;
  //    p[FLI->FileNameLength] = 0;
  //    p[FLI->FileNameLength + 1] = 0;
  //    if (PathJudge::_ins_()->judgeAndRedirect(
  //            path_old, path_new, (int)std::size(path_new))) {
  //      auto path_len = wcslen(path_new);
  //      uint32_t path_byte_num = uint32_t(path_len + 1) * sizeof(wchar_t);

  //      FLI_new = (FILE_LINK_INFORMATION*)new uint8_t[sizeof(FILE_RENAME_INFO)
  //      +
  //                                                    path_byte_num];
  //      *FLI_new = *FLI;
  //      FLI_new->FileNameLength = path_byte_num;
  //      memcpy(FLI_new->FileName, path_new, path_byte_num);
  //      FLI_new->FileName[path_len] = 0;
  //    }
  //  }

  //  if (FRI_new)  // not nullptr indicates it was redirected
  //  {
  //    FileInformation = FRI_new;
  //  }
  //  if (FLI_new)
  //    FileInformation = FLI_new;
  //  auto ret = NtSetInformationFile_raw(FileHandle, IoStatusBlock,
  //      FileInformation, Length, FileInformationClass);
  //  delete FRI_new;
  //  delete FLI_new;

  //  return ret;
  //}
  //
}
