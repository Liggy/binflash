#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include "plscsi/plscsi.h"
#include "types.h"
#include "necinternal.h"


int main(int argc,char* argv[])
{
   Scsi* scsi;
   char ParsedDevice[256];
   char bootkey[8];
   int dumpid;
   u8 buffer[0x10];
   CONSOLE_SCREEN_BUFFER_INFO consInfo;
   HANDLE console;

   if (argc!=2)
   {
      printf("Call:\n%s {Drive}\n",argv[0]);
      return 1;
   }

   scsi=newScsi();
   if (!scsi)
   {
      printf("Can't create SCSI handle\n");
      return 1;
   }

   if (argv[1][1]==':' && !argv[1][2])
   {
      *argv[1]&=0xdf;
      sprintf(ParsedDevice,"\\\\.\\%c:",*argv[1]);
   }
   else
      strncpy(ParsedDevice,argv[1],255);

   if (scsiOpen(scsi,ParsedDevice))
   {
      printf("Can't open SCSI device %s\n",ParsedDevice);
      return 1;
   }

   scsiLimitSense(scsi,0xe);
   scsiLimitSeconds(scsi,300,0);

   console=GetStdHandle(STD_OUTPUT_HANDLE);
   GetConsoleScreenBufferInfo(console,&consInfo);
   
   bool found=false;
   for (char letter='A';(letter<='Z') && !found;letter++)
   {
      for (int digit=0;(digit<1000) && !found;digit++)
      {
         sprintf(bootkey,"%c%03d",letter,digit);
         SetConsoleCursorPosition(console,consInfo.dwCursorPosition);
         printf("Trying: %s\n",bootkey);
         if (SetSafeMode(scsi,bootkey,0,1))
            found=true;
      }
   }

   SetConsoleCursorPosition(console,consInfo.dwCursorPosition);
   if (!found)
      printf("Unable to find bootkey\n");
   else
   {
      printf("Found bootkey \"%s\"\n",bootkey);

      GetConsoleScreenBufferInfo(console,&consInfo);
      found=false;
      for (dumpid=0;dumpid<0x100 && !found;)
      {
         SetConsoleCursorPosition(console,consInfo.dwCursorPosition);
         printf("Trying: %02X\n",dumpid);
         if (ReadRAM(scsi,buffer,0x10000,0x10,0,dumpid))
            found=true;
         else
            dumpid++;
      }
      SetConsoleCursorPosition(console,consInfo.dwCursorPosition);
      if (!found)
         printf("Unable to find dumpid\n");
      else
      {
         printf("Found dumpid \"%02X\"\n",dumpid);
      }

      SetSafeMode(scsi,bootkey,0,0);
   }

   return 0;
}