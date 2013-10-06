@echo off
setlocal enabledelayedexpansion

if [%1] ==[] goto usage

call ..\setenv.bat
call :parseArguments %*

set PATH="%PATH%"

if "%x32%" == "true" (
	echo starting win32
	SET PLATFORM=Win32
)

if "%x64%" == "true" (
	echo starting x64
	SET PLATFORM=x64
)

TarTool pthreads.tar.gz
cd pthreads

IF "%PLATFORM%" == "x64" (
    copy dll\x64\pthreadVC2.dll ..\libs
    copy lib\x64\pthreadVC2.lib ..\libs
)
IF "%PLATFORM%" == "Win32" (
    copy dll\x86\pthreadVC2.dll ..\libs
    copy lib\x86\pthreadVC2.lib ..\libs
)

copy /y include\*.h ..\includes\

GOTO END

:usage

Echo Usage: %0 [-x32] [-x64] [-python PYTHON_DIR] [-svnpath SVN_PATH]
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

:findfile
@rem this will look for a file. call :findfile DIR file result

for /R %~1 %%f in (%~2) do (
	set "%~3=%%f"
)

goto:eof

:checkrequired 
set pathfound=

set list=%PATH%
set list=%list:"=%

:nextreq
FOR /f "tokens=1* delims=;" %%a IN ("%list%") DO (
  if exist %%a\%~1 (
     set pathfound=%%a\%~1
  )
  if not "%pathfound%" == "" (
    goto exitloopreq
  )
  set list=%%b
  goto nextreq
)
:exitloopreq
set "%~2=%pathfound%"

goto:eof

rem ----------------------------------------------------------------------------------

:Exit
