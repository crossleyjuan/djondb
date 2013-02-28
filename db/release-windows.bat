@echo off
call setenv.bat

cd %PATH_SRC_STARTUP%\driverbase\drivers
call update.bat

cd %PATH_SRC_STARTUP%
%PATH_MSBUILD%\msbuild %PATH_SRC_STARTUP%\windows\djondb_vs2008.sln /t:Rebuild /p:Configuration=Release /p:Platform=Win32

cd %PATH_SRC_STARTUP%\driverbase\drivers\java\java
call "%ANT_PATH%\bin\ant"
cd %PATH_SRC_STARTUP%

"%INNO_PATH%\compil32" /cc "%PATH_SRC_STARTUP%\windows\installer\inno script.iss"
"%INNO_PATH%\compil32" /cc "%PATH_SRC_STARTUP%\windows\installer\csharp driver.iss"

REM Everything done, now it's time to create the output
REM

echo Creating output dir
del /Q   "%OUTPUTDIR%\*.*"
mkdir  "%OUTPUTDIR%"
copy   "%PATH_SRC_STARTUP%\windows\installer\output\*" "%OUTPUTDIR%"
copy   "%PATH_SRC_STARTUP%\driverbase\drivers\java\java\dist\lib\djondb_java.jar" "%OUTPUTDIR%\djondb_client_Windows_i686.jar"
