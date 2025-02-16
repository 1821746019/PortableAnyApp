module;
#include <ws2tcpip.h>
#include <Windows.h>
#include <WinDNS.h>
#include <toml++/toml.hpp>
#pragma comment(lib, "dnsapi.lib")
#pragma comment(lib, "ws2_32.lib")

export module HijackDnsQuery;
import std;
import Hooker;
import selfInfo;
using namespace std;
namespace fs = filesystem;

class ConfigMgr {
  string configContent_;
  toml::table config_;
  set<wstring> domainsBlocked;
  void initFinalConfigContent() {
    fs::path config_path =
        (selfDir() / fs::path(__FILE__).filename().replace_extension(".toml"));
    if (!fs::exists(config_path)) {
      throw fs::filesystem_error(
          format("Please make sure the {} exists", config_path.string()).data(),
          config_path, make_error_code(errc::no_such_file_or_directory));
    }
    ifstream ifs(config_path);
    configContent_ =
        string(istreambuf_iterator<char>(ifs), istreambuf_iterator<char>());
  }
  void updateDomainBlockList() {
    domainsBlocked.clear();
    for (auto& e : *config_["domainBlockList"].as_array()) {
      domainsBlocked.emplace(e.value<wstring>().value());
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

  bool isNeedBlocking(const wstring& domain) const {
    return domainsBlocked.contains(domain);
  }
};

decltype(&DnsQuery_W) DnsQuery_W_raw = &DnsQuery_W;

DNS_STATUS WINAPI DnsQuery_W_mod(PCWSTR pszName,
                                 WORD wType,
                                 DWORD Options,
                                 PVOID pExtra,
                                 PDNS_RECORD* ppQueryResults,
                                 PVOID* pReserved) {
  bool isBlocked = ConfigMgr::_ins_().isNeedBlocking(pszName);

  return isBlocked ? 1
                   : DnsQuery_W_raw(pszName, wType, Options, pExtra,
                                    ppQueryResults, pReserved);
}
decltype(&getaddrinfo) getaddrinfo_raw = &getaddrinfo;
INT WSAAPI getaddrinfo_mod(PCSTR pNodeName,
                           PCSTR pServiceName,
                           const ADDRINFOA* pHints,
                           PADDRINFOA* ppResult) {
  bool isBlocked =
      ConfigMgr::_ins_().isNeedBlocking(filesystem::path(pNodeName).wstring());
  return isBlocked ? 0
                   : getaddrinfo_raw(pNodeName, pServiceName, pHints, ppResult);
}
decltype(&gethostbyname) gethostbyname_raw = &gethostbyname;
struct hostent* WSAAPI gethostbyname_mod(const char* name) {
  bool isBlocked =
      ConfigMgr::_ins_().isNeedBlocking(filesystem::path(name).wstring());
  return isBlocked ? nullptr : gethostbyname_raw(name);
}
void setHook() {
  DetoursHooker hooker;
  hooker.endeque({
      {&DnsQuery_W_raw, &DnsQuery_W_mod},
      {&getaddrinfo_raw, &getaddrinfo_mod},
      {&gethostbyname_raw, &gethostbyname_mod},

  });
  hooker.setHook();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReson, LPVOID lpReserved) {
  if (dwReson != DLL_PROCESS_ATTACH)
    return TRUE;

  DisableThreadLibraryCalls(hModule);

  // init the ConfigMgr
  try {
    ConfigMgr::_ins_();
    setHook();
  } catch (const exception& e) {
    MessageBoxA(nullptr, e.what(), "Exception occured", MB_ICONERROR);
    exit(-1);
  }
}