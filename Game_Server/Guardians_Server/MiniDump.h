#pragma once

#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <DbgHelp.h>

#define MINIDUMP CMiniDump::GetInstance()

class CMiniDump
{
public:
	static CMiniDump* GetInstance()
	{
		static CMiniDump instance;
		return &instance;
	}

	BOOL Begin(VOID);
	BOOL End(VOID);
};