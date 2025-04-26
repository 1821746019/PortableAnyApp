module;
#include <Windows.h>
export module _;
import std;
import rollingWheelToToggleTab;

// 显示托盘图标
NOTIFYICONDATA CreateTrayIcon(HWND hWnd) {
  NOTIFYICONDATA nid = {0};
  nid.cbSize = sizeof(NOTIFYICONDATA);
  nid.hWnd = hWnd;
  nid.uID = 1;
  nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
  nid.uCallbackMessage = WM_USER + 1;
  nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  lstrcpy(nid.szTip, TEXT("鼠标滚轮快捷键"));
  Shell_NotifyIcon(NIM_ADD, &nid);
  return nid;
}

// 窗口消息处理函数
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
    case WM_DESTROY:
      PostQuitMessage(0);
      break;
    case WM_USER + 1:
      if (lParam == WM_RBUTTONUP) {
        POINT pt;
        GetCursorPos(&pt);
        HMENU hMenu = CreatePopupMenu();
        AppendMenu(hMenu, MF_STRING, 1, TEXT("退出"));
        SetForegroundWindow(hWnd);
        TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hWnd, NULL);
        DestroyMenu(hMenu);
      }
      break;
    case WM_COMMAND:
      if (LOWORD(wParam) == 1) {
        DestroyWindow(hWnd);
      }
      break;
    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}

// 创建隐藏窗口
HWND CreateHiddenWindow(HINSTANCE hInstance) {
  WNDCLASS wc = {0};
  wc.lpfnWndProc = WndProc;
  wc.hInstance = hInstance;
  wc.lpszClassName = TEXT("MouseHookWindowClass");
  RegisterClass(&wc);

  return CreateWindow(TEXT("MouseHookWindowClass"), TEXT(""), 0, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
  // 创建窗口
  HWND hWnd = CreateHiddenWindow(hInstance);
  if (!hWnd) {
    MessageBox(NULL, TEXT("无法创建窗口！"), TEXT("错误"), MB_ICONERROR);
    return 1;
  }

  // 创建托盘图标
  NOTIFYICONDATA nid = CreateTrayIcon(hWnd);

  // 安装钩子
  if (!InstallHook()) {
    MessageBox(NULL, TEXT("无法安装鼠标钩子！"), TEXT("错误"), MB_ICONERROR);
    return 1;
  }

  // 消息循环
  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  // 清理资源
  UninstallHook();
  Shell_NotifyIcon(NIM_DELETE, &nid);

  return (int)msg.wParam;
}