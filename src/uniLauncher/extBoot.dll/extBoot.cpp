import std;


#include <Windows.h>


using namespace std;
namespace fs = std::filesystem;
static fs::path selfPath;
//fs::path selfDir;



void loadAllDllExcludeUnderlinePrefix() {
  vector<fs::path> dllPath_list;
  for (auto& e : fs::directory_iterator(selfPath.parent_path() / "Ext")) {
    if (e.is_regular_file() && e.path().extension() == ".dll" &&
        !e.path().parent_path().filename().wstring().starts_with(L"_")) {
      dllPath_list.emplace_back(e);
    }
  }
  for (auto& e : dllPath_list) {
    LoadLibraryW(e.c_str());
    _putenv(format("LoadedExt={}",
                   reduce(dllPath_list.begin(), dllPath_list.end(), string(),
                          [](const fs::path& a, const fs::path& b) {
                            return a.string() + ";" + b.string();
                          }))
                .data());
  }
}
BOOL DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved) {
  if (dwReason == DLL_PROCESS_ATTACH) {
#ifdef BS_DBG
	  std::wcout << selfPath.wstring() << L'\n';
#endif

  } else if (dwReason == DLL_PROCESS_DETACH) {
  }
  return TRUE;
}
static struct Init {
  Init() {
    HMODULE hModule = nullptr;
    
    // 使用 GetModuleHandleExW 获取当前模块句柄
    if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           reinterpret_cast<LPCWSTR>(&DllMain), &hModule)) {
      wchar_t buf[MAX_PATH];
     GetModuleFileNameW(hModule, buf, size(buf));

      selfPath = fs::path(buf).parent_path();
    }
  }
} init;