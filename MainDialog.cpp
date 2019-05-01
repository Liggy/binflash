///////////////////////////////////////////////////////////////////////
//
//	MainDialog.cpp
//
//		Implementation for the main dialog
//
//	Revision History:
//
// 20130103  v1.64   No change. Add 7717H and x79x drives                                         
// 20130103  v1.63   No change. Make flashing routine for Opti4 compatible with official flasher
// 20120204  v1.62   No change. Add 774x drives
// 20120110  v1.61   No change. Add x25x, x27x, x29x, x80x drives
// 20111222  v1.60   No change. Add G305 drives
// 20111206  v1.59   No change. Add AD-x71xH drives
// 20111122  v1.58   No change. Fix AD-x28xS drives, Add AD-x69xH drives
// 20110615  v1.57   No change. Add optional output on region code reset
// 20110318  v1.56   Add support for region code resets
// 20110220  v1.55   No change. Bugfix for some older NEC drives
// 20110123  v1.54   No change. Add support for AD-x591x drives
// 20101215  v1.53   No change. Add support HP OEM drives
// 20100908  v1.52   No change. Add support for BluRay drives
// 20100823  v1.51   No change. Add support for AD-5680H drive
// 20100119  v1.50   No change. Add support for AD-7230S and AD-7260S series drives
// 20091218  v1.49b  No change. Bugfix for LiteOn OEM drives
// 20091214  v1.49   Allow non-threaded version via define
//                   Add support for AD-7700S series drives
// 20090907  v1.48   Add support for AD-5670S, AD-5960S. Fix bug in message loop
// 20090906  v1.47c  Do not show error message when flashing was OK
// 20090830  v1.47b  No error output for E_CANCELLED
//                   Fixed thread loop CPU consumption
//                   Disable Debug button when flashing or dumping (Debug only)
// 20090425  v1.47   No change. Add support for AD-7240S
// 20090401  v1.47p  Run dump and flash in separate threads to update GUI elements
// 20081215  v1.46   Add support for flashing files containing multiple firmwares
//                   Move version display before flashing to necinterface
// 20080609  v1.45   No change. Add support for AD-x91xA and AD-5170A duplicator drives
// 20071207  v1.43   No change. Add support for AD-x20xA/S drives
// 20071016  v1.42   No change. Add support for AD-5630A/AD-5633A drives
// 20070813  v1.41   No change. Additional check for correct bootcode if provided by firmware
// 20070813  v1.40   No change. Add support for 5170S drives
// 20070404  v1.39   No change. Add support for HR-1100A drives
// 20070326  v1.38   No change. Add support for 7173S drives
// 20061231  v1.37   Remove references to Herrie's site
// 20061215  v1.36   No change. Add support for 7170S drives
// 20061126  v1.35   No change. Fix ID for 7543 drives
// 20061006  v1.34   Default to extended view
// 20061004  v1.33   No change. Fix dumping x57x drives
// 20060927  v1.32   No change. Add support for Optiarc x17x drives
// 20060722  v1.31   Handle okcancel messages
// 20060625  v1.30   No change. Corrected ID for 3650/3651 drives
// 20060525  v1.29   No change. Add support for x56x and 357x drives
// 20060326  v1.27   No change. Fix master/slave detection for 6650 drives
// 20060326  v1.27   No change. Fix notebook detection. Add manifest again (skin support)
// 20060210  v1.26   No change. Add support for 7551
// 20060210  v1.24X  Convert to Visual Studio 2005, remove LG stuff
// 20060105  v1.24   No change. Add support for 3551
// 20051022  v1.23   No change. Add support for 7550
// 20051022  v1.21a  No change.
// 20051016  v1.21   No change. Add support for 3550
// 20051013  v1.20   Change drivetype from int to enum
// 20050802  v1.18   Add yesno to MessageCallback
// 20050524  v1.17   No change. Add support for 6650
// 20050331  v1.16   No change. Add support for 3540
// 20050331  v1.15   No change. Fixed flashing some executable firmwares
// 20050327  v1.14   No change. Add support for 3530
// 20041224  v1.10   No change. Add support for 3520
// 20041126  v1.01   include debug support for debug build
// 20041014  v1.00   version number to 1.00
// 20041009  v0.95a  improved Ricoh bitsetting support
// 20041021  v0.95   new version info function
// 20041015  v0.94c  support for selfflasher to create a
//	                  self flashing executable:
//	                  copy /b necwinflash.exe + firmware.bin self.exe
//	20041014  v0.94   first released version
//	20040912  v0.93   Created
//
#include "BinflashGui.h"
#include "MainDialog.h"
#include "Utils.h"

