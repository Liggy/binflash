#include <string.h>
#include <stdlib.h>
#include "plscsi/plscsi.h"
#include "types.h"
#include "necinternal.h"

#define ignoreerror 1

#define bootcodesize 32768

int main(int argc,char* argv[])
{
   char ParsedDevice[256];
   Scsi* scsi;

   if (argc!=4)
   {
      printf("Call:\n%s {Filename} {Drive} {Safemodetext}\n",argv[0]);
      return 1;
   }

   scsi=newScsi();
   if (!scsi)
   {
      printf("Can't create SCSI handle\n");
      return 1;
   }

   if (argv[2][1]==':' && !argv[2][2])
   {
      *argv[2]&=0xdf;
      sprintf(ParsedDevice,"\\\\.\\%c:",*argv[2]);
   }
   else
      strncpy(ParsedDevice,argv[2],255);

   if (scsiOpen(scsi,ParsedDevice))
   {
      printf("Can't open SCSI device %s\n",ParsedDevice);
      return 1;
   }

   scsiLimitSense(scsi,0xe);
   scsiLimitSeconds(scsi,300,0);

   FILE *firmfile=fopen(argv[1],"wb");
   
   u8 buffer[bootcodesize];

   if (!SetSafeMode(scsi,argv[3],0,1))
   {
      printf("Error when entering safe mode bank 0\n");
      fclose(firmfile);
      return 1;
   }

   if (!ReadBoot(scsi,buffer,0,bootcodesize))
   {
#if ignoreerror
      for (int i=0;i<bootcodesize;i+=4)
      {
         buffer[i]=0;
         buffer[i+1]=0xff;
         buffer[i+2]=0x55;
         buffer[i+3]=0xaa;
      }
#else
      printf("Error reading RAM\n");
      SetSafeMode(scsi,argv[3],0,0,1);
      return 1;
#endif // ignoreerror
   }
   
   if (fwrite(buffer,bootcodesize,1,firmfile)!=1)
   {
      printf("Error writing firmware to file\n");
      SetSafeMode(scsi,argv[3],0,0);
      return 1;
   }
   SetSafeMode(scsi,argv[3],0,0);

   fclose(firmfile);

   return 0;
}
