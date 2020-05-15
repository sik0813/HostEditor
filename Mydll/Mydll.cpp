// Mydll.cpp : DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//

#include "stdafx.h"
#include "Mydll.h"

LPCWSTR kPipeName = L"\\\\.\\pipe\\MyPipe";

// Entry Point
void CALLBACK GetAuthStart(HINSTANCE hPInstance, HINSTANCE hMInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	if (0 == strcmp(lpszCmdLine, "Server"))
	{
		if (0 != RunServer())
		{
			//MessageBoxW(g_hwnd, L"Server Run Fail!", L"ERROR", MB_OK);
			wprintf(L"Server Run Fail");
		}
	}
}

// Server API
VOID ConnectClient(HANDLE namedPipe)
{
	DWORD recvSize = 0;
	DWORD sendSize = 0;
	BOOL successFunc = FALSE;

	CFStruct myCF;
	memset(&myCF, 0, sizeof(myCF)); // 데이터 수신 전 초기화

	// 구조체 바이트 수
	successFunc = ReadFile(namedPipe, &myCF, sizeof(CFStruct), &recvSize, NULL);
	if (!successFunc)
	{
		//MessageBoxW(g_hwnd, L"Receive error!", L"ERROR", MB_OK);
		wprintf(L"Receive error! \n");
		return;
	}

	HANDLE copyHandle = INVALID_HANDLE_VALUE;
	HANDLE sendHandle = INVALID_HANDLE_VALUE;
	sendHandle = CreateFileW(
		myCF.FileName,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	
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

		successFunc = CloseHandle(sendHandle);
		sendHandle = INVALID_HANDLE_VALUE;
		if (!successFunc)
		{
			wprintf(L"CloseHandle fail! \n");
			return;
		}

		myCF.Done = copyHandle;
	}

	// 구조체 바이트 수
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

EXPORT BOOL RunServer(void)
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
		TRUE, // TRUE 세번째 인자 사용
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
		20000,       // 대기 Timeout 시간
		&sa
		);
	if (INVALID_HANDLE_VALUE == namedPipe)
	{
		wprintf(L"CreateNamePipe error! \n");
		return -1;
	}

	while (1)
	{
		//생성한 Named Pipe의 핸들을 누군가 얻어갈 때까지 대기..
		successFunc = ConnectNamedPipe(
			namedPipe,
			NULL);
		if (!successFunc)
		{
			CloseHandle(namedPipe);
			namedPipe = INVALID_HANDLE_VALUE;
			return -1;
		}
		else
		{
			ConnectClient(namedPipe);
			successFunc = DisconnectNamedPipe(namedPipe);
			if (!successFunc)
			{
				wprintf(L"DisconnectNamedPipe failed with %d.\n", GetLastError());
			}

			wprintf(L"Send & Disconnect Done\n");
		}
	}

	CloseHandle(namedPipe);
	namedPipe = INVALID_HANDLE_VALUE;
	return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////

// Client API
EXPORT HANDLE RunClient(CFStruct inputSt)
{
	BOOL sucessFunc = FALSE;

	// 서버에서 생성한 파이프 이름으로 핸들 열기 CallNamedPipe
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

// Client 에서 Server 연결
HANDLE ConnectServer(HANDLE hNamePipe, CFStruct myCF)
{
	DWORD recvSize = 0;
	DWORD sendSize = 0;
	BOOL successFunc = FALSE;

	// 전송할 데이터
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

	// 전송 받은 데이터
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
