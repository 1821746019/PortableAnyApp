// #include <spdlog/spdlog.h>
#include <Windows.h>
#include <ntdll.h>
// #include <cstdio>
import std;
import reg_common;
#pragma comment(lib, "winmm.lib")
using namespace std;

string loadText(const string& filePath) {
  ifstream ifs(filePath);
  return string(istreambuf_iterator<char>(ifs), istreambuf_iterator<char>());
}
HKEY getCachedAppHiveRootKey() {
  // return AppHiveMgr::_ins_().hKey();

  static HKEY ret = [] {
    HKEY ret;
    auto status = RegLoadAppKeyW(
        LR"(D:\_tmp\RegisterFinder\MachineGUID_AppRegHive)", &ret, KEY_ALL_ACCESS, REG_PROCESS_APPKEY, 0
    );
    println("{}", (void*)ret);

    if (status != ERROR_SUCCESS) {
      throw std::runtime_error(
          "failed to load AppRegHive. Please check if it exists in the same "
          "dir of the dll"
      );
    }
    getchar();
    return ret;
  }();
  return ret;
};
wstring getAppRegHiveRegPath() {
  static wstring ret = GetKeyPath(getCachedAppHiveRootKey());
  return ret;
};

int test() {
  UNICODE_STRING KeyPath;
  UNICODE_STRING ValueName;
  OBJECT_ATTRIBUTES ObjectAttributes;
  HANDLE KeyHandle = NULL;
  NTSTATUS Status;
  ULONG ResultLength = 0;
  PKEY_VALUE_PARTIAL_INFORMATION KeyInfo = NULL;

  // 初始化注册表路径
  RtlInitUnicodeString(&KeyPath, (PWSTR)L"\\Registry\\Machine\\SOFTWARE\\Microsoft\\Cryptography");

  // 初始化对象属性
  InitializeObjectAttributes(&ObjectAttributes, &KeyPath, OBJ_CASE_INSENSITIVE, NULL, NULL);

  // 打开注册表键
  Status = NtOpenKey(&KeyHandle, KEY_READ, &ObjectAttributes);
  if (!NT_SUCCESS(Status)) {
    printf("打开注册表键失败. 状态码: 0x%08X\n", Status);
    return 1;
  }

  // 初始化值名称
  RtlInitUnicodeString(&ValueName, (PWSTR)L"MachineGuid");

  // 查询值信息大小
  Status = NtQueryValueKey(KeyHandle, &ValueName, KeyValuePartialInformation, NULL, 0, &ResultLength);
  if (Status != STATUS_BUFFER_TOO_SMALL && Status != STATUS_BUFFER_OVERFLOW) {
    printf("查询值大小失败. 状态码: 0x%08X\n", Status);
    NtClose(KeyHandle);
    return 1;
  }

  // 分配内存
  KeyInfo = (PKEY_VALUE_PARTIAL_INFORMATION)malloc(ResultLength);
  if (!KeyInfo) {
    printf("内存分配失败\n");
    NtClose(KeyHandle);
    return 1;
  }

  // 清空内存
  memset(KeyInfo, 0, ResultLength);

  // 查询值信息
  Status = NtQueryValueKey(
      KeyHandle, &ValueName, KeyValuePartialInformation, KeyInfo, ResultLength, &ResultLength
  );
  if (!NT_SUCCESS(Status)) {
    printf("查询值信息失败. 状态码: 0x%08X\n", Status);
    free(KeyInfo);
    NtClose(KeyHandle);
    return 1;
  }

  // 检查值类型并处理数据
  if (KeyInfo->Type == REG_SZ) {
    // 计算字符串长度（以字节为单位）
    ULONG stringLength = KeyInfo->DataLength;

    // 检查字符串是否已以NULL终止
    BOOL isNullTerminated = FALSE;
    if (stringLength >= 2) {  // 至少需要2个字节才能包含一个空的宽字符
      WCHAR* lastChar = (WCHAR*)(KeyInfo->Data + stringLength - 2);
      isNullTerminated = (*lastChar == L'\0');
    }

    // 创建一个临时缓冲区来存储字符串
    WCHAR* tempBuffer = NULL;

    if (isNullTerminated) {
      // 字符串已经以NULL终止，直接使用
      tempBuffer = (WCHAR*)KeyInfo->Data;
    } else {
      // 字符串未以NULL终止，创建一个新的终止字符串
      tempBuffer = (WCHAR*)malloc(stringLength + sizeof(WCHAR));
      if (!tempBuffer) {
        printf("内存分配失败\n");
        free(KeyInfo);
        NtClose(KeyHandle);
        return 1;
      }

      memcpy(tempBuffer, KeyInfo->Data, stringLength);
      tempBuffer[stringLength / sizeof(WCHAR)] = L'\0';  // 添加NULL终止符
    }

    // 打印GUID
    wprintf(L"MachineGuid: %s\n", tempBuffer);

    // 如果创建了新的缓冲区，则释放它
    if (!isNullTerminated && tempBuffer) {
      free(tempBuffer);
    }
  } else {
    printf("MachineGuid不是字符串值 (类型: %d)\n", KeyInfo->Type);
  }

  // 清理
  free(KeyInfo);
  NtClose(KeyHandle);

  return 0;
}
int main() {
  test();
  getchar();
  // NtOpenKey()
  //{
  //   filesystem::path dll_path =
  //       R"(D:\Users\Administrator\Desktop\myCode\_CMake\PortableAnyApp\build\x64-debug\src\ext\allocConsole.dll)";

  //   // putenv(format("LOADED_DLL={}", dll_path.string()).data());
  //   LoadLibraryW(dll_path.c_str());
  //   getchar();

  //   return 0;
  //}
  timeBeginPeriod(0);
  filesystem::path dll_path =
      R"(D:\Users\Administrator\Desktop\myCode\_CMake\PortableAnyApp\build\x64-debug\src\uniLauncher\extBoot.dll)";

  // putenv(format("LOADED_DLL={}", dll_path.string()).data());
  LoadLibraryW(dll_path.c_str());
  system("cmd");
  string line;
  while (getline(cin, line)) {
    system(line.data());
  }

  // getchar();
  getchar();
}