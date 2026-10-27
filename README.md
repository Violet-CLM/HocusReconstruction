# Hocus Pocus Reconstruction

## Purpose

This repository is a place to make changes (fixes/additions) to Hocus Pocus (1994) but with source control. Most likely, the repository owner (Violet CLM) will make changes in a single branch for a few days, and then get bored and move on. But maybe there'll be pull requests and forks and all that community stuff! There's no way to know.

## Installation

On your computer, this repository must be in the same folder as a copy of the Hocus Pocus game, preferably the Registered 1.1. That is to say, `Hocus.dat` and `Reconstruct.bat` must be in the same folder, and `RECONSTR` must be a subfolder of that same folder.

## Compilation

Create a shortcut that runs `Reconstruct.bat` through your local copy of DOSBox, then run that shortcut. You can use the same shortcut for building and playing the game: Reconstruct.bat builds any files that have been changed since you last built the game, and then runs the game.

For example, your shortcut might be:
**Target:** `"C:\DOSBOX\DOSBox.exe" "C:\Games\DOS\HocusRec\Recons~1.bat"`
**Start in:** `C:\Games\DOS\HocusRec\`

If you want to run the game with any command line options, either change the last line in Reconstruct.bat (e.g. `HOCUS -MENU` instead of just `HOCUS`), or else make yourself a separate shortcut that opens Hocus.exe directly.

## Context/Credits

This is [K1n9_Duk3](http://k1n9duk3.shikadi.net/)'s reconstruction of Hocus Pocus's source code as described/announced in [this PCKF thread](https://pckf.com/viewtopic.php?t=18675), but uploaded to github, and with the Borland C++ 3.1 compiler included for convenience.

K1n9_Duk3 recommends using Borland 2.0 for the Apogee Sound System code, and 3.1 for the game code, in order to compile the game as a byte-identical copy of the original releases. This repository intends to *make changes to* the original releases, so being able to produce a byte-identical copy is not considered important. The two `MOUNTDIR.BAT` files are edited to point to this Borland folder, and use relative paths, so that you may install this repository into any folder you wish.

Almost all code here, except as visible in the commit history, was the work of K1n9_Duk3. It continues to be released under the terms of the GNU GPL.