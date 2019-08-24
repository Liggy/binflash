// BootflashDlg.cpp : implementation file
//
#include "BootflashAFX.h"
#include "Bootflash.h"
#include "BootflashDlg.h"
#include "necinternal.h"
#include "utils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


static Scsi* scsi;


// CBootflashDlg dialog
BEGIN_MESSAGE_MAP(CBootflashDlg, CDialog)
   //{{AFX_MSG_MAP
   ON_WM_SYSCOMMAND()
   ON_WM_PAINT()
   ON_WM_QUERYDRAGICON()
   ON_BN_CLICKED(IDC_RELOAD_DRIVES, OnReloadDrives)
   ON_BN_CLICKED(IDC_FLASH, OnFlash)
   ON_BN_CLICKED(IDCANCEL, OnExit)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()


CBootflashDlg::CBootflashDlg(CWnd* pParent /*=NULL*/)
   : CDialog(CBootflashDlg::IDD, pParent)
{
   m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
   IsAuthenticated=FALSE;
   DidFlash=FALSE;
}

CBootflashDlg::~CBootflashDlg()
{
}

void CBootflashDlg::OnExit()
{
   ClearDriveList();
   EndDialog(IDCANCEL);
}

void CBootflashDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
}

// CBootflashDlg message handlers
BOOL CBootflashDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

   // Add "About..." menu item to system menu.

   // IDM_ABOUTBOX must be in the system command range.
   ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
   ASSERT(IDM_ABOUTBOX < 0xF000);

   CMenu* pSysMenu = GetSystemMenu(FALSE);
   if (pSysMenu != NULL)
   {
      CString strAboutMenu;
      strAboutMenu.LoadString(IDS_ABOUTBOX);
      if (!strAboutMenu.IsEmpty())
      {
         pSysMenu->AppendMenu(MF_SEPARATOR);
         pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
      }
   }

   // Set the icon for this dialog.  The framework does this automatically
   //  when the application's main window is not a dialog
   SetIcon(m_hIcon, TRUE);			// Set big icon
   SetIcon(m_hIcon, FALSE);		// Set small icon

   FillDriveList();

   return TRUE;  // return TRUE  unless you set the focus to a control
}


void CBootflashDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
   if ((nID & 0xFFF0) == IDM_ABOUTBOX)
   {
      CAboutDlg dlgAbout;
      dlgAbout.DoModal();
   }
   else
   {
      CDialog::OnSysCommand(nID, lParam);
   }
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CBootflashDlg::OnPaint() 
{
   if (IsIconic())
   {
      CPaintDC dc(this); // device context for painting

      SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

      // Center icon in client rectangle
      int cxIcon = GetSystemMetrics(SM_CXICON);
      int cyIcon = GetSystemMetrics(SM_CYICON);
      CRect rect;
      GetClientRect(&rect);
      int x = (rect.Width() - cxIcon + 1) / 2;
      int y = (rect.Height() - cyIcon + 1) / 2;

      // Draw the icon
      dc.DrawIcon(x, y, m_hIcon);
   }
   else
   {
      CDialog::OnPaint();
   }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CBootflashDlg::OnQueryDragIcon()
{
   return static_cast<HCURSOR>(m_hIcon);
}


BOOL CBootflashDlg::GetInquiry(Scsi* scsi,Inquiry* driveinfo)
{
   u8 buffer[0x80];

   memset(driveinfo,0,sizeof(Inquiry));
   memset(cdb,0,CDB_SIZE);

   cdb[0]=0x12;cdb[4]=0x60;

   if (scsiSay(scsi,(char*) cdb,6,(char*) buffer,0x60,X1_DATA_IN)<0)
      return false;

   memcpy(driveinfo->vendor,buffer+8,8);
   memcpy(driveinfo->identification,buffer+16,16);
   memcpy(driveinfo->revision,buffer+32,4);
   memcpy(driveinfo->specific,buffer+36,20);

   driveinfo->devicetype=buffer[0]&0x1f;

   return true;
}


BOOL CBootflashDlg::IsUnitReady(Scsi* scsi)
{
   u8 sense[0x10];
   memset(cdb,0,CDB_SIZE);

   if (scsiSay(scsi,(char*) cdb,6,0,0,X0_DATA_NOT)<0)
   {
      if (scsiGetSense(scsi,(char*) sense,0x10,0)>=14)
      {
         if ((sense[12]!=0x30) && (sense[12]!=0x3a) && (sense[12]!=0x3e)) // Drive busy
            return true;
      }
      return false;
   }

   return true;
}


void CBootflashDlg::ClearDriveList()
{
   char* DeviceName;
   CComboBox* DropDown=(CComboBox*) GetDlgItem(IDC_SELECT_DRIVE);
   for (int i=0;i<DropDown->GetCount();i++)
   {
      DeviceName=(char*) DropDown->GetItemDataPtr(i);
      if (DeviceName)
         delete[] DeviceName;
   }
   DropDown->ResetContent();
}


void CBootflashDlg::FillDriveList()
{
   Inquiry inq;
   char DeviceName[256];
   CComboBox* DriveList=(CComboBox*) GetDlgItem(IDC_SELECT_DRIVE);
   
   scsi=newScsi();
   ClearDriveList();
   scsiSetErr(scsi, fopen("NUL", "w"));
   while (scsiReadName(scsi, DeviceName, 255) >= 0)
   {
      if (!scsiOpen(scsi,DeviceName))
      {
         scsiLimitSense(scsi,0x0e);
         scsiLimitSeconds(scsi,20,0);

         if (GetInquiry(scsi, &inq) && inq.devicetype==0x05)
         {
            if (IdentifyDrive(scsi)!=DT_INVALID)
            {
               int i;

               CString DisplayName;
               CString Vendor, Identification, Revision;
               Vendor=CString(inq.vendor).Trim();
               Identification=CString(inq.identification).Trim();
               Revision=CString(inq.revision).Trim();
               DisplayName.Format("%s [%s %s  %s]",
                     DeviceName+((DeviceName[5]==':')?4:0),
                     Vendor, Identification, Revision);
               i=DriveList->AddString(DisplayName);

               if (i>=0)
               {
                  char* NameForList=new char[strlen(DeviceName)+1];
                  strcpy(NameForList,DeviceName);
                  DriveList->SetItemDataPtr(i, (void*) NameForList);
               }
            }
         }
         scsiClose(scsi);
      }
   }
   DriveList->SetCurSel(0);
   scsiSetErr(scsi,stderr);
   scsiClose(scsi);
}



bool CBootflashDlg::RetrieveCalibration(DriveType dt,XFlashCalib* CalibData,u32* chksum)
{
   u8 buffer[0x2000];
   bool ok;

   if (GetFlashSizeFromDriveType(dt)<FlashSize2MOpti4)
   {
      ok=ReadRAM(scsi,buffer,0x4000,0x1000,false,0);
      ok&=ReadRAM(scsi,buffer+0x1000,0x5000,0x1000,false,0);
   }
   else
   {
      ok=ReadRAM(scsi,buffer,0x10000,0x1000,false,0);
      ok&=ReadRAM(scsi,buffer+0x1000,0x20000,0x1000,false,0);
   }

   if (GetFlashSizeFromDriveType(dt)<=FlashSize768K)
   {
      memcpy(CalibData->ID,buffer+0x500,4);
      memcpy(CalibData->RID,buffer+0xf10,4);
      memcpy(CalibData->Drive,buffer+0xf43,4);
   }
   else
   {
      memcpy(CalibData->ID,buffer+0x800,4);
      memcpy(CalibData->RID,buffer+0x1f10,4);
      memcpy(CalibData->Drive,buffer+0x1f43,4);
   }

   if (chksum)
   {
      *chksum=0;
      for (int i=0;i<0x400;i+=4)
      {
         (*chksum)+=(buffer[i]<<24);
         (*chksum)+=(buffer[i+1]<<16);
         (*chksum)+=(buffer[i+2]<<8);
         (*chksum)+=(buffer[i+3]);
      }
   }

   return ok;
}


bool CBootflashDlg::UpdateCalibration(DriveType dt,XFlashCalib* CalibData)
{
   bool ok=true;
   u8 check=0;

// Don't modify this data when it was empty before
   for (int i=0;i<4;i++)
   {
      check|=CalibData->RID[i];
      check|=CalibData->Drive[i];
   }

   if (GetFlashSizeFromDriveType(dt)<=FlashSize768K)
   {
      ok&=WriteCalib(scsi,CalibData->ID,0x4500,4,0);
      if (check)
      {
         ok&=WriteCalib(scsi,CalibData->RID,0x4f10,4,0);
         ok&=WriteCalib(scsi,CalibData->Drive,0x4f43,4,0);
      }
   }
   else if (GetFlashSizeFromDriveType(dt)<FlashSize2MOpti2)
   {
      ok&=WriteCalib(scsi,CalibData->ID,0x4800,4,0);
      if (check)
      {
         ok&=WriteCalib(scsi,CalibData->RID,0x5f10,4,0);
         ok&=WriteCalib(scsi,CalibData->Drive,0x5f43,4,0);
      }
   }
   else
   {
      ok&=WriteCalib(scsi,CalibData->ID,0x10800,4,0);
      if (check)
      {
         ok&=WriteCalib(scsi,CalibData->RID,0x20f10,4,0);
         ok&=WriteCalib(scsi,CalibData->Drive,0x20f43,4,0);
      }
   }

   return ok;
}


void CBootflashDlg::OnReloadDrives()
{
   FillDriveList();
}


void CBootflashDlg::OnFlash()
{
   if (DidFlash)
   {
      AfxMessageBox(IDS_DID_ALREADY_FLASH, MB_OK|MB_ICONINFORMATION);
      return;
   }

   u32 rc=DoBootFlash();

   if (!rc)
      AfxMessageBox(IDS_DO_FLASH, MB_OK|MB_ICONINFORMATION);
   else if (DidFlash)
   {
      CString ErrorFormat,ErrorMessage;
      ErrorFormat.LoadString(IDS_ERROR_FLASHING);
      ErrorMessage.Format(ErrorFormat,rc);
      AfxMessageBox(ErrorMessage, MB_OK|MB_ICONSTOP);
   }
}


u32 CBootflashDlg::Flash640K()
{
   return 0xffff;
}

u32 CBootflashDlg::Flash768K()
{
   u32 rc=0;
   for (int i=8;i<20;i++)
   {
      if (!EraseSingleBlock(scsi,0,i))
         rc=0x1000+i;
   }


   FlashBlock(scsi, false, 0x0f);

   return rc;
}


u32 CBootflashDlg::Flash1M()
{
   u32 rc=0;
   for (int i=2;i<19;i++)
   {
      if (!EraseSingleBlock(scsi,0,i))
         rc=0x1000+i;
   }

   FlashBlock(scsi, false, 0x01);

   return rc;
}


u32 CBootflashDlg::Flash2M(DriveType drivetype,BootflashAppend* append)
{
   u32 rc=0;
   u16 check=0;

   for (int i=0;i<0x3ffe;i+=2)
      check+=((append->Firmware[i]<<8) + (append->Firmware[i+1]));

#if IGNORE_CHK_ERROR
   SendChecksum(scsi,false,0,check,1);
#else
   if (!SendChecksum(scsi,false,0,check,1))
   {
      SetSafeMode(scsi, drivetype, 0, 0);
      return 0x4000;
   }
#endif

   for (int i=4;i<35;i++)
   {
      if (!EraseSingleBlock(scsi,0,i))
         rc=0x1000+i;
   }

   FlashBlock(scsi, false, 0x01);

   return rc;
}


u32 CBootflashDlg::Flash2MOpti(DriveType drivetype,BootflashAppend* append)
{
   u32 rc=0;
   u16 check=0;

   for (int i=0;i<0x7ffe;i+=2)
      check+=((append->Firmware[i]<<8) + (append->Firmware[i+1]));

#if IGNORE_CHK_ERROR
   SendChecksum(scsi,false,0,check,1);
#else
   if (!SendChecksum(scsi,false,0,check,1))
   {
      SetSafeMode(scsi, drivetype, 0, false);
      return 0x4000;
   }
#endif

// Erase from 0x230000 to 0x3effff
   for (int i=3;i<31;i++)
   {
      if (!EraseSingleBlock(scsi,0,i))
         rc=0x1000+i;
   }

   FlashBlock(scsi, false, 0x01);

   return rc;
}


u32 CBootflashDlg::Flash2MOpti2(DriveType drivetype,BootflashAppend* append)
{
   u32 rc=0;
   u16 check=0;

   if (!UnlockBootflash(scsi))
      return 0x4001;

   for (int i=0;i<0x7ffe;i+=2)
      check+=((append->Firmware[i]<<8) + (append->Firmware[i+1]));

#if IGNORE_CHK_ERROR
   SendChecksum(scsi,false,0,check,1);
#else
   if (!SendChecksum(scsi,false,0,check,1))
   {
      SetSafeMode(scsi, drivetype, 0, false);
      return 0x4000;
   }
#endif

// Erase from 0x030000 to 0x1effff
   for (int i=3;i<31;i++)
   {
      if (!EraseBlock(scsi,false,(u8) i))
         rc=0x1000+i;
   }

   FlashBlock(scsi, false, 0x00);

   return rc;
}


u32 CBootflashDlg::Flash2MOpti3(DriveType drivetype,BootflashAppend* append)
{
   u32 rc=0;
   u16 check=0;

   if (!UnlockBootflash(scsi))
      return 0x4001;

   for (int i=0;i<0x7ffe;i+=2)
      check+=((append->Firmware[i]<<8) + (append->Firmware[i+1]));

#if IGNORE_CHK_ERROR
   SendChecksum(scsi,false,0,check,1);
#else
   if (!SendChecksum(scsi,false,0,check,1))
   {
      SetSafeMode(scsi, drivetype, 0, false);
      return 0x4000;
   }
#endif

// Erase from 0x030000 to 0x1effff
   for (u32 i=3;i<31;i++)
   {
      if (!EraseRegion(scsi,false,i*0x10000,0x10000))
         rc=0x1000+i;
   }

   FlashRegion(scsi,false,0x0,0x8000,1);

   return rc;
}


u32 CBootflashDlg::Flash2MOpti4(DriveType drivetype,BootflashAppend* append)
{
   u32 rc=0;
   u16 check=0;

   if (!UnlockBootflash(scsi))
      return 0x4001;

   for (int i=0;i<0x7ffe;i+=2)
      check+=((append->Firmware[i]<<8) + (append->Firmware[i+1]));

#if IGNORE_CHK_ERROR
   SendChecksum(scsi,false,0,check,1);
#else
   if (!SendChecksum(scsi,false,0,check,1))
   {
      SetSafeMode(scsi, drivetype, 0, false);
      return 0x4000;
   }
#endif

// Erase from 0x030000 to 0x1dffff
   for (int i=3;i<30;i++)
   {
      if (!EraseRegion(scsi,false,i*0x10000,0x8000))
         rc=0x1000+i;
   }

// Untested
   FlashRegion(scsi,false,0x0,0x8000,1);

   return rc;
}


u32 CBootflashDlg::DoBootFlash()
{
   bool ok=false;
   bool ChangeID=false;
   DriveType DriveTarget, DriveSource;
   u8 target[4], source[4];
   BootflashAppend AppendCopy;
   XFlashCalib CalibOld, CalibNew;
   FlashType flashsize;
   u32 chksum;
   u32 bootcodesize;
   u32 rc;

   CComboBox* DriveList=(CComboBox*) GetDlgItem(IDC_SELECT_DRIVE);

   if (DriveList->GetCurSel()<0)
      return 1;
   
   scsiOpen(scsi, (char*) DriveList->GetItemDataPtr(DriveList->GetCurSel()));

   DriveSource=CheckXFlash(scsi, IdentifyDrive(scsi));
   memcpy(source,GetDriveID(DriveSource),4);

   memcpy(target,theApp.append->CalibData.ID,4);
   DriveTarget=IdentifyDrive(0, target);

   memcpy(CalibNew.Drive,theApp.append->CalibData.Drive,4);
   memcpy(CalibNew.ID,theApp.append->CalibData.ID,4);
   memcpy(CalibNew.RID,theApp.append->CalibData.RID,4);

   if (DriveSource==DriveTarget)
      ok=TRUE;
#if (ALLOW_XFLASH>=1)
   else if (GetBaseHardware(DriveSource,ALLOW_XFLASH)==GetBaseHardware(DriveTarget,ALLOW_XFLASH))
   {
      ok=TRUE;
      ChangeID=TRUE;
   }
#elif (ALLOW_XFLASH==2)
   else if (((DriveTarget==D355) && (DriveSource=K355)) || ((DriveTarget==K355) && (DriveSource=D355)))
   {
      ok=TRUE;
      ChangeID=TRUE;
   }
#endif

   if (!ok)
   {
      AfxMessageBox(IDS_INCOMPATIBLE_DRIVE,MB_OK|MB_ICONEXCLAMATION);
      scsiClose(scsi);
      return 2;
   }

   ok&=RetrieveCalibration(DriveSource,&CalibOld,&chksum);

   memcpy(&AppendCopy, theApp.append, sizeof(BootflashAppend));

   if (AfxMessageBox(IDS_LAST_WARNING, MB_YESNO|MB_ICONSTOP|MB_DEFBUTTON2)!=IDYES)
   {
      scsiClose(scsi);
      return 5;
   }

   while (IsUnitReady(scsi))
   {
      if (AfxMessageBox(IDS_DRIVE_NOT_READY, MB_OKCANCEL|MB_ICONEXCLAMATION|MB_DEFBUTTON2)!=IDOK)
         return E_OK;
   }

   DidFlash=TRUE; // Didn't actually flash yet, but sufficient to supply a warning

#if ALLOW_XFLASH
   if (ChangeID)
      ok&=UpdateCalibration(DriveSource,&CalibNew);
#endif

   ok&=SetSafeMode(scsi, DriveSource, 0, true);
   bootcodesize=GetBootcodeSizeFromDriveType(DriveSource);
   flashsize=GetFlashSizeFromDriveType(DriveSource);
   if (bootcodesize==0x4000)
      ok&=WriteRAM(scsi,AppendCopy.Firmware,0x0000,4,0,1);
   else if ((flashsize!=FlashSize2MOpti3) && (flashsize!=FlashSize2MOpti4))
      ok&=WriteRAMOpti(scsi,AppendCopy.Firmware,0x0000,8,0,1);
   else
      ok&=WriteRAMOpti3(scsi,AppendCopy.Firmware,0x0000,0x8000,0,1);

   if (!ok)
   {
      SetSafeMode(scsi, DriveSource, 0, false);

#if ALLOW_XFLASH
      if (ChangeID)
      {
         UpdateCalibration(DriveSource,&CalibOld);
      }
#endif
      AfxMessageBox(IDS_ERROR_SEND_BOOTCODE,MB_OK|MB_ICONEXCLAMATION);
      scsiClose(scsi);
      return 6;
   }

   switch (flashsize)
   {
   case FlashSize640K:
      rc=Flash640K();
      break;

   case FlashSize768K:
      rc=Flash768K();
      break;

   case FlashSize1M:
      rc=Flash1M();
      break;

   case FlashSize2M:
      rc=Flash2M(DriveSource,&AppendCopy);
      break;

   case FlashSize2MOpti:
      rc=Flash2MOpti(DriveSource,&AppendCopy);
      break;

   case FlashSize2MOpti2:
      rc=Flash2MOpti2(DriveSource,&AppendCopy);
      break;

   case FlashSize2MOpti3:
      rc=Flash2MOpti3(DriveSource,&AppendCopy);
      break;

   case FlashSize2MOpti4:
      rc=Flash2MOpti4(DriveSource,&AppendCopy);
      break;

   default: // Should not happen
      DidFlash=FALSE;
      rc=0xffff;
      break;
   }

#if ALLOW_XFLASH
   if (rc && ChangeID)
   {
// Try to revert calibration data if conversion failed
      UpdateCalibration(DriveSource,&CalibOld);
   }
#endif
   scsiClose(scsi);
   return rc;
}



// About dialog
BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
}
