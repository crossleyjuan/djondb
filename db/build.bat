mkdir build
cd build
cmake .. 
pause

set CONFIGURATION=Release
rem set CONFIGURATION=Release
rem C:\WINDOWS\Microsoft.NET\Framework\v4.0.30319\MSBuild djondb.sln /t:Rebuild /p:Configuration=Debug 
rem C:\WINDOWS\Microsoft.NET\Framework\v4.0.30319\MSBuild djondb.sln /p:Configuration=Debug 
C:\WINDOWS\Microsoft.NET\Framework\v4.0.30319\MSBuild djondb.sln /p:Configuration=%CONFIGURATION% 

copy /Y driverbase\%CONFIGURATION%\djon-client.*  shell\%CONFIGURATION%
copy /Y ..\third_party\libs\Win32\*.dll  shell\%CONFIGURATION%
copy /Y ..\third_party\libs\Win32\*.dll  djondb_win\%CONFIGURATION%

cd ..
