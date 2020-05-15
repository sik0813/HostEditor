// Mydll.cpp : DLL ���� ���α׷��� ���� ������ �Լ��� �����մϴ�.
//

#include "stdafx.h"
#include "Mydll.h"

// Entry Point
HWND g_hwnd;
void CALLBACK authStart(HINSTANCE hPInstance, HINSTANCE hMInstance, LPSTR lpszCmdLine, int nCmdShow){
	if (strcmp(lpszCmdLine, "Server") == 0){
		g_hwnd = (HWND)hMInstance;
		if (Server() != 0){
			wprintf(L"Server Run Fail");
		}
	}
	else {
		wprintf(L"Client Want Run");
	}
}


// Server API
int ConnectClient(HANDLE hNamePipe)
{
	WCHAR receiveBuf[100];
	WCHAR sendBuf[100];
	DWORD dwRecvSize;
	DWORD dwSendSize;
	CFStruct myCF;

	memset(&myCF, 0, sizeof(myCF)); // ������ ���� �� �ʱ�ȭ
	// ����ü ����Ʈ ��
	if (!(ReadFile(
		hNamePipe,
		&myCF,
		sizeof(CFStruct),
		&dwRecvSize,
		NULL
		)))
	{
		wprintf(L"Receive error! \n");
		return -1;
	}

	HANDLE sendHandle;
	HANDLE tmpHandle;
	tmpHandle = CreateFileW(
		myCF.FileName,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (tmpHandle == INVALID_HANDLE_VALUE){
		//MessageBoxW(g_hwnd, L"CreateFile error!", L"ERROR", MB_OK);
		wprintf(L"CreateFile error! \n");
		return -1;
	}
	HANDLE targetProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, myCF.processId);
	DuplicateHandle(GetCurrentProcess(), tmpHandle, targetProcess, &sendHandle, 0, TRUE, DUPLICATE_SAME_ACCESS);
	CloseHandle(tmpHandle);
	myCF.Done = sendHandle;

	
	// ����ü ����Ʈ ��
	if (!(WriteFile(
		hNamePipe,
		&myCF,
		sizeof(CFStruct),
		&dwSendSize,
		NULL
		)))          // 
	{
		wprintf(L"Send error! \n");
		return -1;
	}
	FlushFileBuffers(hNamePipe);

	return -1;
}

EXPORT int Server(void)
{
	wprintf(L"Run Server \n");
	HANDLE hNamedPipe;
	WCHAR pipe_name[] = MyPipe;

	SECURITY_ATTRIBUTES sa = { sizeof(sa), };
	sa.bInheritHandle = TRUE;

	SECURITY_DESCRIPTOR sd;
	InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);

	// �ι�° ���� TRUE ����° ���� ���
	// ����? ���� NULL allows all access to the objeclt
	SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE);
	sa.lpSecurityDescriptor = &sd;

	hNamedPipe = CreateNamedPipe(
		pipe_name,
		PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES,
		0,
		0,
		20000,       // ��� Timeout �ð�
		&sa
		);
	if (hNamedPipe == INVALID_HANDLE_VALUE)
	{
		wprintf(L"CreateNamePipe error! \n");
		return -1;
	}
	while (1)
	{
		//������ Named Pipe�� �ڵ��� ������ �� ������ ���..
		if (!(ConnectNamedPipe(hNamedPipe, NULL)))
		{
			CloseHandle(hNamedPipe);
			return -1;
		}
		else
		{
			if (ConnectClient(hNamedPipe) == -1)
			{
				if (!DisconnectNamedPipe(hNamedPipe))
				{
					wprintf(L"DisconnectNamedPipe failed with %d.\n", GetLastError());
				}
				wprintf(L"Send & Disconnect Done\n");
			}
		}
	}
	CloseHandle(hNamedPipe);
	return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////

// Client API
EXPORT HANDLE Client(CFStruct inputSt)
{
	HANDLE hNamePipe;
	WCHAR pipe_name[] = MyPipe;
	HANDLE returnHandle;
	// �������� ������ ������ �̸����� �ڵ� ���� CallNamedPipe
	hNamePipe = CreateFile(pipe_name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hNamePipe == INVALID_HANDLE_VALUE)
	{
		wprintf(L"CreateFile error! \n");
		return NULL;
	}

	DWORD pipeMode = PIPE_READMODE_MESSAGE | PIPE_WAIT;
	if (!(SetNamedPipeHandleState(hNamePipe, &pipeMode, NULL, NULL)))
	{
		wprintf(L"SetNamedPipeHandleState error! \n");
		CloseHandle(hNamePipe);
		return NULL;
	}
	returnHandle = ConnectServer(hNamePipe, inputSt);
	CloseHandle(hNamePipe);
	HANDLE test = (void*)100;
	
	return returnHandle;
}

// Client ���� Server ����
HANDLE ConnectServer(HANDLE hNamePipe, CFStruct myCF)
{
	WCHAR receiveBuf[100];
	WCHAR sendBuf[100];
	DWORD dwRecvSize;
	DWORD dwSendSize;

	// ������ ������
	if (!(WriteFile(
		hNamePipe,
		&myCF,
		sizeof(CFStruct),
		&dwSendSize,
		NULL
		)))
	{
		wprintf(L"Send error! \n");
		return NULL;
	}

	if (!(ReadFile(
		hNamePipe,
		&myCF,
		sizeof(CFStruct),
		&dwRecvSize,
		NULL
		)))
	{
		wprintf(L"Receive error! \n");
		return NULL;
	}

	FlushFileBuffers(hNamePipe);

	return myCF.Done;
}
