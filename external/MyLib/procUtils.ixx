module;

#include <string>
#include <windows.h>
#include <TlHelp32.h>
#include <vector>
export module procUtils;

using namespace std;
export {
	struct ProcessInf
	{
		string name;
		int pid;
		string path;
	};
	vector<ProcessInf> getProcessInf()
	{
		vector<ProcessInf> proccess_infs;
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		PROCESSENTRY32 pe;
		pe.dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(hSnapshot, &pe))
		{
			do
			{
				auto result = strchr(pe.szExeFile, '\\');
				proccess_infs.emplace_back(result?result:pe.szExeFile, pe.th32ProcessID, pe.szExeFile);
			} while (Process32Next(hSnapshot, &pe));
		}
		return proccess_infs;
	}
	bool isProcAlive(const string& exe_name)
	{
		bool alive = false;
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		PROCESSENTRY32 pe;
		pe.dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(hSnapshot, &pe))
		{
			do
			{
				if (string_view(pe.szExeFile).ends_with(exe_name))
				{
					alive = true;
					break;
				}
			} while (Process32Next(hSnapshot, &pe));
		}
		return alive;
	}
	void killProcess(const string& img_path)
	{
		string cmdline = "taskkill /f /im" + img_path;
		system(cmdline.c_str());
	}

	void killProcess(int pid)
	{
		string cmdline = "taskkill /f /pid" + to_string(pid);
		system(cmdline.c_str());
	}
	bool createProcess(const string_view& cmdline) {
		// 初始化 STARTUPINFO 结构体和 PROCESS_INFORMATION 结构体
		STARTUPINFO si{ 0 };
		PROCESS_INFORMATION pi{ 0 };
		si.cb = sizeof(si);

		// 创建进程
		if (!CreateProcessA(
			nullptr,           // 不指定模块名，使用命令行
			(LPSTR)cmdline.data(),  // 命令行
			nullptr,           // 进程句柄不可继承
			nullptr,           // 线程句柄不可继承
			FALSE,          // 句柄继承选项
			0,              // 没有创建标志
			nullptr,           // 使用父进程的环境块
			nullptr,           // 使用父进程的起始目录
			&si,            // 指向 STARTUPINFO 结构的指针
			&pi)            // 指向 PROCESS_INFORMATION 结构的指针
			) {
			printf("CreateProcess failed (%d).\n", GetLastError());
			return false;
		}

		// 关闭进程和线程句柄
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);


		return true;
	}
	void detachExecCmd(const string& cmdline) {
		//create
	}
}
