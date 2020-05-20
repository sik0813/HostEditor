#include <Windows.h>

class printObject 
{
public:
	printObject(HWND _mainHWND)
	{
		mainHWND = _mainHWND;

	}
	~printObject(){}

	BOOL printInit();

private:
	HWND mainHWND;


};