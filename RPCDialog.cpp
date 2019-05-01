// 1.56  - 20110318 - added region code reset
// 1.57  - 20110615 - extended region code reset. Allow forcing reset when CTRL was clicked on opening the dialog

#include "BinflashGui.h"
#include "RPCDialog.h"
#include "Utils.h"


// Message handlers
BOOL	RPC_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
void	RPC_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
void	OnRPCReset(HWND hwnd,bool global);
void	ShowRegion(HWND hwnd);


// MainDialog callback function. Message Crackers handle messages
LRESULT CALLBACK RPCDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   switch (uMsg)
   {
      HANDLE_MSG(hDlg, WM_INITDIALOG,	RPC_OnInitDialog);
      HANDLE_MSG(hDlg, WM_COMMAND,	RPC_OnCommand);
   }
   return FALSE;
}


///////////////////////////////////////////////////////////////////
//	Message Handlers
///////////////////////////////////////////////////////////////////
// Init Dialog
BOOL RPC_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
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

    ShowRegion(hwnd);

    return FALSE;
}


// Command processing
void RPC_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
   switch (LOWORD(id))
   {
   case IDOK:
   case IDCANCEL:
      EndDialog(hwnd, TRUE);
      break;
   case IDC_RPC_RESET:
      OnRPCReset(hwnd,false);
      break;
   case IDC_RPC_GLOBAL_RESET:
      OnRPCReset(hwnd,true);
      break;
   }
}


void OnRPCReset(HWND hwnd, bool global)
{
   HWND hParentWnd	= GetParent(hwnd);
   char* pDevice		= (char*) GetProp(hParentWnd, PROP_DEVICE);
   NECDRIVE* pDrive	= (NECDRIVE*) GetProp(hParentWnd, PROP_NECDRIVE);
   bool _force       = *(bool*) GetProp(hParentWnd,PROP_FORCE);
   ErrorCode rc;

   rc=pDrive->ResetRegion(pDevice,global?"global":"vendor",_force);

   if (rc)
      ShowError(hwnd, rc);
   else
      ShowMessage(hwnd, "Region counter was reset", MB_OK | MB_ICONINFORMATION);

   ShowRegion(hwnd);
}


void ShowRegion(HWND hwnd)
{
   RegionInfo info;
   HWND hParentWnd	= GetParent(hwnd);
   char* pDevice		= (char*) GetProp(hParentWnd, PROP_DEVICE);
   NECDRIVE* pDrive	= (NECDRIVE*) GetProp(hParentWnd, PROP_NECDRIVE);
   bool _force       = *(bool*) GetProp(hParentWnd,PROP_FORCE);

   if (pDrive->GetRegion(pDevice,&info)==E_OK)
   {
      EnableWindow(GetDlgItem(hwnd,IDC_RPC_RESET),true);
      EnableWindow(GetDlgItem(hwnd,IDC_RPC_GLOBAL_RESET),true);

      if (info.RPC2)
      {
         SetDlgItemText(hwnd,IDC_RPC_STATE,"RPC2");
         SetDlgItemInt(hwnd,IDC_RPC_USER,info.UserChanges,false);
         SetDlgItemInt(hwnd,IDC_RPC_VENDOR,info.VendorChanges,false);

         if (info.Region)
            SetDlgItemInt(hwnd,IDC_RPC_REGION,info.Region,false);
         else
            SetDlgItemText(hwnd,IDC_RPC_REGION,"none set");
      }
      else
      {
         SetDlgItemText(hwnd,IDC_RPC_STATE,"RPC1");
         SetDlgItemText(hwnd,IDC_RPC_USER,"n/a");
         SetDlgItemText(hwnd,IDC_RPC_VENDOR,"n/a");
         SetDlgItemText(hwnd,IDC_RPC_REGION,"n/a");
      }

      if (!_force)
      {
         if (!info.VendorChanges || (info.UserChanges==5) || !info.RPC2)
            EnableWindow(GetDlgItem(hwnd,IDC_RPC_RESET),false);
         if (((info.VendorChanges==4) && (info.UserChanges==5)) || !info.RPC2)
            EnableWindow(GetDlgItem(hwnd,IDC_RPC_GLOBAL_RESET),false);
      }
   }
   else
   {
      EnableWindow(GetDlgItem(hwnd,IDC_RPC_RESET),false);
      EnableWindow(GetDlgItem(hwnd,IDC_RPC_GLOBAL_RESET),false);
      SetDlgItemText(hwnd,IDC_RPC_STATE,"n/a");
   }
}
