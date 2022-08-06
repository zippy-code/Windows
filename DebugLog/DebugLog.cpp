#include "DebugLog.h"

CRITICAL_SECTION g_cs;
TCHAR g_szLogFilePath[MAX_PATH] = _T("");
BOOL g_Init = FALSE;

static BOOL IsValidCriticalSection(const LPCRITICAL_SECTION lpCriticalSection)
{
	if ( NULL != lpCriticalSection 
		&& NULL != lpCriticalSection->DebugInfo )
	{
		return TRUE;
	}

	return FALSE;
}


CDebugLog::CDebugLog()
{

}

CDebugLog::~CDebugLog()
{

}


DWORD CDebugLog::InitLog()
{
	DWORD dwRet = ERROR_SUCCESS;
	if (FALSE != g_Init)
		return dwRet;

	dwRet = SetLogFilePath();
	if (ERROR_SUCCESS == dwRet)
		g_Init = TRUE;
	
	if ( TRUE != IsValidCriticalSection( &g_cs ) )
	{
		InitializeCriticalSection( &g_cs );
	}

	return dwRet;
}



DWORD CDebugLog::PutLog(LPCTSTR szSourceName, DWORD dwLogLevel, LPCTSTR lpszLogFormat, ...)
{
	DWORD dwRet = ERROR_SUCCESS;
	TCHAR Log[LOG_BUFFER] = {0};
	TCHAR Buffer[LOG_BUFFER] = {0};
	TCHAR* pszLogLevel[2] = {_T("[NOTICE]"), _T("[ERROR]")};
	va_list args; // ���� ���� 

	if (!g_Init)
		InitLog();

	va_start(args, lpszLogFormat);

	// �������� ���� ���� �Լ�
	StringCchVPrintf(Buffer, LOG_BUFFER, lpszLogFormat, args);

	va_end(args);

	TCHAR* Ptr = NULL;
	if (NULL != szSourceName)
	{
		// Source ���۹��ں���
		Ptr = (TCHAR*)szSourceName;
		// ��� ���� ����
		while ( *Ptr )
		{	
			// ��ΰ� '/' �� ��� '\' �� ����
			if (*Ptr == _T('/'))
				*Ptr = _T('\\');
			// ���� ����
			Ptr++;
		}
		// �� �������� '\' �� ������ �κб��� �߶�
		Ptr = _tcsrchr((TCHAR*)szSourceName, _T('\\'));
		if (NULL == Ptr)
			Ptr = (TCHAR*)szSourceName;
		else
			// \asdf.cpp �� ���, '\' ���� �����Ͽ� ���� �̸��� ����
			Ptr++;
	}
	// abc.cpp [LOG_LEVEL] log contents
	StringCchPrintf(Log, LOG_BUFFER, LOG_FORMAT, Ptr, pszLogLevel[dwLogLevel], Buffer);


	dwRet = WriteLog(_T("%s"), Log);
	if(ERROR_NOT_READY == dwRet)
	{
		//error
	}

	return dwRet;
}

DWORD CDebugLog::WriteLog(LPCTSTR lpszLogText, ...)
{
	DWORD dwRet = ERROR_SUCCESS;
	va_list	argList = NULL;
	size_t nPrefixLength = 0;
	TCHAR szLogText[LOG_BUFFER]	= {0,};

	// Log ���� �տ� �� Prefix�� ���Ѵ�.
	dwRet = FormatLogPrefix(szLogText, LOG_BUFFER, &nPrefixLength);
	if (ERROR_SUCCESS != dwRet)
		return dwRet;

	// Format message�� �����Ѵ�.
	va_start(argList, lpszLogText);
	if (FAILED(StringCchVPrintf(&szLogText[nPrefixLength], LOG_BUFFER - nPrefixLength -2 -1, lpszLogText, argList)))
	{
		// keep going
	}
	va_end(argList);

	// ���� ������ ���δ�.
	StringCchCatW(szLogText, LOG_BUFFER, L"\r\n");

	// UTF-16 ������ Log�� Write�Ѵ�.
	dwRet = WriteLogText(szLogText);

	return dwRet;
}

