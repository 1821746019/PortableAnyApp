#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#define FAKE_HOME "c\\users\\user"
#define NT_PREFIX_W L"\\??\\"
#define NT_PREFIX_LEN 4

#define NT_PREFIX "\\??\\"
struct JudgeContext;
struct FsConfig;



class PathJudge {
	inline static PathJudge* ins_ = nullptr;
	bool blacklist_mode = true;
	std::vector<std::wstring> path_list_;
	std::vector<std::wstring> excluded_path_list_;
	std::unordered_map<std::wstring, std::wstring> old2new_;
	std::wstring default_redirected_path_prefix_;
	//void _init_();

	PathJudge();

public:
	static void _init_(const FsConfig& fs_config);
	static PathJudge* _ins_();
	static PathJudge* _reinit_();
	bool judge(const wchar_t* old, JudgeContext* judge_context = nullptr, bool is_nt_path = true)const;
	bool redirect(const wchar_t* old, const JudgeContext* judge_context, wchar_t* buffer, int len, bool is_nt_path = true) const;
	bool judgeAndRedirect(const wchar_t* old, wchar_t* buffer, int len) const;
};

