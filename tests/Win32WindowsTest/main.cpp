#include <iostream>

#include <Windows.h>

#include "EString.h"

std::wstring text_to_display_wide;

LRESULT WINAPI window_procedure(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {
  switch (message) {
    case WM_CLOSE:
      PostQuitMessage(0);
      return 0;
    case WM_PAINT: {
      PAINTSTRUCT paint_struct;
      HDC draw_context = BeginPaint(window, &paint_struct);

      TextOutW(draw_context, 5, 5, text_to_display_wide.c_str(), (int)text_to_display_wide.length());

      EndPaint(window, &paint_struct);
    } break;
  }

  return DefWindowProcW(window, message, wparam, lparam);
}

void create_window_class(String const& name) {
  std::wstring wide_name = name.encode<wchar_t>();

  WNDCLASSW wndClass;
  ZeroMemory(&wndClass, sizeof(wndClass));
  wndClass.lpszClassName = wide_name.c_str();
  wndClass.lpfnWndProc = window_procedure;
  wndClass.hInstance = GetModuleHandleW(nullptr);

  RegisterClassW(&wndClass);
}

void create_window(String const& window_class_name, String const& title) {
  HWND window = CreateWindowW(
    window_class_name.encode<wchar_t>().c_str(),
    title.encode<wchar_t>().c_str(),
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT,
    1280, 720,
    NULL, NULL, NULL, NULL
  );
  ShowWindow(window, SW_SHOW);
  UpdateWindow(window);
}

int main() {
  String window_name;
  String window_class_name;
  String text_to_display;
  
  std::cout << "Enter window name: ";
  std::cin >> window_name;

  std::cout << "Enter window class name: ";
  std::cin >> window_class_name;

  std::cout << "Enter text to display: ";
  std::cin >> text_to_display;

  text_to_display_wide = text_to_display.encode<wchar_t>();

  create_window_class(window_class_name);
  create_window(window_class_name, window_name);

  MSG msg;
  while (GetMessageW(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessageW(&msg);
  }

  return 0;
}