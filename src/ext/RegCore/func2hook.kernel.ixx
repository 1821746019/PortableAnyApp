module;
#include <Windows.h>
export module func2hook.kernel;
import std;
import ConfigMgr;
import RegHandler_B;
import RegHandlerMgr;
import func2hook.kernel.raw;
import reg_common;
import strUtils;
#define cfgMgr ConfigMgr::_ins_()
#define regHandle RegHandlerMgr::_ins_().RegHandler()

std::string WideMultiSzToAnsiMultiSz(const wchar_t* wMultiSz, size_t cchW) {
  // wMultiSz: 宽字符缓冲
  // cchW: 宽字符个数(包含最后的双'\0'吗？可根据实际情况传入)

  std::string result;
  if (!wMultiSz || cchW == 0)
    return result;

  // 遍历每个子串
  const wchar_t* pCur = wMultiSz;
  while (*pCur) {
    // 把当前子串转 ANSI
    std::string ansi = WideToAnsi(pCur);
    // 追加到result，添加一个 '\0' 分隔
    result.append(ansi.c_str(), ansi.size());
    result.push_back('\0');

    // 跳到下一个子串(以 L'\0' 为结束)
    pCur += wcslen(pCur) + 1;
  }
  // 最终再追加一个 '\0' 作为终止
  result.push_back('\0');
  return result;
}

// 把输入的 ANSI 多字符串(以 '\0'分隔, 双'\0' 结尾, 长度=cbData) =>
// 转为宽多字符串
std::vector<wchar_t> AnsiMultiSzToWideMultiSz(const char* ansiMulti, size_t cbData) {
  std::vector<wchar_t> result;

  if (!ansiMulti || cbData == 0) {
    // 如果空，就放一个双空宽字符串
    result.push_back(L'\0');
    result.push_back(L'\0');
    return result;
  }

  // 遍历多字符串中的每个子串
  const char* pCur = ansiMulti;
  size_t offset = 0;

  while (true) {
    // 判断是否到达或超过缓冲区末尾
    if (offset >= cbData) {
      // 没有检测到双'\0'就已到末尾，也给它补一个 L'\0'
      result.push_back(L'\0');
      // 再补一个L'\0'结束
      result.push_back(L'\0');
      break;
    }

    // 如果当前字符是 '\0'
    if (pCur[0] == '\0') {
      // 看看是不是双空 => 全部结束
      if ((offset + 1) < cbData && pCur[1] == '\0') {
        // 这里是双'\0'
        // 先给当前子串末尾加个 L'\0' (表明本子串结束)
        result.push_back(L'\0');
        // 再给多字符串末尾加个 L'\0'(双空)
        result.push_back(L'\0');
        break;
      } else {
        // 只是单空，表示一个子串结束
        result.push_back(L'\0');
        offset++;
        pCur++;
        // 继续下一个子串
        continue;
      }
    }

    // 读当前子串直到遇到 '\0'
    std::string oneString;
    while (offset < cbData && pCur[0] != '\0') {
      oneString.push_back(pCur[0]);
      offset++;
      pCur++;
    }
    // 把当前子串转成宽字符
    if (!oneString.empty()) {
      int needed = MultiByteToWideChar(CP_ACP, 0, oneString.c_str(), (int)oneString.size(), nullptr, 0);
      if (needed > 0) {
        size_t oldSize = result.size();
        result.resize(oldSize + needed);
        MultiByteToWideChar(CP_ACP, 0, oneString.c_str(), (int)oneString.size(), &result[oldSize], needed);
      }
    }
    // 遇到 '\0' 就会在下一轮 or 条件里处理 => append L'\0'
    // 循环回到上面的 if(pCur[0]=='\0') 分支
  }

  // 如果 result 全空，也要保证有双空终止
  if (result.empty()) {
    result.push_back(L'\0');
    result.push_back(L'\0');
  }

  return result;
}

