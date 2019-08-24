#include <string.h>
#include <stdlib.h>
#include "plscsi/plscsi.h"
#include "types.h"
#include "necinternal.h"

#define bank0 1
#define bank1 0

#define ignoreerror 1

int main(int argc,char* argv[])
{
   char ParsedDevice[256];
   Scsi* scsi;

   u8 DumpID=0;

   if (argc<3 || argc>4)
   {
      printf("Call:\n%s {Filename} {Drive} [Bootkey]\n",argv[0]);
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
   
   u8 buffer[4096];
   u32 address;
   u32 start;
   u32 end;

#if 1

// Search for DumpID
   for (u16 FoundID=0;FoundID<256;FoundID++)
   {
      if (ReadRAM(scsi,buffer,0,4,0,DumpID))
      {
         DumpID=(u8) FoundID;
         printf("Found DumpID: %02X\n",(int) DumpID);
         break;
      }
   }

   for (address=0x0;address<0x200000;address+=0x1000)
   {
      if (!ReadRAM(scsi,buffer,address,0x1000,0,DumpID))
      {
         for (int i=0;i<0x1000;i+=4)
         {
            buffer[i]=0;
            buffer[i+1]=0xff;
            buffer[i+2]=0x55;
            buffer[i+3]=0xaa;
         }
      }
      fwrite(buffer,0x1000,1,firmfile);
   }

   u8 Bootcode[0x8000];
   if (!ReadBoot(scsi,Bootcode,0,0x8000))
   {
      for (int i=0;i<0x8000;i+=4)
      {
         Bootcode[i]=0;
         Bootcode[i+1]=0xff;
         Bootcode[i+2]=0x55;
         Bootcode[i+3]=0xaa;
      }
   }
   fwrite(Bootcode,0x8000,1,firmfile);



#endif

#if 0
   if (argc==4)
   {
      if (!stricmp(argv[3],"auto"))
      {
         u8 IDbuff[4];

         if (!ReadRAM(scsi, (u8*) IDbuff, 0x4800, 4, false))
            argc=3;
         else
            memcpy(argv[3],IDbuff,4);
      }

      if ((argc==4) && (!SetSafeMode(scsi,argv[3],0,1)))
      {
         printf("Error when entering safe mode bank 0\n");
         return 1;
      }
   }
#if bank0

   start=0x0;
   end=0x200000;

// Search for DumpID
   for (u16 FoundID=0;FoundID<256;FoundID++)
   {
      DumpID=(u8) FoundID;
      if (ReadRAM(scsi,buffer,0,4,0,DumpID))
         break;
   }

//   if (DumpID)
//      printf("Dumpid: 0x%02X\n", (int) DumpID);


   for (address=start;address<end;address+=0x1000)
   {
      printf("Reading FLASH at 0x%06lx in bank 0\n",address);

         
      if (!ReadRAM(scsi,buffer,address,0x1000,0,DumpID))
      {
#if ignoreerror
         for (int i=0;i<0x1000;i+=4)
         {
            buffer[i]=0;
            buffer[i+1]=0xff;
            buffer[i+2]=0x55;
            buffer[i+3]=0xaa;
         }
#else
         printf("Error reading RAM\n");
         if (argc==4)
            SetSafeMode(scsi,argv[3],0,0);
         return 1;
#endif // ignoreerror
      }
      if (fwrite(buffer,4096,1,firmfile)!=1)
      {
         printf("Error writing firmware to file\n");
         if (argc==4)
            SetSafeMode(scsi,argv[3],0,0);
         return 1;
      }
   }
   if (argc==4)
      SetSafeMode(scsi,argv[3],0,0);
#endif // bank0

#if bank1
   if (argc==4)
   {
      if (!SetSafeMode(scsi,argv[3],1,1))
      {
         printf("Error when entering safe mode bank 1\n");
         return 1;
      }
   }

   for (address=0;address<0x100000;address+=0x1000)
   {
      printf("Reading FLASH at 0x%06lx in bank 1\n",address);

      if (!ReadRAM(scsi,buffer,address,0x1000,1))
      {
#if ignoreerror
         for (int i=0;i<0x1000;i+=4)
         {
            buffer[i]=0;
            buffer[i+1]=0xff;
            buffer[i+2]=0x55;
            buffer[i+3]=0xaa;
         }
#else
         printf("Error reading RAM\n");
         if (argc==4)
            SetSafeMode(scsi,argv[3],1,0);
         return 1;
#endif
      }
      if (fwrite(buffer,4096,1,firmfile)!=1)
      {
         printf("Error writing firmware to file\n");
         if (argc==4)
            SetSafeMode(scsi,argv[3],1,0);
         return 1;
      }
   }

   if (argc==4)
      SetSafeMode(scsi,argv[3],1,0);
#endif // bank1

#endif
   fclose(firmfile);

   return 0;
}
