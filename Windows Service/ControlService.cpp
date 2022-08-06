#include "ControlService.h"

CControlService::CControlService()
{

}

CControlService::~CControlService()
{

}

DWORD CControlService::CreateService(IN LPCTSTR lpServiceName, 
									 IN LPCTSTR lpDisplayName, 
									 IN DWORD dwServiceType, 
									 IN DWORD dwStartType, 
									 IN LPCTSTR lpBinaryPathName, 
									 IN LPCTSTR lpLoadOrderGroup, 
									 OUT LPDWORD lpdwTagId, 
									 IN LPCTSTR lpDependencies, 
									 IN LPCTSTR lpServiceStartName, 
									 IN LPCTSTR lpPassword)
{
	DWORD dwRet = ERROR_SUCCESS;
	SC_HANDLE schSCManager = NULL;
	SC_HANDLE schService = NULL;

	schSCManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);  //Required to call the CreateService function to create a service object and add it to the database.
	if (schSCManager)
	{
		schService = ::CreateService(schSCManager,		
			lpServiceName,							// "AhnLabEPS"
			lpDisplayName,							// "AhnLabEPS"
			SERVICE_ALL_ACCESS,
			dwServiceType,							// SERVICE_WIN32_OWN_PROCESS
			dwStartType,							// SERVICE_AUTO_START
			SERVICE_ERROR_NORMAL,
			lpBinaryPathName,						// "C:\Program Files\AhnLab\EPS\BbSvc.exe" -> "" 이 없을 경우 시스템에 따라서 서비스 시작시 193 에러가 반환됨.
			lpLoadOrderGroup,						// NULL
			lpdwTagId,								// NULL
			lpDependencies,							// "\0"
			lpServiceStartName,						// NULL
			lpPassword);							// NULL

		if (schService)
			::CloseServiceHandle(schService);
		// 이미 서비스가 있는 경우, ERROR_SERVICE_EXISTS 리턴
		else if (ERROR_SERVICE_EXISTS != GetLastError())
			dwRet = GetLastError();

		::CloseServiceHandle(schSCManager);
	}
	else
	{
		dwRet = GetLastError();
	}

	return dwRet;
}

DWORD CControlService::ChangeServiceConfig(LPCTSTR lpServiceName, DWORD dwStartType, LPCTSTR lpszDescription)
{
	DWORD dwRet = ERROR_SUCCESS;
	SC_HANDLE schSCManager = NULL;
	SC_HANDLE schService = NULL;

	schSCManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	
	if (schSCManager)
	{
		schService = ::OpenService(schSCManager, lpServiceName, SERVICE_CHANGE_CONFIG);

		if (schService)
		{
			if (!::ChangeServiceConfig(schService,	// handle of service 
				SERVICE_NO_CHANGE,					// service type: no change 
				dwStartType,						// change service start type 
				SERVICE_NO_CHANGE,					// error control: no change 
				NULL,								// binary path: no change 
				NULL,								// load order group: no change
				NULL,								// tag ID: no change 
				NULL,								// dependencies: no change 
				NULL,								// account name: no change 
				NULL,								// password: no change 
				lpszDescription))					// display name
			{
				dwRet = GetLastError();
			}

			//:) NT4.0에서는 ChangeServiceConfig2()가 없다.
#if _MSC_VER >= 1500
			SERVICE_DESCRIPTION sd = {0};
			sd.lpDescription = const_cast<LPWSTR>(lpszDescription);

			if( !::ChangeServiceConfig2(
				schService,					// handle to service
				SERVICE_CONFIG_DESCRIPTION,	// change: description
				&sd) )						// new description
			{
				dwRet = GetLastError();
			}
#endif
			::CloseServiceHandle(schService);
		}
		else
		{
			dwRet = GetLastError();
		}

		::CloseServiceHandle(schSCManager);
	}
	else
	{
		dwRet = GetLastError();
	}

	return dwRet;
}

DWORD CControlService::StartService(IN LPCTSTR lpServiceName)
{
	DWORD dwRet = ERROR_SUCCESS;
	SC_HANDLE schSCManager = NULL;
	SC_HANDLE schService = NULL;

	schSCManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT); // local computer, SERVICES_ACTIVE_DATABASE
	if (schSCManager)
	{
		schService = ::OpenService(schSCManager, lpServiceName, SERVICE_ALL_ACCESS);

		if (schService)
		{
			if (!::StartService(schService, 0, NULL))
			{
				if (ERROR_SERVICE_ALREADY_RUNNING != GetLastError())
					dwRet = GetLastError();
			}

			::CloseServiceHandle(schService);
		}
		else
		{
			dwRet = GetLastError();
		}

		::CloseServiceHandle(schSCManager);
	}
	else
	{
		dwRet = GetLastError();
	}

	return dwRet;
}

