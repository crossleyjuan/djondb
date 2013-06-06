
@rem Capture startup path
@SET PATH_SRC_STARTUP=%~dp0

@rem Set source path based on current path
@pushd "%CD%"
@CD /D "%PATH_SRC_STARTUP%..\..\.."

@rem Sample: D:\dv\Src\BizAgiSuite_9.1\Development\Team\Source
@SET PATH_SRC_ALL_BIZAGI="%CD%"
@popd

@rem Applications Paths
@SET PATH_MSBUILD=C:\WINDOWS\Microsoft.NET\Framework\v4.0.30319
@SET PATH_MSBUILD_x64=C:\WINDOWS\Microsoft.NET\Framework64\v4.0.30319

@echo source dir: %PATH_SRC_STARTUP%

@set INNO_PATH=c:\Program Files (x86)\Inno Setup 5
@set ANT_PATH=C:\development\Java\apache-ant-1.8.4
@SET OUTPUTDIR=%PATH_SRC_STARTUP%\windows\output


