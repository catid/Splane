/*
	Copyright (c) 2009-2010 Christopher A. Taylor.  All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:

	* Redistributions of source code must retain the above copyright notice,
	  this list of conditions and the following disclaimer.
	* Redistributions in binary form must reproduce the above copyright notice,
	  this list of conditions and the following disclaimer in the documentation
	  and/or other materials provided with the distribution.
	* Neither the name of LibCat nor the names of its contributors may be used
	  to endorse or promote products derived from this software without
	  specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
	AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
	IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
	ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
	LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
	CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
	SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
	CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
	ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
	POSSIBILITY OF SUCH DAMAGE.
*/

#include "WindowsService.hpp"
#include <conio.h> // kbhit()
using namespace cat;

#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

const char *SETTINGS_FILE_PATH = "LoginDatabase.cfg";

// maximum mumber of lines the output console should have
static const WORD MAX_CONSOLE_LINES = 500;

void RedirectIOToConsole()
{
	int hConHandle;
	long lStdHandle;
	CONSOLE_SCREEN_BUFFER_INFO coninfo;
	FILE *fp;

	// allocate a console for this app
	AllocConsole();

	// set the screen buffer to be big enough to let us scroll text
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),
		&coninfo);

	coninfo.dwSize.Y = MAX_CONSOLE_LINES;
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE),
		coninfo.dwSize);

	// redirect unbuffered STDOUT to the console
	lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "w" );
	*stdout = *fp;
	setvbuf( stdout, NULL, _IONBF, 0 );

	// redirect unbuffered STDIN to the console
	lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "r" );
	*stdin = *fp;
	setvbuf( stdin, NULL, _IONBF, 0 );

	// redirect unbuffered STDERR to the console
	lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "w" );
	*stderr = *fp;
	setvbuf( stderr, NULL, _IONBF, 0 );

	// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
	// point to console as well
	ios::sync_with_stdio();
}



//// Entrypoint

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	if (lpCmdLine && iStrEqual(lpCmdLine, "console"))
	{
		RedirectIOToConsole();

		if (!InitializeFramework(SETTINGS_FILE_PATH))
		{
			FatalStop("Unable to initialize framework!");
		}

		Logging::ref()->SetThreshold(LVL_INANE);

		INFO("LoginDatabase") << "Starting in console mode.  Press any key to terminate.";

		WindowsService::ref()->Startup();

		while (!kbhit())
		{
			Sleep(100);
		}

		WindowsService::ref()->Shutdown();

		ShutdownFramework(true);

		return 0;
	}

	// Start in service mode

	if (!InitializeFramework(SETTINGS_FILE_PATH, "LoginDatabase"))
	{
		FatalStop("Unable to initialize framework!");
	}

	if (lpCmdLine && iStrEqual(lpCmdLine, "install"))
	{
		if (WindowsService::ref()->IsExistingService())
		{
			WindowsService::ref()->RemoveService();
		}

		if (!WindowsService::ref()->InstallService())
		{
			MessageBoxA(0, "Unable to install service", "Cannot Install Server", 0);
		}
	}
	else if (lpCmdLine && iStrEqual(lpCmdLine, "remove"))
	{
		if (WindowsService::ref()->IsExistingService())
		{
			if (!WindowsService::ref()->RemoveService())
			{
				MessageBoxA(0, "Unable to remove service", "Cannot Remove Server", 0);
			}
		}
		else
		{
			MessageBoxA(0, "Service is not installed", "Cannot Remove Server", 0);
		}
	}
	else
	{
		if (!WindowsService::ref()->StartServiceMain())
		{
			MessageBoxA(0, "Unable to initialize service.\n\nThe server should be started from an Administrator command prompt by typing\n\n\"login_db_srv.exe install\"\n\nOr started as an interactive console by typing\n\n\"login_db_srv.exe console\"", "Cannot Start Server", 0);
		}
	}

	ShutdownFramework(true);

	return 0;
}


WindowsService::WindowsService()
{
	const char *settings_service_name = Settings::ii->getStr("Service.Name", "LoginDatabase");

	CAT_STRNCPY(_service_name, settings_service_name, sizeof(_service_name));

	_service_handle = 0;

	_server = 0;
}

WindowsService::~WindowsService()
{

}

bool WindowsService::IsExistingService()
{
	// If able to open Service Control Manager to check service,
	SC_HANDLE scm_check = OpenSCManager(0, 0, SC_MANAGER_CONNECT);
	if (scm_check)
	{
		// If service already exists,
		SC_HANDLE svc_check = OpenServiceA(scm_check, _service_name, SERVICE_QUERY_STATUS);
		if (svc_check)
		{
			CloseServiceHandle(svc_check);
			CloseServiceHandle(scm_check);
			return true;
		}

		CloseServiceHandle(scm_check);
	}
	else
	{
		FATAL("ServiceManager") << "IsExistingService(): Unable to connect Service Control Manager";
	}

	return false;
}

