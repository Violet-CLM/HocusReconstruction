cd RECONSTR
cd ASS
cycles max
call MOUNTDIR.BAT
make
cd ..\
::call MOUNTDIR.BAT
make
cd OBJ
copy HOCPOC.EXE ..\..\HOCUS.EXE
cd ..\..\
HOCUS