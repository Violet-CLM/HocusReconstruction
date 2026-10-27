Reconstructed Hocus Pocus Source Code
Copyright (C) 2026 K1n9_Duk3
===============================================================================

This is an UNOFFICIAL reconstruction of the original Hocus Pocus source code.
Parts of the code are based on the Apogee Sound System.

Apogee Sound System by James R. Dose.
Copyright (C) 1994-1995 Apogee Software, Ltd.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.


Getting this code to compile
============================

You will need:

- Borland C++ 2.0 for the Apogee Sound System (ASS)
- Borland C++ 3.1 for the Hocus Pocus game & setup code
- DOSBox (or a real DOS computer)
- this source code archive

Other versions of Borland C++ (or Turbo C++) might also work, but will not
result in a 100% identical copy of the original HOCUS.EXE and SETUP.EXE files.

Setting up a working environment in DOSBox
------------------------------------------

The Borland C++ compiler and tools (including Turbo Assembler) are all DOS
programs, so you will need a system that is capable of running DOS programs or
you will have to use an emulator like DOSBox to run these programs.

If you are going to use DOSBox, start out by preparing a base directory on your
system that you are going to mount as drive C: in DOSBox (mounting your real C:
drive as C: in DOSBox is NOT recommended). Let's assume your base directory is
"C:\BASE". Extract the contents of this package into that directory.

You could just start DOSBox and manually mount your base directory as the C:
drive in DOSBox by typing "mount c c:\base" at the prompt, but this source code
comes with a couple of batch files that make the process much easier, as long
as things are set up correctly.

In case you didn't know, dragging and dropping a file onto DOSBox.exe (or onto
a shortcut to DOSBox.exe) will start DOSBox and mount the directory in which
that file is located in as the C: drive in DOSBox, then it will try to execute
that file in DOSBox.

Setting up the compiler paths for DOSBox
----------------------------------------

After extracting the files from this archive, you need to edit the MOUNTDIR.BAT
files and insert the correct path to your copy of Borland C++ in the lines that
contain the "mount" command. The MOUNTDIR.BAT in the main source directory
should use Borland C++ 3.1 and the one in the ASS directory should use Borland
C++ 2.0.

These batch files mount the compiler directory as drive X: in DOSBox to make
things easier. As long as your copy of Borland C++ was installed with the
default options for the subdirectories (BIN, INCLUDE, LIB), the name of the
main directory will not matter once it is mounted as drive X.

Compiling the code with DOSBox
------------------------------

After the mount directories have been set up, you need to compile the source
code for the Apogee Sound System first. Drag-and-drop the MAKELIB.BAT file from
the ASS directory onto a shortcut to DOSBox.

After the audio library has been created, you can compile the main game code.
If you want to compile the code from the IDE, drag-and-drop the _GAME.BAT file
or the _SETUP.BAT file onto your shortcut to DOSBox. Select "Compile" -> "Make"
from the menu or simply press F9 to compile the code. Press ALT+X to quit when
you're done. Do NOT simply close DOSBox while the IDE is still running.

If you want to compile the code from the command line, drag-and-drop the
MOUNTDIR.BAT file from the main source directory onto your shortcut to DOSBox
and then type "make all" at the command prompt.

Possible "targets" for MAKE are:
  game  -- compiles the game code and creates HOCPOC.EXE
  setup -- compiles the setup code and created HOCSET.EXE
  all   -- game and setup
  clean -- deletes all .OBJ files in the OBJ directory

If you just type "make", it will have the same effect as "make game".


Setting up the compiler paths and compiling on real DOS systems
---------------------------------------------------------------

If you are using a real DOS PC instead of using DOSBox, you cannot use the
"mount" command to assign a new drive letter to your Borland C++ directory, but
you can use "subst x:" instead of "mount x" in that case (assuming the
LASTDRIVE setting in your CONFIG.SYS is set to a letter from X to Z). You can
use the command "subst x: /d" later to get rid of drive X: again.

There might also be some problems due to how my batch files set up and modify
the PATH environment variable if you want to be able to use both Borland C++
2.0 and 3.1. I would recommend that you edit the MAKEFILE in the ASS directory
and change the COMPILERDIR line at the beginning into the correct path of your
Borland C++ 2.0 directory instead of running MOUNTDIRS.BAT or MAKELIB.BAT.
This allows you to run "make" from the ASS to compile the code with the correct
version of the compiler as long as a directory containing a compatible version
of MAKE.EXE has been added to your PATH environment variable.

Alternatively, you can type the full path to your MAKE.EXE to run it from the
ASS directory. For example, if you installed Borland C++ with the default
options, you would type "c:\borlandc\bin\make" (without the quotes) instead of
just "make".

You can do the same with the MAKEFILE in the main source directory. Just keep
in mind that you need to use your Borland C++ 3.1 directory for it if you want
to create authentic copies of the original executables.

