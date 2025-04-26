module;
#include <set>
#include <string>

export module FsGuard;

export {
  enum AccessFlag { banRead, banWrite };
  struct Node {
    std::wstring name;
    AccessFlag access_mask;
    std::set<Node*> child_set;
  };
  class FsGuard {};
}