#include <stdio.h>
#include <string.h>

static int count = 1;
static int innercount = 1;
long filesize;

void howto()
{
	printf("Usage: chksum {Firmware} [set]\n");
}

void crc(FILE *binfile, int size, bool set = false, int inner = 0)
{
	unsigned int chksum = 0;
	unsigned int chksum2 = 0;
	unsigned int a, b;
	long pos;
	int i;

	for (i = 0; i < size; i += 2)
	{
		a = fgetc(binfile);
		b = fgetc(binfile);

		if (inner)
		{
			if (i < (size - 0x12))
			{
				if ((inner == 2) && ((i & 0xff0000) == 0x120000))
				{
					// Inner=2 -> Check 0x150ff0-0x150fff instead of 0x15fff0-0x15ffff (0x120000 in file - 0x150000 in firmware)
					if ((((i & 0xffff) >= 0x0ff0) && ((i & 0xffff) < 0x1000)) || ((i & 0xffff) < 0x10))
					{
						//                  printf("Adding 0x%x: %02x%02x\n",i+0x30000,a,b);
						chksum2 += ((a << 8) + b);
					}
				}
				else if (((i & 0xffff) >= 0xfff0) || ((i & 0xffff) < 0x10))
				{
					//               printf("Adding 0x%x: %02x%02x\n",i+0x30000,a,b);
					chksum2 += ((a << 8) + b);
				}
			}
			else if (i < (size - 2))
			{
				chksum2 += ((a << 8) + b);
				//            printf("Adding 0x%x: %02x%02x\n",i+0x30000,a,b);
			}
			else
			{
				pos = ftell(binfile);

				printf("Inner Chksum %d:\n", innercount++);

				printf("Found   : %02x - %02x\n", a, b);

				chksum2 &= 0xffff;
				a = (chksum2 >> 8);
				b = (chksum2 & 0xff);

				printf("Expected: %02x - %02x\n", a, b);
				if (set)
				{
					fseek(binfile, pos - 2, SEEK_SET);
					fputc(a, binfile);
					fputc(b, binfile);
					printf("New inner chechksum set\n");
					fseek(binfile, pos, SEEK_SET); // Further fgetc would give 0xff without this
				}
			}
		}

		chksum += ((a << 8) + b);
	}
	pos = ftell(binfile);
	a = fgetc(binfile);
	b = fgetc(binfile);

	printf("Chksum %d:\n", count++);

	printf("Found   : %02x - %02x\n", a, b);

	chksum &= 0xffff;
	a = (chksum >> 8);
	b = (chksum & 0xff);

	printf("Expected: %02x - %02x\n", a, b);
	if (set)
	{
		fseek(binfile, pos, SEEK_SET);
		fputc(a, binfile);
		fputc(b, binfile);
		printf("New chechksum set\n");
		fseek(binfile, pos + 2, SEEK_SET);
	}
}

int GetDriveType(FILE *binfile)
{
	char type[4];

	fseek(binfile, 0, SEEK_END);
	filesize = ftell(binfile);
	fseek(binfile, 0, SEEK_SET);

	if (filesize == 634888)
		return 0;
	if (filesize == 753668)
		return 1;
	if (filesize == 1024004 || filesize == 1024006)
	{
		fseek(binfile, 1024000, SEEK_SET);
		fread(type, 1, 4, binfile);
		fseek(binfile, 0, SEEK_SET);

		if (!memcmp(type, "K210", 4))
			return 2;
		if (!memcmp(type, "K250", 4))
			return 2;
		if (!memcmp(type, "K251", 4))
			return 2;

		return 3;
	}
	if ((filesize == 1966086) || (filesize == 1966088))
		return 4;
	if (filesize == 1835016)
		return 5;
	if (filesize == 1835014)
	{
		fseek(binfile, 1835008, SEEK_SET);
		fread(type, 1, 4, binfile);
		fseek(binfile, 0, SEEK_SET);

		if (!memcmp(type, "M110", 4))
			return 6;

		return 5;
	}
	if (filesize == 1835012)
		return 6;
	if (!(filesize & 0xffff) && (filesize > 0x10000))
		return 7;
	if (filesize == 1769480)
	{
		fseek(binfile, 1769474, SEEK_SET);
		fread(type, 1, 4, binfile);
		fseek(binfile, 0, SEEK_SET);

		if (!memcmp(type, "Q32", 3))
			return 11;
		if (!memcmp(type, "G32", 3))
			return 11;

		return 8;
	}
	if (filesize == 1769478)
		return 8;
	if (filesize == 3670032)
		return 9;
	if (filesize == 3538960)
		return 10;
	if (filesize == 0x4000)
		return -2;
	if (filesize == 0x8000)
		return -3;
	if (filesize == 0x1000)
		return -4;

	return -1;
}

int main(int argc, char *argv[])
{
	FILE *binfile;
	int drivetype;
	bool set = false, full = false;

	if ((argc < 2) || (argc > 3))
	{
		howto();
		return 1;
	}

	if (argc == 3)
	{
		if (!stricmp(argv[2], "set"))
			set = true;
		else if (!stricmp(argv[2], "full"))
			full = true;
		else
		{
			howto();
			return 1;
		}
	}

	binfile = fopen(argv[1], "r+b");
	if (!binfile)
	{
		printf("Error opening file %s\n", argv[1]);
		return 2;
	}

	drivetype = GetDriveType(binfile);
	//   printf("Drivetype: %d\n",drivetype);
	if (!full)
	{
		switch (drivetype)
		{
		case -1:
			printf("Cannot determine drivetype\n");
			return 3;

		case -2:
			crc(binfile, 0x3ffe, set, 0);
			break;

		case -3:
			crc(binfile, 0x7ffe, set, 0);
			break;

		case -4:
			crc(binfile, 0x0ffe, set, 0);
			break;

		case 0:
			crc(binfile, 0x7affe, set, 0);
			fseek(binfile, 4, SEEK_CUR);
			crc(binfile, 0x1fffe, set, 0);
			break;

		case 1:
			crc(binfile, 0xb7ffe, set, 0);
			break;

		case 2:
			crc(binfile, 0x29ffe, set, 0);
			crc(binfile, 0xcfffe, set, 0);
			break;

		case 3:
			crc(binfile, 0xf9ffe, set, 0);
			break;

		case 4:
			crc(binfile, 0x1e0000, set, 1);
			break;

		case 5:
			crc(binfile, 0x1c0000, set, 2);
			break;

		case 6:
			crc(binfile, 0x180000, set, 0);
			break;

		case 7:
			fseek(binfile, 24576, SEEK_SET);
			crc(binfile, filesize - 24578, set, 0);
			break;

		case 8:
			crc(binfile, 0x1b0000, set, 2);
			break;

		case 9:
			crc(binfile, 0x1c0000, set, 2);
			fseek(binfile, 6, SEEK_CUR);
			crc(binfile, 0x1c0000, set, 2);
			break;

		case 10:
			crc(binfile, 0x1b0000, set, 2);
			fseek(binfile, 6, SEEK_CUR);
			crc(binfile, 0x1b0000, set, 2);
			break;

		case 11:
			crc(binfile, 0x1b0000, set, 1);
			break;
		}
	}
	else
		crc(binfile, filesize - 2, false, 0);

	fclose(binfile);

	return 0;
}
