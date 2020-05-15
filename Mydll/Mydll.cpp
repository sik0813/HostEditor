// Mydll.cpp : DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//

#include "stdafx.h"
#include "Mydll.h"

// Entry Point
void CALLBACK authStart(HINSTANCE hInstance, HINSTANCE prehInstande, LPSTR lpszCmdLine, int nCmdShow){
	if (strcmp(lpszCmdLine, "Server") == 0){
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

	memset(&myCF, 0, sizeof(myCF)); // 데이터 수신 전 초기화
	// 구조체 바이트 수
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
	if (tmpHandle == NULL){
		wprintf(L"CreateFile error! \n");
		return -1;
	}
	HANDLE targetProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, myCF.processId);
	DuplicateHandle(GetCurrentProcess(), tmpHandle, targetProcess, &sendHandle, 0, TRUE, DUPLICATE_SAME_ACCESS);
	CloseHandle(tmpHandle);
	myCF.Done = sendHandle;

	
	// 구조체 바이트 수
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
	hNamedPipe = CreateNamedPipe(
		pipe_name,
		PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES,
		0,
		0,
		20000,       // 대기 Timeout 시간
		NULL
		);
	if (hNamedPipe == INVALID_HANDLE_VALUE)
	{
		wprintf(L"CreateNamePipe error! \n");
		return -1;
	}
	while (1)
	{
		//생성한 Named Pipe의 핸들을 누군가 얻어갈 때까지 대기..
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
	// 서버에서 생성한 파이프 이름으로 핸들 열기 CallNamedPipe
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

// Client 에서 Server 연결
HANDLE ConnectServer(HANDLE hNamePipe, CFStruct myCF)
{
	WCHAR receiveBuf[100];
	WCHAR sendBuf[100];
	DWORD dwRecvSize;
	DWORD dwSendSize;

	// 전송할 데이터
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
