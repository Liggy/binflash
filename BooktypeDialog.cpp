// 1.13  - 20050312 - changed dropdown boxes

#include "BinflashGui.h"
#include "BooktypeDialog.h"
#include "Utils.h"

// Message handlers
BOOL	Booktype_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
void	Booktype_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);

void	OnApply(HWND hwnd);
void	OnReset(HWND hwnd);

void	ShowBooktypes(HWND hwnd);
void	SetDL(HWND hwnd, Mediatype mtPerm, Mediatype mtTemp, int version);
void	SetSL(HWND hwnd, Mediatype mtPerm, Mediatype mtTemp, int version);
void	SetRW(HWND hwnd, Mediatype mtPerm, Mediatype mtTemp, int version);

Mediatype _activeDL;
Mediatype _activeSL;
Mediatype _activeRW;

// MainDialog callback function. Message Crackers handle messages
LRESULT CALLBACK BooktypeDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   switch (uMsg)
   {
      HANDLE_MSG(hDlg, WM_INITDIALOG,	Booktype_OnInitDialog);
      HANDLE_MSG(hDlg, WM_COMMAND,	Booktype_OnCommand);
   }
   return FALSE;
}

///////////////////////////////////////////////////////////////////
//	Message Handlers
///////////////////////////////////////////////////////////////////
// Init Dialog
BOOL Booktype_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
   HWND hwndOwner; 
   RECT rc, rcDlg, rcOwner;

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

   ShowBooktypes(hwnd);

   return FALSE;
}

// Command processing
void Booktype_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
   switch (LOWORD(id))
   {
   case IDOK:
   case IDCANCEL:
      EndDialog(hwnd, TRUE);
      break;
   case BTN_APPLY:
      OnApply(hwnd);
      break;
   case BTN_RESET:
      OnReset(hwnd);
      break;
   }
}

// Apply the new changes
void OnApply(HWND hwnd)
{
   Mediatype newDL;
   Mediatype newSL;
   Mediatype newRW;
   int nError = 0;
   int nSuccess = 0;

   newDL = (Mediatype) ComboBox_GetItemData(GetDlgItem(hwnd, IDC_DL), ComboBox_GetCurSel(GetDlgItem(hwnd, IDC_DL)));
   newSL = (Mediatype) ComboBox_GetItemData(GetDlgItem(hwnd, IDC_SL), ComboBox_GetCurSel(GetDlgItem(hwnd, IDC_SL)));
   newRW = (Mediatype) ComboBox_GetItemData(GetDlgItem(hwnd, IDC_RW), ComboBox_GetCurSel(GetDlgItem(hwnd, IDC_RW)));

   // Apply changes
   if (newDL != _activeDL || newSL != _activeSL || newRW != _activeRW)
   {
      HWND hParentWnd		= GetParent(hwnd);
      char* pDevice		= (char*) GetProp(hParentWnd, PROP_DEVICE);
      NECDRIVE* pDrive	= (NECDRIVE*) GetProp(hParentWnd, PROP_NECDRIVE);
      ErrorCode rc		= E_OK;

      if (newDL != _activeDL)
      {
         rc = pDrive->SetBooktype(pDevice, MT_PlusR9, newDL, false);
         if (!rc)
            nSuccess++;
         else
            nError++;
      }

      if (newSL != _activeSL)
      {
         rc = pDrive->SetBooktype(pDevice, MT_PlusR, newSL, false);
         if (!rc)
            nSuccess++;
         else
            nError++;
      }

      if (newRW != _activeRW)
      {
         rc = pDrive->SetBooktype(pDevice, MT_PlusRW, newRW, false);
         if (!rc)
            nSuccess++;
         else
            nError++;
      }

      if (nError == 0)
         ShowMessage(hwnd, "Booktype(s) changed.", MB_OK | MB_ICONINFORMATION);
      else
      {
         if (nSuccess == 0 && nError > 0)
            ShowError(hwnd, rc);
         else
            ShowMessage(hwnd, "Some booktype(s) could not be changed.", MB_OK | MB_ICONEXCLAMATION);
      }

      ShowBooktypes(hwnd);
   }
}

void OnReset(HWND hwnd)
{
   if (ShowMessage(hwnd, 
               "Resetting the booktype change counter also\n"
               "changes the booktypes back to their factory\n"
               "defaults. Do you want to continue?",
               MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) == IDYES)
   {
      HWND hParentWnd		= GetParent(hwnd);
      char* pDevice		= (char*) GetProp(hParentWnd, PROP_DEVICE);
      NECDRIVE* pDrive	= (NECDRIVE*) GetProp(hParentWnd, PROP_NECDRIVE);
      ErrorCode rc		= E_OK;

      rc = pDrive->ClearBTSettings(pDevice);

      if (rc)
         ShowError(hwnd, rc);

      ShowBooktypes(hwnd);
   }
}

