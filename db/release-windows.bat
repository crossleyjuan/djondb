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
%PATH_MSBUILD%\msbuild %PATH_SRC_STARTUP%\windows\djondb_vs2008.sln /t:Rebuild /p:Configuration=Release /p:Platform=%PLATFORM%

SET RELEASE_DIR=%PATH_SRC_STARTUP%\windows\Release

xcopy %PATH_SRC_STARTUP%\windows\djonjavadriver.dll "%PATH_SRC_STARTUP%\driverbase\drivers\java\native\vs2008\"
cd %PATH_SRC_STARTUP%\driverbase\drivers\java\java
call "%ANT_PATH%\bin\ant"
cd %PATH_SRC_STARTUP%

mkdir "%RELEASE_DIR%"
xcopy /y "%PATH_SRC_STARTUP%\Windows\libs\%PLATFORM%\*.*" "%RELEASE_DIR%\"
if x32 == true (
    rem the Csharp driver is been copied on other folder
    xcopy /y "%PATH_SRC_STARTUP%\Windows\libs\x86\*.*" "%RELEASE_DIR%\"
)
xcopy /y "%PATH_SRC_STARTUP%\third_party\libs\%PLATFORM%\pthreadVC2.dll" "%RELEASE_DIR%\"

"%INNO_PATH%\compil32" /cc "%PATH_SRC_STARTUP%\windows\installer\inno script.iss"
"%INNO_PATH%\compil32" /cc "%PATH_SRC_STARTUP%\windows\installer\csharp driver.iss"

REM Everything done, now it's time to create the output
REM

echo Creating output dir
del /Q   "%OUTPUTDIR%\*.*"
mkdir  "%OUTPUTDIR%"
copy   "%PATH_SRC_STARTUP%\windows\installer\output\*" "%OUTPUTDIR%"
copy   "%PATH_SRC_STARTUP%\driverbase\drivers\java\java\dist\lib\djondb_java.jar" "%OUTPUTDIR%\djondb_client_Windows_i686.jar"

echo Changing names to target the platform
move "%PATH_SRC_STARTUP%\windows\output\djondb_client_Windows_i686.jar" "%PATH_SRC_STARTUP%\windows\output\djondb_client_Windows_%PLATFORM%.jar" 
move "%PATH_SRC_STARTUP%\windows\output\setup_djoncsharp.exe" "%PATH_SRC_STARTUP%\windows\output\setup_djoncsharp_%PLATFORM%.exe" 
move "%PATH_SRC_STARTUP%\windows\output\setup_djondb.exe" "%PATH_SRC_STARTUP%\windows\output\setup_djondb_%PLATFORM%.exe" 

GOTO %NEXT_STEP%

:usage

Echo Usage: release-windows [-x32] [-x64]
goto exit

:END

Echo Done!

:Exit
