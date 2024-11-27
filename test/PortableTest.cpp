extern "C"{
}
#include <LPP_API_x64_CPP.h>
import std;
import VarInterpolationMgr;
import ConfigMgr;
import fs_common;
import my_converter.str;

using namespace std;

static class MyGlobalInit
{
  lpp::LppDefaultAgent lppAgent;
public:
  MyGlobalInit(){
    // create a default agent, loading the Live++ agent from the given path, e.g. "ThirdParty/LivePP"
    lppAgent = lpp::LppCreateDefaultAgent(nullptr, LR"(D:\environment\c++\LivePP)");

    // bail out in case the agent is not valid
    if (!lpp::LppIsValidDefaultAgent(&lppAgent))
    {
      throw runtime_error("Failed to create default agent");
    }

    // enable Live++ for all loaded modules
    lppAgent.EnableModule(lpp::LppGetCurrentModulePath(), lpp::LPP_MODULES_OPTION_ALL_IMPORT_MODULES, nullptr, nullptr);

  }
  ~MyGlobalInit()
  {
    // destroy the Live++ agent
    lpp::LppDestroyDefaultAgent(&lppAgent);
  }
} my_global_init;
int main() {

  getchar();
  MacroMgr macro_mgr;
  macro_mgr.add(pair{ L"USER_HOME",getUserHomePath() });
  wstring app_home_dir;
  try
  {
    app_home_dir = getAppHomePathW();
  }
  catch (...)
  {
    app_home_dir = filesystem::current_path();//brv::strConvert(getAppHomePathW(AppHomeGetSchema::exeDir));
  }
  ConfigMgr config_mgr(&macro_mgr, L"D:\\fs_guard.toml");


  getchar();
}