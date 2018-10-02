/************************************************************************************
This dll provides 2 abilities for an NSIS installer window:

1. Dragging ability - enable moving the main NSIS installer window from any child
window area (i.e: dragging from an upper bar window).

2. buttons hovering - enable changing buttons pictures while the mouse is 
hovering them
************************************************************************************/

#include "stdafx.h"
#include "string"
#include <map>

#undef AFX_DATA  
#define AFX_DATA AFX_EXT_DATA

using namespace std;

// the main NSIS window handle
HWND ParentWindowHandle;

// Each button as both hover and non hover pictures
struct Button {
	HWND hwnd;
	wstring no_hover_pic;
	wstring hover_pic;
};

// Data structures
map<HWND, Button> buttons;
map<HWND, WNDPROC> wnd_procs;


// A window procedure which subclasses the needed messages in order to change a button picture on hover
LRESULT APIENTRY hoverWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if ((uMsg == WM_SETCURSOR) || (uMsg == WM_MOUSELEAVE)) {
		if (buttons.find(hwnd) != buttons.end()) {
			Button curr_button = buttons[hwnd];
			wstring button_pic;
			if (uMsg == WM_SETCURSOR) {
				button_pic = curr_button.hover_pic;
			}
			else {
				button_pic = curr_button.no_hover_pic;
			}
			HANDLE image = LoadImage(0, button_pic.c_str(), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
			SendMessage(curr_button.hwnd, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)image);
		}
	}

	return CallWindowProc(wnd_procs[hwnd], hwnd, uMsg, wParam, lParam);
}


// A window procedure which subclasses the needed messages in order to drag the main window from a child window area
LRESULT APIENTRY dragWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	
	if (uMsg == WM_NCHITTEST) {
		if (hwnd == ParentWindowHandle) {
			return HTCAPTION;
		}
		else {
			SendMessage(ParentWindowHandle, WM_SYSCOMMAND, 0xf012, 0);
		}
	}
	return CallWindowProc(wnd_procs[hwnd], hwnd, uMsg, wParam, lParam);
}


// Sets a button's properties - hover and non-hover pictures. These will be used if the button was registered using 'EnableHoverPics()'
VOID __stdcall SetButton(HWND hwnd, LPCWSTR no_hover_pic, LPCWSTR hover_pic) {
	Button new_button;
	new_button.hwnd = hwnd;
	new_button.no_hover_pic = wstring(no_hover_pic);
	new_button.hover_pic = wstring(hover_pic);

	buttons[hwnd] = new_button;
}


// Sets the NSIS installer parent window, which will be the window to be dragged by any provided child windows area
VOID __stdcall SetParentWindow(HWND hwnd) {
	ParentWindowHandle = hwnd;
}

// Register a child window as an area that enabled the NSIS naim window dragging
VOID __stdcall EnableDrag(HWND hwnd) {
	wnd_procs[hwnd] = (WNDPROC)GetWindowLong(hwnd, GWL_WNDPROC);
	SetWindowLong(hwnd, GWL_WNDPROC, (LONG)dragWndProc);
}


// Register a button in order to recieve hovering events notifications
VOID __stdcall EnableHoverPics(HWND hwnd) {
	wnd_procs[hwnd] = (WNDPROC)GetWindowLong(hwnd, GWL_WNDPROC);
	SetWindowLong(hwnd, GWL_WNDPROC, (LONG)hoverWndProc);
}

#undef AFX_DATA  
#define AFX_DATA  