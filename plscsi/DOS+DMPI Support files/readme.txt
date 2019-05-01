o CWSDPMI.EXE
  If you compile for DOS with DJGPP, you will first need cwsdpmi.exe in
  the same directory where you run your executable.
  The default cwsdpmi tries to use your C: drive for temporary, which is
  rather nasty and has reportedly caused some corruption on NTFS systems.
  Moreover, this temp space is seldom needed. The cwsdpmi.exe provided 
  here has the temp space disabled.
o ASPI.SYS
  Also, you will need a DOS ASPI driver to access CD-ROM units. This 
  driver is provided as well (OAK CD-ROM driver).
o addDev/delDev
  Finally, rather than go through the cumbersome process of loading this
  driver through config.sys, the addDev utility is provided (with source)
  You can load the aspi.sys driver (or any other driver) at any time by
  using the command: 
    addDev ASPI.SYS
  The delDev counterpart is also provided.
