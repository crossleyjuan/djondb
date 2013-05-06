@echo off

if [%1] ==[] goto usage

set x32=false
if [%1] == [-x32] (
   set x32=true
)
if [%2] == [-x32] (
   set x32=true
)
set x64=false
if [%1] == [-x64] (
   set x64=true
)
if [%2] == [-x64] (
   set x64=true
)

call setenv.bat

cd %PATH_SRC_STARTUP%\driverbase\drivers
call update.bat

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
cd build%PLATFORM%
rem cmake .. -G "Visual Studio 10 Win64"  -DCMAKE_BUILD_TYPE=Debug
cmake .. -G "Visual Studio 10 Win64"  -DCMAKE_BUILD_TYPE=Release
rem %PATH_MSBUILD%\msbuild djondb.sln /t:Rebuild /p:Configuration=Release /p:Platform=%PLATFORM%
%PATH_MSBUILD%\msbuild djondb.sln /p:Configuration=Release /p:Platform=%PLATFORM%
rem %PATH_MSBUILD%\msbuild djondb.sln /p:Configuration=Debug /p:Platform=%PLATFORM%

cpack
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

REM Everything done, now it's time to create the output
REM

GOTO END

:usage

Echo Usage: release-windows [-x32] [-x64]
goto exit

:END

Echo Done!

:Exit
