// ACDLockerConsole.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ACDLockerConsole.h"
#include <winioctl.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;

BOOL CloseKbdDriver(VOID);
BOOL OpenKbdDriver(VOID);
BOOL KbSyncRequest(BOOLEAN i_bEnabled);

#define ACD_IOCTL_INDEX  0x830

#define IOCTL_ACD_KBDFILTER_MODE				CTL_CODE(FILE_DEVICE_KEYBOARD,  \
												ACD_IOCTL_INDEX+18,  \
												METHOD_BUFFERED,       \
												FILE_ANY_ACCESS)

HANDLE m_hKbdDriver = INVALID_HANDLE_VALUE;
#define ACD_KEY 3847
BYTE m_Buffer[2048];

int main(int argc,      // Number of strings in array argv  
	char *argv[],   // Array of command-line argument strings  
	char *envp[])  // Array of environment variable strings  
{
    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        // initialize MFC and print and error on failure
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: change error code to suit your needs
            wprintf(L"Fatal Error: MFC initialization failed\n");
            nRetCode = 1;
        }
        else
        {
            // TODO: code your application's behavior here.
			BOOL bDriverOpen = OpenKbdDriver();
			int count = 0;
			for (count = 1; count < argc; count++) {
				if (_stricmp(argv[1], "-i") == 0) {
					// Install Driver here
				}
				else if (_stricmp(argv[1], "-u") == 0) {
					// Uninstall driver here
				}
				else if (_stricmp(argv[1], "-e") == 0) {
					// Enable driver here
					if (bDriverOpen) {
						KbSyncRequest(TRUE);
						CloseKbdDriver();
					}
				}
				else if (_stricmp(argv[1], "-d") == 0) {
					// Disable driver here
					if (bDriverOpen) {
						KbSyncRequest(FALSE);
						CloseKbdDriver();
					}
				}
			}
			
        }
    }
    else
    {
        // TODO: change error code to suit your needs
        wprintf(L"Fatal Error: GetModuleHandle failed\n");
        nRetCode = 1;
    }

    return nRetCode;
}


// It opens handle to keyboard driver
BOOL OpenKbdDriver(VOID)
{
	m_hKbdDriver = CreateFile(TEXT("\\\\.\\DosACDLocker"),
		0,                // no access to the drive
		FILE_SHARE_READ | FILE_SHARE_WRITE, // share mode read write
		NULL,             // default security attributes
		OPEN_EXISTING,    // disposition
		0,				  // for synchronous i/o
		NULL);            // do not copy file attributes

	if (m_hKbdDriver == INVALID_HANDLE_VALUE) // cannot open the drive
	{
		return FALSE;
	}

	return TRUE;
}

// It closes handle to keyboard driver
BOOL CloseKbdDriver(VOID)
{
	if (m_hKbdDriver != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hKbdDriver);
		m_hKbdDriver = INVALID_HANDLE_VALUE;
	}
	return TRUE;
}


BOOL KbSyncRequest(BOOLEAN i_bEnabled)
{
	if (m_hKbdDriver == INVALID_HANDLE_VALUE)
		return FALSE;

	DWORD bResult = 0;
	DWORD dwResult = 0;
	ULONG Key = ACD_KEY;
	DWORD BytesReturned = 0;

	memcpy(m_Buffer, &Key, sizeof(ULONG));
	memcpy(m_Buffer + sizeof(ULONG), &i_bEnabled, sizeof(BOOLEAN));

	bResult = DeviceIoControl(m_hKbdDriver,  // device to be queried
		IOCTL_ACD_KBDFILTER_MODE,  // operation to perform
		m_Buffer, sizeof(ULONG) + sizeof(BOOLEAN), // input buffer
		NULL, 0, // no output buffer
		&BytesReturned,  // # bytes returned
		NULL);  // synchronous I/O

	if (!bResult)
	{
		return FALSE;
	}
	return TRUE;
}