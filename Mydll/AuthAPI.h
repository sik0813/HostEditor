#pragma once
#include "Mydll.h"

class MYDLL_API Server {
public:
	Server() {}
	~Server() {}
	// Server API
	int RunServer(void);

private:
	LPCWSTR kPipeName = L"\\\\.\\pipe\\MyPipe";

	VOID ConnectClient(HANDLE namedPipe);
};

class MYDLL_API Client {
public:
	Client() {}
	~Client() {}

	// Client API
	HANDLE RunClient(CFStruct inputSt);


private:
	LPCWSTR kPipeName = L"\\\\.\\pipe\\MyPipe";

	// Client 에서 Server 연결
	HANDLE ConnectServer(HANDLE hNamePipe, CFStruct myCF);

};