export {
  auto WINAPI RegCreateKeyExInternalW_mod(
      HKEY hKey, LPCWSTR lpSubKey, DWORD Reserved, LPWSTR lpClass, DWORD dwOptions, REGSAM samDesired,
      const LPSECURITY_ATTRIBUTES lpSecurityAttributes, PHKEY phkResult, LPDWORD lpdwDisposition,
      LPVOID lpReserved
  ) {
    return regHandle.RegCreateKeyExInternalW(
        hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired, lpSecurityAttributes, phkResult,
        lpdwDisposition, lpReserved
    );
  }

  auto WINAPI RegOpenKeyExW_mod(
      HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult
  ) {
    return regHandle.RegOpenKeyExW(hKey, lpSubKey, ulOptions, samDesired, phkResult);
  }

  auto WINAPI RegQueryInfoKeyW_mod(
      HKEY hKey, LPWSTR lpClass, LPDWORD lpcClass, LPDWORD lpReserved, LPDWORD lpcSubKeys,
      LPDWORD lpcMaxSubKeyLen, LPDWORD lpcMaxClassLen, LPDWORD lpcValues, LPDWORD lpcMaxValueNameLen,
      LPDWORD lpcMaxValueLen, LPDWORD lpcbSecurityDescriptor, PFILETIME lpftLastWriteTime
  ) {
    return regHandle.RegQueryInfoKeyW(
        hKey, lpClass, lpcClass, lpReserved, lpcSubKeys, lpcMaxSubKeyLen, lpcMaxClassLen, lpcValues,
        lpcMaxValueNameLen, lpcMaxValueLen, lpcbSecurityDescriptor, lpftLastWriteTime
    );
  }

  auto WINAPI RegQueryValueExW_mod(
      HKEY hKey, LPCWSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData
  ) {
    return regHandle.RegQueryValueExW(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
  }

  auto WINAPI RegEnumKeyExW_mod(
      HKEY hKey, DWORD dwIndex, LPWSTR lpName, LPDWORD lpcName, LPDWORD lpReserved, LPWSTR lpClass,
      LPDWORD lpcClass, PFILETIME lpftLastWriteTime
  ) {
    return regHandle.RegEnumKeyExW(
        hKey, dwIndex, lpName, lpcName, lpReserved, lpClass, lpcClass, lpftLastWriteTime
    );
  }
  LSTATUS WINAPI RegEnumValueW_mod(
      HKEY hKey, DWORD dwIndex, LPWSTR lpValueName, LPDWORD lpcchValueName, LPDWORD lpReserved,
      LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData
  ) {
    return regHandle.RegEnumValueW(
        hKey, dwIndex, lpValueName, lpcchValueName, lpReserved, lpType, lpData, lpcbData
    );
  }
  LSTATUS WINAPI RegSetValueExW_mod(
      HKEY hKey, LPCWSTR lpValueName, DWORD Reserved, DWORD dwType, const BYTE* lpData, DWORD cbData
  ) {
    return regHandle.RegSetValueExW(hKey, lpValueName, Reserved, dwType, lpData, cbData);
  }
  // ******************************************************************
  // for the ANSI func, convert the input to unicode and call the unicode func
  // 有output的情况下，需要把unicode转回ansi
  // ******************************************************************
  LONG WINAPI RegCreateKeyExInternalA_mod(
      HKEY hKey, LPCSTR lpSubKey, DWORD Reserved, LPSTR lpClass, DWORD dwOptions, REGSAM samDesired,
      const LPSECURITY_ATTRIBUTES lpSecurityAttributes, PHKEY phkResult, LPDWORD lpdwDisposition,
      LPVOID lpReserved  // 保持和W_mod一致
  ) {
    // 1. 转换输入为宽字符
    auto wSubKey = strConvert(lpSubKey);
    auto wClass = strConvert(lpClass);

    // 2. 调用W版
    LONG ret = RegCreateKeyExInternalW_mod(
        hKey, wSubKey.get(), Reserved, wClass.get(), dwOptions, samDesired, lpSecurityAttributes, phkResult,
        lpdwDisposition, lpReserved
    );

    return ret;
  }
  LONG WINAPI RegOpenKeyExA_mod(
      HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult
  ) {
    auto wSubKey = strConvert(lpSubKey);

    return RegOpenKeyExW_mod(hKey, wSubKey.get(), ulOptions, samDesired, phkResult);
  }
  LONG WINAPI RegQueryInfoKeyA_mod(
      HKEY hKey, LPSTR lpClass, LPDWORD lpcchClass, LPDWORD lpReserved, LPDWORD lpcSubKeys,
      LPDWORD lpcbMaxSubKeyLen, LPDWORD lpcbMaxClassLen, LPDWORD lpcValues, LPDWORD lpcbMaxValueNameLen,
      LPDWORD lpcbMaxValueLen, LPDWORD lpcbSecurityDescriptor, PFILETIME lpftLastWriteTime
  ) {
    LONG ret;
    DWORD cchClassW = 0;
    std::vector<WCHAR> tmpClassW;

    // 如果调用方想要class( lpClass != nullptr 并且 lpcchClass != nullptr )
    if (lpClass && lpcchClass && *lpcchClass > 0) {
      cchClassW = *lpcchClass;          // lpcchClass 里是ANSI字符数, 先直接拿来当WCHAR最大值
      tmpClassW.resize(cchClassW + 1);  // +1存放终止符
    }

    ret = RegQueryInfoKeyW(
        hKey, tmpClassW.empty() ? nullptr : tmpClassW.data(), tmpClassW.empty() ? nullptr : &cchClassW,
        lpReserved, lpcSubKeys, lpcbMaxSubKeyLen, lpcbMaxClassLen, lpcValues, lpcbMaxValueNameLen,
        lpcbMaxValueLen, lpcbSecurityDescriptor, lpftLastWriteTime
    );

    if (ret == ERROR_SUCCESS && !tmpClassW.empty()) {
      // cchClassW是实际返回的宽字符数，不含终止符(官方文档)
      // 手动加个终止符，防御安全
      if (cchClassW < tmpClassW.size()) {
        tmpClassW[cchClassW] = L'\0';
      } else {
        // 出于安全，写不了就不写
      }

      // 转回ANSI
      int neededA = WideCharToMultiByte(
          CP_ACP, 0, tmpClassW.data(), (int)cchClassW, lpClass, (lpcchClass ? *lpcchClass : 0), nullptr,
          nullptr
      );

      if (neededA > 0 && neededA < (int)*lpcchClass) {
        lpClass[neededA] = '\0';  // 终止符
        *lpcchClass = neededA;
      } else {
        // 缓冲不足
        ret = ERROR_MORE_DATA;
      }
    }

    return ret;
  }
  LONG WINAPI RegEnumKeyExA_mod(
      HKEY hKey, DWORD dwIndex, LPSTR lpName, LPDWORD lpcName, LPDWORD lpReserved, LPSTR lpClass,
      LPDWORD lpcClass, PFILETIME lpftLastWriteTime
  ) {
    if (!lpName || !lpcName) {
      return ERROR_INVALID_PARAMETER;
    }

    DWORD cchNameW = *lpcName;
    std::vector<WCHAR> tmpNameW(cchNameW + 1);

    DWORD cchClassW = 0;
    std::vector<WCHAR> tmpClassW;
    if (lpClass && lpcClass && *lpcClass > 0) {
      cchClassW = *lpcClass;
      tmpClassW.resize(cchClassW + 1);
    }

    LONG ret = RegEnumKeyExW(
        hKey, dwIndex, tmpNameW.data(), &cchNameW, lpReserved, tmpClassW.empty() ? nullptr : tmpClassW.data(),
        tmpClassW.empty() ? nullptr : &cchClassW, lpftLastWriteTime
    );

    if (ret == ERROR_SUCCESS) {
      // 转换name
      if (cchNameW <= tmpNameW.size()) {
        tmpNameW[cchNameW] = L'\0';
      }
      int neededNameA =
          WideCharToMultiByte(CP_ACP, 0, tmpNameW.data(), cchNameW, lpName, *lpcName, nullptr, nullptr);
      if (neededNameA >= 0 && neededNameA < (int)*lpcName) {
        lpName[neededNameA] = '\0';
        *lpcName = neededNameA;
      } else {
        ret = ERROR_MORE_DATA;
      }

      // 转换class
      if (ret == ERROR_SUCCESS && !tmpClassW.empty()) {
        if (cchClassW <= tmpClassW.size()) {
          tmpClassW[cchClassW] = L'\0';
        }
        int neededClassA = WideCharToMultiByte(
            CP_ACP, 0, tmpClassW.data(), cchClassW, lpClass, (lpcClass ? *lpcClass : 0), nullptr, nullptr
        );
        if (lpcClass && neededClassA >= 0 && neededClassA < (int)*lpcClass) {
          lpClass[neededClassA] = '\0';
          *lpcClass = neededClassA;
        } else {
          ret = ERROR_MORE_DATA;
        }
      }
    }

    return ret;
  }

  LONG WINAPI RegQueryValueExA_mod(
      HKEY hKey, LPCSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData
  ) {
    if (!lpValueName) {
      return ERROR_INVALID_PARAMETER;
    }

    // 1) 转换ValueName
    std::wstring wValName = AnsiToWide(lpValueName);

    // 2) 先探测所需大小
    DWORD dwType = 0;
    DWORD cbNeeded = 0;
    LONG ret = RegQueryValueExW(hKey, wValName.c_str(), lpReserved, &dwType, nullptr, &cbNeeded);

    if (ret != ERROR_SUCCESS && ret != ERROR_MORE_DATA) {
      return ret;
    }

    // 把探测到的type和size写回
    if (lpType)
      *lpType = dwType;
    if (lpcbData) {
      // 如果lpData==nullptr，调用方仅仅想获取大小和类型
      if (!lpData) {
        *lpcbData = cbNeeded;
        return ret;
      }
    } else {
      // 调用方不关心大小，但给了lpData，这样也能尝试一次
    }

    // 3) 分配临时缓冲区（cbNeeded 可能是0表示值为空字符串）
    std::vector<BYTE> tmpBuf(cbNeeded + 2);  // 给MULTI_SZ等多留点空间

    // 4) 再次调用W版获取数据
    DWORD dwType2 = 0;
    DWORD cbActual = cbNeeded;  // second call
    ret = RegQueryValueExW(hKey, wValName.c_str(), lpReserved, &dwType2, tmpBuf.data(), &cbActual);
    if (ret != ERROR_SUCCESS) {
      return ret;
    }

    // 再次写回type
    if (lpType) {
      *lpType = dwType2;
    }

    // 5) 根据类型判断
    if (dwType2 == REG_SZ || dwType2 == REG_EXPAND_SZ) {
      // tmpBuf里是一个以L'\0'终止的宽字符串
      // 转成ANSI
      LPCWSTR wData = reinterpret_cast<LPCWSTR>(tmpBuf.data());

      // 安全获取宽字符长度(可能包括终止符)
      size_t cchW = 0;
      if (cbActual >= 2) {
        // 宽字符个数 = cbActual / sizeof(WCHAR)，还要保证不越界
        cchW = cbActual / sizeof(WCHAR);
      }

      // 转ANSI(单字符串)
      int neededA = 0;
      if (cchW > 0) {
        // 不要用wcslen()盲取，因为REG_SZ 的数据不一定都写满?
        // 最安全是把末尾多余空间补0
        // 这里假设系统保证值结尾有\0
        neededA = WideCharToMultiByte(
            CP_ACP, 0, wData,
            (int)(cchW - 1),  // -1 去掉终止符？
            reinterpret_cast<LPSTR>(lpData), (lpcbData ? *lpcbData : 0), nullptr, nullptr
        );
      }

      if (neededA < 0 || (lpcbData && (DWORD)neededA >= *lpcbData)) {
        // 缓冲不够
        ret = ERROR_MORE_DATA;
      } else if (lpData) {
        // 写ANSI终止符
        lpData[neededA] = 0;
        if (lpcbData) {
          *lpcbData = neededA;
        }
      }
    } else if (dwType2 == REG_MULTI_SZ) {
      // tmpBuf里是一个多字符串(L'\0'分隔, 双 L'\0' 结尾)
      LPCWSTR wMulti = reinterpret_cast<LPCWSTR>(tmpBuf.data());
      size_t cchW = cbActual / sizeof(WCHAR);

      // 将“宽多字符串” 转成 “ANSI多字符串”
      // 形如: Wide1\0Wide2\0Wide3\0\0 => Ansi1\0Ansi2\0Ansi3\0\0
      std::string ansiMulti = WideMultiSzToAnsiMultiSz(wMulti, cchW);

      DWORD needed = (DWORD)ansiMulti.size();  // 包含最后的双 \0

      if (lpcbData && needed > *lpcbData) {
        // 缓冲不够
        ret = ERROR_MORE_DATA;
      } else {
        // 拷贝
        if (lpData) {
          memcpy(lpData, ansiMulti.data(), needed);
        }
        if (lpcbData) {
          *lpcbData = needed;
        }
      }
    } else {
      // 非字符串类(含REG_BINARY, REG_DWORD, REG_QWORD, etc.), 直接复制
      if (lpcbData && cbActual > *lpcbData) {
        ret = ERROR_MORE_DATA;
      } else {
        if (lpData) {
          memcpy(lpData, tmpBuf.data(), cbActual);
        }
        if (lpcbData) {
          *lpcbData = cbActual;
        }
      }
    }

    return ret;
  }

  LSTATUS WINAPI RegEnumValueA_mod(
      HKEY hKey, DWORD dwIndex, LPSTR lpValueName, LPDWORD lpcchValueName, LPDWORD lpReserved, LPDWORD lpType,
      LPBYTE lpData,
      LPDWORD lpcbData
  ) {  // 1. 先用宽缓冲枚举 ValueName
    DWORD cchNameW = (lpcchValueName && *lpcchValueName) ? *lpcchValueName : 0;
    std::vector<WCHAR> nameBufW(cchNameW + 2);  // 多留2个

    // 2. 准备 Data 缓冲
    DWORD cbDataTmp = (lpcbData ? *lpcbData : 0);
    std::vector<BYTE> tmpDataBuf(cbDataTmp + 2);

    DWORD dwTypeLocal = 0;
    LONG ret = RegEnumValueW_mod(
        hKey, dwIndex, nameBufW.data(), &cchNameW, lpReserved, &dwTypeLocal, tmpDataBuf.data(), &cbDataTmp
    );
    if (ret != ERROR_SUCCESS)
      return ret;

    // 3. 处理 ValueName(Unicode->ANSI)
    if (lpValueName && lpcchValueName && cchNameW > 0) {
      // 追加终止符
      if (cchNameW < nameBufW.size()) {
        nameBufW[cchNameW] = L'\0';
      }
      int neededA = WideCharToMultiByte(
          CP_ACP, 0, nameBufW.data(), cchNameW, lpValueName, *lpcchValueName, nullptr, nullptr
      );
      if (neededA >= 0 && (DWORD)neededA < *lpcchValueName) {
        lpValueName[neededA] = '\0';
        *lpcchValueName = neededA;
      } else {
        // 缓冲不够
        ret = ERROR_MORE_DATA;
      }
    } else {
      // 如果没给指针或大小, 直接不处理
      if (lpcchValueName)
        *lpcchValueName = 0;
    }

    if (lpType) {
      *lpType = dwTypeLocal;
    }

    if (ret != ERROR_SUCCESS) {
      return ret;
    }

    // 4. 根据dwTypeLocal 处理 data
    if (lpData && lpcbData) {
      if (dwTypeLocal == REG_SZ || dwTypeLocal == REG_EXPAND_SZ) {
        // tmpDataBuf -> 单宽字符字符串
        LPCWSTR wData = reinterpret_cast<LPCWSTR>(tmpDataBuf.data());
        size_t cchW = cbDataTmp / sizeof(WCHAR);

        if (cchW > 0) {
          // 追加终止符
          // (注意REG_SZ一般保证\0终止, 但防御起见多写)
          if (cchW + 1 < tmpDataBuf.size() / sizeof(WCHAR)) {
            ((LPWSTR)tmpDataBuf.data())[cchW] = L'\0';
          }
          int neededA = WideCharToMultiByte(
              CP_ACP, 0, wData, (int)(cchW - 1),  // -1排除末尾'\0'
              reinterpret_cast<LPSTR>(lpData), *lpcbData, nullptr, nullptr
          );
          if (neededA >= 0 && (DWORD)neededA < *lpcbData) {
            lpData[neededA] = 0;
            *lpcbData = neededA;
          } else {
            ret = ERROR_MORE_DATA;
          }
        } else {
          // 空字符串
          if (*lpcbData > 0) {
            lpData[0] = 0;
          }
          *lpcbData = 1;  // 仅 '\0'
        }
      } else if (dwTypeLocal == REG_MULTI_SZ) {
        LPCWSTR wMulti = reinterpret_cast<LPCWSTR>(tmpDataBuf.data());
        size_t cchW = cbDataTmp / sizeof(WCHAR);

        std::string ansiMulti = WideMultiSzToAnsiMultiSz(wMulti, cchW);
        DWORD needed = (DWORD)ansiMulti.size();  // 包含双'\0'

        if (needed > *lpcbData) {
          ret = ERROR_MORE_DATA;
        } else {
          memcpy(lpData, ansiMulti.data(), needed);
          *lpcbData = needed;
        }
      } else {
        // 其他直接复制
        if (cbDataTmp > *lpcbData) {
          ret = ERROR_MORE_DATA;
        } else {
          memcpy(lpData, tmpDataBuf.data(), cbDataTmp);
          *lpcbData = cbDataTmp;
        }
      }
    }
    return ret;
  }
  LSTATUS WINAPI RegSetValueExA_mod(
      HKEY hKey, LPCSTR lpValueName, DWORD Reserved, DWORD dwType, const BYTE* lpData, DWORD cbData
  ) {
    // 1) 转换 ValueName => 宽字符
    //   如果 lpValueName 为空或字符串空, wValueName 为空, 传空指针也行
    std::wstring wValueName;
    if (lpValueName && *lpValueName) {
      int needed = MultiByteToWideChar(CP_ACP, 0, lpValueName, -1, nullptr, 0);
      if (needed > 0) {
        wValueName.resize(needed - 1);  // -1因为needed包含终止符
        MultiByteToWideChar(CP_ACP, 0, lpValueName, -1, &wValueName[0], needed);
      }
    }

    // 如果没有数据或数据长度=0, 可以直接调用W版(有些类型可写空值)
    if (!lpData || cbData == 0) {
      return RegSetValueExW_mod(
          hKey, wValueName.empty() ? nullptr : wValueName.c_str(), Reserved, dwType, nullptr, 0
      );
    }

    // 2) 根据dwType判断是否要把 lpData (ANSI) 转成宽字符
    if (dwType == REG_SZ || dwType == REG_EXPAND_SZ) {
      // 2.1 单字符串: ANSI => Unicode
      // cbData是包含终止符的长度(字节数)? WinAPI里写字符串时一般是字节数(含\0),
      //   需转化成 wchar_t length.
      //   先把原ANSI字符串变成 std::wstring
      //   (也可以直接MultiByteToWideChar, 看你习惯)
      const char* ansiStr = reinterpret_cast<const char*>(lpData);

      int neededW = MultiByteToWideChar(
          CP_ACP, 0, ansiStr,
          (int)cbData / sizeof(char),  // 这个是估计, -1 也可以
          nullptr, 0
      );
      if (neededW <= 0) {
        // 转换失败, 你可以根据需求返回ERROR_INVALID_PARAMETER
        return ERROR_INVALID_PARAMETER;
      }

      // 分配临时宽缓冲
      std::vector<wchar_t> tmpW(neededW + 1);  // +1防止尾部越界
      MultiByteToWideChar(CP_ACP, 0, ansiStr, (int)cbData / sizeof(char), tmpW.data(), neededW);

      // 可能最后不带 \0, 这里补上
      tmpW[neededW] = L'\0';

      // 2.2 现在调用W版时, lpData要传tmpW.data(),
      // 长度是(neededW+1)*sizeof(wchar_t)
      DWORD cbDataW = (neededW + 1) * sizeof(wchar_t);

      return RegSetValueExW_mod(
          hKey, wValueName.empty() ? nullptr : wValueName.c_str(), Reserved, dwType,
          reinterpret_cast<const BYTE*>(tmpW.data()), cbDataW
      );
    } else if (dwType == REG_MULTI_SZ) {
      // 3) 多字符串: ANSI MultiSZ => Unicode MultiSZ
      // cbData = 字节数(含所有子串+双\0)
      // 调用前面定义的 AnsiMultiSzToWideMultiSz
      const char* ansiMulti = reinterpret_cast<const char*>(lpData);
      std::vector<wchar_t> wMulti = AnsiMultiSzToWideMultiSz(ansiMulti, cbData);

      // 要把 wMulti 传给 W 版
      DWORD cbDataW = (DWORD)(wMulti.size() * sizeof(wchar_t));  // vector里含双空结尾

      return RegSetValueExW_mod(
          hKey, wValueName.empty() ? nullptr : wValueName.c_str(), Reserved, dwType,
          reinterpret_cast<const BYTE*>(wMulti.data()), cbDataW
      );
    } else {
      // 4) 其他类型(如 REG_BINARY, REG_DWORD, etc.), 直接原样写即可
      return RegSetValueExW_mod(
          hKey, wValueName.empty() ? nullptr : wValueName.c_str(), Reserved, dwType, lpData, cbData
      );
    }
  }
}
