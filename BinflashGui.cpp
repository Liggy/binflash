///////////////////////////////////////////////////////////////////////
//
//	BinflashGui.cpp
//
//		Main entry point
//
#include "BinflashGui.h"
#include "MainDialog.h"
#include "Manifest.h"

ATOM				RegisterClass(HINSTANCE hInstance);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE _hInstance;

int APIENTRY WinMain(HINSTANCE hInstance,
                HINSTANCE hPrevInstance,
                LPTSTR    lpCmdLine,
                int       nCmdShow
               )
{
   RegisterClass(hInstance);
   _hInstance = hInstance;
   InitCommonControls();
   return (int) DialogBox(hInstance, (LPCTSTR)IDD_MAINDLG, GetDesktopWindow(), (DLGPROC) MainDialog);
}

ATOM RegisterClass(HINSTANCE hInstance)
{
   WNDCLASSEX wcex;
   static char szWindowClass[] = "BinFlashGui";

   wcex.cbSize = sizeof(WNDCLASSEX); 

   wcex.style			= CS_HREDRAW | CS_VREDRAW;
   wcex.lpfnWndProc	= (WNDPROC)WndProc;
   wcex.cbClsExtra		= 0;
   wcex.cbWndExtra		= 0;
   wcex.hInstance		= hInstance;
   wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAIN));
   wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
   wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
   wcex.lpszMenuName	= NULL;
   wcex.lpszClassName	= szWindowClass;
   wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_MAIN));

   return RegisterClassEx(&wcex);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   switch (message) 
   {
   case WM_DESTROY:
      PostQuitMessage(0);
      break;
   default:
      return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}
