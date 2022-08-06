#pragma once

/////////////////////////////////////////////////////////////////////
// Service 
//		Windows ���� ����Ǵ� ���α׷� ���� �� �ϳ�.
//		��׶��忡�� ����Ǵ� ���α׷�.
//		����ڿ��� ������ ������ �ý��� ����, ������ ����, �ϵ���� ���� ���� ��� ����.
//		UI ����
//		���� ���� ���α׷��̹Ƿ� NT, ������ windows ������ ��ġ/���� ��.
//		������Ʈ���� ��� (HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Service)
//
// services.msc (���� ���ø�)
//		HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Service �ش� ��ο� ���� �� �����ؾ� �ϴ� ���� ����� ��ϵ�
//		�� ������Ʈ���� �ִ� ������ ���� ������ ���̽� ��� �θ�.
//		SCM�� GUI�� Ȯ���� �� ����.
//
// SCM (Service Control Manager)
//		���� DB�� �����ϴ� �ý��� ���α׷�.
/////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <Windows.h>
#include <winsvc.h>

class CControlService 
{
public:
	CControlService();
	virtual ~CControlService();

public:
	DWORD CreateService(IN LPCTSTR lpServiceName,
		IN LPCTSTR lpDisplayName,
		IN DWORD dwServiceType,
		IN DWORD dwStartType,
		IN LPCTSTR lpBinaryPathName,
		IN LPCTSTR lpLoadOrderGroup,
		OUT LPDWORD lpdwTagId,
		IN LPCTSTR lpDependencies,
		IN LPCTSTR lpServiceStartName,
		IN LPCTSTR lpPassword);
	DWORD ChangeServiceConfig(LPCTSTR lpServiceName, DWORD dwStartType, LPCTSTR lpszDescription);
	DWORD StartService(LPCTSTR lpServiceName);
	DWORD StopService(LPCTSTR lpServiceName);
	DWORD PauseService(LPCTSTR lpServiceName);
	DWORD ContinueSerivce(LPCTSTR lpServiceName);
	DWORD ShutdownService(LPCTSTR lpServiceName);
	DWORD DeleteService(LPCTSTR lpServiceName);
	BOOL IsServiceInstalled(LPCTSTR lpServiceName);
	BOOL IsServiceRunning(LPCTSTR lpServiceName);
};
