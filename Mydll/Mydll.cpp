// Mydll.cpp : DLL ���� ���α׷��� ���� ������ �Լ��� �����մϴ�.
//

#include "stdafx.h"
#include "AuthAPI.h"

// Entry Point
void CALLBACK GetAuthStart(HINSTANCE hPInstance, HINSTANCE hMInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	if (0 == strcmp(lpszCmdLine, "Server"))
	{
		Server myServer;
		if (0 != myServer.RunServer())
		{
			//MessageBoxW(g_hwnd, L"Server Run Fail!", L"ERROR", MB_OK);
			wprintf(L"Server Run Fail");
		}
	}
}