#include "Hyperlinks.h"
#include "BooktypeDialog.h"
#include "RPCDialog.h"

#define	_MAX_DEVICELENGTH	16
#define  VERBOSITY_RELEASE 1
#define  VERBOSITY_DEBUG 2

// Callbacks
int		MessageCallback(MessageType, const char*,...);
void	ScanCallback(const char* device, const Inquiry* Info);

// Message handlers
BOOL	Main_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
void	Main_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
void	OnDetails();
#ifdef _DEBUG
void	OnDebug();
#endif
void	OnVersion();
void	OnScan();
void	OnFlash();
void	OnDump();
void	OnBookType();
void	OnRegion();

// Private functions
BOOL	FileOpenDialog(LPTSTR lpFile, BOOL bOpen);
void	SetButtons(bool bEnable);
void	DoFileFlash();
void	DoInternalFlash(char* pFilename);
void	ClearOutput();
bool	GetDevice(char * pDevice);
void	SetProgress(int Steps, int Increment);
void	StepProgress();
void	SetStatus(const char* pMessage);
void	ShowDlgItem(int nChild, bool bShow);

// Globals
HWND		_hDialogWnd;
DWORD    _hMainThread;
NECDRIVE	_drive(MessageCallback);
int		_verbosity;
bool		_bExpanded;
int      _numsteps, _currstep;

#if MSVC_VER >= 1600
ITaskbarList3* _pTaskBarlist=NULL;
#endif

#ifdef _DEBUG
int			_nCallbackCount;
#endif

static char			empty_combo[] = "[No devices found]";
static char			homepage[]="http://binflash.cdfreaks.com";
static char			Reboot[] = "In order for the new firmware to become active, you need to restart your computer.";
#ifdef _DEBUG
static const char*	WinVersion="1.64 **DEBUG**";
#else
static const char*	WinVersion="1.64";
#endif
static int			NormalHeight   = 173;
static int			ExpandedHeight = 342;

// MainDialog callback function. Message Crackers handle messages
LRESULT CALLBACK MainDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   switch (uMsg)
   {
      HANDLE_MSG(hDlg, WM_INITDIALOG,	Main_OnInitDialog);
      HANDLE_MSG(hDlg, WM_COMMAND,	Main_OnCommand);
   }
   return FALSE;
}

///////////////////////////////////////////////////////////////////
//	Message Handlers
///////////////////////////////////////////////////////////////////
// Init Dialog
BOOL Main_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
   HWND hwndOwner; 
   RECT rc, rcDlg, rcOwner;
   char szTitle[80];

   _hDialogWnd	= hwnd;
   _verbosity  = 0;
   _bExpanded	= false; // Will be negated in OnDetails

#ifdef _DEBUG
   _drive.SetDebug(false);
#else
   ShowDlgItem(IDB_DEBUG, false);
#endif

   // Set the icons
   SendMessage(hwnd, WM_SETICON, TRUE, (LPARAM) LoadIcon(_hInstance, MAKEINTRESOURCE(IDI_MAIN)));
   SendMessage(hwnd, WM_SETICON, FALSE, (LPARAM) LoadIcon(_hInstance, MAKEINTRESOURCE(IDI_MAIN)));
   _hMainThread=GetCurrentThreadId();

   // Set the window title
   wsprintf(szTitle, "Binflash GUI %s", WinVersion);
   SetWindowText(hwnd, szTitle);

   // Center Dialog
   hwndOwner = GetDesktopWindow();

    GetWindowRect(hwndOwner, &rcOwner); 
    GetWindowRect(hwnd, &rcDlg); 
    CopyRect(&rc, &rcOwner);

    OffsetRect(&rcDlg, -rcDlg.left, -rcDlg.top); 
    OffsetRect(&rc, -rc.left, -rc.top); 
    OffsetRect(&rc, -rcDlg.right, -rcDlg.bottom); 

    SetWindowPos(hwnd, 
        HWND_TOP, 
        rcOwner.left + (rc.right / 2), 
        rcOwner.top + (rc.bottom / 2), 
        0, 0,
        SWP_NOSIZE); 

   // Make hyperlinks
   ConvertStaticToHyperlink(hwnd, IDC_BINFLASH);

   // Hide the progress bar
   ShowDlgItem(IDC_PROGRESS, false);

   // Init the combo box
   OnScan();

   // Set focus on scan button
   SetFocus(GetDlgItem(hwnd, IDB_SCAN));

   // Collapse the dialog
   OnDetails();

   return FALSE;
}

