#include <Windows.h>
#include <windowsx.h>
#include <tchar.h>
#include "resource.h"
#include "..\Mydll\Mydll.h"

#define chHANDLE_DLGMSG(hWnd, message, fn)                 \
   case (message): return (SetDlgMsgResult(hWnd, uMsg,     \
      HANDLE_##message((hWnd), (wParam), (lParam), (fn))))

HANDLE g_hMainDlg;

HANDLE g_hFile;
#define BUFSIZE 2048

WCHAR* AtoW(CHAR* input){
	WCHAR *wString;

	return wString;
}

BOOL readFile(HWND hwnd){
	BOOL bSuccess;
	CHAR chBuf[BUFSIZE];
	DWORD dwRead, dwWritten;

	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	for (;;){
		// 파일 읽어서 chBuf에 저장하고 읽은 크기를 dwRead에 저장
		bSuccess = ReadFile(g_hFile, chBuf, BUFSIZE, &dwRead, NULL);
		if (!bSuccess || dwRead == 0) break;

		// STDOUT 핸들에 chBuf 내용 쓰기
		//bSuccess = WriteFile(hStdout, chBuf, dwRead, &dwWritten, NULL);
		bSuccess = SetWindowTextA(GetDlgItem(hwnd, IDC_CONTENT), chBuf);
		if (!bSuccess)
			break;
	}
	return TRUE;
}

BOOL reloadFile(HWND hwnd){
	CFStruct myST = { 0, };
	if (g_hFile == NULL){
		myST.processId = GetCurrentProcessId();
		g_hFile = Client(myST);
		if (g_hFile == NULL){
			SetWindowTextW(hwnd, _T("Fail"));
			return(FALSE);
		}
	}
	SetWindowTextW(hwnd, _T("Hosts"));
	readFile(hwnd);
	return TRUE;
}

// 다이얼로그 초기화
BOOL Dlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam) {
	/*
	1. 
	*/
	SetWindowText(GetDlgItem(hwnd, IDC_CONTENT), TEXT(""));
	CFStruct myST = { 0, };
	myST.processId = GetCurrentProcessId();
	g_hFile = Client(myST);
	if (g_hFile == NULL){
		SetWindowTextW(hwnd, _T("Fail"));
		return(TRUE);
	}
	SetWindowTextW(hwnd, _T("Hosts"));
	g_hMainDlg = hwnd;
	readFile(hwnd);

	return(TRUE);
}


///////////////////////////////////////////////////////////////////////////////


void Dlg_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify) {

	switch (id) {
	case IDCANCEL:
		CloseHandle(g_hFile);
		EndDialog(hwnd, id);
		break;
	case IDC_REFRESH:
		reloadFile(hwnd);
		break;
	case IDC_SAVE:
		break;
	}
}


INT_PTR WINAPI Dlg_Proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		chHANDLE_DLGMSG(hwnd, WM_INITDIALOG, Dlg_OnInitDialog);
		chHANDLE_DLGMSG(hwnd, WM_COMMAND, Dlg_OnCommand);
	}

	return(FALSE);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow){
	LocalAlloc(LOCALE_ALL, 0);
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG), NULL, Dlg_Proc);
	return 0;
}