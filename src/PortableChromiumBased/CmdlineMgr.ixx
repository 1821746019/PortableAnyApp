module;
#include <Windows.h>
#include <filesystem>
#include <iostream>
export module CmdlineMgr;

import Hooker;
import procUtils;
import versionFunc;
import shared;
using namespace std;
namespace fs = std::filesystem;

size_t getEndIdx(const string& cmdline,
                 size_t offset,
                 bool already_in_quotes = false) {
  bool is_in_quote = false;
  int quote_num = 0;
  for (auto i = offset; i < cmdline.size(); ++i) {
    if (cmdline[i] == '\"') {
      if (already_in_quotes) {
        return i - 1;
      }
      quote_num++;
      if (quote_num == 2) {
        return i;
      }
      continue;
    }
    if (!already_in_quotes && quote_num % 2 == 0 && cmdline[i] == ' ') {
      return i - 1;
    }
  }
  return cmdline.size();
}
size_t getPathFromCmdline(const string& cmdline,
                          const string& path_flag,
                          string* out) {
  size_t idx = cmdline.find(path_flag);
  if (idx == -1)
    return -1;
  size_t idx1 = idx + path_flag.size();
  int quote_num = 0;

  size_t idx2 = getEndIdx(cmdline, idx1, cmdline[idx - 1] == '\"');
  *out = cmdline.substr(idx1, idx2 - idx1 + 1);

  return idx;
}

int procCmdlinePathFlag(string& cmdline, const string& path_flag, string path) {
  int ret = true;
  string path_old;
  size_t idx = getPathFromCmdline(cmdline, path_flag, &path_old);
  bool need_quote = true;
  // quotePathIfNeed(&cache_path);
  if (idx == -1)  // do not find this flag, append it
  {
    quotePathIfNeed(&path);
    string opt_new = path_flag + path;

    cmdline.append(" " + opt_new);

  } else  // find the flag, replace it
  {
    // if ()
    if (path_old.find(path) != -1)  // find the dst data_dir
    {
      ret = false;
    } else  // not found, replace it
    {
      string opt_new = path_flag + path;

      if (cmdline[idx - 1] == '\"')
        need_quote = false;
      if (need_quote) {
        quotePathIfNeed(&path);
      }
      string cmdline_old = cmdline;
      cmdline.replace(idx, getEndIdx(cmdline, idx) - idx + 1, opt_new);
      if (cmdline.find(cmdline_old) != -1)
        ret = false;
    }
  }
  return ret;
}
export {
  bool procCmdline(string cmdline, string * out) {
    fs::path home_path = fs::path(getNowExePath()).parent_path().parent_path();
    fs::path data_path = home_path / "Data";
    string user_data_path = (data_path / "user_data").string(),
           cache_path = (data_path / "cache").string();

    string user_data_flag = "--user-data-dir=",
           cache_flag = "--disk-cache-dir=";

    bool status1 = procCmdlinePathFlag(cmdline, user_data_flag, user_data_path);
    bool status2 =
        false;  // procCmdlinePathFlag(cmdline, cache_flag, cache_path);
    if (out)
      *out = std::move(cmdline);
    return status1 || status2;
  }

  void portableByCmdline(const string& cmdline)  {
    string cmdline_new;
    if (procCmdline(cmdline, &cmdline_new)) {
      createProcess(cmdline_new);
      exit(-1);
    }

    // flag=""
  }
}
