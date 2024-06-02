module;

#include <string>
#include <unordered_map>
#include <fstream>
//#include <string>
#define TOML_HEADER_ONLY 0
#include <sstream>
#include <toml++/toml.hpp>
export module ConfigMgr;
import  MacroMgr;
import fs_common;
import my_converter.str;
import FsConfig;


export class ConfigMgr
{
	MacroMgr* macro_mgr_;

	FsConfig fs_config_;
	static std::string preprocConfig(const std::wstring& config_path)
	{
		std::ifstream ifs(config_path);
		if(!ifs)
		{
			throw std::runtime_error("failed to open file");
		}
		std::ostringstream oss;
		oss << ifs.rdbuf();  // 直接将文件内容读入ostringstream对象中
		std::string content = oss.str();
		to2BackSlash(content);
		return content;
	}
public:
	FsConfig fsConfig() { return fs_config_; }
	ConfigMgr(MacroMgr* macro_mgr, const std::wstring& config_path)
	{
		macro_mgr_ = macro_mgr;
		std::string afterPreproc = preprocConfig(config_path);
		auto config = toml::parse(afterPreproc);
		AppHomeGetSchema AHGS = (AppHomeGetSchema)config["AppHomeGetSchema"].as_integer()->get();
		
		macro_mgr->add({ L"AppHome",getAppHomePathW(AHGS)});
		const auto &user_defined_macros = *config["user_defined_macro_list"].as_table();
		for(auto& [k,v]:user_defined_macros)
		{
			std::wstring u_m= brv::strConvert(k.str().data()),
			u_v = brv::strConvert(v.as_string()->get());
			macro_mgr->add(std::pair{u_m, macro_mgr->replace(u_v)});
		}
		//FsConfig
		const toml::table& FsConfig = *config["FsConfig"].as_table();
		fs_config_.blacklist_mode = FsConfig["blacklist_mode"].as_boolean();
		for(auto& e: *FsConfig["excluded_path_list"].as_array())
		{
			fs_config_.excluded_path_list.emplace_back(
			macro_mgr->replace(brv::strConvert(e.as_string()->get()))
			);

		}
		for(auto& e:*FsConfig["path_list"].as_array())
		{
			fs_config_.path_list.emplace_back(
				macro_mgr->replace(brv::strConvert(e.as_string()->get()))
			);
		}
		//get redirection obj arr
		auto redirections = *config["redirection"].as_array();
		auto type = config.type();
		for(auto& r:redirections)
		{
			//auto type = old2new.type();
			auto redirection = *r.as_table();
			auto old_ = brv::strConvert(redirection["old"].as_string()->get());
			auto new_ = brv::strConvert( redirection["new"].as_string()->get());

			fs_config_.old2new.emplace(macro_mgr->replace(old_),macro_mgr->replace(new_));
		}
		//for(auto& r:redirections)
		//{
		//	std::wstring path_old, path_new;
		//	path_old=r["old"]
		//	//std::pair<std::wstring,std::wstring> old2new=r.
		//	fs_config_.old2new.emplace(std::pair(
		//	)
		//}

		
	}
	
};