bool WindowsService::InstallService()
{
	// If able to open Service Control Manager to create service,
	SC_HANDLE scm_create = OpenSCManager(0, 0, SC_MANAGER_CREATE_SERVICE);
	if (scm_create)
	{
		char path[MAX_PATH+1];

		// Get the path to this executable module
		DWORD len = GetModuleFileNameA(0, path, sizeof(path));
		path[sizeof(path) - 1] = '\0';
		if (len == strlen(path))
		{
			// Create the service
			SC_HANDLE svc_create = CreateServiceA(scm_create, _service_name, _service_name, SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_IGNORE, path, 0, 0, 0, 0, 0);
			if (svc_create)
			{
				INFO("ServiceManager") << "InstallService(): Starting service";

				StartService(svc_create, 0, 0);

				CloseServiceHandle(svc_create);
				CloseServiceHandle(scm_create);
				return true;
			}
			else
			{
				FATAL("ServiceManager") << "InstallService(): Unable to create service";
			}
		}
		else
		{
			FATAL("ServiceManager") << "InstallService(): Unable to get module file path";
		}

		CloseServiceHandle(scm_create);
	}
	else
	{
		FATAL("ServiceManager") << "InstallService(): Unable to create Service Control Manager";
	}

	return false;
}

bool WindowsService::RemoveService()
{
	// If able to open Service Control Manager to create service,
	SC_HANDLE scm_remove = OpenSCManager(0, 0, STANDARD_RIGHTS_REQUIRED);
	if (scm_remove)
	{
		// If service already exists,
		SC_HANDLE svc_remove = OpenServiceA(scm_remove, _service_name, SERVICE_STOP | DELETE);
		if (svc_remove)
		{
			SERVICE_STATUS status;
			ControlService(svc_remove, SERVICE_CONTROL_STOP, &status);

			if (DeleteService(svc_remove))
			{
				CloseServiceHandle(svc_remove);
				CloseServiceHandle(scm_remove);
				return true;
			}
			else
			{
				FATAL("ServiceManager") << "RemoveService(): Unable to delete service, error " << GetLastError();
			}

			CloseServiceHandle(svc_remove);
		}
		else
		{
			FATAL("ServiceManager") << "RemoveService(): Unable to open service for deletion, error " << GetLastError();
		}

		CloseServiceHandle(scm_remove);
	}
	else
	{
		FATAL("ServiceManager") << "RemoveService(): Unable to open Service Control Manager, error " << GetLastError();
	}

	return false;
}

bool WindowsService::StartServiceMain()
{
	_quit_handle = CreateEvent(0, FALSE, FALSE, 0);
	if (!_quit_handle)
	{
		FATAL("ServiceManager") << "StartServiceMain(): Unable to create quit event";
		return false;
	}

	SERVICE_TABLE_ENTRYA table[] = {
		{ (LPSTR)WindowsService::ii->_service_name, ServiceMain },
		{ 0, 0 }
	};

	if (!StartServiceCtrlDispatcherA(table))
	{
		FATAL("ServiceManager") << "StartServiceMain(): Unable to start service control dispatcher";
		return false;
	}

	return true;
}

void WindowsService::OnControl(DWORD fdwControl)
{
	switch (fdwControl)
	{
	case SERVICE_CONTROL_PAUSE:
		Pause();
		break;

	case SERVICE_CONTROL_CONTINUE:
		Continue();
		break;

	case SERVICE_CONTROL_SHUTDOWN:
	case SERVICE_CONTROL_STOP:
		Shutdown();
		break;
	}
}

void WindowsService::OnMain(DWORD dwNumServicesArgs, LPSTR *lpServiceArgVectors)
{
	_service_handle = RegisterServiceCtrlHandlerA(_service_name, ServiceHandler);

	if (!_service_handle)
	{
		FATAL("ServiceManager") << "ServiceMain(): Unable to register control handler for " << _service_name;
		return;
	}

	Startup();

	// Wait until quit object is signaled
	WaitForSingleObject(_quit_handle, INFINITE);
}

void WINAPI WindowsService::ServiceHandler(DWORD fdwControl)
{
	WindowsService::ii->OnControl(fdwControl);
}

void WINAPI WindowsService::ServiceMain(DWORD dwNumServicesArgs, LPSTR *lpServiceArgVectors)
{
	WindowsService::ii->OnMain(dwNumServicesArgs, lpServiceArgVectors);
}

void WindowsService::SetState(DWORD state)
{
	if (_service_handle)
	{
		SERVICE_STATUS serv_status;

		CAT_OBJCLR(serv_status);
		serv_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
		serv_status.dwCurrentState = state;
		serv_status.dwControlsAccepted = SERVICE_ACCEPT_PAUSE_CONTINUE | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_STOP;

		SetServiceStatus(_service_handle, &serv_status);
	}
}


//// Service Events

void WindowsService::Startup()
{
	SetState(SERVICE_START_PENDING);

	_server = new LoginDatabaseServer;

	if (!_server)
	{
		Shutdown();
		return;
	}

	if (!_server->Initialize())
	{
		Shutdown();
		return;
	}

	SetState(SERVICE_RUNNING);
}

void WindowsService::Shutdown()
{
	SetState(SERVICE_STOP_PENDING);

	// If server is started,
	if (_server)
	{
		_server->Shutdown();

		_server = 0;
	}

	if (_quit_handle != INVALID_HANDLE_VALUE) SetEvent(_quit_handle);

	// Need to set state to stopped or else application will never terminate
	SetState(SERVICE_STOPPED);
}

void WindowsService::Pause()
{
	SetState(SERVICE_PAUSE_PENDING);

	if (_server)
	{
		_server->Pause();
	}

	SetState(SERVICE_PAUSED);
}

void WindowsService::Continue()
{
	SetState(SERVICE_CONTINUE_PENDING);

	if (_server)
	{
		_server->Continue();
	}

	SetState(SERVICE_RUNNING);
}
