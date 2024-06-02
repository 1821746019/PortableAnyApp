#include "PathJudge.h"



extern "C"{
#include "strutils.h"
}
#include <cstdarg>
#include <ranges>

#include <Windows.h>



void PathJudge::_init_()
{
	if (getenv("whitelist_mode"))blacklist_mode = false;
	//init path_list according env variables "path_list"
	char delim = '|';
	std::string paths = getenv("path_list");
	to_lowercase(paths.data());
	auto view = paths | std::ranges::views::split(delim);
	for (auto&& part : view)
	{
		path_list_.emplace_back(part.begin(), part.end());
	}
	//add NT_PREFIX for every path;
	for (auto& e : path_list_)
	{
		e = NT_PREFIX + e;

	}
	std::string excluded_path = getenv("excluded_path_list");
	to_lowercase(excluded_path.data());
	view = excluded_path | std::ranges::views::split(delim);
	for(auto&& p:view)
	{
		excluded_path_list_.emplace_back(p.begin(), p.end());
	}
	for(auto& e:excluded_path_list_)
	{
		e = NT_PREFIX + e;
	}

}

PathJudge::PathJudge()
{
	_init_();
}

PathJudge* PathJudge::_ins_()
{
	if (!ins_)ins_ = new PathJudge;
	return ins_;
}
void DBG_printf(const char* fmt, ...) {

	char s[0x100] = { 0x00 };
	va_list args;
	va_start(args, fmt);
	vsnprintf(s, sizeof(s) - 1, fmt, args);
	va_end(args);
	s[sizeof(s) - 1] = 0x00;
	OutputDebugStringA(s);

}
bool PathJudge::judgeNT(const char* old) const
{
	//judgeNT it is or belongs to excluded_path_list first
	for(auto& e:excluded_path_list_)
	{
		if(_strnicmp(old,e.data(),e.size())==0)
		{
			return true;
		}
	}
	bool ret = true;//default is false

	// judgeNT it is or belongs to path_list
	for (auto& e : path_list_)
	{
		if (_strnicmp(old, e.data(), e.size())==0)
		{
			ret = false;
			DBG_printf("[Redirected]: %s\n", old);

			break;
		}else
		{
			DBG_printf("old path is %s\n", old);

		}

	}
	return ret;
}
