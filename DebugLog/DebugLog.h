#pragma once

#ifndef __DEBUGLOG_H_
#define __DEBUGLOG_H_

#include <stdio.h>
#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>

#define LOG_LEVEL_NOTICE			0
#define LOG_LEVEL_ERROR				1
#define _F_							TEXT(__FILE__)
#define LOG_FILENAME				_T("SJLog")
#define LOG_BUFFER					2048
#define LOG_FORMAT					TEXT("%20s %10s %s")
#define LOG_FILE_MAX_SIZE			10*1024*1024

typedef struct tagST_LOG_PREFIX_INFO
{
	SYSTEMTIME	stTime;
	DWORD		dwProcessId;
	DWORD		dwThreadId;
} ST_LOG_PREFIX_INFO, *LPST_LOG_PREFIX_INFO;

class CDebugLog
{
public:
	CDebugLog();
	~CDebugLog();

public:
	DWORD InitLog();
	DWORD PutLog(LPCTSTR szSourceName, DWORD dwLogLevel, LPCTSTR loszLogFormat, ...);
	VOID GetLogPrefix(LPST_LOG_PREFIX_INFO pstOut);
	DWORD FormatLogPrefix(LPTSTR lpszOut, size_t dwCchOut, size_t* pdwPrefixLength);
	DWORD WriteLogText(LPCTSTR lpszLogText);
	DWORD WriteLog(LPCTSTR lpszLogText, ...);
	DWORD CheckFileSize(HANDLE hFile, PBOOL pbNeedRename);
	HANDLE OpenLogFile(LPCTSTR lpszLogFilePath);
	DWORD WriteLogData(HANDLE hFile, LPSTR lpszUtf8, INT nCchUtf8);
	BOOL RenameLogFile(LPCTSTR lpszLogFilePath, LPTSTR lpszNewLogFilePath, DWORD dwCchNewLogFilePath);
	DWORD SetLogFilePath();
};

#endif