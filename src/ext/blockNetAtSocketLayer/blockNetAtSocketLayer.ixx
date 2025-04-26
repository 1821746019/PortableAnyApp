module;

#include <WinSock2.h>
#include <Windows.h>
#include <toml++/toml.hpp>
#pragma comment(lib, "ws2_32.lib")
export module HijackDnsQuery;
import std;
import Hooker;
import selfInfo;
import strUtils;
using namespace std;
namespace fs = filesystem;

class ConfigMgr {
  string configContent_;
  toml::table config_;
  set<wstring> domainsBlocked;
  void initFinalConfigContent() {
    fs::path config_path = (selfDir() / fs::path(__FILE__).filename().replace_extension(".toml"));
    if (!fs::exists(config_path)) {
      throw fs::filesystem_error(
          format("Please make sure the {} exists", config_path.string()).data(), config_path,
          make_error_code(errc::no_such_file_or_directory)
      );
    }
    ifstream ifs(config_path);
    configContent_ = string(istreambuf_iterator<char>(ifs), istreambuf_iterator<char>());
  }
  void updateDomainBlockList() {
    domainsBlocked.clear();
    for (auto& e : *config_["domainBlockList"].as_array()) {
      auto tmp = e.value<wstring>().value();
      to_lowercase(tmp.data());
      domainsBlocked.emplace(tmp);
    }
  }
  ConfigMgr() {
    initFinalConfigContent();
    config_ = toml::parse(configContent_);
  }
  inline static unique_ptr<ConfigMgr> ins_ = nullptr;

 public:
  ConfigMgr(const ConfigMgr&) = delete;
  ConfigMgr& operator=(const ConfigMgr&) = delete;
  static ConfigMgr& _ins_() {
    if (!ins_) {
      ins_ = unique_ptr<ConfigMgr>(new ConfigMgr);
    }
    return *ins_;
  }

  bool isNeedBlocking(wstring domain) const {
    to_lowercase(domain.data());
    return domainsBlocked.contains(domain);
  }
};
decltype(&connect) connect_raw = &connect;
int WSAAPI
connect_mod(_In_ SOCKET s, _In_reads_bytes_(namelen) const struct sockaddr FAR* name, _In_ int namelen) {
  return SOCKET_ERROR;
}
decltype(&WSAConnect) WSAConnect_raw = &WSAConnect;
int WSAAPI WSAConnect_mod(
    SOCKET s,
    const struct sockaddr* name,
    int namelen,
    LPWSABUF lpCallerData,
    LPWSABUF lpCalleeData,
    LPQOS lpSQOS,
    LPQOS lpGQOS
) {
  return SOCKET_ERROR;
}

void setHook() {
  DetoursHooker hooker;
  hooker.endeque({
      {&connect_raw, &connect_mod},
      {&WSAConnect_raw, &WSAConnect_mod},

  });
  hooker.setHook();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReson, LPVOID lpReserved) {
  if (dwReson != DLL_PROCESS_ATTACH)
    return TRUE;

  DisableThreadLibraryCalls(hModule);

  // init the ConfigMgr
  try {
    // ConfigMgr::_ins_();
    setHook();
  } catch (const exception& e) {
    MessageBoxA(nullptr, e.what(), "Exception occured", MB_ICONERROR);
    exit(-1);
  }
  return TRUE;
}