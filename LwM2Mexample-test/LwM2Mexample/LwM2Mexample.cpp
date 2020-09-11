// LwM2Mexample.cpp : Defines the entry point for the console application.
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <crtdbg.h>
using namespace std;

int WinExec_test(char* payload, int payloadLen)
{
	//wmic cpu get loadpercentage /Value
	char* head = NULL;
	char* tail = NULL;

	FILE *fp = NULL;
	wchar_t  wcs[128] = L"";
	int i = 0;
	int fileSize = 0;


	remove("cpuusage.txt");

	WinExec("cmd.exe /v /c wmic cpu get loadpercentage /Value > cpuusage.txt", SW_HIDE);
	//WinExec("cmd.exe /v /c wmic OS get FreePhysicalMemory /Value > freeMemory.txt", SW_HIDE);
#if 1
	for(i =0 ; i<7 ; i++)
	{
		Sleep(200);
		if( (fp = fopen("cpuusage.txt", "r,ccs=UTF-16LE")) != NULL)
		{
			break;
		}
	}

	if(fp == NULL)
	{
		printf("[E] getCPUUsage() fp is NULL\n");
		return -1;
	}

	for(i =0 ; i<12 ; i++)
	{
		//printf("i = (%d)\n",i);
		Sleep(100);
		fseek(fp, 0, SEEK_END);
		fileSize = ftell(fp);
		if(fileSize > 0)
		{
			//printf("fileSize = (%d)\n",fileSize);
			break;
		}
	}
	fclose(fp);

	if( (fp = fopen("cpuusage.txt", "r,ccs=UTF-16LE")) == NULL)
	{
		printf("[E] getCPUUsage() fp is NULL\n");
		return -1;
	}

	memset(payload, 0, payloadLen);
	fread(wcs, sizeof(wcs), 1, fp);

	sprintf(payload, "%ws", wcs);

	printf("[I] payload (%s) (%d)\n", payload, payloadLen);

	if( (head = strstr(payload, "=")) != NULL)
	{
		head = head +1;
		if( (tail = strchr(head, '\n')) != NULL)
		{
			*tail = '\0'; 
			strcpy(payload, head);
		}
		else
		{
			printf("[E] getCPUUsage() (%s) is not contain space\n", payload);
			return -1;
		}
	}
	else
	{
		printf("[E] getCPUUsage() (%s) is not contain =\n", payload);
		return -1;
	}

	fclose(fp);
	//remove("cpuusage.txt");
#endif
	return 0;
}
int popen_test(char* payload, int payloadLen)
{
	FILE *fp   = NULL;
	char* head = NULL;
	char* tail = NULL;
	HWND hWnd = GetConsoleWindow();
	//ShowWindow(hWnd, SW_HIDE);
	fp = _popen("wmic cpu get loadpercentage /Value", "r");
	ShowWindow(hWnd, SW_HIDE);
	//fp = _wpopen(_T("wmic cpu get loadpercentage /Value"), _T("r"));
	if(payload == NULL)
	{
		printf("[E] getCPUUsage() payload is NULL\n");
		return -1;
	}	
	
	if(fp == NULL)
	{
		printf("[E] getCPUUsage() fp is NULL\n");
		return -1;
	}
	memset(payload, 0, payloadLen);
	fread(payload, payloadLen, 1, fp);

	if( (head = strstr(payload, "=")) != NULL)
	{
		head = head +1;
		if( (tail = strchr(head, '\r')) != NULL)
		{
			*tail = '\0'; 
			strcpy(payload, head);
		}
		else
		{
			printf("[E] getCPUUsage() (%s) is not contain space\n", payload);
			return -1;
		}
	}
	else
	{
		printf("[E] getCPUUsage() (%s) is not contain =\n", payload);
		return -1;
	}

	_pclose(fp);
}
int runCmd(char* myCmd, string& outOutput) {
    // read output
    #define BUFSIZE 4096
    DWORD dwRead;
    CHAR chBuf[BUFSIZE];
    bool bSuccess2 = FALSE;
	//cmd = "cmd /c dir";
	char* cmd = "cmd /v /c wmic cpu get loadpercentage /Value";
    PROCESS_INFORMATION piProcInfo;
    STARTUPINFO siStartInfo;
    bool bSuccess = FALSE;

    HANDLE g_hChildStd_OUT_Rd = NULL;
    HANDLE g_hChildStd_OUT_Wr = NULL;
    HANDLE g_hChildStd_ERR_Rd = NULL;
    HANDLE g_hChildStd_ERR_Wr = NULL;

    SECURITY_ATTRIBUTES sa;
    // Set the bInheritHandle flag so pipe handles are inherited.
	wchar_t wtext[2000];
	size_t cmd_len ;
	LPWSTR ptr;
	cmd_len = strlen(cmd)+1;
	mbstowcs(wtext, cmd, cmd_len);
	ptr = wtext;

    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;
    if (!CreatePipe(&g_hChildStd_ERR_Rd, &g_hChildStd_ERR_Wr, &sa, 0))     { return 1; } // Create a pipe for the child process's STDERR.
    if (!SetHandleInformation(g_hChildStd_ERR_Rd, HANDLE_FLAG_INHERIT, 0)) { return 1; } // Ensure the read handle to the pipe for STDERR is not inherited.
    if (!CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &sa, 0))     { return 1; } // Create a pipe for the child process's STDOUT.
    if (!SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0)) { return 1; } // Ensure the read handle to the pipe for STDOUT is not inherited

    // Set up members of the PROCESS_INFORMATION structure.
    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

    // Set up members of the STARTUPINFO structure.
    // This structure specifies the STDERR and STDOUT handles for redirection.
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
    
	siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdError  = g_hChildStd_ERR_Wr;
    siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
	siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    // Create the child process.
    bSuccess = CreateProcess(
        NULL,             // program name
        ptr,       // command line
        NULL,             // process security attributes
        NULL,             // primary thread security attributes
        TRUE,             // handles are inherited
        CREATE_NO_WINDOW, // creation flags (this is what hides the window)
        NULL,             // use parent's environment
        NULL,             // use parent's current directory
        &siStartInfo,     // STARTUPINFO pointer
        &piProcInfo       // receives PROCESS_INFORMATION
    );

    CloseHandle(g_hChildStd_ERR_Wr);
    CloseHandle(g_hChildStd_OUT_Wr);


    for (;;) { // read stdout
        bSuccess2 = ReadFile(g_hChildStd_OUT_Rd, chBuf, BUFSIZE, &dwRead, NULL);
        if(!bSuccess2 || dwRead == 0) break;
        string s(chBuf, dwRead);
        outOutput += s;
    }
    dwRead = 0;
	
    for (;;) { // read stderr
        bSuccess2 = ReadFile(g_hChildStd_ERR_Rd, chBuf, BUFSIZE, &dwRead, NULL);
        if(!bSuccess2 || dwRead == 0) break;
        string s(chBuf, dwRead);
        outOutput += s;
    }
    // The remaining open handles are cleaned up when this process terminates.
    // To avoid resource leaks in a larger application,
    // close handles explicitly.
    return 0;
}
int main(int argc, char* argv[])
//int WinMain(int argc, char* argv[])
{
	char input[2560] = "";
	string str ;
	int ret = 0;
	//ShowWindow( GetConsoleWindow(), SW_HIDE );
	//失敗
	//ret = popen_test(input, sizeof(input));
	//暫時成功
	//WinExec_test(input, sizeof(input));

	ret = runCmd(input,str);
	strcpy(input, str.c_str());

	return 0;
}