#include <string.h>
#include "plscsi/plscsi.h"
#include "types.h"
#include "necinternal.h"

int main(int argc,char* argv[])
{
   char ParsedDevice[256];
   Scsi* scsi;

   if (argc!=3)
   {
      printf("Call:\n%s {Drive} {ID}\n",argv[0]);
      return 1;
   }

   scsi=newScsi();
   if (!scsi)
   {
      printf("Can't create SCSI handle\n");
      return 1;
   }

   if ((argv[2][0]<'0') || (argv[2][0]>'9') || (argv[2][1]<'0') || (argv[2][1]>'9') || argv[2][2])
   {
      printf("Invalid ID\n");
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

   u8 buffer[16];
   buffer[0]='N';
   buffer[1]='D';
   buffer[2]=argv[2][0];
   buffer[3]=argv[2][1];

   printf("Trying to change ID\n");
   if (!WriteCalib(scsi, buffer, 0x5f10, 4, false))
   {
      printf("Could not change ID\n");
      return 1;
   }

   printf("ID changed successfully\n");
   return 0;
}