DWORD CDebugLog::FormatLogPrefix(LPTSTR lpszOut, size_t dwCchOut, size_t* pdwPrefixLength)
{
	DWORD dwRet = ERROR_SUCCESS;
	ST_LOG_PREFIX_INFO	stInfo = {0,};

	if ((NULL == lpszOut) || (NULL == pdwPrefixLength))
		return ERROR_INVALID_PARAMETER;

	// Prefix ������ ���Ѵ�.
	GetLogPrefix(&stInfo);

	for (;;)
	{
		if (FAILED(StringCchPrintf(lpszOut, 
			dwCchOut, 
			L"[%04d/%02d/%02d %02d:%02d:%02d.%03d][p%u][t%u] ", 
			stInfo.stTime.wYear, 
			stInfo.stTime.wMonth, 
			stInfo.stTime.wDay, 
			stInfo.stTime.wHour, 
			stInfo.stTime.wMinute, 
			stInfo.stTime.wSecond,
			stInfo.stTime.wMilliseconds,
			stInfo.dwProcessId,
			stInfo.dwThreadId)))
		{
			dwRet = GetLastError();
			break;
		}

		if (FAILED(StringCchLength(lpszOut, dwCchOut, pdwPrefixLength)))
		{
			dwRet = GetLastError();
			break;
		}

		if (*pdwPrefixLength >= dwCchOut)
		{
			dwRet = ERROR_BAD_LENGTH;
			break;
		}

		break;
	}

	return dwRet;
}

VOID CDebugLog::GetLogPrefix(LPST_LOG_PREFIX_INFO pstOut)
{
	if (NULL == pstOut)
		return;

	pstOut->dwProcessId = ::GetCurrentProcessId();
	pstOut->dwThreadId  = ::GetCurrentThreadId();
	::GetLocalTime(&pstOut->stTime);
}

DWORD CDebugLog::WriteLogText(LPCTSTR lpszLogText)
{
	DWORD dwRet = ERROR_SUCCESS;
	size_t nCchLength = 0;
	CHAR szUtf8[LOG_BUFFER] = {0,};
	LPSTR lpszUtf8 = NULL;
	INT nCchUtf8 = 0;
	INT nCchNeedLength = 0;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	TCHAR szLogFilePath[MAX_PATH + 1] = {0};

	::EnterCriticalSection(&g_cs);

	for (;;)
	{
		hFile = OpenLogFile(g_szLogFilePath);

		if (INVALID_HANDLE_VALUE == hFile)
		{
			dwRet = GetLastError();
			if (ERROR_SUCCESS == dwRet)
				dwRet = ERROR_OPEN_FAILED;
			break;
		}

		// ���� ������ ���Ѵ�.
		StringCchLength(lpszLogText, LOG_BUFFER, &nCchLength);
		if (0 >= nCchLength)
		{
			dwRet = ERROR_BAD_LENGTH;
			break;
		}
		if (LOG_BUFFER <= nCchLength)
		{
			// ���� ���̰� �ʰ��� ������ ���ڸ� �ڸ���.
			nCchLength = LOG_BUFFER - 1;
		}

		// ����
		//WideCharToMultiByte���� ��ȯ �� szUtf8 �� ũ�⸦ 252�� ����

		nCchUtf8 = ::WideCharToMultiByte(CP_UTF8, 0, lpszLogText, (INT)nCchLength, szUtf8, LOG_BUFFER, NULL, NULL);	
		if (0 == nCchUtf8)
		{
			dwRet = ::GetLastError();
			break;
		}
		else
		{
			lpszUtf8 = szUtf8;
		}

		dwRet = WriteLogData(hFile, lpszUtf8, nCchUtf8);
	
		break;
	}
	
	if (INVALID_HANDLE_VALUE != hFile)
	{
		CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;
	}
	
	LeaveCriticalSection(&g_cs);

	return dwRet;
}