However, if you want to use the IDE to compile the game and setup programs, you
MUST add the correct path to the BIN subdirectory of your Borland C++ 3.1
installation to your PATH environment variable, otherwise the IDE might be
unable to run TASM.EXE for the assembly files.

To start the IDE, navigate to your source directory and type "bc", then select
"Project" -> "Open Project..." from the menu and open either HOCPOC.PRJ or
HOCSET.PRJ. Alternatively, you can type "bc hocpoc.prj" or "bc hocset.prj" at
the DOS prompt to load the desired project right from the start. Once the
project is loaded, you can press F9 or select "Compile" -> "Make" from the menu
to compile the code.

If you haven't assigned drive X: to your Borland C++ directory (the one
containing the BIN, INCLUDE and LIB subdirectories), you will also need to edit
the directory settings -- FOR EACH PROJECT -- before you can compile the code
from the IDE. Select "Options" -> "Directories..." from the menu and enter the
correct paths to your Include and Library directories.


Recreating the original executables
===================================

This source code is set up for recreating the registered version 1.1 game and
setup executables by default. There are no project files or makefiles for other
versions, you will need to edit VERSION.H to switch to a different version.

The supported versions are

- Registered Version 1.0
- Registered Version 1.1
- Shareware Version 1.1

As mentioned above, you need to compile the audio library with Borland C++ 2.0
and compile the main game and setup executables with Borland C++ 3.1 to get a
100% identical copy of the original executables.

For reference, these are the results you should be getting:

- Registered Version 1.0
  Game executable  : size = 181872 bytes, CRC32 = 636B43B5
  Setup executable : size =  18176 bytes, CRC32 = 52F844CB

- Registered Version 1.1
  Game executable  : size = 182656 bytes, CRC32 = BA26F8FA
  Setup executable : size =  18176 bytes, CRC32 = 4277B609

- Shareware Version 1.1
  Game executable  : size = 179360 bytes, CRC32 = 4FF269B3
  Setup executable : size =  18176 bytes, CRC32 = 4277B609

The game executables for use with Gravis UltraSound (__HPGRVS.EXE) are NOT
covered by this source code. They require a completely different revision of
the audio library that removes support for all the sound cards in the current
version and adds support for the UltraSound. Maybe I'll add support for that in
an update at some point in the future, but for now, this is all there is.


About the reconstructed Apogee Sound System code
================================================

Except for the OLDTIMBR.C file, all the source code files in the ASS directory
were written from scratch, based on the disassembled code of HOCUS.EXE.

The names of the routines and the global variables in that code match the
original code, but there are almost no comments in the source files and no
named constants to make the code easier to read and understand. The indentation
scheme is also completely different from what was used in the original source.

My focus was on getting an accurate reconstruction of the resulting machine
code, not on reconstructing what the original source code *might* have looked
like. If you want to understand what's going on, refer to the original code.


DOSBox 0.74-3 issues and workarounds
====================================

When using the latest "official" DOSBox build version 0.74-3 for Windows, you
will face certain quirks when working with Borland C++.

- Using the "Build all" option from the "Compile" menu will not work as
  intended. It will have the same effect as "Make EXE file", meaning only the
  files that have been changed will be re-compiled.

- TASM might be unable to overwrite outdated .OBJ files. If you get an "Error
  writing object file" message, delete the respective .OBJ file manually
  outside DOSBox and compile again. The preceding "Transfer to program: Turbo
  Assembler" message should tell you which file caused this error.

  Note: If you are using MAKE, the old .OBJ file will be deleted automatically
  when TASM reports an error, so you can usually run the exact same command a
  second time and it should work.

- While I was working on my reconstruction of the BioMenace source code, I had
  a few problems with the Borland C++ 2.0 IDE outright crashing DOSBox
  sometimes. This is why there is no project file for the Apogee Sound System,
  only a MAKEFILE.

Other "forks" of DOSBox might have already fixed these problems. DOSBox Staging
v0.81.0 doesn't appear to have these problems, so you might want to use that
instead of the "official" DOSBox version.


Bonus Material!
===============

I have included an archive called "hpsprites.zip". I put this archive together
shortly after HocusEditor v1.1 was released and sent it to Hisymak, the author
of HocusEditor, to explain why the sprite format works the way it does and
maybe help him implement a sprite editor. HocusEditor v1.2 was released less
than two months after this, with a sprite editor.

There are lots of spelling errors in the readme.txt file and some bugs in the
pascal code I suggested, but I decided to leave it all unchanged.

If you have never worked with planar VGA graphics, the readme.txt file will
give you a quick introduction that might help you understand what the Hocus
Pocus source code is doing. The archive also contains a dump of the entire VGA
memory (screen.png), something you never get to see while playing the game.

Oh yeah, and there is also a program that converts the game's sprites into PNG
images and another ZIP archive full of sprite images.


Special Thanks
==============

Thanks must go to Apogee for releasing the source code for the Apogee Sound
System to the public. This made it a lot easier to reconstruct the source code
for the revision that was used in Hocus Pocus.

[END OF FILE]