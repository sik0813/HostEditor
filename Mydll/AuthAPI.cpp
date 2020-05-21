#include "stdafx.h"
#include "AuthAPI.h"

// Server API
int Server::RunServer(void)
{
	wprintf(L"Run Server \n");

	SECURITY_DESCRIPTOR sd;
	BOOL successFunc = InitializeSecurityDescriptor(
		&sd,
		SECURITY_DESCRIPTOR_REVISION);
	if (!successFunc)
	{
		wprintf(L"InitializeSecurityDescriptor Fail \n");
		return -1;
	}

	successFunc = SetSecurityDescriptorDacl(
		&sd,
		TRUE, // TRUE ����° ���� ���
		NULL, // NULL allows all access to the objeclt
		FALSE);
	if (!successFunc)
	{
		wprintf(L"SetSecurityDescriptorDacl Fail \n");
		return -1;
	}

	SECURITY_ATTRIBUTES sa;
	memset(&sa, 0, sizeof(SECURITY_ATTRIBUTES));
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = &sd;

	HANDLE namedPipe = CreateNamedPipe(
		kPipeName,
		PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES,
		0,
		0,
		20000,       // ��� Timeout �ð�
		&sa
	);
	if (INVALID_HANDLE_VALUE == namedPipe)
	{
		wprintf(L"CreateNamePipe error! \n");
		return -1;
	}

	while (1)
	{
		//������ Named Pipe�� �ڵ��� ������ �� ������ ���..
		successFunc = ConnectNamedPipe(
			namedPipe,
			NULL);
		if (!successFunc)
		{
			CloseHandle(namedPipe);
			namedPipe = INVALID_HANDLE_VALUE;
			return -1;
		}
		ConnectClient(namedPipe);
		successFunc = DisconnectNamedPipe(namedPipe);
		if (!successFunc)
		{
			wprintf(L"DisconnectNamedPipe failed with %d.\n", GetLastError());
		}

		wprintf(L"Send & Disconnect Done\n");

	}

	CloseHandle(namedPipe);
	namedPipe = INVALID_HANDLE_VALUE;
	return 0;
}

VOID Server::ConnectClient(HANDLE namedPipe)
{
	DWORD recvSize = 0;
	DWORD sendSize = 0;
	BOOL successFunc = FALSE;

	CFStruct myCF;
	memset(&myCF, 0, sizeof(myCF)); // ������ ���� �� �ʱ�ȭ

									// ����ü ����Ʈ ��
	successFunc = ReadFile(namedPipe, &myCF, sizeof(CFStruct), &recvSize, NULL);
	if (!successFunc)
	{
		//MessageBoxW(g_hwnd, L"Receive error!", L"ERROR", MB_OK);
		wprintf(L"Receive error! \n");
		return;
	}

	HANDLE copyHandle = INVALID_HANDLE_VALUE;
	HANDLE sendHandle = INVALID_HANDLE_VALUE;
	if (TRUE == myCF.isRead) // read flag�� ���� �з�
	{
		sendHandle = CreateFileW(
			myCF.FileName,
			GENERIC_READ,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
	}
	else
	{
		sendHandle = CreateFileW(
			myCF.FileName,
			GENERIC_WRITE,
			0,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
	}
	if (INVALID_HANDLE_VALUE == sendHandle)
	{
		//MessageBoxW(g_hwnd, L"CreateFile Fail", L"ERROR", MB_OK);
		wprintf(L"CreateFile Fail\n");
		myCF.Done = INVALID_HANDLE_VALUE;
	}
	else
	{
		HANDLE targetProcess = OpenProcess(
			PROCESS_ALL_ACCESS,
			TRUE,
			myCF.processId);
		if (NULL == targetProcess)
		{
			wprintf(L"OpenProcess fail: %d \n", GetLastError());
			return;
		}

		successFunc = DuplicateHandle(
			GetCurrentProcess(),
			sendHandle,
			targetProcess,
			&copyHandle,
			0,
			TRUE,
			DUPLICATE_SAME_ACCESS);
		if (!successFunc)
		{
			wprintf(L"DuplicateHandle fail! \n");
			return;
		}

		successFunc = CloseHandle(targetProcess);
		targetProcess = INVALID_HANDLE_VALUE;
		if (!successFunc)
		{
			wprintf(L"CloseHandle fail! \n");
			return;
		}

		successFunc = CloseHandle(sendHandle);
		sendHandle = INVALID_HANDLE_VALUE;
		if (!successFunc)
		{
			wprintf(L"CloseHandle fail! \n");
			return;
		}

		myCF.Done = copyHandle;
	}

	// ����ü ����Ʈ ��
	successFunc = WriteFile(
		namedPipe,
		&myCF,
		sizeof(CFStruct),
		&sendSize,
		NULL);
	if (!successFunc)
	{
		wprintf(L"Send error! \n");
		return;
	}

	FlushFileBuffers(namedPipe);

	return;
}

// Client API
HANDLE Client::RunClient(CFStruct inputSt)
{
	BOOL sucessFunc = FALSE;

	// �������� ������ ������ �̸����� �ڵ� ���� CallNamedPipe
	HANDLE namedPipe = CreateFile(
		kPipeName,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);
	if (INVALID_HANDLE_VALUE == namedPipe)
	{
		wprintf(L"CreateFile error! \n");
		return INVALID_HANDLE_VALUE;
	}

	DWORD pipeMode = PIPE_READMODE_MESSAGE | PIPE_WAIT;
	sucessFunc = SetNamedPipeHandleState(
		namedPipe,
		&pipeMode,
		NULL,
		NULL);
	if (!sucessFunc)
	{
		wprintf(L"SetNamedPipeHandleState error! \n");
		CloseHandle(namedPipe);
		namedPipe = INVALID_HANDLE_VALUE;
		return INVALID_HANDLE_VALUE;
	}

	HANDLE returnValue = ConnectServer(namedPipe, inputSt);

	sucessFunc = CloseHandle(namedPipe);
	namedPipe = INVALID_HANDLE_VALUE;
	if (!sucessFunc)
	{
		wprintf(L"CloseHandle error! \n");
	}

	return returnValue;
}

// Client ���� Server ����

HANDLE Client::ConnectServer(HANDLE hNamePipe, CFStruct myCF)
{
	DWORD recvSize = 0;
	DWORD sendSize = 0;
	BOOL successFunc = FALSE;

	// ������ ������
	successFunc = WriteFile(
		hNamePipe,
		&myCF,
		sizeof(CFStruct),
		&sendSize,
		NULL);
	if (!successFunc)
	{
		wprintf(L"Send error! \n");
		return INVALID_HANDLE_VALUE;
	}

	// ���� ���� ������
	successFunc = ReadFile(
		hNamePipe,
		&myCF,
		sizeof(CFStruct),
		&recvSize,
		NULL);
	if (!successFunc)
	{
		wprintf(L"Receive error! \n");
		return INVALID_HANDLE_VALUE;
	}

	FlushFileBuffers(hNamePipe);

	return myCF.Done;
}
