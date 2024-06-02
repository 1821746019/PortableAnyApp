#pragma once


//#include "fs_redirect.h"

#include <string>
#include <vector>

struct PathConfig
{

};

class PathJudge {
	inline static PathJudge* ins_ = nullptr;
	bool blacklist_mode = true;
	std::vector<std::string> path_list_;
	std::vector<std::string> excluded_path_list_;
	void _init_();

	PathJudge();

public:
	static PathJudge* _ins_();

	bool judgeNT(const char* old)const;
};