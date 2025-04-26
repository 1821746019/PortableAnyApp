module;
#include <WinSock2.h>

export module func2hook.maybe;
import std;
import fs_common;
import PathJudge;

export {
  //decltype(&bind) bind_raw = &bind;
  //int bind_mod(SOCKET s, const sockaddr* name, int namelen) {
  //  if (name->sa_family != AF_UNIX)
  //    return bind_raw(s, name, namelen);
  //  wchar_t new_path[MAX_PATH + 1];
  //  auto path_judge = PathJudge::_ins_();
  //  sockaddr_un addr;
  //  if (path_judge->judgeAndRedirect(name->sun_path, new_path,
  //                                   (int)std::size(new_path))) {
  //    name->sun_path = (char*)new_path;
  //  }
  //}
}