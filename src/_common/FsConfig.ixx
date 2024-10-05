module;

export module FsConfig;


import std;
export{

	struct FsConfig
	{
		bool blacklist_mode = true;
		std::vector<std::wstring> path_list;
		std::vector<std::wstring> excluded_path_list;
		std::unordered_map<std::wstring, std::wstring> old2new;
		std::wstring default_redirected_path_prefix;
	};
}