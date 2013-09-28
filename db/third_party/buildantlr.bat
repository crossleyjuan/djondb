@echo off
setlocal enabledelayedexpansion

if [%1] ==[] goto usage

call ..\setenv.bat
call:parseArguments %*

if "%x32%" == "true" (
	echo starting win32
	SET PLATFORM=Win32
)

if "%x64%" == "true" (
	echo starting x64
	SET PLATFORM=x64
)

IF NOT EXIST libs mkdir libs
IF NOT EXIST includes mkdir includes

@rem tartool libantlr3c-3.4.tar.gz
cd libantlr3c-3.4

%PATH_MSBUILD%\msbuild C.sln /p:Configuration=Release /p:Platform=%PLATFORM%

xcopy /Y Release\antlr3c.lib ..\libs
xcopy /Y antlr3config.h ..\includes\
xcopy /Y include\*.h ..\includes\
GOTO END

:usage

Echo Usage: %0 [-x32] [-x64]
goto exit

:END

Echo Done!

goto Exit

:getArg
set valname=%~1
echo arg: !%valname%!
goto:eof

:parseArguments
rem ----------------------------------------------------------------------------------
@echo off

:loop
IF "%~1"=="" GOTO cont

set argname=%~1
set argname=%argname:~1,100%
set value=%~2

@rem if the next value starts with - then it's a new parameter
if "%value:~0,1%" == "-" (
   set !argname!=true
   SHIFT & GOTO loop
)

if "%value%" == "" (
   set !argname!=true
   SHIFT & GOTO loop
)

set !argname!=%~2

@rem jumps first and second parameter
SHIFT & SHIFT & GOTO loop

:cont

goto:eof
rem ----------------------------------------------------------------------------------

:Exit
