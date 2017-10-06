@echo off

:: tune setting below: vs2017, vs2015, vs2013
set VSVERSION=vs2017
IF EXIST vs2013.txt set VSVERSION=vs2013
IF EXIST vs2015.txt set VSVERSION=vs2015

:: without unified build (agglomerated build)
%~dp0\premake5.exe %VSVERSION% %*
