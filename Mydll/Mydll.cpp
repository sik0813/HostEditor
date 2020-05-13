// Mydll.cpp : DLL ���� ���α׷��� ���� ������ �Լ��� �����մϴ�.
//

#include "stdafx.h"
#include "Mydll.h"

// Server API

int ConnectClient(HANDLE hNamePipe)
{
	TCHAR receiveBuf[100];
	TCHAR sendBuf[100];
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
		_tprintf(_T("Receive error! \n"));
		return -1;
	}

	
	// ����ü ����Ʈ ��
	if (!(WriteFile(
		hNamePipe,
		&myCF,
		sizeof(CFStruct),
		&dwSendSize,
		NULL
		)))          // 
	{
		_tprintf(_T("Send error! \n"));
		return -1;
	}
	FlushFileBuffers(hNamePipe);

	return -1;
}

EXPORT int Server(void)
{
	_tprintf(_T("Run Server \n"));
	HANDLE hNamedPipe;
	WCHAR pipe_name[] = MyPipe;
	hNamedPipe = CreateNamedPipe(
		pipe_name,
		PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES,
		0,
		0,
		20000,       // ��� Timeout �ð�
		NULL
		);
	if (hNamedPipe == INVALID_HANDLE_VALUE)
	{
		_tprintf(_T("CreateNamePipe error! \n"));
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
					_tprintf(_T("DisconnectNamedPipe failed with %d.\n"), GetLastError());
				}
				_tprintf(_T("Send & Disconnect Done\n"));
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
		_tprintf(_T("CreateFile error! \n"));
		return NULL;
	}

	DWORD pipeMode = PIPE_READMODE_MESSAGE | PIPE_WAIT;
	if (!(SetNamedPipeHandleState(hNamePipe, &pipeMode, NULL, NULL)))
	{
		_tprintf(_T("SetNamedPipeHandleState error! \n"));
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
	TCHAR receiveBuf[100];
	TCHAR sendBuf[100];
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
		_tprintf(_T("Send error! \n"));
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
		_tprintf(_T("Receive error! \n"));
		return NULL;
	}

	FlushFileBuffers(hNamePipe);

	return myCF.Done;
}
