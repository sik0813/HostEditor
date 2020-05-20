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
	// 다이얼로그 초기화
	BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);

	// 열고 -> 읽고 -> 닫기
	BOOL loadFile(HWND hwnd);

	// 열고 -> 쓰고 -> 닫기
	BOOL saveData(HWND hwnd);

private:
	Client myClient;
	BOOL readSuccess = FALSE;
	LPCWSTR kFileName = L"C:\\Windows\\System32\\drivers\\etc\\hosts";
	//LPCWSTR kFileName = L"C:\\Users\\newlcb0813\\Desktop\\hostss";

	// 파일 핸들을 기반으로 파일 열기
	BOOL readFile(HWND hwnd, HANDLE& readFileHandle);
};