#include "fileAuth.h"

// ���� -> �а� -> �ݱ�
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
		MessageBoxW(hwnd, L"�����ڵ� ���� ����", L"Fail", MB_OK);
		return FALSE;
	}

	successFunc = readFile(hwnd, readFileHandle);
	if (!successFunc)
	{
		MessageBoxW(hwnd, L"�����б� ����", L"Fail", MB_OK);
		return FALSE;
	}

	if (TRUE == readSuccess)
	{
		MessageBoxW(hwnd, L"���ϻ��ΰ�ħ �Ϸ�", L"Success", MB_OK);
	}
	else 
	{
		MessageBoxW(hwnd, L"���Ͽ��� �Ϸ�", L"Success", MB_OK);
		readSuccess = TRUE;
	}
	
	return TRUE;
}

// ���� �ڵ��� ������� ���� ����

BOOL fileAuth::readFile(HWND hwnd, HANDLE& readFileHandle)
{
	BOOL successFunc = FALSE;
	CHAR readFileBuf[BUFSIZE] = { 0, };
	DWORD readSize = 0;
	UINT repeatBuf = 1;
	std::string combineFileContent = "";
	while (1)
	{
		// ���� �о chBuf�� �����ϰ� ���� ũ�⸦ dwRead�� ����
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

// ���� -> ���� -> �ݱ�

BOOL fileAuth::saveData(HWND hwnd)
{
	//CHAR nowContent[BUFSIZE] = { 0, };
	DWORD wirteSize = NULL;
	BOOL successFunc = FALSE;
	CFStruct myST;
	memset(&myST, 0, sizeof(myST));
	if (!readSuccess)
	{
		MessageBoxW(hwnd, L"������ ���� �����ּ���", L"Fail", MB_OK);
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
		MessageBoxW(hwnd, L"�����ڵ� ���� ����", L"Fail", MB_OK);
		return(FALSE);
	}

	int currentLen = GetWindowTextLengthA(GetDlgItem(hwnd, IDC_CONTENT)) + 1; // ���� ����
	LPSTR nowContent = new CHAR[currentLen]; // �޸� �Ҵ�
	GetDlgItemTextA(hwnd, IDC_CONTENT, nowContent, currentLen); // ���� ������ �б�
	successFunc = WriteFile(writeFileHandle, nowContent, strlen(nowContent), &wirteSize, NULL);
	CloseHandle(writeFileHandle);
	if (!successFunc)
	{
		MessageBoxW(hwnd, L"���ϼ��� ����", L"Fail", MB_OK);
		return(FALSE);
	}

	MessageBoxW(hwnd, L"�������� �Ϸ�", L"Success", MB_OK);
	return (TRUE);
}
