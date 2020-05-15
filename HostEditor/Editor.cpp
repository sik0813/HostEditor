#include <Windows.h>
#include <windowsx.h>
#include "resource.h"
#include "..\Mydll\Mydll.h"

#define chHANDLE_DLGMSG(hWnd, message, fn)                 \
   case (message): return (SetDlgMsgResult(hWnd, uMsg,     \
      HANDLE_##message((hWnd), (wParam), (lParam), (fn))))

HANDLE g_hMainDlg;

HANDLE g_hFile;
#define BUFSIZE 2048
CONST LPWSTR FileName = L"C:\\Users\\newlcb0813\\Desktop\\hosts";

LPWSTR AtoW(CHAR *input, UINT strLength){
	WCHAR wString[BUFSIZE] = { 0, };
	int nLen = MultiByteToWideChar(CP_ACP, 0, input, strLength, NULL, NULL);
	MultiByteToWideChar(CP_ACP, 0, input, strLength, wString, nLen);
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
		if (dwRead < BUFSIZE){
			chBuf[dwRead] = '\0';
		}

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
		StringCchCopyW(myST.FileName, FILE_LENGTH, FileName);
		g_hFile = Client(myST);
		if (g_hFile == NULL){
			MessageBoxW(hwnd, L"파일 핸들 에러", L"Fail", MB_OK);
			return(FALSE);
		}
	}
	readFile(hwnd);
	MessageBoxW(hwnd, L"새로고침 완료", L"Success", MB_OK);
	return TRUE;
}

// 다이얼로그 초기화
BOOL Dlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam) {
	/*
	1. 
	*/
	SetWindowTextW(hwnd, L"Hosts");
	SetWindowText(GetDlgItem(hwnd, IDC_CONTENT), TEXT(""));
	CFStruct myST = { 0, };
	myST.processId = GetCurrentProcessId();
	StringCchCopyW(myST.FileName, FILE_LENGTH, FileName);
	g_hFile = Client(myST);
	if (g_hFile == NULL){
		MessageBoxW(hwnd, L"파일 핸들 에러", L"Fail", MB_OK);
		return(TRUE);
	}
	g_hMainDlg = hwnd;
	readFile(hwnd);
	MessageBoxW(hwnd, L"열기 완료", L"Success", MB_OK);
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