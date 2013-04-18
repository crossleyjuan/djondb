mkdir build
cd build
cmake .. 
pause
rem C:\WINDOWS\Microsoft.NET\Framework\v4.0.30319\MSBuild djondb.sln /t:Rebuild /p:Configuration=Debug 
C:\WINDOWS\Microsoft.NET\Framework\v4.0.30319\MSBuild djondb.sln /p:Configuration=Debug 

copy /Y driverbase\Debug\djon-client.*  shell\Debug

cd ..
