#include "ControlService.h"
#include <tchar.h>
#include <strsafe.h>

#define SERVICE_NAME L"Safy"
#define EVENT_NAME	L"Global\\SAFY"

DWORD Register(LPCWSTR lpszInstallPath, bool IsInstall)
{
	DWORD dwRet = ERROR_SUCCESS;
	CControlService SCM;

	if (IsInstall)
	{
		if (FALSE == SCM.IsServiceInstalled(SERVICE_NAME))
		{
			WCHAR szServicePath[MAX_PATH] = { 0 };
			StringCchPrintf(szServicePath, MAX_PATH, _T("\"%s\""), lpszInstallPath);
			dwRet = SCM.CreateService(SERVICE_NAME,
				SERVICE_NAME,
				SERVICE_WIN32_OWN_PROCESS,
				SERVICE_AUTO_START,
				(LPCWSTR)szServicePath,
				NULL,
				NULL,
				L"\0",
				NULL,
				NULL);
			if (ERROR_SUCCESS != dwRet)
			{
				//error
			}
		}
		else
		{
			dwRet = ERROR_SUCCESS;
		}
	}
	else
	{
		dwRet = SCM.DeleteService(SERVICE_NAME);
		if (ERROR_SUCCESS != dwRet)
		{
			// error
		}
	}

	return dwRet;
}

DWORD Start(LPCWSTR lpszServiceName)
{
	if (NULL == lpszServiceName)
		return ERROR_INVALID_PARAMETER;

	DWORD dwRet = ERROR_SUCCESS;
	CControlService SCM;

	dwRet = SCM.StartService(lpszServiceName);
	if (ERROR_SUCCESS != dwRet)
	{
		// error
	}
	const UINT MAX_SECOND = 120; // 500ms * 120 = 1min
	UINT count = 0;
	while (FALSE == SCM.IsServiceRunning(lpszServiceName)) // ���񽺰� ����� ������ ��� loop
	{
		Sleep(500);
		count++;
		if (count >= MAX_SECOND)
		{
			break;
		}
	}


	return dwRet;
}

DWORD Stop(LPCWSTR lpszServiceName)
{
	if (NULL == lpszServiceName)
	{
		return ERROR_INVALID_PARAMETER;
	}

	// ���� ���� ���
	DWORD dwRet = ERROR_SUCCESS;
	CControlService SCM;

	dwRet = SCM.StopService(lpszServiceName);
	if (ERROR_SUCCESS != dwRet)
	{
		// error
	}
	else
	{
		// �����ϸ� ����� ���������� ����Ѵ�.
		// ���� ��⸦ ���� ���ؼ� 1���� �Ѿ�� �׳� �����Ѵ�.
		const UINT MAX_SECOND = 60;
		UINT count = 0;
		while (TRUE == SCM.IsServiceRunning(lpszServiceName)) // ���񽺰� �������� ���ȿ��� ��� loop
		{
			Sleep(500);
			count++;
			if (count >= MAX_SECOND)
			{
				dwRet = ERROR_TIMEOUT;
				break;
			}
		}
	}
	return dwRet;
}

int main()
{
	HANDLE hEvent = NULL;
	DWORD dwRet = ERROR_SUCCESS;

	hEvent = CreateEvent(NULL, TRUE, FALSE, EVENT_NAME);
	if (NULL == hEvent)
	{
		dwRet = GetLastError();
		return dwRet;
	}

	Register(L"C:\\Users\\seonj\\OneDrive\\���� ȭ��\\Code\\Windows Programming\\ServiceApp\\x64\\Debug\\ServiceApp.exe", true);
	Start(SERVICE_NAME);
	Sleep(1000);
	SetEvent(hEvent);
	Stop(SERVICE_NAME);

	return 0;
}