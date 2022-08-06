#include "ServiceApp.h"
#include <tchar.h>
#include <stdio.h>
#include <Windows.h>
#include <strsafe.h>

#define SERVICE_NAME L"Safy"

int main()
{
	CServiceApp ServiceApp;
	ServiceApp.Run();
	
	return 0;
}