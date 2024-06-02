module;
#include <string>
#include <Windows.h>
export module shared;


using namespace std;
export{
	// 转换filesystem::path为string，如果路径中包含空格，则在两端添加双引号
	int quotePathIfNeed(string* path) {
		string& pathStr = *path;
		// 检查路径字符串中是否包含空格
		if (pathStr.find(' ') != string::npos) {
			// 在路径两端添加双引号
			pathStr = "\"" + pathStr + "\"";
			return true;
		}
		return false;
	}
	string quotePathIfNeed(const string& path)
	{
		string ret = path;
		quotePathIfNeed(&ret);
		return ret;
	}
	int unquotePathIfNeed(string* path)
	{
		string& path_ = *path;
		if (path_[0] == '\"')
		{
			path_ = path_.substr(1, path_.size() - 2);
			return true;
		}
		return false;
	}
	string unquotePathIfNeed(const string& path)
	{
		string ret = path;
		unquotePathIfNeed(&ret);
		return ret;
	}
	const string& getNowExePath()
	{
		static string exe_path;
		if (exe_path.empty())
		{
			char buf[MAX_PATH];
			GetModuleFileNameA(GetModuleHandleA(nullptr), buf, sizeof(buf));
			exe_path.assign(buf);
		}
		return exe_path;
	}
}