// Command processing
void Main_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
// On Windows version before Windows 7 / Server 2008R2 the following command will fail and _pTaskBarlist stays 0
// This is handled in the SetProgress/StepProgress routines
#if MSVC_VER >= 1600
   if (_pTaskBarlist==NULL)
      CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_ALL, IID_ITaskbarList3, (void**) &_pTaskBarlist);
#endif

   switch (LOWORD(id))
   {
   case IDOK:
      EndDialog(hwnd, TRUE);
      break;
   case IDCANCEL:
      if (IsWindowEnabled(GetDlgItem(hwnd, IDOK)))
         EndDialog(hwnd, FALSE);
      break;
   case IDB_SCAN:
      OnScan();
      break;
   case IDB_DUMP:
      OnDump();
      break;
   case IDB_FLASH:
      OnFlash();
      break;
   case IDB_BOOKTYPE:
      OnBookType();
      break;
   case IDB_REGION:
      OnRegion();
      break;
   case IDB_DETAILS:
      OnDetails();
      break;
#ifdef _DEBUG
   case IDB_DEBUG:
      OnDebug();
      break;
#endif
   case IDC_BINFLASH:
        ShellExecute(hwnd, "open", "http://binflash.cdfreaks.com", NULL, NULL, SW_SHOWNORMAL);
      break;
   }
}

void OnDetails()
{
   RECT rcDlg, rcSts;
   int	 left, top, width, height;
   HWND hStsWnd;

   hStsWnd = GetDlgItem(_hDialogWnd, IDC_STATUS);
   _bExpanded = !_bExpanded;

   // Expand / Collapse dialog
   GetWindowRect(_hDialogWnd, &rcDlg);
   rcDlg.bottom = rcDlg.top + (_bExpanded ? ExpandedHeight : NormalHeight);
   MoveWindow(_hDialogWnd, rcDlg.left, rcDlg.top, 
            rcDlg.right - rcDlg.left, 
            rcDlg.bottom - rcDlg.top, 
            TRUE);

   // Move Status bar to the bottom of the dialog
   GetClientRect(_hDialogWnd, &rcDlg);
   GetWindowRect(hStsWnd, &rcSts);
   ScreenToClient(_hDialogWnd, (LPPOINT) &rcSts);
   ScreenToClient(_hDialogWnd, ((LPPOINT) &rcSts) + 1);
   left	= rcSts.left;
   top		= rcDlg.bottom - 25;
   width	= rcSts.right - rcSts.left;
   height	= rcSts.bottom - rcSts.top;
   MoveWindow(hStsWnd, left, top, width, height, TRUE);

   // Adjust controls based on dialog state
   if (_bExpanded)
   {
      ShowDlgItem(IDC_OUTPUT, true);
      Button_SetText(GetDlgItem(_hDialogWnd, IDB_DETAILS), "Details<<");
      OnVersion();
   }
   else
   {
      ShowDlgItem(IDC_OUTPUT, false);
      Button_SetText(GetDlgItem(_hDialogWnd, IDB_DETAILS), "Details>>");
   }
}

// Toggle debug mode
#ifdef _DEBUG
void OnDebug()
{
   if (IsDlgButtonChecked(_hDialogWnd, IDB_DEBUG) == BST_CHECKED)
      _drive.SetDebug(true);
   else
      _drive.SetDebug(false);
}
#endif

// Get version info
void OnVersion()
{
   ClearOutput();
   WriteOutput(GetDlgItem(_hDialogWnd, IDC_OUTPUT), _bExpanded, 
      "GUI version v%s\r\nDrive interface v%s\r\n%s interface v%s\r\n%s internal v%s\r\n\r\n",
      WinVersion,
      _drive.GetDriveVersion(),
      _drive.GetManufacturer(),
      _drive.GetInterfaceVersion(),
      _drive.GetManufacturer(),
      _drive.GetInternalVersion());
}