HANDLE CDebugLog::OpenLogFile(LPCTSTR lpszLogFilePath)
{
	HANDLE hFile = INVALID_HANDLE_VALUE;
	BOOL bRename = FALSE;
	TCHAR szNewLogPath[MAX_PATH] = {0,};
	
	for (;;)
	{
		hFile = CreateFile(lpszLogFilePath,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

		if (INVALID_HANDLE_VALUE == hFile)
			break;
		
		if (ERROR_SUCCESS != CheckFileSize(hFile, &bRename))
		{
			if (INVALID_HANDLE_VALUE != hFile)
			{
				CloseHandle(hFile);
				hFile = INVALID_HANDLE_VALUE;
				break;
			}
		}

		if (bRename)
		{
			if (FALSE == RenameLogFile(lpszLogFilePath, szNewLogPath, MAX_PATH))
			{
				if (INVALID_HANDLE_VALUE != hFile)
				{
					CloseHandle(hFile);
					hFile = INVALID_HANDLE_VALUE;
				}
				break;
			}

			hFile = CreateFile(lpszLogFilePath,
				GENERIC_READ | GENERIC_WRITE,
				FILE_SHARE_READ,
				NULL,
				OPEN_ALWAYS,
				FILE_ATTRIBUTE_NORMAL,
				NULL);
			
			if (INVALID_HANDLE_VALUE == hFile)
				break;


			if (ERROR_SUCCESS != CheckFileSize(hFile, &bRename))
			{
				if (INVALID_HANDLE_VALUE != hFile)
				{
					CloseHandle(hFile);
					hFile = INVALID_HANDLE_VALUE;
					break;
				}
			}
		}

		break;
	}
	
	return hFile;
}

DWORD CDebugLog::CheckFileSize(HANDLE hFile, PBOOL pbNeedRename)
{
	DWORD dwRet = ERROR_SUCCESS;
	DWORD dwSize = 0;

	if (INVALID_HANDLE_VALUE == hFile || NULL == pbNeedRename)
		return ERROR_INVALID_PARAMETER;

	for (;;)
	{
		dwSize = ::GetFileSize(hFile, NULL);
		if (INVALID_FILE_SIZE == dwSize)
		{
			dwRet = ERROR_INVALID_FUNCTION;
			break;
		}
		else if(LOG_FILE_MAX_SIZE < dwSize)
		{
			*pbNeedRename = TRUE;
			break;
		}
		else
		{
			//
			break;
		}

		break;
	}

	return dwRet;
}

BOOL CDebugLog::RenameLogFile(LPCTSTR lpszLogFilePath, LPTSTR lpszNewLogFilePath, DWORD dwCchNewLogFilePath)
{
	BOOL bRet = TRUE;

	if (NULL == lpszLogFilePath || NULL == lpszNewLogFilePath)
		return ERROR_INVALID_PARAMETER;

	StringCchPrintf(lpszNewLogFilePath, dwCchNewLogFilePath, _T("%s.old.log"), lpszLogFilePath);
	
	// ���� old ���� ����
	::DeleteFile(lpszNewLogFilePath);

	if (FALSE == ::MoveFileEx(lpszLogFilePath, lpszNewLogFilePath, MOVEFILE_REPLACE_EXISTING))
	{
		return bRet;
	}

	return bRet;
}

DWORD CDebugLog::WriteLogData(HANDLE hFile, LPSTR lpszUtf8, INT nCchUtf8)
{
	DWORD dwRet = ERROR_SUCCESS;
	DWORD dwWritten = 0;

	if (INVALID_HANDLE_VALUE == hFile || NULL == lpszUtf8)
		return ERROR_INVALID_PARAMETER;

	for (;;)
	{
		if (INVALID_SET_FILE_POINTER == ::SetFilePointer(hFile, 0, NULL, FILE_END))
		{
			dwRet = GetLastError();
			break;
		}

		if (FALSE == ::WriteFile(hFile, lpszUtf8, nCchUtf8, &dwWritten, NULL))
		{
			dwRet = GetLastError();
			if (ERROR_SUCCESS == dwRet)
				dwRet = ERROR_WRITE_FAULT;

			break;
		}

		if (nCchUtf8 != dwWritten)
		{
			dwRet = ERROR_WRITE_FAULT;
			break;
		}

		break;
	}
	
	return dwRet;
}

DWORD CDebugLog::SetLogFilePath()
{
	DWORD dwRet = ERROR_SUCCESS;
	DWORD dwLen = 0;
	// get current process path
	dwLen = GetCurrentDirectory(MAX_PATH, g_szLogFilePath);
	if (0 == dwLen)
	{
		dwRet = GetLastError();
		return dwRet;
	}
	else
	{
		if (_T('\\') != g_szLogFilePath[dwLen - 1])
			StringCchCat(g_szLogFilePath, MAX_PATH, _T("\\"));
		StringCchCat(g_szLogFilePath, MAX_PATH, LOG_FILENAME);
	}

	return dwRet;
}