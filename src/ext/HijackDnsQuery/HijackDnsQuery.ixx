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
import selfDir;
using namespace std;
namespace fs = filesystem;

class ConfigMgr {
  string configContent_;
  toml::table config_;
  set<wstring> domainBlockList_;
  void initFinalConfigContent() {
    fs::path config_path =
        (selfDir / fs::path(__FILE__).filename().replace_extension(".toml"));
    ifstream ifs(config_path);

    if (!ifs)
      MessageBoxA(
          nullptr,
          format("Please make sure the {} exists", config_path.string()).data(),
          "File Not Found", MB_ICONERROR);
    configContent_ =
        string(istreambuf_iterator<char>(ifs), istreambuf_iterator<char>());
  }
  void updateDomainBlockList() {
    domainBlockList_.clear();
    for (auto& e : *config_["domainBlockList"].as_array()) {
      domainBlockList_.emplace(e.value<wstring>().value());
    }
  }
  ConfigMgr() {
    initFinalConfigContent();
    config_ = toml::parse(configContent_);
  }
  inline static unique_ptr<ConfigMgr> ins_=nullptr;

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
    return domainBlockList_.contains(domain);
  }
};
static ConfigMgr& configMgr = ConfigMgr::_ins_();
decltype(&DnsQuery_W) DnsQuery_W_raw = &DnsQuery_W;

DNS_STATUS WINAPI DnsQuery_W_mod(PCWSTR pszName,
                                 WORD wType,
                                 DWORD Options,
                                 PVOID pExtra,
                                 PDNS_RECORD* ppQueryResults,
                                 PVOID* pReserved) {
  bool isBlocked = configMgr.isNeedBlocking(pszName);

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
      configMgr.isNeedBlocking(filesystem::path(pNodeName).wstring());
  return isBlocked ? 0
                   : getaddrinfo_raw(pNodeName, pServiceName, pHints, ppResult);
}
decltype(&gethostbyname) gethostbyname_raw = &gethostbyname;
struct hostent* WSAAPI gethostbyname_mod(const char* name) {
  bool isBlocked = configMgr.isNeedBlocking(filesystem::path(name).wstring());
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

extern "C" __declspec(dllexport) BOOL APIENTRY DllMain(HMODULE hModule,
                                                       DWORD dwReson,
                                                       LPVOID lpReserved) {
  // DisableThreadLibraryCalls(hModule);
  if (dwReson == DLL_PROCESS_ATTACH) {
    setHook();
  }
  return TRUE;
}