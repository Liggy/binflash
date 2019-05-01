@echo off
set PATH=d:\djgpp\bin;%PATH%
set DJGPP=d:\djgpp\djgpp.env
gpp -o plscsi.exe -Wno-deprecated *.c*
pause
