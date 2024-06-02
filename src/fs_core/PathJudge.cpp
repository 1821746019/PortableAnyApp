import FsConfig;
import my_converter.str;

#include "PathJudge.h"

#include <algorithm>
#include <format>
#include <ranges>
#include <Windows.h>



using namespace std;
struct JudgeContext
{
	enum JudgeContextFlag { InPathList, InOld2new };
	JudgeContextFlag flag;
	const wstring* matched = nullptr;
	int offset = 0;
};

void PathJudge::_init_(const FsConfig& fs_config)
{
	auto addNtPrefix = [](const wstring& p)
		{
			return NT_PREFIX_W + p;
		};
	ins_ = new PathJudge;
	ins_->blacklist_mode = fs_config.blacklist_mode;
	//exluded_path_list
	auto v = fs_config.excluded_path_list | views::transform(addNtPrefix);
	ins_->excluded_path_list_ = vector<wstring>(v.begin(), v.end());
	//path_list
	v = fs_config.path_list | views::transform(addNtPrefix);
	ins_->path_list_ =vector(v.begin(),v.end());
	//default prefix
	ins_->default_redirected_path_prefix_ = addNtPrefix(fs_config.default_redirected_path_prefix);
	// old2new_
	auto v2 = fs_config.old2new | views::transform([&](const pair<wstring, wstring>& p)
		{
			return pair{ addNtPrefix(p.first),addNtPrefix(p.second) };
		});
	ins_->old2new_ = unordered_map<wstring,wstring>(v2.begin(),v2.end());

}
//void PathJudge::_init_()
//{
//	if (getenv("whitelist_mode"))blacklist_mode = false;
//	//init path_list according env variables "path_list"
//	char delim = '|';
//	wstring paths = brv::strConvert(getenv("path_list"));
//	//to_lowercase(paths.data());
//	auto view = paths | ranges::views::split(delim);
//	for (auto&& part : view)
//	{
//		path_list.emplace_back(part.begin(), part.end());
//	}
//	//add NT_PREFIX for every path;
//	for (auto& e : path_list)
//	{
//		e = NT_PREFIX_W + e;
//
//	}
//	wstring excluded_path = brv::strConvert(getenv("excluded_path_list"));
//	//to_lowercase(excluded_path.data());
//	view = excluded_path | ranges::views::split(delim);
//	for (auto&& p : view)
//	{
//		excluded_path_list.emplace_back(p.begin(), p.end());
//	}
//	for (auto& e : excluded_path_list)
//	{
//		e = NT_PREFIX_W + e;
//	}
//	default_redirected_path_prefix_ = NT_PREFIX_W + brv::strConvert(getenv("default_redirected_path_prefix"));
//}

PathJudge::PathJudge()
{
	//_init_();
}

PathJudge* PathJudge::_ins_()
{
	//if (!ins_)ins_ = new PathJudge;
	return ins_;
}

PathJudge* PathJudge::_reinit_()
{
	delete ins_;
	ins_ = new PathJudge;
	return ins_;
}

bool PathJudge::judge(const wchar_t* old, JudgeContext* judge_context, bool is_nt_path) const
{
	if(wcscmp(old,LR"(\??\C:\Users\Administrator\AppData\LocalLow)")==0)
	{
		int hit = 1;
	}
	//judge it is or belongs to excluded_path_list first
	for (auto& e : excluded_path_list_)
	{
		const wchar_t* to_match = e.data();
		int to_match_len = (int)e.size();
		if (!is_nt_path)
		{
			to_match += NT_PREFIX_LEN;
			to_match_len -= NT_PREFIX_LEN;
		}
		if (_wcsnicmp(old, to_match, to_match_len) == 0)
		{
			return false;
		}
	}
	//default is not to redirect
	bool ret = false;
	//judge if it can be found in old2new
	for (const auto& k : old2new_ | views::keys)
	{
		const wchar_t* to_match = k.data();
		int to_match_len = (int)k.size();
		if (!is_nt_path)
		{
			to_match += NT_PREFIX_LEN;
			to_match_len -= NT_PREFIX_LEN;
		}
		if (_wcsnicmp(old, to_match, to_match_len) == 0)
		{
			ret = true;
			if (judge_context)
			{
				judge_context->flag = JudgeContext::InOld2new;
				judge_context->matched = &k;
				judge_context->offset = is_nt_path ? 0 : NT_PREFIX_LEN;
			}
			break;
		}
	}
	if (ret)return ret;
	// judge it is or belongs to path_list

	for (const auto& i : path_list_)
	{
		const wchar_t* to_match = i.data();
		int to_match_len = (int)i.size();
		if (!is_nt_path)
		{
			to_match += NT_PREFIX_LEN;
			to_match_len -= NT_PREFIX_LEN;
		}

		if (_wcsnicmp(old, to_match, to_match_len) == 0)
		{

			ret = true;
			if (judge_context)
			{
				judge_context->flag = JudgeContext::InOld2new;
				judge_context->matched = &i;
				judge_context->offset = is_nt_path ? 0 : NT_PREFIX_LEN;

			}
			break;
		}


	}


	return ret;
}

bool PathJudge::redirect(const wchar_t* old, const JudgeContext* judge_context, wchar_t* buffer, int len, bool is_nt_path) const
{
	bool ret = false;
	//redirect for old2new


	if (judge_context->flag == JudgeContext::InOld2new) {
		int offset = is_nt_path ? 0 : NT_PREFIX_LEN ;
		const wstring& old_prefix = *judge_context->matched,
			new_prefix = old2new_.at(old_prefix);
		wstring result = wstring(old).replace(0, old_prefix.size()-offset, new_prefix.data()+offset
		);
		//uin_
		//memcpy_s(buffer,len,result.data())
		wcscpy_s(buffer, len, result.data() );
		ret = true;
	}
	//redirect for path_list
	else if (judge_context->flag == JudgeContext::InPathList)
	{

		wstring result;
		const wstring& old_prefix = *judge_context->matched;
		//default prefix as a base dir
		int offset = is_nt_path ? NT_PREFIX_LEN : 0;

		//if (!is_nt_path)old_path_prefix_len -= NT_PREFIX_LEN;
		result = default_redirected_path_prefix_ + L"\\" + (old + offset);

		wcscpy_s(buffer, len, is_nt_path ? result.data() : result.data() + NT_PREFIX_LEN);
		ret = true;

	}
	return ret;
}
//#define dbgOutput OutputDebugStringW
//
//#ifndef BS_DBG
//#define  dbgOutput
//#endif

bool PathJudge::judgeAndRedirect(const wchar_t* old, wchar_t* buffer, int len) const
{
	if (!old)return false;
	bool ret = false;
	JudgeContext context;
	bool is_nt_path = (_wcsnicmp(old, NT_PREFIX_W, NT_PREFIX_LEN) == 0);
	if (judge(old, &context, is_nt_path))
	{

		ret = redirect(old, &context, buffer, len, is_nt_path);
		OutputDebugStringW((format(L"[Redirected]: {}\n=>{}\n", old, buffer).data()));
	}
	else
	{
		OutputDebugStringW((format(L"[Bypass]: {}\n", old).data()));
	}
	return ret;
}
