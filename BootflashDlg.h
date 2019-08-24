// BootflashDlg.h : header file
//
#pragma once


// Information received after inquiry
struct Inquiry
{
   char vendor[9];
   char identification[17];
   char revision[5];
   char specific[21];

   u8 devicetype;
};



// CBootflashDlg dialog
class CBootflashDlg : public CDialog
{
private:
   enum { IDD = IDD_BOOTFLASH_DIALOG };
   BOOL GetInquiry(Scsi* scsi,Inquiry* driveinfo);
   BOOL IsUnitReady(Scsi* scsi);
   u8 cdb[12];
   BOOL IsAuthenticated;
   BOOL DidFlash;
   u32 DoBootFlash();

   bool DecryptBootcodeHTTP(BootflashAppend* append,u32 checksum);

   bool RetrieveCalibration(DriveType,XFlashCalib*,u32* chksum=0);
   bool UpdateCalibration(DriveType,XFlashCalib*);

   u32 Flash640K();
   u32 Flash768K();
   u32 Flash1M();
   u32 Flash2M(DriveType,BootflashAppend*);
   u32 Flash2MOpti(DriveType,BootflashAppend*);
   u32 Flash2MOpti2(DriveType,BootflashAppend*);
   u32 Flash2MOpti3(DriveType,BootflashAppend*);
   u32 Flash2MOpti4(DriveType,BootflashAppend*);

protected:
   DECLARE_MESSAGE_MAP()

   HICON m_hIcon;

   // Generated message map functions
   virtual BOOL OnInitDialog();
   afx_msg void OnExit();
   afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
   afx_msg void OnPaint();
   afx_msg HCURSOR OnQueryDragIcon();
   afx_msg void OnReloadDrives();
   virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

   void ClearDriveList();
   void FillDriveList();

public:
   CBootflashDlg(CWnd* pParent = NULL);	// standard constructor
   ~CBootflashDlg();
   afx_msg void OnFlash();
};


// CAboutDlg dialog used for App About
class CAboutDlg : public CDialog
{
public:
   CAboutDlg();

// Dialog Data
   enum { IDD = IDD_ABOUTBOX };

   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
   DECLARE_MESSAGE_MAP()
};