// Perform a scan to fill combobox
void OnScan()
{
   HWND hComboWnd = GetDlgItem(_hDialogWnd, IDC_DEVICES);
   ErrorCode rc;

   // suppress info messages
   _verbosity = -1;

   // Reset combo & scan for new devices
   ComboBox_ResetContent(hComboWnd);
   rc = _drive.ScanForDevices(true, ScanCallback);

   if (rc)
      ShowError(_hDialogWnd, rc);
   else
      SetStatus("Devices scanned successfully.");

   if (ComboBox_GetCount(hComboWnd) < 1 || rc)
      ComboBox_InsertString(hComboWnd, 0, empty_combo);

   ComboBox_SetCurSel(hComboWnd, 0);

   // Acivate buttons
   SetButtons(true);
}

// Perform a flash
void OnFlash()
{
   HANDLE hFile;
   DWORD	 dwModuleSize;
   DWORD	 dwFileSize;
   char	 szFileName[_MAX_PATH];
   BOOL	 bInternal = FALSE;

   // Get filesize based on running instance and through
   // filesystem to check whether we have something on our tail
   dwModuleSize = GetSizeOfImage(_hInstance);

   GetModuleFileName(_hInstance, szFileName, sizeof(szFileName));
   hFile = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, 
                  OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
   if (hFile != INVALID_HANDLE_VALUE)
   {
      // Copy the firmware to a buffer when found
      dwFileSize = GetFileSize(hFile, NULL);
      if (dwFileSize > dwModuleSize)
         bInternal = TRUE;
      CloseHandle(hFile);
   }

   if (!bInternal)
      DoFileFlash();
   else
      DoInternalFlash(szFileName);
}

// Change the BookType
void OnBookType()
{
   char szDevice[_MAX_DEVICELENGTH];
   bool bDevice = GetDevice(szDevice);

   if (bDevice)
   {
#ifdef _DEBUG
      _verbosity = VERBOSITY_DEBUG;
#else
      _verbosity = VERBOSITY_RELEASE;
#endif
      SetProp(_hDialogWnd, PROP_NECDRIVE, (HANDLE) &_drive);
      SetProp(_hDialogWnd, PROP_DEVICE, (char*) szDevice);
      DialogBox(_hInstance, (LPCTSTR)IDD_BOOKTYPEDLG, _hDialogWnd, (DLGPROC) BooktypeDialog);
      RemoveProp(_hDialogWnd, PROP_NECDRIVE);
      RemoveProp(_hDialogWnd, PROP_DEVICE);
   }
}

void OnRegion()
{
   char szDevice[_MAX_DEVICELENGTH];
   bool bDevice = GetDevice(szDevice);
   bool _force;

// Force if CTRL was pressed
   if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
      _force=true;
   else
      _force=false;


   if (bDevice)
   {
#ifdef _DEBUG
      _verbosity = VERBOSITY_DEBUG;
#else
      _verbosity = VERBOSITY_RELEASE;
#endif

      SetProp(_hDialogWnd, PROP_NECDRIVE, (HANDLE) &_drive);
      SetProp(_hDialogWnd, PROP_DEVICE, (char*) szDevice);
      SetProp(_hDialogWnd, PROP_FORCE, (bool*) &_force);
      DialogBox(_hInstance, (LPCTSTR)IDD_RPCDLG, _hDialogWnd, (DLGPROC) RPCDialog);
      RemoveProp(_hDialogWnd, PROP_NECDRIVE);
      RemoveProp(_hDialogWnd, PROP_DEVICE);
	   RemoveProp(_hDialogWnd, PROP_FORCE);
   }
}


