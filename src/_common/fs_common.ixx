module;

#include <filesystem>
#include <Windows.h>
#include <stdexcept>
#include <string>

export module fs_common;

export {
	std::string getExeDir()
	{
		char exe_path2dir[MAX_PATH];
		GetModuleFileNameA(nullptr, exe_path2dir, sizeof(exe_path2dir));
		strrchr(exe_path2dir, '\\')[0] = '\0';
		return exe_path2dir;
	}
	std::wstring findConfigPath()
	{
		auto config_name = "fs_guard.toml";
		std::filesystem::path exeDir = getExeDir();
		while (!std::filesystem::exists(exeDir / config_name))
		{
			exeDir = exeDir.parent_path();
		}
		return exeDir / config_name;
	}
	std::wstring getExeDirW()
	{
		wchar_t exe_path2dir[MAX_PATH];
		GetModuleFileNameW(nullptr, exe_path2dir, std::size(exe_path2dir));
		wcsrchr(exe_path2dir, L'\\')[0] = '\0';
		return exe_path2dir;
	}
	enum AppHomeGetSchema { from_exePath,exeDir, exeDirParent, configDir};

	std::wstring getAppHomePathW(AppHomeGetSchema schema = from_exePath)
	{
		std::wstring ret;
		std::wstring exe_dir = getExeDirW();
		if (schema == from_exePath)
		{
			std::wstring flag = L"\\App";
			size_t count = exe_dir.find(flag);
			if (count == std::wstring::npos)
			{
				auto msg = "exe_path must contains App dir to seek APP_HOME";

				throw std::runtime_error(msg);
			}
			ret = exe_dir.substr(0, count);
		}
		else if (schema == exeDir)
		{
			ret = exe_dir;
		}else if(schema==exeDirParent)
		{
			ret = exe_dir.substr(0, exe_dir.rfind(L'\\'));
		}else if(schema==configDir)
		{
			ret = std::filesystem::path(findConfigPath()).parent_path();
		}

		return ret;
	}
	std::wstring getUserHomePath()
	{
		;
		return _wgetenv(L"USERPROFILE");
	}
	//nullptr means to find the Path based on the exe path
	void seAppHomePathEnvVar(const std::string& AppHomePath)
	{
		std::string to_put = "AppHomePath=" + AppHomePath;
		_putenv(to_put.data());
		return;
	}
	int to2BackSlash(std::string& _path)
	{
		int cnt = 0;
		for(auto i=0;i< (int)_path.size(); ++i)
		{
			if (_path[i] == '/')
			{
				_path[i] = '\\';
				_path.insert(i,"\\");
				cnt++;
			};
		}
		return cnt;
	}
	//std::wstring toForwardSlash(const std::wstring& path)
	//{
	//	for ()
	//}
}
