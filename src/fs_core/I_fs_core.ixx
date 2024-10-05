export module I_fs_core;


import std;
import  Hooker;
import func2hook;
import func2hook.kernelbase;
import MacroMgr;
import ConfigMgr;
import fs_common;
import PathJudge;

export{

	void setHook();
	void fs_core_init(const std::wstring& config_path);
	void fs_core_uninit();
}

void setHook()
{
	DetoursHooker hooker;

	hooker.endeque({
		{ &NtCreateFile_raw, NtCreateFile_mod },
		{ &NtOpenFile_raw, NtOpenFile_mod },
		{ &NtDeleteFile_raw, NtDeleteFile_mod },
		{ &NtQueryFullAttributesFile_raw, &NtQueryFullAttributesFile_mod },
		{ &NtQueryAttributesFile_raw, NtQueryAttributesFile_mod },
		//{ &NtQueryDirectoryFile_raw, NtQueryDirectoryFile_mod },
		//{ &NtSetInformationFile_raw, NtSetInformationFile_mod },
		{&MoveFileWithProgressTransactedW_raw,MoveFileWithProgressTransactedW_mod}


		});

	//hooker.endeque({
	//	});
	hooker.setHook();
}

void fs_core_init(const std::wstring& config_path)
{
	MacroMgr macro_mgr(
		{
				{L"USER_HOME",getUserHomePath()
				} }
	);
	ConfigMgr config_mgr(&macro_mgr, config_path);
	PathJudge::_init_(config_mgr.fsConfig());
	setHook();
}

void fs_core_uninit()
{

}