void ShowBooktypes(HWND hwnd)
{
   BooktypeInfo btPerm;
   BooktypeInfo btTemp;
   HWND hParentWnd		= GetParent(hwnd);
   char* pDevice		= (char*) GetProp(hParentWnd, PROP_DEVICE);
   NECDRIVE* pDrive	= (NECDRIVE*) GetProp(hParentWnd, PROP_NECDRIVE);
   ErrorCode rc		= E_OK;
   int version			= 0;

   _activeSL = _activeDL = _activeRW = MT_error;
   
   // MT_PlusR9 is supported by NEC versions 2 and higher
   rc = pDrive->QuerySingleBooktype(pDevice, MT_PlusR9, false, &btPerm);
   if (!rc)
   {
      if (btPerm.ricoh == false)
      {
         char szLeft[16];
         wsprintf(szLeft, " %d", btPerm.changesleft);
         Static_SetText(GetDlgItem(hwnd, IDC_LEFT), szLeft);
         version = (int) btPerm.version;
         WriteOutput(GetDlgItem(hParentWnd, IDC_OUTPUT), _bExpanded, "Using NEC bitsetting commands version %d\r\n", version);
      }
      else
      {
         ShowWindow(GetDlgItem(hwnd, TXT_LEFT), SW_HIDE);
         ShowWindow(GetDlgItem(hwnd, IDC_LEFT), SW_HIDE);
         ShowWindow(GetDlgItem(hwnd, BTN_RESET), SW_HIDE);
         WriteOutput(GetDlgItem(hParentWnd, IDC_OUTPUT), _bExpanded, "Using Ricoh bitsetting commands\r\n");
      }

      // Query various booktype settings and show/hide
      // then on the dialog.
      ZeroMemory(&btTemp, sizeof(BooktypeInfo));
      btTemp.booktype = MT_error;
      if (btPerm.usetemp)
         pDrive->QuerySingleBooktype(pDevice, MT_PlusR9, true, &btTemp);
      SetDL(hwnd, btPerm.booktype, btTemp.booktype, version);

      // MT_PlusR is supported by NEC versions 3 and higher
      pDrive->QuerySingleBooktype(pDevice, MT_PlusR, false, &btPerm);
      btTemp.booktype = MT_error;
      if (btPerm.usetemp)
         pDrive->QuerySingleBooktype(pDevice, MT_PlusR, true, &btTemp);
      SetSL(hwnd, btPerm.booktype, btTemp.booktype, version);

      // MT_PlusRW is supported by NEC versions 4 and higher
      pDrive->QuerySingleBooktype(pDevice, MT_PlusRW, false, &btPerm);
      btTemp.booktype = MT_error;
      if (btPerm.usetemp)
         pDrive->QuerySingleBooktype(pDevice, MT_PlusRW, true, &btTemp);
      SetRW(hwnd, btPerm.booktype, btTemp.booktype, version);
   }

   if (rc)
      ShowError(hwnd, rc);
}

