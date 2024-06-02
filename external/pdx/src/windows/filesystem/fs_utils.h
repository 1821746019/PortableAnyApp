#pragma once
#include <stddef.h>
#include "../common/ntmin/ntmin.h" 


#define X_MAX_PATH 1024
#define FLAG_BYPASS 0x04000000

int path_exists(char* path);
int path_is_symlink(char* path);
void resolve_abspath(char* in_path, char* out_path, int follow_symlinks);
void makedir(char* path);
void delete_path(char* path);
void copy_file(char* src_path, char* dst_path);

// WinNT Only
int get_abspath_from_handle(void* hObject, wchar_t* in_path, wchar_t* out_path);


//struct POBJECT_ATTRIBUTES; struct PUNICODE_STRING;
typedef NTSTATUS __stdcall tNtCreateFile(PHANDLE FileHandle, DWORD DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, PIO_STATUS_BLOCK IoStatusBlock, PLARGE_INTEGER AllocationSize, ULONG FileAttributes, ULONG ShareAccess, ULONG CreateDisposition, ULONG CreateOptions, PVOID EaBuffer, ULONG EaLength);
typedef NTSTATUS __stdcall tNtOpenFile(PHANDLE FileHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, PIO_STATUS_BLOCK IoStatusBlock, ULONG ShareAccess, ULONG OpenOptions);
typedef NTSTATUS __stdcall tNtOpenDirectoryObject(PHANDLE DirectoryHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes);
typedef NTSTATUS __stdcall tNtCreateDirectoryObject(PHANDLE DirectoryHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes);
typedef NTSTATUS __stdcall tNtCreateDirectoryObjectEx(PHANDLE DirectoryHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, HANDLE ShadowDirectoryHandle, ULONG Flags);
typedef NTSTATUS __stdcall tNtQueryAttributesFile(POBJECT_ATTRIBUTES ObjectAttributes, PFILE_BASIC_INFORMATION FileInformation);
typedef NTSTATUS __stdcall tNtQueryFullAttributesFile(POBJECT_ATTRIBUTES ObjectAttributes, PVOID Attributes);
typedef NTSTATUS __stdcall tLdrLoadDll(PWCHAR PathToFile, PULONG Flags, PUNICODE_STRING ModuleFileName, PHANDLE ModuleHandle);
extern tNtCreateFile* ntdll_NtCreateFile;
extern tNtOpenFile* ntdll_NtOpenFile ;
extern tNtOpenDirectoryObject* ntdll_NtOpenDirectoryObject ;
extern tNtCreateDirectoryObject* ntdll_NtCreateDirectoryObject ;
extern tNtCreateDirectoryObjectEx* ntdll_NtCreateDirectoryObjectEx ;
extern tNtQueryAttributesFile* ntdll_NtQueryAttributesFile ;
extern tNtQueryFullAttributesFile* ntdll_NtQueryFullAttributesFile ;
extern tLdrLoadDll* ntdll_LdrLoadDll ;