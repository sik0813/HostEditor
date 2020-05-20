#include <Windows.h>
//#include <windowsx.h>
#include <strsafe.h>
#include <string>
#include "resource.h"
#include "fileAuth.h"

HANDLE gMainDlgHandle;
Client myClient;
fileAuth *myFileAuth;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow);
INT_PTR WINAPI RunProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	LocalAlloc(LOCALE_ALL, 0);
	myFileAuth = new fileAuth();
	DialogBoxW(hInstance, MAKEINTRESOURCEW(IDD_DIALOG), NULL, RunProc);
	delete myFileAuth;
	return 0;
}


INT_PTR WINAPI RunProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		OnInitDialog(hwnd, (HWND)(wParam), lParam);
		break;
	case WM_COMMAND:
		OnCommand(hwnd, (int)(LOWORD(wParam)), (HWND)(lParam), (UINT)HIWORD(wParam));
		break;
	}

	return(FALSE);
}

void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch (id)
	{
	case IDCANCEL:
	case IDOK:
		//CloseHandle(g_hFileRead); 
		//CloseHandle(g_hFileWrite);
		EndDialog(hwnd, id);
		break;
	case IDC_REFRESH:
		myFileAuth->loadFile(hwnd);
		break;
	case IDC_SAVE:
		myFileAuth->saveData(hwnd);
		break;
	case IDC_PRINT:

		break;
	}
}

// 다이얼로그 초기화

BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	BOOL successFunc = FALSE;

	SetWindowTextW(hwnd, L"Hosts");
	SetWindowText(GetDlgItem(hwnd, IDC_CONTENT), TEXT(""));

	successFunc = myFileAuth->loadFile(hwnd);
	if (FALSE == successFunc)
	{
		return FALSE;
	}

	MessageBoxW(hwnd, L"파일오픈 완료", L"Success", MB_OK);
	return(TRUE);
}