void SetDL(HWND hwnd, Mediatype mtPerm, Mediatype mtTemp, int version)
{
   char szData[32];
   HWND hGroupWnd	= GetDlgItem(hwnd, GRP_R9);
   HWND hComboWnd	= GetDlgItem(hwnd, IDC_DL);
   BOOL bEnable;
   int	 nIndex;
   int	 nSelIndex;

   nIndex		= -1;
   nSelIndex	= 0;
   _activeDL	= mtPerm;
   if (version == 0)
      bEnable = false;
   else
      bEnable = (version >= 2);

   ComboBox_ResetContent(hComboWnd);

   if (mtPerm == MT_error)
   {
      nIndex = ComboBox_AddString(hComboWnd, bEnable ? "Factory default (active)" : "Not supported");
      ComboBox_SetItemData(hComboWnd, nIndex, mtPerm);
      nSelIndex = nIndex;
   }

   lstrcpy(szData, "dvd+r dl");
   if (mtPerm == MT_PlusR9)
   {
      lstrcat(szData, " (active)");
      nSelIndex = nIndex+1;
   }
   nIndex = ComboBox_AddString(hComboWnd, szData);
   ComboBox_SetItemData(hComboWnd, nIndex, MT_PlusR9);

   lstrcpy(szData, "dvd-rom");
   if (mtPerm == MT_ROM)
   {
      lstrcat(szData, " (active)");
      nSelIndex = nIndex+1;
   }
   nIndex = ComboBox_AddString(hComboWnd, szData);
   ComboBox_SetItemData(hComboWnd, nIndex, MT_ROM);

   lstrcpy(szData, "dvd+r");
   if (mtPerm == MT_PlusR)
   {
      lstrcat(szData, " (active)");
      nSelIndex = nIndex+1;
   }
   nIndex = ComboBox_AddString(hComboWnd, szData);
   ComboBox_SetItemData(hComboWnd, nIndex, MT_PlusR);

   lstrcpy(szData, "dvd-r");
   if (mtPerm == MT_MinusR)
   {
      lstrcat(szData, " (active)");
      nSelIndex = nIndex+1;
   }
   nIndex = ComboBox_AddString(hComboWnd, szData);
   ComboBox_SetItemData(hComboWnd, nIndex, MT_MinusR);

   lstrcpy(szData, "dvd+rw");
   if (mtPerm == MT_PlusRW)
   {
      lstrcat(szData, " (active)");
      nSelIndex = nIndex+1;
   }
   nIndex = ComboBox_AddString(hComboWnd, szData);
   ComboBox_SetItemData(hComboWnd, nIndex, MT_PlusRW);

   lstrcpy(szData, "dvd-rw");
   if (mtPerm == MT_MinusRW)
   {
      lstrcat(szData, " (active)");
      nSelIndex = nIndex+1;
   }
   nIndex = ComboBox_AddString(hComboWnd, szData);
   ComboBox_SetItemData(hComboWnd, nIndex, MT_MinusRW);

   ComboBox_SetCurSel(hComboWnd, nSelIndex);
   ComboBox_Enable(hComboWnd, bEnable);

   switch(mtTemp)
   {
   case MT_PlusR9:
      Static_SetText(hGroupWnd, "DVD+R DL (DVD+R)");
      break;
   case MT_ROM:
      Static_SetText(hGroupWnd, "DVD+R DL (DVD-ROM)");
      break;
   default:
      Static_SetText(hGroupWnd, "DVD+R DL (none)");
   }
}

void SetSL(HWND hwnd, Mediatype mtPerm, Mediatype mtTemp, int version)
{
   char szData[32];
   HWND hGroupWnd	= GetDlgItem(hwnd, GRP_R);
   HWND hComboWnd	= GetDlgItem(hwnd, IDC_SL);
   BOOL bEnable;
   int	 nIndex;
   int	 nSelIndex;

   nIndex		= -1;
   nSelIndex	= 0;
   _activeSL	= mtPerm;
   if (version == 0)
      bEnable = true;
   else
      bEnable = (version >= 3);

   ComboBox_ResetContent(hComboWnd);

   if (mtPerm == MT_error)
   {
      nIndex = ComboBox_AddString(hComboWnd, bEnable ? "Factory default (active)" : "Not supported");
      ComboBox_SetItemData(hComboWnd, nIndex, mtPerm);
      nSelIndex = nIndex;
   }

   lstrcpy(szData, "dvd+r");
   if (mtPerm == MT_PlusR)
   {
      lstrcat(szData, " (active)");
      nSelIndex = nIndex+1;
   }
   nIndex = ComboBox_AddString(hComboWnd, szData);
   ComboBox_SetItemData(hComboWnd, nIndex, MT_PlusR);

   lstrcpy(szData, "dvd-rom");
   if (mtPerm == MT_ROM)
   {
      lstrcat(szData, " (active)");
      nSelIndex = nIndex+1;
   }
   nIndex = ComboBox_AddString(hComboWnd, szData);
   ComboBox_SetItemData(hComboWnd, nIndex, MT_ROM);

   lstrcpy(szData, "dvd-r");
   if (mtPerm == MT_MinusR)
   {
      lstrcat(szData, " (active)");
      nSelIndex = nIndex+1;
   }
   nIndex = ComboBox_AddString(hComboWnd, szData);
   ComboBox_SetItemData(hComboWnd, nIndex, MT_MinusR);

   lstrcpy(szData, "dvd+rw");
   if (mtPerm == MT_PlusRW)
   {
      lstrcat(szData, " (active)");
      nSelIndex = nIndex+1;
   }
   nIndex = ComboBox_AddString(hComboWnd, szData);
   ComboBox_SetItemData(hComboWnd, nIndex, MT_PlusRW);

   lstrcpy(szData, "dvd-rw");
   if (mtPerm == MT_MinusRW)
   {
      lstrcat(szData, " (active)");
      nSelIndex = nIndex+1;
   }
   nIndex = ComboBox_AddString(hComboWnd, szData);
   ComboBox_SetItemData(hComboWnd, nIndex, MT_MinusRW);

   lstrcpy(szData, "dvd+r dl");
   if (mtPerm == MT_PlusR9)
   {
      lstrcat(szData, " (active)");
      nSelIndex = nIndex+1;
   }
   nIndex = ComboBox_AddString(hComboWnd, szData);
   ComboBox_SetItemData(hComboWnd, nIndex, MT_PlusR9);

   ComboBox_SetCurSel(hComboWnd, nSelIndex);
   ComboBox_Enable(hComboWnd, bEnable);

   switch(mtTemp)
   {
   case MT_PlusR:
      Static_SetText(hGroupWnd, "DVD+R SL (DVD+R)");
      break;
   case MT_ROM:
      Static_SetText(hGroupWnd, "DVD+R SL (DVD-ROM)");
      break;
   default:
      Static_SetText(hGroupWnd, "DVD+R SL (none)");
   }
}

