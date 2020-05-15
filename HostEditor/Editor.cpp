#include <Windows.h>
#include <windowsx.h>
#include <strsafe.h>
#include "resource.h"
#include "..\Mydll\Mydll.h"

#define chHANDLE_DLGMSG(hWnd, message, fn)                 \
   case (message): return (SetDlgMsgResult(hWnd, uMsg,     \
      HANDLE_##message((hWnd), (wParam), (lParam), (fn))))
#define BUFSIZE 4096


HANDLE gMainDlgHandle;
LPCWSTR kFileName = L"C:\\Windows\\System32\\drivers\\etc\\hosts";
//LPCWSTR FILE_NAME = L"C:\\Users\\newlcb0813\\Desktop\\hosts";
HANDLE gReadFile;

BOOL readFile(HWND hwnd) {
	BOOL successFunc = FALSE;
	CHAR readFileBuf[BUFSIZE];
	DWORD readSize = 0;
	LPWSTR wideString = NULL;

	while (1)
	{
		// 파일 읽어서 chBuf에 저장하고 읽은 크기를 dwRead에 저장
		successFunc = ReadFile(gReadFile, readFileBuf, BUFSIZE, &readSize, NULL);
		if (!successFunc || readSize == 0)
		{
			break;
		}

		if (readSize < BUFSIZE)
		{
			readFileBuf[readSize] = '\0';
		}

		//wideString = new WCHAR[readSize];
		//MultiByteToWideChar(CP_ACP, 0, readFileBuf, readSize, wideString, readSize);

		//LPWSTR tmp = AtoW(chBuf, dwRead);

		successFunc = SetWindowTextA(GetDlgItem(hwnd, IDC_CONTENT), readFileBuf);
		//delete wideString;
		if (!successFunc)
		{
			break;
		}
	}

	successFunc = CloseHandle(gReadFile);
	gReadFile = INVALID_HANDLE_VALUE;
	if (!successFunc)
	{
		MessageBoxW(hwnd, L"CloseHandleFail", L"Fail", MB_OK);
		return(FALSE);
	}

	return TRUE;
}

BOOL reloadFile(HWND hwnd)
{
	CFStruct myST = { 0, };
	BOOL successFunc;
	myST.processId = GetCurrentProcessId();
	myST.isRead = TRUE;
	myST.Done = INVALID_HANDLE_VALUE;

	HRESULT SuccessCopyString = StringCchCopyW(myST.FileName, (wcslen(kFileName) + 1) * sizeof(WCHAR), kFileName);
	if (S_OK != SuccessCopyString)
	{
		MessageBoxW(hwnd, L"StringCchCopyW Error", L"Fail", MB_OK);
	}

	gReadFile = RunClient(myST);
	if (INVALID_HANDLE_VALUE == gReadFile)
	{
		MessageBoxW(hwnd, L"파일핸들 수신 에러", L"Fail", MB_OK);
		return(FALSE);
	}

	successFunc = readFile(hwnd);
	if (!successFunc)
	{
		MessageBoxW(hwnd, L"파일읽기 실패", L"Fail", MB_OK);
		return FALSE;
	}

	MessageBoxW(hwnd, L"파일새로고침 완료", L"Success", MB_OK);
	return TRUE;
}

// 다이얼로그 초기화
BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	BOOL successFunc = FALSE;

	SetWindowTextW(hwnd, L"Hosts");
	SetWindowText(GetDlgItem(hwnd, IDC_CONTENT), TEXT(""));

	CFStruct myST;
	memset(&myST, 0, sizeof(CFStruct));
	myST.processId = GetCurrentProcessId();
	myST.isRead = TRUE;
	myST.Done = INVALID_HANDLE_VALUE;

	HRESULT SuccessCopyString = StringCchCopyW(myST.FileName, (wcslen(kFileName) + 1) * sizeof(WCHAR), kFileName);
	if (S_OK != SuccessCopyString)
	{
		MessageBoxW(hwnd, L"StringCchCopyW Error", L"Fail", MB_OK);
	}

	gReadFile = RunClient(myST);
	if (INVALID_HANDLE_VALUE == gReadFile)
	{
		MessageBoxW(hwnd, L"파일핸들 수신 에러", L"Fail", MB_OK);
		return(TRUE);
	}

	gMainDlgHandle = hwnd;
	readFile(hwnd);
	MessageBoxW(hwnd, L"파일오픈 완료", L"Success", MB_OK);
	return(TRUE);
}


///////////////////////////////////////////////////////////////////////////////


void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch (id)
	{
	case IDCANCEL:
		//CloseHandle(g_hFileRead); 
		//CloseHandle(g_hFileWrite);
		EndDialog(hwnd, id);
		break;
	case IDC_REFRESH:
		reloadFile(hwnd);
		break;
	case IDC_SAVE:
		break;
	}
}


INT_PTR WINAPI RunnignProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		chHANDLE_DLGMSG(hwnd, WM_INITDIALOG, OnInitDialog);
		chHANDLE_DLGMSG(hwnd, WM_COMMAND, OnCommand);
	}

	return(FALSE);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	LocalAlloc(LOCALE_ALL, 0);
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG), NULL, RunnignProc);
	return 0;
}