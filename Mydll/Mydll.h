#include <Windows.h>
#include <stdio.h>
#include <strsafe.h>

#ifdef MYDLL_EXPORTS
#define MYDLL_API __declspec(dllexport)
#else
#define MYDLL_API __declspec(dllimport)
#endif

#ifdef __cplusplus
#define EXPORT extern "C" MYDLL_API
#else
#define EXPORT extern MYDLL_API
#endif

#define FILE_LENGTH 1000
typedef struct _CFStruct{
	DWORD processId;
	WCHAR FileName[FILE_LENGTH];
	HANDLE Done;
} CFStruct;

#define MyPipe L"\\\\.\\pipe\\MyPipe"

void CALLBACK authStart(HINSTANCE hInstance, HINSTANCE prehInstande, LPWSTR lpszCmdLine, int nCmdShow);
int ConnectClient(HANDLE hNamePipe);
EXPORT int Server(void);
HANDLE ConnectServer(HANDLE hNamePipe, CFStruct SendSt);
EXPORT HANDLE Client(CFStruct inputSt);