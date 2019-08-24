#include <string.h>
#include <stdlib.h>
#include "plscsi/plscsi.h"
#include "types.h"
#include "necinternal.h"


static const char *typestring[]={"CD-R","CD-RW","DVD+R","DVD+R9","DVD+RW","DVD-R","DVD-RW","DVD-R9","DVD-RAM","DVD+RW9"};


bool GetDates(Scsi* scsi,u8* date1, u8* date2)
{
   u8 cdb[16];
   u8 buffer[0x100];

   memset(buffer,0,0x100);
   memset(cdb,0x00,CDB_SIZE);
   cdb[0]=0xcf;cdb[9]=0xa0;

   if (scsiSay(scsi,(char*) cdb,12,(char*) buffer,0x100,X1_DATA_IN)<0)
      return false;
   memcpy(date1,buffer+2,0xf0);

   memset(buffer,0,0x100);
   memset(cdb,0x00,CDB_SIZE);
   cdb[0]=0x12;cdb[4]=0x80;

   if (scsiSay(scsi,(char*) cdb,6,(char*) buffer,0x80,X1_DATA_IN)<0)
      return false;
   memcpy(date2,buffer+0x24,0x40);


   return true;
}


int main(int argc,char* argv[])
{
   Scsi* scsi;
   char ParsedDevice[256];
   u8 buffer[1024];
   u8 date1[0xf0], date2[0x40];

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

   if (!GetDates(scsi,date1,date2))
      return 1;

   printf("Date 1: %s\n", date1);
   printf("Date 2: %s\n", date2);

   if (!GetStrategyVersions(scsi, buffer, 1024))
      return 2;
   for(int i=0;(i<256) && (buffer[i]);i+=8)
   {
      printf("%s: ",typestring[i>>3]);
      for (int j=0;j<8;j++)
         if (buffer[i+j]!='_')
            putchar(buffer[i+j]);

      putchar('\n');
   }

   return 0;
}