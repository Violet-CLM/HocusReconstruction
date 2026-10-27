@echo off
call MOUNTDIR.BAT
cycles max
make
echo Press any key to exit.
pause > NUL
exit