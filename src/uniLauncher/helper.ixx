module;
#include <Windows.h>

export module helper;


import std;
export {
  // 枚举类定义
  enum class PECheckStatus {
    Failed,
    Unknown,
    PE32,
    PE64,
  };


  PECheckStatus CheckPE(const std::filesystem::path& filePath) {
    // 打开文件
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
      std::cerr << "无法打开文件: " << filePath << std::endl;
      return PECheckStatus::Failed;
    }

    // 读取DOS头
    IMAGE_DOS_HEADER dosHeader;
    file.read(reinterpret_cast<char*>(&dosHeader), sizeof(dosHeader));
    if (!file) {
      std::cerr << "无法读取DOS头" << std::endl;
      return PECheckStatus::Failed;
    }

    // 检查DOS标志
    if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE) {  // "MZ"
      std::cerr << "无效的DOS签名" << std::endl;
      return PECheckStatus::Failed;
    }

    // 定位到PE头
    file.seekg(dosHeader.e_lfanew, std::ios::beg);
    if (!file) {
      std::cerr << "无法定位到PE头" << std::endl;
      return PECheckStatus::Failed;
    }

    // 读取PE签名
    DWORD peSignature;
    file.read(reinterpret_cast<char*>(&peSignature), sizeof(peSignature));
    if (!file || peSignature != IMAGE_NT_SIGNATURE) {  // "PE\0\0"
      std::cerr << "无效的PE签名" << std::endl;
      return PECheckStatus::Failed;
    }

    // 读取文件头
    IMAGE_FILE_HEADER fileHeader;
    file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    if (!file) {
      std::cerr << "无法读取文件头" << std::endl;
      return PECheckStatus::Failed;
    }

    // 读取可选头的Magic字段
    WORD magic;
    file.read(reinterpret_cast<char*>(&magic), sizeof(magic));
    if (!file) {
      std::cerr << "无法读取可选头Magic字段" << std::endl;
      return PECheckStatus::Failed;
    }

    if (magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC) {  // 0x10b
      return PECheckStatus::PE32;
    } else if (magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC) {  // 0x20b
      return PECheckStatus::PE64;
    } else {
      std::cerr << "未知的可选头Magic字段: " << std::hex << magic << std::endl;
      return PECheckStatus::Unknown;
    }
  }
}