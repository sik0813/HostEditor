#include <Windows.h>
#include <tchar.h>

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

typedef struct _CFStruct{
	DWORD processId;
	WCHAR FileName[1000];
	HANDLE Done;
} CFStruct;

#define MyPipe L"\\\\.\\pipe\\MyPipe"

int ConnectClient(HANDLE hNamePipe);
EXPORT int Server(void);
HANDLE ConnectServer(HANDLE hNamePipe, CFStruct SendSt);
EXPORT HANDLE Client(CFStruct inputSt);