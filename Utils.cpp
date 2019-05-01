#include "BinflashGui.h"
#include "Utils.h"

// Get filesize of module
DWORD GetSizeOfImage(PVOID pImageBase)
{
    // get DOS header
    IMAGE_DOS_HEADER * pDosHeader = (IMAGE_DOS_HEADER *)pImageBase;

    // find an offset to the main PE header ...
    IMAGE_FILE_HEADER * pFileHeader = 
        (IMAGE_FILE_HEADER *)(((LPBYTE)pImageBase) + pDosHeader->e_lfanew + sizeof(IMAGE_NT_SIGNATURE));

    // ... and optional PE header
    IMAGE_OPTIONAL_HEADER * pOptHeader = 
        (IMAGE_OPTIONAL_HEADER *)(((LPBYTE)pFileHeader) + IMAGE_SIZEOF_FILE_HEADER);

    // calculate the size
    DWORD nSizeOfImage = pOptHeader->SizeOfHeaders;

    IMAGE_SECTION_HEADER * pSecHeader = 
        (IMAGE_SECTION_HEADER *)(((LPBYTE)pOptHeader) + pFileHeader->SizeOfOptionalHeader);

    // sum size of all image sections; this will result in the image size
    for (int i = 0; i < pFileHeader->NumberOfSections; i++, pSecHeader++)
        nSizeOfImage += pSecHeader->SizeOfRawData;

    // return size of the executable
    return nSizeOfImage;
}

// Write output
int WriteOutput(HWND hEditWnd, BOOL _bExpanded, const char * pFormat, ...)
{
   char	szData[1024];
   va_list argList;
   int		size;
   int		rc = 0;

   if (pFormat != NULL && _bExpanded)
   {
      va_start(argList, pFormat);
      rc = vsprintf(szData, pFormat, argList);
      va_end(argList);

      // Edit box must have cr/lf as a line terminator
      if (rc > 1)
      {
         if (szData[rc-1] == '\n' && szData[rc-2] != '\r')
         {
            while (rc > 0 && szData[rc-1] < ' ')
               szData[--rc] = 0;
            lstrcat(szData, "\r\n");
            rc += 2;
         }
      }
      size = Edit_GetTextLength(hEditWnd);
      Edit_SetSel(hEditWnd, size, size);
      Edit_ReplaceSel(hEditWnd, szData);
   }

   return rc;
}

// Show an Error Message
void ShowError(HWND hwnd, ErrorCode errnum)
{
   char* errtext;

   switch (errnum)
   {
   case E_CANCELLED:
      errtext="Action canceled";break;
   case E_ACCESSFIRMFILE:
      errtext="Error accessing firmware file";break;
   case E_NOMEM:
      errtext="Could not allocate enough memory";break;
   case E_SCSINEW:
      errtext="Error creating SCSI handle";break;
   case E_SCSIOPEN:
      errtext="Error opening SCSI device";break;
   case E_NODRIVETYPE:
      errtext="Drivetype could not be determined";break;
   case E_UNSUPPORTEDDRIVE:
      errtext="Drive is not supported";break;
   case E_DUMPNOSUPPORT:
      errtext="Dumping not supported with this flasher";break;
   case E_DUMPERROR:
      errtext="Error while dumping firmware";break;
   case E_FLASHNOSUPPORT:
      errtext="Flashing not supported with this flasher";break;
   case E_FLASHERROR:
      errtext="Error while flashing firmware";break;
   case E_FIRMNOMATCH:
      errtext="Firmware is for a different drive";break;
   case E_NOFIRMWARE:
      errtext="Supplied file does not contain a valid firmware";break;
   case E_BOOKNOTSUPPORTED:
      errtext="This booktype operation is not supported with your firmware";break;
   case E_BOOKTYPEMEDIA:
      errtext="Operation not supported on current media";break;
   case E_BOOKINVALIDNAME:
      errtext="Invalid name for mediatype or booktype";break;
   case E_IDENTIFYBOOTKEY:
      errtext="Could not find key";break;
   case E_IDENTIFYMEM:
      errtext="Could not read drive memory";break;
   case E_RPC1:
      errtext="Cannot reset RPC1 drives";break;
   case E_RPCNOVENDOR:
      errtext="No vendor resets left";break;
   case E_RPCNOTNEEDED:
      errtext="No reset required";break;
   default:
      errtext="Received unknown error code";break;
   }

   ShowMessage(hwnd, errtext, MB_OK | MB_ICONERROR);
}

// Show a messagebox
int ShowMessage(HWND hwnd, const char * pMessage, UINT flags)
{
   return MessageBox(hwnd, pMessage, "Binflash", flags);
}