// Callback for messages
int MessageCallback(MessageType level, const char* format,...)
{
   static char	szData[1024];
   int		rc = 0;
   va_list	arg_ptr;

   va_start(arg_ptr,format);
   rc = vsprintf(szData, format,arg_ptr);
   if (rc>0)
   {
      if ((int) level < 0)
      {
         switch (level)
         {
         case fatal:
            MessageBox(_hDialogWnd,szData,"Fatal Error",MB_ICONERROR);
            break;
         case error:
            MessageBox(_hDialogWnd,szData,"Error",MB_ICONWARNING);
            break;
         case warning:
            MessageBox(_hDialogWnd,szData,"Warning",MB_ICONINFORMATION);
            break;
         case numsteps:
#ifdef _DEBUG
            _nCallbackCount = 0;
            WriteOutput(GetDlgItem(_hDialogWnd, IDC_OUTPUT), _bExpanded, "Step count: %s\r\n", szData);
#endif
            ShowDlgItem(IDC_PROGRESS, true);
            SetProgress(atoi(szData), 1);
            break;
         }
      }
      else if (level <= _verbosity)
         WriteOutput(GetDlgItem(_hDialogWnd, IDC_OUTPUT), _bExpanded, szData);
      else if (level == step)
      {
         StepProgress();
#ifdef _DEBUG
         _nCallbackCount++;
#endif
         WriteOutput(GetDlgItem(_hDialogWnd, IDC_OUTPUT), _bExpanded, szData);
         if (_verbosity >= verbose)
            SetStatus(szData);
      }
      else if (level == yesno)
      {
         if (MessageBox(_hDialogWnd,szData,"Question",MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2)==IDYES)
            rc=1;
         else
            rc=0;
      }
      else if (level == okcancel)
      {
         if (MessageBox(_hDialogWnd,szData,"Action required",MB_OKCANCEL|MB_ICONEXCLAMATION|MB_DEFBUTTON2)==IDOK)
            rc=1;
         else
            rc=0;
      }
   }
   va_end(arg_ptr);

   return rc;
}

// Callback for scan
void ScanCallback(const char* device, const Inquiry* Info)
{
   char szDevice[80];
   wsprintf(szDevice, "%s [%s %s %s]",
               device+((device[5]==':')?4:0),
               Info->vendor,
               Info->identification,
               Info->revision);
#ifdef _DEBUG
   WriteOutput(GetDlgItem(_hDialogWnd, IDC_OUTPUT), _bExpanded, "Specific : %s\r\n",Info->specific);
#endif

   ComboBox_AddString(GetDlgItem(_hDialogWnd, IDC_DEVICES), szDevice);
}

///////////////////////////////////////////////////////////////////
//	Private functions
///////////////////////////////////////////////////////////////////
// Show a file open dialog
BOOL FileOpenDialog(LPTSTR lpFile, BOOL bOpen)
{
   OPENFILENAME ofn;

   // Initialize OPENFILENAME
   ZeroMemory(&ofn, sizeof(ofn));
   ofn.lStructSize		= sizeof(ofn);
   ofn.hwndOwner		= _hDialogWnd;
   ofn.lpstrFile		= lpFile;
   ofn.nMaxFile		= _MAX_PATH - 1;
   ofn.lpstrFilter		= "Firmware Files (*.bin)\0*.bin\0Windows flashers (*.exe)\0*.exe\0All Files (*.*)\0*.*\0\0";
   ofn.lpstrDefExt		= bOpen ? NULL : "bin";
   ofn.nFilterIndex	= 1;
   ofn.lpstrFileTitle	= NULL;
   ofn.nMaxFileTitle	= 0;
   ofn.lpstrInitialDir	= NULL;
   ofn.Flags			= bOpen ? OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST : OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

   return bOpen ? GetOpenFileName(&ofn) : GetSaveFileName(&ofn);
}

// Enable/Disable some buttons based on drive capabilities
void SetButtons(bool bEnable)
{
   char szDevice[_MAX_DEVICELENGTH];
   bool bDevice = GetDevice(szDevice);

   Button_Enable(GetDlgItem(_hDialogWnd, IDC_DEVICES), bEnable);
   Button_Enable(GetDlgItem(_hDialogWnd, IDB_SCAN), bEnable);
   Button_Enable(GetDlgItem(_hDialogWnd, IDB_DUMP), bEnable && _drive.CanDump() && bDevice);
   Button_Enable(GetDlgItem(_hDialogWnd, IDB_FLASH), bEnable && _drive.CanFlash() && bDevice);
   Button_Enable(GetDlgItem(_hDialogWnd, IDB_BOOKTYPE), bEnable && _drive.CanBitset() && bDevice);
   Button_Enable(GetDlgItem(_hDialogWnd, IDB_REGION), bEnable && bDevice);
   Button_Enable(GetDlgItem(_hDialogWnd, IDB_DETAILS), bEnable);
   Button_Enable(GetDlgItem(_hDialogWnd, IDOK), bEnable);
#ifdef _DEBUG
   Button_Enable(GetDlgItem(_hDialogWnd, IDB_DEBUG), bEnable);
#endif
}

// Clear the output window
void ClearOutput()
{
   Edit_SetText(GetDlgItem(_hDialogWnd, IDC_OUTPUT), "");
   WriteOutput(GetDlgItem(_hDialogWnd, IDC_OUTPUT), _bExpanded, 
      "Binflash - %s GUI version - (C) by Liggy, Herrie and r-man\r\n%s\r\n\r\n", _drive.GetManufacturer(), homepage);
}


