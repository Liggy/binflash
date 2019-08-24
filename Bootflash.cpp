// Bootflash.cpp : Defines the class behaviors for the application.
//

#include "BootflashAFX.h"
#include "Bootflash.h"
#include "BootflashDlg.h"
#include "Utils.h"
#include "manifest.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CBootflashApp

BEGIN_MESSAGE_MAP(CBootflashApp, CWinApp)
END_MESSAGE_MAP()


// CBootflashApp construction

CBootflashApp::CBootflashApp()
{
   append=NULL;
}

CBootflashApp::~CBootflashApp()
{
   if (append)
      delete append;
}


// The one and only CBootflashApp object

CBootflashApp theApp;


// CBootflashApp initialization

BOOL CBootflashApp::InitInstance()
{
   // InitCommonControls() is required on Windows XP if an application
   // manifest specifies use of ComCtl32.dll version 6 or later to enable
   // visual styles.  Otherwise, any window creation will fail.
   InitCommonControls();

   CWinApp::InitInstance();

   if (GetAppend())
   {
      CBootflashDlg dlg;
      m_pMainWnd = &dlg;
      INT_PTR nResponse = dlg.DoModal();
      return TRUE;
   }
   else
   {
      AfxMessageBox(IDS_INVALID_FILE, MB_OK|MB_ICONSTOP);
      return FALSE;
   }
}


BOOL CBootflashApp::GetAppend()
{
   BOOL rc=FALSE;
   HANDLE hFile;
   HINSTANCE hInstance;
   TCHAR AppName[MAX_PATH+1];
   DWORD imagesize, filesize;

   hInstance=AfxGetInstanceHandle();
   if (!GetModuleFileName(hInstance, AppName, MAX_PATH+1))
      return rc;

   imagesize=GetSizeOfImage(hInstance);

   hFile = CreateFile(AppName, GENERIC_READ, FILE_SHARE_READ, NULL, 
                  OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
   if (hFile == INVALID_HANDLE_VALUE)
      return rc;

   filesize = GetFileSize(hFile, NULL);
   if (filesize==imagesize+sizeof(BootflashAppend))
   {
      DWORD dwRead;

      SetFilePointer(hFile, imagesize, NULL, FILE_BEGIN);
      append=new BootflashAppend;
      if (ReadFile(hFile, append, sizeof(BootflashAppend), &dwRead, NULL)
      && (dwRead==sizeof(BootflashAppend)))
      {
         u8* appenddata=(u8*)append;
         u32 calc=0;
         for (int i=offsetof(BootflashAppend,Firmware);i<offsetof(BootflashAppend,checksum);i+=4)
         {
            calc+=appenddata[i]<<24;
            calc+=appenddata[i+1]<<16;
            calc+=appenddata[i+2]<<8;
            calc+=appenddata[i+3];
         }

         if (calc==append->checksum)
         {
            rc=TRUE;
         }
      }
   }

   CloseHandle(hFile);

   return rc;
}