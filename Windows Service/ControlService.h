#pragma once

/////////////////////////////////////////////////////////////////////
// Service 
//		Windows 에서 실행되는 프로그램 종류 중 하나.
//		백그라운드에서 실행되는 프로그램.
//		사용자에게 보이지 않지만 시스템 유지, 데이터 제공, 하드웨어 관리 등의 기능 수행.
//		UI 없음
//		서버 응용 프로그램이므로 NT, 서버급 windows 에서만 설치/실행 됨.
//		레지스트리에 등록 (HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Service)
//
// services.msc (서비스 애플릿)
//		HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Service 해당 경로에 부팅 시 실행해야 하는 서비스 목록이 기록됨
//		이 레지스트리에 있는 정보를 서비스 데이터 베이스 라고 부름.
//		SCM을 GUI로 확인할 수 있음.
//
// SCM (Service Control Manager)
//		서비스 DB를 관리하는 시스템 프로그램.
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
