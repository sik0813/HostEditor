#include "fileAuth.h"

// 열고 -> 읽고 -> 닫기
BOOL fileAuth::loadFile(HWND hwnd)
{
	CFStruct myST;
	memset(&myST, 0, sizeof(CFStruct));
	BOOL successFunc;
	myST.processId = GetCurrentProcessId();
	myST.isRead = TRUE;
	myST.Done = INVALID_HANDLE_VALUE;

	HRESULT SuccessCopyString = StringCchCopyW(myST.FileName, (wcslen(kFileName) + 1) * sizeof(WCHAR), kFileName);
	if (S_OK != SuccessCopyString)
	{
		MessageBoxW(hwnd, L"StringCchCopyW Error", L"Fail", MB_OK);
	}

	HANDLE readFileHandle = myClient.RunClient(myST);
	if (INVALID_HANDLE_VALUE == readFileHandle)
	{
		MessageBoxW(hwnd, L"파일핸들 수신 에러", L"Fail", MB_OK);
		return FALSE;
	}

	successFunc = readFile(hwnd, readFileHandle);
	if (!successFunc)
	{
		MessageBoxW(hwnd, L"파일읽기 실패", L"Fail", MB_OK);
		return FALSE;
	}

	if (TRUE == readSuccess)
	{
		MessageBoxW(hwnd, L"파일새로고침 완료", L"Success", MB_OK);
	}
	else 
	{
		MessageBoxW(hwnd, L"파일열기 완료", L"Success", MB_OK);
		readSuccess = TRUE;
	}
	
	return TRUE;
}

// 파일 핸들을 기반으로 파일 열기

BOOL fileAuth::readFile(HWND hwnd, HANDLE& readFileHandle)
{
	BOOL successFunc = FALSE;
	CHAR readFileBuf[BUFSIZE] = { 0, };
	DWORD readSize = 0;
	UINT repeatBuf = 1;
	std::string combineFileContent = "";
	while (1)
	{
		// 파일 읽어서 chBuf에 저장하고 읽은 크기를 dwRead에 저장
		successFunc = ReadFile(readFileHandle, readFileBuf, BUFSIZE, &readSize, NULL);
		std::string tmpFileContent(readFileBuf, readFileBuf + BUFSIZE);
		combineFileContent += tmpFileContent;
		ZeroMemory(readFileBuf, BUFSIZE);
		if (!successFunc || readSize == 0)
		{
			break;
		}
	}

	successFunc = SetWindowTextA(GetDlgItem(hwnd, IDC_CONTENT), combineFileContent.c_str());
	if (!successFunc)
	{
		MessageBoxW(hwnd, L"SetWindowTextA", L"Fail", MB_OK);
		return(FALSE);
	}

	successFunc = CloseHandle(readFileHandle);
	readFileHandle = INVALID_HANDLE_VALUE;
	if (!successFunc)
	{
		MessageBoxW(hwnd, L"CloseHandleFail", L"Fail", MB_OK);
		return(FALSE);
	}

	return TRUE;
}

// 열고 -> 쓰고 -> 닫기

BOOL fileAuth::saveData(HWND hwnd)
{
	//CHAR nowContent[BUFSIZE] = { 0, };
	DWORD wirteSize = NULL;
	BOOL successFunc = FALSE;
	CFStruct myST;
	memset(&myST, 0, sizeof(myST));
	if (!readSuccess)
	{
		MessageBoxW(hwnd, L"파일을 먼저 열어주세요", L"Fail", MB_OK);
		return(FALSE);
	}

	myST.processId = GetCurrentProcessId();
	myST.isRead = FALSE;
	myST.Done = INVALID_HANDLE_VALUE;

	HRESULT SuccessCopyString = StringCchCopyW(myST.FileName, (wcslen(kFileName) + 1) * sizeof(WCHAR), kFileName);
	if (S_OK != SuccessCopyString)
	{
		MessageBoxW(hwnd, L"StringCchCopyW Error", L"Fail", MB_OK);
	}

	HANDLE writeFileHandle = myClient.RunClient(myST);
	if (INVALID_HANDLE_VALUE == writeFileHandle)
	{
		MessageBoxW(hwnd, L"파일핸들 수신 에러", L"Fail", MB_OK);
		return(FALSE);
	}

	int currentLen = GetWindowTextLengthA(GetDlgItem(hwnd, IDC_CONTENT)) + 1; // 현재 길이
	LPSTR nowContent = new CHAR[currentLen]; // 메모리 할당
	GetDlgItemTextA(hwnd, IDC_CONTENT, nowContent, currentLen); // 현재 데이터 읽기
	successFunc = WriteFile(writeFileHandle, nowContent, strlen(nowContent), &wirteSize, NULL);
	CloseHandle(writeFileHandle);
	if (!successFunc)
	{
		MessageBoxW(hwnd, L"파일수정 실패", L"Fail", MB_OK);
		return(FALSE);
	}

	MessageBoxW(hwnd, L"파일저장 완료", L"Success", MB_OK);
	return (TRUE);
}
