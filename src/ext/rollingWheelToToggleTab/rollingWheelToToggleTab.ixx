module;
#include <Windows.h>
export module rollingWheelToToggleTab;
import std;

// 全局变量
bool g_rightButtonDown = false;
bool g_shouldBlockNextRightUp = false;
HHOOK g_mouseHook = NULL;

// 模拟键盘按键
void SimulateKeyPress(WORD key, bool withCtrl = false, bool withShift = false) {
  INPUT inputs[6] = {};
  int inputCount = 0;

  // 如果需要按下Ctrl键
  if (withCtrl) {
    inputs[inputCount].type = INPUT_KEYBOARD;
    inputs[inputCount].ki.wVk = VK_CONTROL;
    inputCount++;
  }

  // 如果需要按下Shift键
  if (withShift) {
    inputs[inputCount].type = INPUT_KEYBOARD;
    inputs[inputCount].ki.wVk = VK_SHIFT;
    inputCount++;
  }

  // 按下目标键
  inputs[inputCount].type = INPUT_KEYBOARD;
  inputs[inputCount].ki.wVk = key;
  inputCount++;

  // 释放目标键
  inputs[inputCount].type = INPUT_KEYBOARD;
  inputs[inputCount].ki.wVk = key;
  inputs[inputCount].ki.dwFlags = KEYEVENTF_KEYUP;
  inputCount++;

  // 如果按下了Shift键，则释放
  if (withShift) {
    inputs[inputCount].type = INPUT_KEYBOARD;
    inputs[inputCount].ki.wVk = VK_SHIFT;
    inputs[inputCount].ki.dwFlags = KEYEVENTF_KEYUP;
    inputCount++;
  }

  // 如果按下了Ctrl键，则释放
  if (withCtrl) {
    inputs[inputCount].type = INPUT_KEYBOARD;
    inputs[inputCount].ki.wVk = VK_CONTROL;
    inputs[inputCount].ki.dwFlags = KEYEVENTF_KEYUP;
    inputCount++;
  }

  // 发送键盘输入
  SendInput(inputCount, inputs, sizeof(INPUT));
}
bool isSysApp() {
  char buf[MAX_PATH];
  GetModuleFileNameA(GetModuleHandleA(nullptr), buf, std::size(buf));
  auto prefix = "C:\\Windows\\";
  // 检查路径是否以prefix开头
  return strncmp(buf, prefix, strlen(prefix)) == 0;
}
export {
  // 鼠标钩子回调函数
  LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
      MSLLHOOKSTRUCT* mouseStruct = (MSLLHOOKSTRUCT*)lParam;

      static HWND hWnd = WindowFromPoint(mouseStruct->pt);
      switch (wParam) {
        case WM_RBUTTONDOWN:
          g_rightButtonDown = true;
          break;

        case WM_RBUTTONUP:
          g_rightButtonDown = false;
          if (g_shouldBlockNextRightUp) {
            g_shouldBlockNextRightUp = false;
            if (!isSysApp()) {
              return 1;  // TODO: 阻止右键松开事件传递给非系统应用
            }
          }
          break;

        case WM_MOUSEWHEEL:
          if (g_rightButtonDown) {
            // 聚焦光标下的窗口
            hWnd = WindowFromPoint(mouseStruct->pt);
            SetForegroundWindow(hWnd);

            // 获取滚轮方向
            short wheelDelta = HIWORD(mouseStruct->mouseData);

            if (wheelDelta > 0) {
              // 滚轮向上滚动 = Ctrl+Shift+Tab
              SimulateKeyPress(VK_TAB, true, true);
              g_shouldBlockNextRightUp = true;  // 标记需要屏蔽下一次右键松开事件
              return 1;                         // 阻止滚轮事件传递给其他应用
            } else if (wheelDelta < 0) {
              // 滚轮向下滚动 = Ctrl+Tab
              SimulateKeyPress(VK_TAB, true);
              g_shouldBlockNextRightUp = true;  // 标记需要屏蔽下一次右键松开事件
              return 1;                         // 阻止滚轮事件传递给其他应用
            }
          }
          break;
      }
    }

    // 将事件传递给下一个钩子
    return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
  }
  // 注册钩子
  extern "C" __declspec(dllexport) bool InstallHook() {
    g_mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, NULL, 0);
    return (g_mouseHook != NULL);
  }

  // 卸载钩子
  extern "C" __declspec(dllexport) void UninstallHook() {
    if (g_mouseHook) {
      UnhookWindowsHookEx(g_mouseHook);
      g_mouseHook = NULL;
    }
  }
}

// DLL入口点
BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved) {
  switch (reason) {
    case DLL_PROCESS_ATTACH:
      // setHook();  // 初始化钩子
      // g_hInstance = (HINSTANCE)hModule;
      DisableThreadLibraryCalls(hModule);  // 提高性能
      break;

    case DLL_PROCESS_DETACH:
      // removeHook();  // 清理资源
      break;
  }
  return TRUE;
}