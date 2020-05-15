#include <Windows.h>
#include <stdio.h>

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

#define FILENAMELEN 1000

typedef struct _CFStruct{
	DWORD processId;
	WCHAR FileName[FILENAMELEN];
	HANDLE Done;
	BOOL isRead;
} CFStruct;

VOID CALLBACK GetAuthStart(HINSTANCE hPInstance, HINSTANCE hMInstance, LPSTR lpszCmdLine, int nCmdShow);
VOID ConnectClient(HANDLE hNamePipe);
EXPORT int RunServer(void);
HANDLE ConnectServer(HANDLE hNamePipe, CFStruct SendSt);
EXPORT HANDLE RunClient(CFStruct inputSt);