module;

export module fs_related;
import std;
import selfInfo;
namespace fs = std::filesystem;
export {
  std::string loadConfigFromDisk(const fs::path& filePath) {
    // 处理绝对路径和相对路径
    fs::path config_path = filePath.is_absolute() ? filePath : (selfDir() / filePath);
    if (!fs::exists(config_path)) {
      throw fs::filesystem_error(
          format("Please make sure the {} exists", config_path.string()).data(), config_path,
          std::make_error_code(std::errc::no_such_file_or_directory)
      );
    }
    std::ifstream ifs(config_path);
    return std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
  }
  //std::string loadConfigFromDisk() {
  //  return loadConfigFromDisk(
  //      selfDir() / fs::path(__FILE__).parent_path().filename().replace_extension(".toml")
  //  );
  //}
  void appendToFile(const std::filesystem::path& filePath, const std::string& content) {
    std::ofstream ofs(filePath, std::ios::app);
    ofs << content << '\n';
    ofs.close();
  }
}