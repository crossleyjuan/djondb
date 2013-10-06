@echo off
setlocal enabledelayedexpansion

if [%1] ==[] goto usage

call ..\setenv.bat
call :parseArguments %*

set PATH="%PATH%;%python%;%svnpath%"

call :checkrequired python.exe pythonpath

if "%pythonpath%" == "" (
   echo python version 2.7 is not in your PATH, if it's installed please add the path using the parameter -python [PYTHON PATH]
   GOTO Exit
)
call :checkrequired svn.exe svnexepath
if "%svnpath%" == "" (
   echo svn.exe is not in your PATH, if it's installed please add the path using the parameter -svnpath [SUBVERSION PATH]
   echo You can get the command line tool from: http://www.sliksvn.com/en/download
   GOTO Exit
)

call buildantlr.bat %*
call buildgtest.bat %*
call buildpthread.bat %*
call buildv8.bat %*

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
