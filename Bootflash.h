// Bootflash.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
   #error include 'BootflashAFX.h' before including this file for PCH
#endif

#include "BootflashRes.h"		// main symbols


// CBootflashApp:
// See Bootflash.cpp for the implementation of this class
//

class CBootflashApp : public CWinApp
{
private:
   BOOL GetAppend();

public:
   CBootflashApp();
   ~CBootflashApp();

   BootflashAppend* append;

// Overrides
   virtual BOOL InitInstance();

// Implementation
   DECLARE_MESSAGE_MAP()
};

extern CBootflashApp theApp;
