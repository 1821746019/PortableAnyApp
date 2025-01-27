module;
#include <Windows.h>
export module GuardConfig;
import std;
import std.compat;
struct Key;
struct KeyHash {
  std::size_t operator()(const Key* k) const;
};

struct KeyEqual {
  bool operator()(const Key* lhs, const Key* rhs) const;
};
struct KeyInfo {
  std::unique_ptr<std::wstring> redirect;
  uint32_t permissions = 7;
  std::unordered_set<Key*, KeyHash,KeyEqual> subKeys;
};
struct Key {
  std::wstring name;

  std::unique_ptr<KeyInfo> keyInfo;
};

std::size_t KeyHash::operator()(const Key* k) const {
  return std::hash<std::wstring>()(k->name);  // 使用 std::hash 对 name 进行哈希
}

bool KeyEqual::operator()(const Key* lhs, const Key* rhs) const {
  return lhs->name == rhs->name;  // 只比较 name
}

wchar_t* tolower(wchar_t str[]) {
  if (!str)
    return nullptr;
  for (wchar_t* p = str; *p; ++p) {
    *p = towlower(*p);
  }
  return str;
}

export class GuardConfig {
  Key* root_ = nullptr;
  static void deleteKey(const Key* curr) {
    if (!curr)
      return;
    const auto& subKeys = curr->keyInfo->subKeys;
    if (!subKeys.empty()) {
      for (auto& subKey : subKeys) {
        deleteKey(subKey);
      }
    }
    delete curr;
  }
  GuardConfig() {}

 public:
  GuardConfig(const GuardConfig&) = delete;
  GuardConfig& operator=(const GuardConfig&) = delete;
  GuardConfig(GuardConfig&&) = delete;
  GuardConfig& operator=(GuardConfig&&) = delete;
  // 工厂模式
  static std::unique_ptr<GuardConfig> _createIns_() {
    return std::unique_ptr<GuardConfig>(new GuardConfig());
  }
  Key* create(std::wstring path) {
    if (!root_)
      root_ =
          new Key{L"", std::make_unique<KeyInfo>(nullptr, 7, std::unordered_set<Key*, KeyHash, KeyEqual>{})};
    Key* currKey = root_;
    wchar_t* context = nullptr;
    wchar_t* tok = tolower(wcstok_s(path.data(), L"\\", &context));
    while (tok != nullptr) {
      auto& subKeys = currKey->keyInfo->subKeys;
      Key tmp(tok, nullptr);

      auto it = subKeys.find(&tmp);
      if (it == subKeys.end()) {
        subKeys.emplace(
            new Key(tok, std::make_unique<KeyInfo>(nullptr, 7, std::unordered_set<Key*, KeyHash, KeyEqual>{}))
        );
      }

      currKey = *subKeys.find(&tmp);
      if (!currKey)
        return currKey;
      tok = tolower(wcstok_s(nullptr, L"\\", &context));
    }
    return currKey;
  }
  // enum
  Key* open(std::wstring path, Key** keyContext = nullptr) {
    // if (root_ == nullptr) {throw runtime_error("PseudoHive: root is null");}
    Key* currKey = root_;
    wchar_t* context = nullptr;
    wchar_t* tok = tolower(wcstok_s(path.data(), L"\\", &context));
    while (tok != nullptr) {
      auto& subKeys = currKey->keyInfo->subKeys;
      if (keyContext)
        *keyContext = currKey;

      Key tmp(tok, nullptr);
      auto it = subKeys.find(&tmp);
      currKey = it != subKeys.end() ? *it : nullptr;
      if (!currKey)
        return currKey;
      tok = tolower(wcstok_s(nullptr, L"\\", &context));
    }
    return currKey;
    // strtok_s()
  }
  ~GuardConfig() { deleteKey(root_); }
};

// extern "C" int main() {
//   GuardConfig guardConfig;
//   Key* key = guardConfig.create(LR"(HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\Internet Download Manager)");
//   key->keyInfo->redirect = make_unique<std::wstring>();
//   Key* key2 = guardConfig.open(LR"(HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\Internet Download Manager)");
//   if (key2->keyInfo->redirect)
//     wcout << *key2->keyInfo->redirect << endl;
//
//   return 0;
// }