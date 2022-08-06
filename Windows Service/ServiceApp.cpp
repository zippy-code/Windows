#include "ServiceApp.h"
#include <atlbase.h>

HANDLE ghSvcStopEvent = NULL;
SERVICE_STATUS			gssStatus;
SERVICE_STATUS_HANDLE	gsshServiceStatus;

CServiceApp::CServiceApp()
{

}

CServiceApp::~CServiceApp()
{

}

BOOL CServiceApp::InitService()
{
	gssStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	gssStatus.dwServiceSpecificExitCode = 0;

	gsshServiceStatus = RegisterServiceCtrlHandler(SERVICE_NAME, ControlNTService);
	
	if (!ReportStatusToSCMgr(SERVICE_START_PENDING, NO_ERROR, 1000))
		return FALSE;

	if (!ReportStatusToSCMgr(SERVICE_RUNNING, NO_ERROR, 0))
		return FALSE;

	return TRUE;
}

BOOL CServiceApp::ReportStatusToSCMgr(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint)
{
	static DWORD dwCheckPoint = 1;
	BOOL bResult = TRUE;

	if (!gsshServiceStatus)
		return FALSE;
	
	if (SERVICE_START_PENDING == dwCurrentState)
		gssStatus.dwControlsAccepted = 0;  // SERVICE_ACCEPT_STOP ~ SERVICE_ACCEPT_PRESHUTDOWN 적절한 값을 넣어야 한다. 동작 확인 필요할 듯

	else
		gssStatus.dwControlsAccepted = SERVICE_CONTROL_SHUTDOWN; // SERVICE_ACCEPT_STOP 도?

	gssStatus.dwCurrentState = dwCurrentState;
	gssStatus.dwWin32ExitCode = dwWin32ExitCode;
	gssStatus.dwWaitHint = dwWaitHint;

	if ((SERVICE_RUNNING == dwCurrentState) || (SERVICE_STOPPED == dwCurrentState)) // PENDING 상태가 아니라면 0, SERVICE_STOPPED / SERVICE_RUNNING / SERVICE_PAUSED?
		gssStatus.dwCheckPoint = 0;
	else
		gssStatus.dwCheckPoint = dwCheckPoint++;

	bResult = SetServiceStatus(gsshServiceStatus, &gssStatus);

	return bResult;
}

void WINAPI CServiceApp::ControlNTService(DWORD dwCtrlCode)
{	
	switch (dwCtrlCode)
	{
	case SERVICE_CONTROL_SHUTDOWN:
		gssStatus.dwCurrentState = SERVICE_STOP_PENDING;
		break;
	case SERVICE_CONTROL_STOP:
		gssStatus.dwCurrentState = SERVICE_STOP_PENDING;
		break;
	case SERVICE_CONTROL_PAUSE:
		gssStatus.dwCurrentState = SERVICE_PAUSED;
		break;
	case SERVICE_CONTROL_CONTINUE:
		gssStatus.dwCurrentState = SERVICE_RUNNING;
		break;
	case SERVICE_CONTROL_INTERROGATE:
		break;
	default:
		break;
	}

	ReportStatusToSCMgr(gssStatus.dwCurrentState, NO_ERROR, 0);
}


void WINAPI CServiceApp::ServiceMain(DWORD argc, LPTSTR argv[])
{
	// 서비스 초기화
	if (!InitService())
		return;

	/////// service app 기능 함수 구현
	ServiceSpecific(argc, argv);
	///////

	ExitService();
}

BOOL CServiceApp::ExitService()
{
	BOOL bRet = TRUE;
	
	ReportStatusToSCMgr(SERVICE_STOP_PENDING, NO_ERROR, 0);
	ReportStatusToSCMgr(SERVICE_STOPPED, NO_ERROR, 0);

	return bRet;
}

// 서비스 시작 유형을 자동으로 변경 (레지스트리)
DWORD SetServiceStartTypeByReg()
{
	CRegKey RegKey;

	DWORD dwRet = RegKey.Open(HKEY_LOCAL_MACHINE,
		L"SYSTEM\\CurrentControlSet\\Services\\AhnLabEPS");
	if (ERROR_SUCCESS == dwRet)
	{
#if !defined(_MSC_VER) || (_MSC_VER<1400)
		dwRet = RegKey.SetValue(SERVICE_AUTO_START, L"Start");		
#else
		dwRet = RegKey.SetDWORDValue(L"Start", SERVICE_AUTO_START);		
#endif
		if (ERROR_SUCCESS != dwRet)
		{
			//error
		}
	}
	else
	{
		// log
	}

	return dwRet;
}

// 서비스 시작 유형을 자동으로 변경해야 할 경우, 사용
DWORD SetServiceStartType()
{
	LPCWSTR lpszServiceName = SERVICE_NAME;

	SC_HANDLE schSCManager;
	SC_HANDLE schService;
	DWORD dwRtVal = ERROR_SUCCESS;

	// Get a handle to the SCM database.
	schSCManager = OpenSCManager(
			NULL,						// local computer
			NULL,						// ServicesActive database
			SC_MANAGER_ALL_ACCESS );	// full access rights

	if (NULL != schSCManager)
	{
		// Get a handle to the service.

		schService = OpenService(
				schSCManager,			// SCM database
				lpszServiceName,		// name of service
				SERVICE_CHANGE_CONFIG);	// need change config access

		if (NULL != schService)
		{
			if( !ChangeServiceConfig(
						schService,
						SERVICE_NO_CHANGE,
						SERVICE_AUTO_START,
						SERVICE_NO_CHANGE,
						NULL,
						NULL,
						NULL,
						NULL,
						NULL,
						NULL,
						NULL ) )
			{
				dwRtVal = GetLastError();
				// log
			}

			CloseServiceHandle(schService);
		}
		else
		{
			dwRtVal = GetLastError();
			// log
		}
		CloseServiceHandle(schSCManager);
	}
	else
	{
		dwRtVal = GetLastError();
		// log
	}

	if (ERROR_SUCCESS != dwRtVal)
	{
		dwRtVal = SetServiceStartTypeByReg();
	}

	return dwRtVal;
}



int CServiceApp::ServiceSpecific(int argc, LPTSTR argv[])
{
	DWORD dwRet = ERROR_SUCCESS;
	
	ghSvcStopEvent = CreateEvent(NULL,		// default security attributes
		TRUE,								// manual reset event
		FALSE,								// not signaled
		NULL);								// no name

	if (NULL == ghSvcStopEvent)
	{
		dwRet = GetLastError();
		ReportStatusToSCMgr(SERVICE_STOPPED, dwRet, 0);
		return dwRet;
	}

	ReportStatusToSCMgr(SERVICE_RUNNING, NO_ERROR, 0);
	
	while (TRUE)
	{
		WaitForSingleObject(ghSvcStopEvent, INFINITE);

		ReportStatusToSCMgr(SERVICE_STOPPED, NO_ERROR, 0);
		return dwRet;
	}
}

DWORD CServiceApp::Run()
{
	DWORD dwRet = ERROR_SUCCESS;

	SERVICE_TABLE_ENTRY DispatchTable[] = 
	{
		{SERVICE_NAME, ServiceMain},
		{NULL, NULL}
	};

	if (!StartServiceCtrlDispatcher(DispatchTable))
	{
		//
	}
	else
	{
		dwRet = GetLastError();
	}

	return dwRet;
}