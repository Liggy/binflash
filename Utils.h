#pragma once

// 0->No XFlash, 1->XFlash allowed, 2->D355<->K355, G26*<->G24* and G28*<->G30* also allowed
#define ALLOW_XFLASH 2
#define VERSION 0
#define HTTP_AUTH 0
#define IGNORE_CHK_ERROR 0

DWORD GetSizeOfImage(PVOID pImageBase);
int WriteOutput(HWND hEditWnd, BOOL _bExpanded, const char * pFormat, ...);
void ShowError(HWND hwnd, ErrorCode errnum);
int ShowMessage(HWND hwnd, const char * pMessage, UINT flags);
u32 CryptString(char* input);
u32 DiffSub(u32 in);
u32 DiffAdd(u32 in);
u32 Turn(u32 in);
u32 Rotate(u32 in,u8 bit);
u32 CalcResponse(u32 chk);
