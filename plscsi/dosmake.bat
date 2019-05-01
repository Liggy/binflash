if "%1"=="" goto :default
goto :%1


:dosmake
PATH=C:\TC\BIN;%PATH%
path
tcc -eplscsi -mh -N -w *.c*
tcc -edosaspi -mh -N -w -DSCSI_STUB dosaspi.cpp scsistub.cpp
goto :exit


:clean

del plscsi.exe
del dosaspi.exe

del cl.obj
del dosaspi.obj
del plscsi.obj
del scl.obj
del scsi.obj
del scsistub.obj
del sgio.obj
del sptx.obj
del wideint.obj
del winaspi.obj

goto :exit


:default
command /c %0 dosmake
goto :exit


:exit
