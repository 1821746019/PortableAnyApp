module;
#include <ntdll.h>

export module func2hook.maybe;

// export {
//
//
//	decltype(&NtQueryDirectoryFile) NtQueryDirectoryFile_raw =
//NtQueryDirectoryFile;
//
//	auto NTAPI NtQueryDirectoryFile_mod(HANDLE FileHandle, HANDLE Event,
//PIO_APC_ROUTINE ApcRoutine, 		PVOID ApcContext, PIO_STATUS_BLOCK IoStatusBlock,
//PVOID FileInformation, ULONG Length, 		FILE_INFORMATION_CLASS
//FileInformationClass, BOOLEAN ReturnSingleEntry, PUNICODE_STRING FileName,
//		BOOLEAN RestartScan)
//	{
//		addFuncHit();
//		NTSTATUS ret;
//		wstring path;
//		GetFilePathByHandle(FileHandle, path);
//		//&& FileInformationClass == FileIdBothDirectoryInformation
//		if (FileName && FileName->Buffer)
//		{
//			wstring file_name(FileName->Buffer);
//			if (path_judge->judgeAndModify(file_name, false))
//			{
//				consolePrint("NtQueryDirectoryFile =>" +
//brv::strConvert(wstring(FileName->Buffer))); 				RtlInitUnicodeString(FileName,
//(wchar_t*)file_name.c_str());
//			}
//			auto file_dir_info =
//(FILE_ID_BOTH_DIR_INFO*)FileInformation;
//
//			ret = NtQueryDirectoryFile_raw(FileHandle, Event,
//ApcRoutine, ApcContext, IoStatusBlock, 				FileInformation, Length,
//FileInformationClass, ReturnSingleEntry, FileName, RestartScan); 			int i = 0;
//		}
//		else
//		{
//			ret = NtQueryDirectoryFile_raw(FileHandle, Event,
//ApcRoutine, ApcContext, IoStatusBlock, 				FileInformation, Length,
//FileInformationClass, ReturnSingleEntry, FileName, RestartScan);
//		}
//
//
//		return ret;
//	};
//
//
//	decltype(&NtQueryInformationFile)NtQueryInformationFile_raw =
//NtQueryInformationFile;
//
//
//	decltype(&NtSetInformationFile) NtSetInformationFile_raw =
//NtSetInformationFile;
//
//	auto NTAPI NtSetInformationFile_mod(HANDLE FileHandle, PIO_STATUS_BLOCK
//IoStatusBlock, 		PVOID FileInformation, 		ULONG Length, FILE_INFORMATION_CLASS
//FileInformationClass)
//	{
//		addFuncHit();
//		NTSTATUS ret;
//		wstring path;
//		GetFilePathByHandle(FileHandle, path);
//
//		if (FileInformationClass == FileRenameInformation)
//		{
//			//重命名的文件信息
//			auto file_info = (FILE_RENAME_INFO*)FileInformation;
//
//			wstring new_path(file_info->FileName,
//file_info->FileNameLength / sizeof(wchar_t));
//
//			if (path_judge->judgeAndModify(new_path, false))
//			{
//				ULONG fileNameLength = (ULONG)new_path.size() *
//sizeof(WCHAR); 				ULONG file_info_size = sizeof(FILE_RENAME_INFO) +
//fileNameLength; //- sizeof(WCHAR); 				std::vector<BYTE> buffer(file_info_size);
//				auto file_info_new =
//reinterpret_cast<FILE_RENAME_INFO*>(buffer.data());
//				file_info_new->ReplaceIfExists =
//file_info->ReplaceIfExists; // 或TRUE，根据需要 				file_info_new->RootDirectory =
//file_info->RootDirectory; // 或有效的目录句柄 				file_info_new->FileNameLength =
//fileNameLength;
//				// 复制新文件名
//				memcpy(file_info_new->FileName,
//new_path.c_str(), fileNameLength);
//
//				ret = NtSetInformationFile_raw(FileHandle,
//IoStatusBlock, file_info_new, file_info_size, 					FileInformationClass);
//			}
//			else//不需要重定向的目录
//			{
//				ret = NtSetInformationFile_raw(FileHandle,
//IoStatusBlock, FileInformation, Length, 					FileInformationClass);
//			}
//		}
//		else
//		{
//			if (FileInformationClass == FileDispositionInformation)
//			{
//				static FILE_DISPOSITION_INFO*
//last_file_disposition = nullptr;
//				//意向 删除文件?
//				auto file_disposition_inf =
//(FILE_DISPOSITION_INFO*)FileInformation;
//
//				consolePrint("NtSetInfo Delete File/Dir =>" +
//brv::strConvert(path)); 				ret = NtSetInformationFile_raw(FileHandle,
//IoStatusBlock, file_disposition_inf, Length, 					FileInformationClass);
//			}
//			else if (FileInformationClass ==
//FileDispositionInformationEx)
//			{
//				auto file_disposition_inf =
//(FILE_DISPOSITION_INFO_EX*)FileInformation;//意向 删除文件?
//				consolePrint("NtSetInfo DeleteEx File/Dir =>" +
//brv::strConvert(path)); 				ret = NtSetInformationFile_raw(FileHandle,
//IoStatusBlock, file_disposition_inf, Length, 					FileInformationClass);
//			}
//			else
//			{
//				ret = NtSetInformationFile_raw(FileHandle,
//IoStatusBlock, FileInformation, Length, 					FileInformationClass);
//			}
//		}
//
//		return ret;
//	};
// }