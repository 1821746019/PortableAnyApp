module;
#include <cwctype>
export module strUtils;

export {

    void to_lowercase(wchar_t* in) {
        for (int i = 0; in[i]; i++) {
            in[i] = std::towlower(in[i]);
        }
    }
}