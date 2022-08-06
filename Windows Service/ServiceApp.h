#pragma once

//////////////////////////////////////////////////////////////////
// SCM 하에서 실행되는 windows  서비스로 변환하려면..
//
// 1. 서비스를 SCM 에 등록하는 새 main() 진입점을 만든다. 등록 시 논리적인 서비스 진입점들과 이름들을 제공해야 한다.
// 2. 기존의 main() 진입 함수를 ServiceMain() 으로 바꾼다. 
//		- SCM 에 서비스 제어 처리부를 등록하고 상태를 SCM에 알려준다.
//		- 이벤트 로깅 명령들을 추가 가능
//		- ServiceMain() 이라는 이름은 하나의 놀리적 서비스를 대표하는 가상의 이름일 뿐.
// 3. SCM 명령에 반응할 서비스 제어 처리부 함수를 작성한다.
//
//
// ServiceMain() 함수들
//	- 매개변수들은 main()  함수와 동일, 반환 형식은 다름.
//	- void WINAPI 형식 반환
//
//	- 서비스 제어 처리부 등록하기
//		- SCM이 호출하는 서비스 제어 처리부는 관련된 논리적 서비스들을 제어할 수 있어야 한다.
//		- 각 논리적 서비스는 즉시 서비스 제어 처리부를 등록해야 한다.
//		- RegisterServiceCtrlHandlerEx 함수를 통해 등록한다.
//		- ServiceMain() 의 시작에서 호출해야 한다.
//		- 이후 서비스 제어 요청을 받은 SCM이 그 서비스에 등록된 처리부를 호출한다.
//
//	- 서비스 상태 설정
//		- 서비스 제어 처리부를 등록한 후 즉시 SetServiceStatus 함수를 이용하여 서비스의 상태를 SERVICE_START_PENDING으로 설정해야 한다.
//		- SetServiceStatus 함수는 서비스의 현재 상태를 알리는 함수.
//		- 서비스 제어 처리부는 자신이 호출될 대마다 서비스 상태를 설정해야 한다. (상태가 변하지 않아도)
//
// 
// 서비스 제어 처리부
//
//	- RegisterServiceCtrlHandlerEx 함수로 등록하는 콜백함수다.
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <Windows.h>

#define SERVICE_NAME L"Safy"
#define EVENT_NAME	L"Global\\SAFY"

class CServiceApp
{
public:
	CServiceApp();
	virtual ~CServiceApp();

public:
	static void WINAPI ServiceMain(DWORD argc, LPTSTR argv[]);
	static void WINAPI ControlNTService(DWORD dwCtrlCode);
	static BOOL InitService();
	static BOOL ExitService();
	static BOOL ReportStatusToSCMgr(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint);
	static int ServiceSpecific(int argc, LPTSTR argv[]);

	DWORD Run();

};