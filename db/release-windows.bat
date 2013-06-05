@echo off
setlocal enabledelayedexpansion

if [%1] ==[] goto usage

call setenv.bat
call:parseArguments %*

cd %PATH_SRC_STARTUP%\driverbase\drivers
call update.bat

echo Creating output dir
del /Q   "%OUTPUTDIR%\*.*"
mkdir  "%OUTPUTDIR%"
cd %PATH_SRC_STARTUP%

:Win32
if %x32% == true (
	echo starting win32
	SET PLATFORM=Win32
	SET NEXT_STEP=x64
	GOTO ExecuteJob
)

:x64
if %x64% == true (
	echo starting x64
	SET PLATFORM=x64
	SET NEXT_STEP=END
	GOTO ExecuteJob
)

GOTO END

:ExecuteJob
mkdir build%PLATFORM%
rem pause
cd build%PLATFORM%
rem pause
rem cmake .. -G "Visual Studio 10 Win64"  -DCMAKE_BUILD_TYPE=Debug
if %PLATFORM% == x64 (
	cmake .. -G "Visual Studio 10 Win64" -DPLATFORM_NAME=%PLATFORM% -DCMAKE_BUILD_TYPE=Release
)
if %PLATFORM% == Win32 (
	cmake .. -G "Visual Studio 10" -DPLATFORM_NAME=%PLATFORM% -DCMAKE_BUILD_TYPE=Release
)
rem pause
rem %PATH_MSBUILD%\msbuild djondb.sln /t:Rebuild /p:Configuration=Release /p:Platform=%PLATFORM%
%PATH_MSBUILD%\msbuild djondb.sln /p:Configuration=Release /p:Platform=%PLATFORM%
rem pause
rem %PATH_MSBUILD%\msbuild djondb.sln /p:Configuration=Debug /p:Platform=%PLATFORM%

cpack
rem pause

cd ..
REM Everything done, now it's time to create the output
REM

copy   "%PATH_SRC_STARTUP%\build%PLATFORM%\djondb-*.exe" "%OUTPUTDIR%"
rem copy   "%PATH_SRC_STARTUP%\driverbase\drivers\java\java\dist\lib\djondb_java.jar" "%OUTPUTDIR%\djondb_client_Windows_i686.jar"
pause

GOTO %NEXT_STEP%

xcopy /y %PATH_SRC_STARTUP%\build%PLATFORM%\driverbase\drivers\java\native\Release\djonjavadriver.dll "%PATH_SRC_STARTUP%\driverbase\drivers\java\native\vs2008\"
cd %PATH_SRC_STARTUP%\driverbase\drivers\java\java
call "%ANT_PATH%\bin\ant"
cd %PATH_SRC_STARTUP%

echo Creating output dir
del /Q   "%OUTPUTDIR%\*.*"
mkdir  "%OUTPUTDIR%"
copy   "%PATH_SRC_STARTUP%\build%PLATFORM%\djondb-*.exe" "%OUTPUTDIR%"
copy   "%PATH_SRC_STARTUP%\driverbase\drivers\java\java\dist\lib\djondb_java.jar" "%OUTPUTDIR%\djondb_client_Windows_i686.jar"

echo Changing names to target the platform
move "%PATH_SRC_STARTUP%\windows\output\djondb_client_Windows_i686.jar" "%PATH_SRC_STARTUP%\windows\output\djondb_client_Windows_%PLATFORM%.jar" 
rem move "%PATH_SRC_STARTUP%\windows\output\setup_djoncsharp.exe" "%PATH_SRC_STARTUP%\windows\output\setup_djoncsharp_%PLATFORM%.exe" 
rem move "%PATH_SRC_STARTUP%\windows\output\setup_djondb.exe" "%PATH_SRC_STARTUP%\windows\output\setup_djondb_%PLATFORM%.exe" 


GOTO END

:usage

Echo Usage: release-windows [-x32] [-x64]
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
