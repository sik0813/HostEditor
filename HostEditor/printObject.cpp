#include "printObject.h"

BOOL printObject::printInit()
{
	HANDLE printerHandle = NULL;
	BOOL successFuc = OpenPrinter(L"dd", &printerHandle, NULL);
	
	return TRUE;
}