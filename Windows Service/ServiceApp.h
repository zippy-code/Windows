#pragma once

//////////////////////////////////////////////////////////////////
// SCM �Ͽ��� ����Ǵ� windows  ���񽺷� ��ȯ�Ϸ���..
//
// 1. ���񽺸� SCM �� ����ϴ� �� main() �������� �����. ��� �� ������ ���� ��������� �̸����� �����ؾ� �Ѵ�.
// 2. ������ main() ���� �Լ��� ServiceMain() ���� �ٲ۴�. 
//		- SCM �� ���� ���� ó���θ� ����ϰ� ���¸� SCM�� �˷��ش�.
//		- �̺�Ʈ �α� ��ɵ��� �߰� ����
//		- ServiceMain() �̶�� �̸��� �ϳ��� ��� ���񽺸� ��ǥ�ϴ� ������ �̸��� ��.
// 3. SCM ��ɿ� ������ ���� ���� ó���� �Լ��� �ۼ��Ѵ�.
//
//
// ServiceMain() �Լ���
//	- �Ű��������� main()  �Լ��� ����, ��ȯ ������ �ٸ�.
//	- void WINAPI ���� ��ȯ
//
//	- ���� ���� ó���� ����ϱ�
//		- SCM�� ȣ���ϴ� ���� ���� ó���δ� ���õ� ���� ���񽺵��� ������ �� �־�� �Ѵ�.
//		- �� ���� ���񽺴� ��� ���� ���� ó���θ� ����ؾ� �Ѵ�.
//		- RegisterServiceCtrlHandlerEx �Լ��� ���� ����Ѵ�.
//		- ServiceMain() �� ���ۿ��� ȣ���ؾ� �Ѵ�.
//		- ���� ���� ���� ��û�� ���� SCM�� �� ���񽺿� ��ϵ� ó���θ� ȣ���Ѵ�.
//
//	- ���� ���� ����
//		- ���� ���� ó���θ� ����� �� ��� SetServiceStatus �Լ��� �̿��Ͽ� ������ ���¸� SERVICE_START_PENDING���� �����ؾ� �Ѵ�.
//		- SetServiceStatus �Լ��� ������ ���� ���¸� �˸��� �Լ�.
//		- ���� ���� ó���δ� �ڽ��� ȣ��� �븶�� ���� ���¸� �����ؾ� �Ѵ�. (���°� ������ �ʾƵ�)
//
// 
// ���� ���� ó����
//
//	- RegisterServiceCtrlHandlerEx �Լ��� ����ϴ� �ݹ��Լ���.
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