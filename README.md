# Binflash

* [About Binflash](#About-Binflash)
* [Documentation](#Documentation)
* [Download](/liggy/Binflash/releases)
* [Supported Drives](#Drivelist)
* [Build](#Build)
* [Version History](#Version-History)

# About Binflash
Binflash is a collection of tools you may use in order to flash your DVD burner with a binary firmware file (most executable flashers are also supported) or to dump the Flash ROM of your burner back to disc. You should be able to work with all devices that are currently accessible in your operating system, even if the drives are attached via Firewire or USB. There have been reports about some external enclosures that do not support sending all commands needed for a successful flash to the drive. Using the flaher in this case can lead to unpredictable results. Furthermore some Windows IDE drivers and some Linux kernel versions are known to cause trouble. So please use the original Microsoft IDE drivers or start the flasher with root user when running from Linux

This flasher is in no way supported by the manufacturers and we cannot guarantee that this program works as expected.

# Documentation
Depending on the manufacturer dependant version of Binflash, not all options may be available for you.

| Option | Function |
| - | - |
| -scan |Scan your IDE-/SCSI/Firewire/USB-adapters for supported devices. Depending on the manufacturer your version of Binflash was written for, some drives may be listed which are not supported. |
| -dump |This option lets you dump the current firmware back to disc if supported. You need to specify a filename that will be used for dumping the firmware and the name of the device you want to use. |
| -flash |If you want to flash your drive with a new firmware, you need to specify this option. Furthermore the name of your firmware file and the device are needed in order to flash your drive. |
| -querybt |Query current booktype settings. The program will display the current disctype, the booktype of the disc and the drive's current booktype settings |
| -setbt |This command lets you change the booktype settings of your drive. You can change the default booktype for DVD+R, DVD+R-DL and DVD+RW to a different setting. Depending on your firmware version not all kind of discs can be changed. Neither do older firmwares with bitsetting based on Ricoh commands support temporary changes |
| -setbtrw |You can change the booktype of a currently inserted DVD+RW to any other value. This function is not supported by newer firmwares |
| -clearbt | In newer firmwares the number of supported booktype changes is limited to 500. If you want to reset this value because you have made 500 changes already, you should try using this option. By calling -clearbt not only the counter is reset, but all booktype settings are restored to default as well. |
| -version | Displays some version information. This is important if something is not working the way it should. Support can only be given if the version numbers are supplied |
| -identify |Used for retrieving data about unknown drives that is required for adding support to Binflash |
| -queryrpc |Displays the current DVD region settings |
| -resetrpc |This command is used to reset the DVD region counter. You can either issue a vendor reset or you can completely reset the region counter, giving 4 vendor and 5 user changes. It is recommended to use auto-reset or RPC1 firmwares instead. |

## Example on using Binflash

First of all scan your adapters for valid devices:

_C:\>**necflash -scan**_

This may lead to an output similar to:

    Binflash - NEC version - (C) by Liggy and Herrie
    Visit http://binflash.cdfreaks.com

    List of supported devices:
    Device   : 1.0.0
    Vendor   : _NEC
    Model    : DVD_RW ND-2500A
    Firmware : 1.07

Now you should backup your current firmware by issueing the following command, using the device that you got during the device scan:

_C:\>**necflash -dump C:\OldFirmware.bin 1.0.0**_

After accepting the disclaimer and waiting for a few seconds you will get the file C:\OldFirmware.bin which contains your current firmware.


Now you probably want to update your drive with a new firmware. You can do so simply by calling the following command:

_C:\>**necflash -flash C:\NewFirmware.bin 1.0.0**_

If you accept the disclaimer and wait for a few seconds again, the burner is updated with the firmware from C:\NewFirmware.bin


The following command is used to set the default booktype for DVD+R to DVD-ROM:

_C:\>**necflash -setbt DVD+R DVD-ROM perm 1.0.0**_

For **DVD+R**, **DVD+RW** and **DVD+R-DL** you can set the booktype to **DVD-ROM**, **DVD-R**, **DVD-RW**, **DVD+R**, **DVD+RW** and **DVD+R-DL**. Instead of using **perm** you may also use **temp** if you just want to use the temporary settings that only last until the next drive reset. (This feature is not supported by every firmware)

# Drivelist
Drives that are supposed to work

| NEC | Optiarc | Others |
| - | - | - |
| ND-1100A | AD-5170A | Sony DW-D150A |
| ND-1300A | AD-5170S | Sony AW-Q170A |
| ND-2100A | AD-5173A | Sony AW-Q170S |
| ND-2500A | AD-5200S | Sony AW-G170A |
| ND-2510A | AD-5200A | Sony AW-G170S |
| ND-3100A | AD-5230S | |
| ND-3450A | AD-5240S | LiteOn / PLDS iHAS124Y |
| ND-3500A | AD-5260S | LiteOn / PLDS iHAS224Y |
| ND-3520A | AD-5280S | LiteOn / PLDS iHAS324Y |
| ND-3530A | AD-5540A | LiteOn / PLDS iHAS424Y |
| ND-3540A | AD-5543A | LiteOn / PLDS DH24AYS |
| ND-3550A | AD-5630A | LiteOn / PLDS DH24AYL |
| ND-3551A | AD-5633A | LiteOn / PLDS iHAS124W |
| ND-3570A | AD-5670S | |
| ND-3571A | AD-5680H | TDK DVDRW440N |
| ND-3650A | AD-5960S | TDK DVDRW880N |
| ND-3651A | AD-5970H | TDK DVDRW882N |
| ND-4550A | AD-7170A | TDK DVDRW1616N |
| ND-4551A | AD-7170S | |
| ND-4570A | AD-7173A | HP DVD Writer 420n  |
| ND-4571A | AD-7173S | HP DVD Writer 520n  |
| ND-4650A | AD-7200A | HP DVD Writer 1270r |
| ND-4651A | AD-7200S | |
| ND-5100A | AD-7201A | IOData DVR-ABN16W |
| ND-5500A | AD-7201S | IOData DVR-ABN16A |
| ND-6100A | AD-7203A | |
| ND-6450A | AD-7203S | AOpen DVD Dual AAG |
| ND-6500A | AD-7230S | |
| ND-6650A | AD-7231S | Ricoh MP5308D |
| ND-6750A | AD-7233S | |
| ND-7550A | AD-7240S | MAD DOG MD-8XDVD9 |
| ND-7551A | AD-7241S | MAD DOG MD-16XDVD9 |
| | AD-7243S | MAD DOG MD-16XDVD9A2 |
| | AD-7260S | MAD DOG MD-16XDVD9A4 |
| | AD-7261S | |
| | AD-7263S | Plextor PX-800A |
| | AD-7280S | Plextor PX-820SA |
| | AD-7283S | |
| | AD-7540A | |
| | AD-7543A | |
| | AD-7590A | |
| | AD-7590S | |
| | AD-7591A | |
| | AD-7591S | |
| | AD-7593A | |
| | AD-7593S | |
| | AD-7630A | |
| | AD-7633A | |
| | AD-7670S | |
| | AD-7700S | |
| | AD-7703S | |
| | AD-7710H | |
| | AD-7740H | |
| | AD-7910A | |
| | AD-7910S | |
| | AD-7913A | |
| | AD-7913S | |
| | AD-7960S | |

# Build
## Prerequisites
Last successful Windows build was done using Visual Studio 2010. More recent versions may require changes to the code or the project files.

Despite throwing some warning, Binflash on Linux still seems to compile with GCC version 8.3 successfully.

The DOS version requires [DJGPP](http://www.delorie.com/djgpp/) setup on your machine

The Mac version was built with XCode 3.2.4 on MacOS 10.6.8 (Snow Leopard). Using recent versions will probably require some work.

## Instructions
On Windows you just open the solution file (binflash.sln) with Visual Studio and build the release you like to have. Choosing target "Oldrelease" should build a version that is compatible with Windows 2000.

The Linux, DOS and Mac versions are generated with "make linux", "make dos" or "make mac" and generate a necflash(.exe) file

# Version history
| version | Change |
| - | - |
| v1.63 | Use different flashing method for AD-7280S series (same as original Optiarc flasher) |
| | Add support for LiteOn iHAS124 W drives|Flashing functionality still untested |
| v1.62 | Add support for AD-7740H drives |
| v1.61 | Fix flashing of some AD-7283S drives |
| | Add support for AD-7250H and AD-7270H drives |
| v1.60 | Add support for newer AD-7280S drives (identify like AD-7300S) |
| v1.59 | Add support for AD-7710H drives |
| v1.58 | Fix flashing AD-7280S drives |
| | Add support for AD-5690H and AD-7690H drives |
| | Scan option now also shows unknown drives on command line |
| v1.57 | Add support for AD-7280S drives |
| | Add BC-5640H drives (no real functionality as of yet) |
| | Improve region code reset routine |
| v1.56 | Add support for AD-5970H drives |
| | Add support for querying and resetting region code data |
| v1.55 | Fix corrupted firmware dumps on older NEC drives |
| v1.54 | Add support for AD-7591A and AD-7591S drives |
| v1.53 | Add support for HP DVD-Writer 1270r |
| v1.52 | Add basic support for BluRay combo drives. Only flashing within the same firmware series is supported. No dumping possible for now |
| | Add support for AD-7700H drives |
| v1.51 | Add support for AD-5680H and PLDS DH24AYS drives |
| | Windows versions now compiled with Visual Studio 2010\. Added progress bar to taskbar icon on Windows 7 |
| | Experimental support for BluRay drives. Mostly disabled for now. |
| | Slightly extended identification for unknown drives |
| v1.50b | Add support for AD-5230S, AD-5260S, AD-7230S, AD-7260S, AD-7231S, AD-7261S, AD-7233S and AD-7263S |
| | Rewrite of Booktype query for command line versions |
| | Mac version compiled on Mac OS X 10.6 with XCode 3.2.1 now|might not work on 10.3 anymore. |
| v1.49b | Fix support for LiteOn OEM drives |
| v1.49 | Add support for AD-7700S, AD-7703S and LiteOn OEM drives (iHAS124Y, iHAS224Y, iHAS324Y and iHAS424Y) |
| v1.48 | Eliminate wait time after flashing or dumping (Windows GUI only) |
| | Add support for AD-5670S, AD-7670S, AD-5960S and AD-7960S series (PATA should work too if available) |
| v1.47c | Fix "Unknown Error" message after flashing (Windows GUI only) |
| v1.47b | Reduce CPU usage while dumping and flashing in GUI versions |
| v1.47a | Fix user confirmation on command line versions (mainly Linux, Mac and DOS) |
| v1.47 | Add support for AD-7240S. Should also include AD-5240S, AD-7241S and AD-7243S |
| | Windows GUI starts flashing routine in new thread. GUI should be more responsive now |
| v1.46b | Fix dumping of ND-3540A (and possibly a few more) drives |
| v1.46a | Fix flashing of firmwares that do not supply bootcode version |
| v1.46 | Support files containing firmwares for different bootcodes |
| | Always show firmware version before flashing. Not only in GUI |
| | Add support for slimline SATA drives |
| v1.45a | Fix detection of firmwares for AD-5170A duplicator drives. |
| | Fix identification output if some data cannot be retrieved |
| v1.45 | Support for AD-5170A duplicator drives (untested), support for AD-7910A and AD-7913A drives |
| | Mac version compiled on Mac OS X 10.5 with XCode 3.0 now|might not work on 10.2 anymore. |
| v1.44 | Support for (again some drives might not exist at all) AD-5201A, AD-5201S, AD-7201A, AD-7201S, AD-5590A, AD-5593A, AD-7590A, AD-7593A |
| | Improve identification routine for unknown drives |
| v1.43 | Support for (not all drives will probably exist) AD-5200A, AD-5200S, AD-5203A, AD-5203S, AD-7200A, AD-7200S, AD-7203A, AD-7203S |
| v1.42 | Support for Optiarc AD-5630A and (if they exist) AD-5633A drives |
| v1.41 | Check for correct bootcode if provided by firmware and add bootcode version to firmware dump |
| | Currently allows to continue even if drive bootcode does not match. This behaviour may change in later versions |
| v1.40 | Add support for Optiarc AD-5170S drives |
| v1.39 | Preliminary support for dumping NEC HR-1100A firmwares |
| | Support for Optiarc AD-7630A (and hopefully AD-7633A) drives |
| | Support for Actima ADRWH00PM (NEC ND-1100A clone) drives |
| v1.38 | Add support for Optiarc AD-7173S drives |
| | Used a different method to create a Universal Binary for Mac (untested) |
| v1.37 | Improved flashing of notebook drives on some hardwares |
| | Changed detection of NEC ND-3650A and ND-4650A drives |
| v1.36 | Add support for AD-7170S (SATA) drives. May not work on some SATA controllers! |
| v1.35 | Fix dumping of 7540 and 7543 firmwares |
| | Add flashing of 7540 and 7543 master firmwares |
| | Fixed support for some special 3550 drives (1.G*|untested) |
| | Mac executable should be a universal binary now (untested) |
| v1.34 | Add flashing of 5540 master firmwares |
| | Addd dumping of 5540, 5543, 7540 and 7543 firmwares (mostly untested) |
| | Added routines to create an identification file for unsupported drives |
| | Support booktype routines on unsupported (new) drives |
| | Fix check if drive is ready |
| | Default GUI to extended view |
| v1.33 | Fix broken firmware dumping on 3570, 3571, 4570 and 4571 drives |
| v1.32 | Add support for Optiarc 5170, 5173, 7170 and 7173 drives |
| | Add support for some special 3550 drives with D355 identification |
| | Some internal code changes |
| v1.31 | Big code cleanup. Removed lots of redundant code |
| | Remove unnecessary operation when dumping firmware on several drives |
| | Make sure that there's no disc inside the drive when flashing |
| v1.30 | Fixed support for 3650 and 3651 drives |
| | Optimized check for correct firmwares on slimline drives after changing bootcode |
| v1.29 | Added experimental (untested) support for 3570, 3571, 3650, 3651, 4650 and 4651 drives |
| v1.28 | Fixed master/slave recognition on 6650 drives |
| v1.27 | Improved detection of compatible notebook drives |
| | Fixed broken skin support |
| v1.26 | Fixed support for 7551 drives |
| | Added support for 4570, 4571 and 6750 drives |
| | New detection of incompatible notebook firmwares (master / slave) |
| v1.25 | Add 64 bit Windows executables (untested so far) |
| | Switched to new Compiler for Windows (VC++ 8) |
| v1.24 | Added support for NEC ND3551 drives |
| v1.23 | Fixed flashing retail NEC ND4551A drives |
| | Added support for NEC ND7550A and ND7551A drives |
| | Internal code reorganization |
| v1.22 | Fixed recognition of NEC ND3550A firmwares |
| v1.21a | Do not allow mixing up master and slave firmwares on notebook drives due to too many stupid people who ignore every warning |
| v1.21 | Add support for NEC ND3550A and ND4551A drives |
| v1.20 | Add support for NEC ND4550A drives |
| | Modularized flashing routines. |
| | Several internal fixes, additions and code cleanup |
| | Prepare support for bitsetting DVD+RW-DL |
| v1.19 | Fix NEC ND1000A detection. This version was never released |
| v1.18 | Disable safe mode if flashing failed |
| | Try to prevent mixing up master and slave firmwares on notebook drives |
| | Minor changes |
| v1.17 | Added support for ND6650A drives (Untested) |
| v1.16b | Fixed a bug in device scan on Mac (fPLSCSI bug) |
| v1.16 | Added support for ND3540A drives (Untested) |
| v1.15 | Fixed a bug when flashing 2XXX and 3XXX executable firmwares with integrated Binflash |
| v1.14 | Added support for ND3530A drives (Untested) |
| | Added support for flashing ND5x00 and ND6x00 drives with slave firmwares. |
| | Do NOT try to switch from master to slave or vice versa by just flashing a different firmware! |
| v1.13 | Modified checks for successful command execution (Thanks Cartman) |
| | Fixed buffer size when querying booktype (NEC style) |
| | Improved booktype dialog in GUI version |
| v1.12 | Fixed bitsetting for ND3520A drives |
| | Changed open command on Linux from O_RDONLY to O_RDWR |
| v1.11 | Added support for ND6450A drives |
| | Fixed a severe bug when flashing ND1x00 and ND5x00 drives |
| v1.10 | Added support for ND3520A drives. |
| v1.03a | Removed dot check in firmware version. Should fix issues with ND3450A drives |
| v1.03 | Added support for ND3450A drives |
| | Cosmetic changes |
| v1.02 | Fixed a severe bug when flashing ND6500A drives |
| v1.01 | Fixed a bug when reading ND6500A firmwares |
| v1.00 | Add support for the ND6500A |
| | Add forgotten scsiClose calls |
| | Minor bugfixes |
| v0.95 | Program renamed to Binflash |
| | Add Bitsetting support |
| | Flash NEC executable firmwares |
| | Several major and minor fixes |
| | Enable flashing drives that are in safe mode |
| v0.94b | Bugfix for reading firmware |
| v0.94 | Enable flashing from buffer for Win32 GUI version |
| v0.93 | Fix for devicename on Mac |
| | Added callback routine for device scan |
| | Modifications for debug build |
| v0.92 | Interface rewrite |
| | Improved error handling |
| v0.91 | Fix too small buffer for scanning |
| v0.90 | First public release |
