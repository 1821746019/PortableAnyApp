module;
#include <Windows.h>
#include <string>
export module hooked_funcs;
import CmdlineMgr;
import my_converter.str;
import shared;
using namespace std;


export {
	string exe_path_quoted_if_need = quotePathIfNeed(getNowExePath());
	using CreateProcessA_t = decltype(&CreateProcessA);
	CreateProcessA_t CreateProcessA_raw = &CreateProcessA;

	BOOL WINAPI
		CreateProcessA_mod
		(LPCSTR lpApplicationName, LPSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCSTR lpCurrentDirectory, LPSTARTUPINFOA lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation
		)
	{
		if (!string_view(lpCommandLine).starts_with(exe_path_quoted_if_need))
		{
			return CreateProcessA_raw(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
		}
		string cmdline_new;
		procCmdline(lpCommandLine, &cmdline_new);
		return CreateProcessA_raw(lpApplicationName, cmdline_new.data(), lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
	}
	using CreateProcessW_t = decltype(&CreateProcessW);
	CreateProcessW_t CreateProcessW_raw = &CreateProcessW;

	BOOL WINAPI
		CreateProcessW_mod
		(LPCWSTR lpApplicationName, LPWSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation
		)
	{
		static wstring exe_path_quoted_if_need_w = brv::strConvert(exe_path_quoted_if_need);
		if (!wstring_view(lpCommandLine).starts_with(exe_path_quoted_if_need_w))
		{
			return CreateProcessW_raw(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
		}
		string cmdline_new;
		procCmdline(brv::strConvert(lpCommandLine), &cmdline_new);

		return CreateProcessW_raw(lpApplicationName, brv::strConvert(cmdline_new).data(), lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
	}

	using CreateProcessAsUserA_t = decltype(&CreateProcessAsUserA);
	CreateProcessAsUserA_t CreateProcessAsUserA_raw = CreateProcessAsUserA;
	BOOL
		WINAPI
		CreateProcessAsUserA_mod(HANDLE hToken, LPCSTR lpApplicationName, LPSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCSTR lpCurrentDirectory, LPSTARTUPINFO lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation
		)
	{
		if (!string_view(lpCommandLine).starts_with(exe_path_quoted_if_need))
		{
			return CreateProcessAsUserA_raw(hToken, lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
		}
		string cmdline_new;
		procCmdline(lpCommandLine, &cmdline_new);
		return CreateProcessAsUserA_raw(hToken, lpApplicationName, cmdline_new.data(), lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
	}
	using CreateProcessAsUserW_t = decltype(&CreateProcessAsUserW);
	CreateProcessAsUserW_t CreateProcessAsUserW_raw = CreateProcessAsUserW;
	BOOL
		WINAPI
		CreateProcessAsUserW_mod(HANDLE hToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation
		)
	{
		static wstring exe_path_quoted_if_need_w = brv::strConvert(exe_path_quoted_if_need);
		if (!wstring_view(lpCommandLine).starts_with(exe_path_quoted_if_need_w))
		{
			return CreateProcessAsUserW_raw(hToken,lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
		}
		string cmdline_new;
		procCmdline(brv::strConvert(lpCommandLine), &cmdline_new);

		return CreateProcessAsUserW_raw(hToken,lpApplicationName, brv::strConvert(cmdline_new).data(), lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
	}
}