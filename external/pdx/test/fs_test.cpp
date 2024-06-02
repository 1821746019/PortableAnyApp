#include <windows.h>
#include <shobjidl.h> 
#include <vector>
#include <string>
#include <iostream>
#include <ntdll.h>

void linkVersion_Winmm()
{
	timeBeginPeriod(0);
	GetFileVersionInfoA(nullptr, 0, 0, nullptr);
}



std::vector<std::string> SelectFiles(const std::string* initialPath=nullptr) {
    // 初始化COM库
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    // 创建文件对话框
    IFileOpenDialog* pFileOpen;
    HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
        IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

    std::vector<std::string> selectedFiles;

    if (SUCCEEDED(hr)) {
        // 设置对话框属性（多选）
        DWORD dwOptions;
        pFileOpen->GetOptions(&dwOptions);
        pFileOpen->SetOptions(dwOptions | FOS_ALLOWMULTISELECT);

        // 设置初始目录
        if (initialPath&&!initialPath->empty()) {
            IShellItem* psiFolder;
            hr = SHCreateItemFromParsingName(std::wstring(initialPath->begin(), initialPath->end()).c_str(), NULL, IID_PPV_ARGS(&psiFolder));
            if (SUCCEEDED(hr)) {
                pFileOpen->SetFolder(psiFolder);
                psiFolder->Release();
            }
        }

        // 显示对话框
        hr = pFileOpen->Show(NULL);

        if (SUCCEEDED(hr)) {
            // 获取用户选择的文件
            IShellItemArray* pItems;
            hr = pFileOpen->GetResults(&pItems);
            if (SUCCEEDED(hr)) {
                DWORD count;
                pItems->GetCount(&count);
                for (DWORD i = 0; i < count; i++) {
                    IShellItem* pItem;
                    hr = pItems->GetItemAt(i, &pItem);
                    if (SUCCEEDED(hr)) {
                        PWSTR pszFilePath;
                        pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                        // 将宽字符路径转换为标准字符串并添加到列表
                        std::wstring ws(pszFilePath);
                        std::string fullPath(ws.begin(), ws.end());
                        selectedFiles.push_back(fullPath);

                        CoTaskMemFree(pszFilePath);
                        pItem->Release();
                    }
                }
                pItems->Release();
            }
        }
        pFileOpen->Release();
    }
    CoUninitialize();
    return selectedFiles;
}
void RedirectPath() {
    // 定义原始和目标路径
    WCHAR originalPathBuffer[] = L"\\??\\C:\\dst";
    WCHAR newPathBuffer[] = L"\\??\\D:\\dst";

    // 初始化 UNICODE_STRING 结构
    UNICODE_STRING originalPath;
    UNICODE_STRING newPath;
    RtlInitUnicodeString(&originalPath, originalPathBuffer);
    RtlInitUnicodeString(&newPath, newPathBuffer);

    // 准备调用 RtlDosApplyFileIsolationRedirection_Ustr
    UNICODE_STRING* redirectedPath = NULL;
    IO_STATUS_BLOCK ioStatusBlock;

    NTSTATUS status = RtlDosApplyFileIsolationRedirection_Ustr(
        RTL_DOS_APPLY_FILE_REDIRECTION_USTR_FLAG_REDIRECT_IF_NECESSARY, // Flags
        &originalPath, // 原始路径
        NULL, // 没有特定的扩展
        NULL, // 静态字符串不使用
        &newPath, // 动态字符串
        &redirectedPath, // 输出：重定向后的路径
        &ioStatusBlock, // IO状态块
        NULL, // 文件句柄（未使用）
        NULL  // 预留参数
    );

    // 检查状态并处理结果
    if (status == STATUS_SUCCESS) {
        wprintf(L"Redirection successful: %s\n", redirectedPath->Buffer);
    }
    else {
        wprintf(L"Redirection failed with status: %08x\n", status);
    }

    // 清理
    if (redirectedPath != NULL) {
        RtlFreeUnicodeString(redirectedPath);
    }
}


int main() {

    getchar();
	ShowWindow(GetConsoleWindow(), SW_HIDE);
	std::string path = getenv("USERPROFILE");
	//path += "\\AppData";

	while (true)
	{
		std::vector<std::string> files = SelectFiles(&path);
		for (const auto& file : files) {
			std::cout << "Selected file: " << file << std::endl;
		}

		std::cout << "\nenter q to quit, others to continue..." << '\n';
		//char c = getchar();
		//if(c=='q')break;

	}
	return 0;
}

BOOL WinMain(HMODULE hModule,HMODULE parent,LPSTR cmdline,int show_cmd)
{
	main();
}