void ProcessMessages(bool* finished)
{
   MSG msg;

   while (!*finished && GetMessage(&msg,NULL,0,0))
      DispatchMessage(&msg);
}


DWORD WINAPI ThreadHandler(LPVOID lpParam)
{
   ThreadInfo* info=(ThreadInfo*) lpParam;

   if (info->flash)
      info->rc = _drive.FlashFirmware(info->filename, info->device);
   else
      info->rc = _drive.DumpFirmware(info->filename, info->device);
   info->finished=true;
   PostThreadMessage(_hMainThread,WM_QUIT,0,0); // Make GetMessage in loop continue

   return 0;
}


// Flash form external file
void DoFileFlash()
{
   char szFile[_MAX_PATH];

   szFile[0] = 0;

   // Get firmware file
   if (FileOpenDialog(szFile, TRUE))
      DoInternalFlash(szFile);
}

void DoInternalFlash(char* pFilename)
{
   char szVersion[256];
   char szDevice[_MAX_DEVICELENGTH];
   ErrorCode rc;

   // Check for valid device
   if (GetDevice(szDevice))
   {
      SetButtons(false);
      ClearOutput();
      RedrawWindow(_hDialogWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
#ifdef _DEBUG
         _verbosity = VERBOSITY_DEBUG;
#else
         _verbosity = VERBOSITY_RELEASE;
#endif

#if UseThread
      ThreadInfo info;
      HANDLE ThreadHandle;
      info.device=szDevice;
      info.filename=pFilename;
      info.flash=true;
      info.finished=false;
      ThreadHandle=CreateThread(NULL, 0, ThreadHandler, &info, 0, NULL);
      if (!ThreadHandle)
      {
         MessageCallback(error,"Could not create thread for flashing. Have to abort.");
         return;
      }
      while (WaitForSingleObject(ThreadHandle,0)==WAIT_TIMEOUT)
         ProcessMessages(&info.finished);
      rc=info.rc;
      CloseHandle(ThreadHandle);
#else
      rc = _drive.FlashFirmware(pFilename, szDevice);
#endif
      SetButtons(true);
      ShowDlgItem(IDC_PROGRESS, false);
      SetStatus("");
      _verbosity = 0;
#ifdef _DEBUG
      WriteOutput(GetDlgItem(_hDialogWnd, IDC_OUTPUT), _bExpanded, "Callback count: %d\r\n", _nCallbackCount);
#endif
      if (rc==E_UNSUPPORTEDDRIVE)
      {
         if (MessageCallback(yesno,"Flashing your drive's firmware is not yet supported. Would you like to write a drive information file instead?"))
         {
            lstrcpy(szVersion, "identify.txt");
            if (FileOpenDialog(szVersion, FALSE))
            {
               rc = _drive.IdentifyNewDrive(szDevice, szVersion);
               if (rc)
                  ShowError(_hDialogWnd, rc);
            }
         }
      }
      else if (!rc)
      {
         SetStatus("Flash ended successfully.");
         ShowMessage(_hDialogWnd, Reboot, MB_OK | MB_ICONEXCLAMATION);
      }
      else if (rc!=E_CANCELLED)
         ShowError(_hDialogWnd, rc);
      else
         SetStatus("");
   }
   else
      ShowMessage(_hDialogWnd, "Invalid Device.", MB_OK | MB_ICONEXCLAMATION);
}

// Perform a dump
void OnDump()
{
   char szFile[_MAX_PATH];
   char szDevice[_MAX_DEVICELENGTH];
   ErrorCode rc;

   lstrcpy(szFile, "dump.bin");

   if (FileOpenDialog(szFile, FALSE))
   {
      if (GetDevice(szDevice))
      {
         SetButtons(false);
         ClearOutput();
         RedrawWindow(_hDialogWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
#ifdef _DEBUG
            _verbosity = VERBOSITY_DEBUG;
#else
            _verbosity = VERBOSITY_RELEASE;
#endif

#if UseThread
         ThreadInfo info;
         HANDLE ThreadHandle;
         info.device=szDevice;
         info.filename=szFile;
         info.flash=false;
         info.finished=false;
         ThreadHandle=CreateThread(NULL, 0, ThreadHandler, &info, 0, NULL);
         if (!ThreadHandle)
         {
            MessageCallback(error,"Could not create thread for dumping. Have to abort.");
            return;
         }
         while (WaitForSingleObject(ThreadHandle,0)==WAIT_TIMEOUT)
           ProcessMessages(&info.finished);
         CloseHandle(ThreadHandle);
         rc=info.rc;
#else
         rc = _drive.DumpFirmware(szFile, szDevice);
#endif
         SetButtons(true);
         ShowDlgItem(IDC_PROGRESS, false);
         SetStatus("");
#ifdef _DEBUG
         WriteOutput(GetDlgItem(_hDialogWnd, IDC_OUTPUT), _bExpanded, "Callback count: %d\r\n", _nCallbackCount);
#endif
         _verbosity = 0;
         if (rc==E_UNSUPPORTEDDRIVE)
         {
            if (MessageCallback(yesno,"Dumping your drive's firmware is not yet supported. Would you like to write a drive information file instead?"))
            {
               lstrcpy(szFile, "identify.txt");
               if (FileOpenDialog(szFile, FALSE))
                  rc = _drive.IdentifyNewDrive(szDevice, szFile);
                  if (rc)
                     ShowError(_hDialogWnd, rc);
            }
         }
         else if (!rc)
            SetStatus("Dump ended successfully.");
         else if (rc!=E_CANCELLED)
            ShowError(_hDialogWnd, rc);
      }
      else
         ShowMessage(_hDialogWnd, "Invalid Device.", MB_OK | MB_ICONEXCLAMATION);
   }
}

// Get the selected device e.g. X: or 1.0.0
// Device is the first part of the combobox
// terminated with a blank (space)
bool GetDevice(char * pDevice)
{
   char szDevice[80];
   bool rc = false;
   char *p;
   HWND hComboWnd = GetDlgItem(_hDialogWnd, IDC_DEVICES);
   
   if (ComboBox_GetLBText(hComboWnd, ComboBox_GetCurSel(hComboWnd), szDevice) > 0)
   {
      if (lstrcmp(szDevice, empty_combo))
      {
         p = strchr(szDevice, ' ');
         if (p != NULL)
         {
            *p = 0;
            lstrcpyn(pDevice, szDevice,_MAX_DEVICELENGTH - 1);
            rc = true;
         }
      }
   }
   return rc;
}

// Initialize the progress bar
void SetProgress(int Steps, int Increment)
{
   SendMessage(GetDlgItem(_hDialogWnd, IDC_PROGRESS), PBM_SETRANGE, 0, MAKELPARAM (0, Steps));
   SendMessage(GetDlgItem(_hDialogWnd, IDC_PROGRESS), PBM_SETSTEP, Increment, 0);
#if MSVC_VER >= 1600
   if (_pTaskBarlist) // Will be 0 on systems before Win7 / Server2008R2
   {
      _numsteps=Steps;_currstep=0;
      _pTaskBarlist->SetProgressState(_hDialogWnd, TBPF_INDETERMINATE);
   }
#endif
}

// Step the progress bar
void StepProgress()
{
   SendMessage(GetDlgItem(_hDialogWnd, IDC_PROGRESS), PBM_STEPIT, 0, 0);
#if MSVC_VER >= 1600
   if (_pTaskBarlist) // Will be 0 on systems before Win7 / Server2008R2
   {
      _currstep++;
      if (_currstep<_numsteps)
         _pTaskBarlist->SetProgressValue(_hDialogWnd, _currstep, _numsteps);
      else
         _pTaskBarlist->SetProgressState(_hDialogWnd, TBPF_NOPROGRESS);
   }
#endif
}

// Set the Status message
void SetStatus(const char* pMessage)
{
   char* pStatusMessage;

   int len = lstrlen(pMessage);
   if ((len > 0) && (pMessage[len-1] == '\n'))
      len--;
   pStatusMessage=new char[len+1];
   memcpy(pStatusMessage,pMessage,len);
   pStatusMessage[len]=0;
   Edit_SetText(GetDlgItem(_hDialogWnd, IDC_STATUS), pStatusMessage);
   RedrawWindow(GetDlgItem(_hDialogWnd, IDC_STATUS), NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
   delete[] pStatusMessage;
}

// Show/Hide a control on the dialog
void ShowDlgItem(int nChild, bool bShow)
{
   ShowWindow(GetDlgItem(_hDialogWnd, nChild), bShow ? SW_SHOW : SW_HIDE);
}