DWORD CControlService::StopService(LPCTSTR lpServiceName)
{
	DWORD dwRet = ERROR_SUCCESS;
	SC_HANDLE schSCManager = NULL;
	SC_HANDLE schService = NULL;
	SERVICE_STATUS ss;

	schSCManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	if (schSCManager)
	{
		schService = ::OpenService(schSCManager, lpServiceName, SERVICE_QUERY_STATUS | SERVICE_STOP); // SERVICE_QUERY_STATUS?
		if (schService)
		{
			::ControlService(schService, SERVICE_CONTROL_STOP, &ss);
			::CloseServiceHandle(schService);
		}
		else
		{
			dwRet = GetLastError();
		}

		::CloseServiceHandle(schSCManager);
	}
	else
	{
		dwRet = GetLastError();
	}

	return dwRet;
}

DWORD CControlService::PauseService(LPCTSTR lpServiceName)
{
	DWORD dwRet = ERROR_SUCCESS;
	SC_HANDLE schSCManager = NULL;
	SC_HANDLE schService = NULL;

	schSCManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	if (schSCManager)
	{
		schService = ::OpenService(schSCManager, lpServiceName, SERVICE_ALL_ACCESS);
		if (schService)
		{
			if (NULL == ::ControlService(schService, SERVICE_CONTROL_PAUSE, NULL))
			{			
				dwRet = GetLastError();			
			}		
			::CloseServiceHandle(schService);
		}
		else
		{
			dwRet = GetLastError();
		}

		::CloseServiceHandle(schSCManager);
	}
	else
	{
		dwRet = GetLastError();
	}

	return dwRet;
}

DWORD CControlService::ContinueSerivce(LPCTSTR lpServiceName)
{
	DWORD dwRet = ERROR_SUCCESS;
	SC_HANDLE schSCManager = NULL;
	SC_HANDLE schService = NULL;

	schSCManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	if (schSCManager)
	{
		schService = ::OpenService(schSCManager, lpServiceName, SERVICE_ALL_ACCESS);
		if (schService)
		{
			if (NULL == ::ControlService(schService, SERVICE_CONTROL_CONTINUE, NULL))
			{			
				dwRet = GetLastError();			
			}		
			::CloseServiceHandle(schService);
		}
		else
		{
			dwRet = GetLastError();
		}

		::CloseServiceHandle(schSCManager);
	}
	else
	{
		dwRet = GetLastError();
	}

	return dwRet;
}

DWORD CControlService::ShutdownService(LPCTSTR lpServiceName)
{
	DWORD dwRet = ERROR_SUCCESS;
	SC_HANDLE schSCManager = NULL;
	SC_HANDLE schService = NULL;

	schSCManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	if (schSCManager)
	{
		schService = ::OpenService(schSCManager, lpServiceName, SERVICE_ALL_ACCESS);
		if (schService)
		{
			if (NULL == ::ControlService(schService, SERVICE_CONTROL_SHUTDOWN, NULL))
			{			
				dwRet = GetLastError();			
			}		
			::CloseServiceHandle(schService);
		}
		else
		{
			dwRet = GetLastError();
		}

		::CloseServiceHandle(schSCManager);
	}
	else
	{
		dwRet = GetLastError();
	}

	return dwRet;
}

DWORD CControlService::DeleteService(LPCTSTR lpServiceName)
{
	DWORD dwRet = ERROR_SUCCESS;
	SC_HANDLE schSCManager = NULL;
	SC_HANDLE schService = NULL;

	schSCManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT); // local computer, SERVICES_ACTIVE_DATABASE
	if (schSCManager)
	{
		schService = ::OpenService(schSCManager, lpServiceName, DELETE);

		if (schService)
		{
			if (!::DeleteService(schService))
				dwRet = GetLastError();

			::CloseServiceHandle(schService);
		}
		else
		{
			dwRet = GetLastError();
		}

		::CloseServiceHandle(schSCManager);
	}
	else
	{
		dwRet = GetLastError();
	}

	return dwRet;
}

BOOL CControlService::IsServiceInstalled(LPCTSTR lpServiceName)
{
	BOOL bRet = FALSE;
	SC_HANDLE schSCManager = NULL;
	SC_HANDLE schService = NULL;

	schSCManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	if (schSCManager)
	{
		schService = ::OpenService(schSCManager, lpServiceName, SERVICE_QUERY_CONFIG);
		if (schService)
		{
			bRet = TRUE;
			::CloseServiceHandle(schService);
		}
		
		::CloseServiceHandle(schSCManager);
	}

	return bRet;
}

BOOL CControlService::IsServiceRunning(LPCTSTR lpServiceName)
{
	BOOL bRet = FALSE;
	SC_HANDLE schSCManager = NULL;
	SC_HANDLE schService = NULL;
	SERVICE_STATUS ss;

	schSCManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	if (schSCManager)
	{
		schService = ::OpenService(schSCManager, lpServiceName, SERVICE_QUERY_CONFIG|SERVICE_QUERY_STATUS);
		if (schService)
		{
			if (::QueryServiceStatus(schService, &ss))
			{
				if (SERVICE_RUNNING == ss.dwCurrentState)
					bRet = TRUE;
			}
			::CloseServiceHandle(schService);
		}

		::CloseServiceHandle(schSCManager);
	}

	return bRet;
}