module;

#include <string_view>
#include <windows.h>


export module my_converter.str;


export namespace brv
{
	void strConvert(const std::wstring_view& src_wstr, std::string& dst_str, uint32_t code_page = 936) //GBK
	{
		int total_len = WideCharToMultiByte(code_page, 0, src_wstr.data(), -1, nullptr, 0, nullptr, nullptr);
		dst_str.resize(total_len - 1);
		WideCharToMultiByte(code_page, 0, src_wstr.data(), -1, dst_str.data(), total_len, nullptr, nullptr);
	}

	std::string strConvert(const std::wstring_view& src_wstr, uint32_t code_page = 936) //GBK
	{
		std::string dst_str;
		int total_len = WideCharToMultiByte(code_page, 0, src_wstr.data(), -1, nullptr, 0, nullptr, nullptr);
		dst_str.resize(total_len - 1);
		WideCharToMultiByte(code_page, 0, src_wstr.data(), -1, dst_str.data(), total_len, nullptr, nullptr);
		return dst_str;
	}

	void strConvert(const std::string_view& src_str, std::wstring& dst_wstr, uint32_t code_page = 936) //GBK
	{
		int total_len = MultiByteToWideChar(code_page, 0, src_str.data(), -1, nullptr, 0);
		dst_wstr.resize(total_len - 1);
		MultiByteToWideChar(code_page, 0, src_str.data(), -1, dst_wstr.data(), total_len);
	}

	std::wstring strConvert(const std::string_view& src_str, uint32_t code_page = 936) //GBK
	{
		std::wstring dst_wstr;
		int total_len = MultiByteToWideChar(code_page, 0, src_str.data(), -1, nullptr, 0);
		dst_wstr.resize(total_len - 1);
		MultiByteToWideChar(code_page, 0, src_str.data(), -1, dst_wstr.data(), total_len);
		return dst_wstr;
	}
}