void SetRW(HWND hwnd, Mediatype mtPerm, Mediatype mtTemp, int version)
{
   char szData[32];
   HWND hGroupWnd	= GetDlgItem(hwnd, GRP_RW);
   HWND hComboWnd	= GetDlgItem(hwnd, IDC_RW);
   BOOL bEnable;
   int	 nIndex;
   int	 nSelIndex;

   nIndex		= -1;
   nSelIndex	= 0;
   _activeRW	= mtPerm;
   if (version == 0)
      bEnable = true;
   else
      bEnable = (version >= 4);

   ComboBox_ResetContent(hComboWnd);

   if (mtPerm == MT_error)
   {
      nIndex = ComboBox_AddString(hComboWnd, bEnable ? "Factory default (active)" : "Not supported");
      ComboBox_SetItemData(hComboWnd, nIndex, mtPerm);
      nSelIndex = nIndex;
   }

   lstrcpy(szData, "dvd+rw");
   if (mtPerm == MT_PlusRW)
   {
      lstrcat(szData, " (active)");
      nSelIndex = nIndex+1;
   }
   nIndex = ComboBox_AddString(hComboWnd, szData);
   ComboBox_SetItemData(hComboWnd, nIndex, MT_PlusRW);

   lstrcpy(szData, "dvd-rom");
   if (mtPerm == MT_ROM)
   {
      lstrcat(szData, " (active)");
      nSelIndex = nIndex+1;
   }
   nIndex = ComboBox_AddString(hComboWnd, szData);
   ComboBox_SetItemData(hComboWnd, nIndex, MT_ROM);

   lstrcpy(szData, "dvd+r");
   if (mtPerm == MT_PlusR)
   {
      lstrcat(szData, " (active)");
      nSelIndex = nIndex+1;
   }
   nIndex = ComboBox_AddString(hComboWnd, szData);
   ComboBox_SetItemData(hComboWnd, nIndex, MT_PlusR);

   lstrcpy(szData, "dvd-r");
   if (mtPerm == MT_MinusR)
   {
      lstrcat(szData, " (active)");
      nSelIndex = nIndex+1;
   }
   nIndex = ComboBox_AddString(hComboWnd, szData);
   ComboBox_SetItemData(hComboWnd, nIndex, MT_MinusR);

   lstrcpy(szData, "dvd-rw");
   if (mtPerm == MT_MinusRW)
   {
      lstrcat(szData, " (active)");
      nSelIndex = nIndex+1;
   }
   nIndex = ComboBox_AddString(hComboWnd, szData);
   ComboBox_SetItemData(hComboWnd, nIndex, MT_MinusRW);

   lstrcpy(szData, "dvd+r dl");
   if (mtPerm == MT_PlusR9)
   {
      lstrcat(szData, " (active)");
      nSelIndex = nIndex+1;
   }
   nIndex = ComboBox_AddString(hComboWnd, szData);
   ComboBox_SetItemData(hComboWnd, nIndex, MT_PlusR9);

   ComboBox_SetCurSel(hComboWnd, nSelIndex);
   ComboBox_Enable(hComboWnd, bEnable);

   switch(mtTemp)
   {
   case MT_PlusRW:
      Static_SetText(hGroupWnd, "DVD+RW (DVD+RW)");
      break;
   case MT_ROM:
      Static_SetText(hGroupWnd, "DVD+RW (DVD-ROM)");
      break;
   default:
      Static_SetText(hGroupWnd, "DVD+RW (none)");
   }
}