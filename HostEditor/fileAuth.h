#pragma once
#include <Windows.h>
#include <strsafe.h>
#include "..\Mydll\AuthAPI.h"
#include "resource.h"
#include <string>

#define BUFSIZE 4096

class fileAuth {
public:
	fileAuth() {

	}
	~fileAuth() {

	}
	// ���̾�α� �ʱ�ȭ
	BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);

	// ���� -> �а� -> �ݱ�
	BOOL loadFile(HWND hwnd);

	// ���� -> ���� -> �ݱ�
	BOOL saveData(HWND hwnd);

private:
	Client myClient;
	BOOL readSuccess = FALSE;
	LPCWSTR kFileName = L"C:\\Windows\\System32\\drivers\\etc\\hosts";
	//LPCWSTR kFileName = L"C:\\Users\\newlcb0813\\Desktop\\hostss";

	// ���� �ڵ��� ������� ���� ����
	BOOL readFile(HWND hwnd, HANDLE& readFileHandle);
};