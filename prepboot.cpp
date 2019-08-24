#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <time.h>
#include <windows.h>
#include "types.h"
#include "utils.h"


int main(int argc, char* argv[])
{
   FILE *in;
   FILE *out;
   size_t BytesRead;
   int calc=0;

   BootflashAppend append;
   u8* AppPoint;

   AppPoint=(u8*) &append;

   if (argc!=3)
   {
      printf("Call with\n%s {Original Bootfile} {Bootflash Append File}\n",argv[0]);
	  printf("Append resulting file to Bootflash executable to create actual flasher\n");
      return 1;
   }

   in=fopen(argv[1],"rb");
   if (!in)
   {
      printf("Unable to open input file\n");
      return 2;
   }
   
   BytesRead=fread(append.Firmware,1,0x8000,in);
   fclose(in);
   if ((BytesRead!=0x4000) && (BytesRead!=0x8000))
   {
      printf("Illegal input file\n");
      return 3;
   }
      
   out=fopen(argv[2],"wb");
   if (!out)
   {
      printf("Unable to open output file\n");
      return 4;
   }

   printf("Enter target drive ID (e.g. K450): ");
   fflush(stdin);
   for (int i=0;i<4;i++)
      append.CalibData.ID[i]=getchar();

   printf("Enter target drive (e.g. 4551): ");
   fflush(stdin);
   for (int i=0;i<4;i++)
      append.CalibData.Drive[i]=getchar();

   printf("Enter target drive RID (e.g. ND22): ");
   fflush(stdin);
   for (int i=0;i<4;i++)
      append.CalibData.RID[i]=getchar();

   for (int i=offsetof(BootflashAppend,Firmware);i<offsetof(BootflashAppend,checksum);i+=4)
   {
      calc+=AppPoint[i]<<24;
      calc+=AppPoint[i+1]<<16;
      calc+=AppPoint[i+2]<<8;
      calc+=AppPoint[i+3];
   }
   append.checksum=calc;

   fwrite(AppPoint,1,sizeof(BootflashAppend),out);

   fclose(in);
   fclose(out);
}
