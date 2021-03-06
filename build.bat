@echo off

set PROJECT_NAME=renderhub
set CODE_BASE_PATH=%CD%
set BUILD_PATH=%CODE_BASE_PATH%\output
set BINARIES_PATH=%BUILD_PATH%\binaries
set INTERMEDIATES_PATH=%BUILD_PATH%\intermediates
set SOURCE_PATH=%CODE_BASE_PATH%\source

set INCLUDE_DIRECTORIES=/I %CODE_BASE_PATH%\include\
set PREPROCESSOR_DEFINES=/DUNICODE /D_UNICODE /D_WIN32 /D_DEBUG
set COMPILER_OPTIONS=/Fe%BINARIES_PATH%\%PROJECT_NAME%.exe /Fd%INTERMEDIATES_PATH%\ /Fo%BINARIES_PATH%\ /nologo /MDd %INCLUDE_DIRECTORIES% /ZI /EHsc /W3 /std:c++17 %PREPROCESSOR_DEFINES% /c
set INCLUDED_LIBRARIES=User32.lib Gdi32.lib Shell32.lib msvcrt.lib dxguid.lib D3D11.lib DXGI.lib
set LINKER_OPTIONS=/link /NODEFAULTLIB:libcmt.lib /NODEFAULTLIB:msvcrtd.lib /DEBUG

IF NOT DEFINED VC_COMPILER_INITIALIZED (
	set /A VC_COMPILER_INITIALIZED=1
	call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
	echo.
)

IF NOT EXIST %BUILD_PATH% (
	mkdir %BUILD_PATH%
)

IF NOT EXIST %INTERMEDIATES_PATH% (
	mkdir %INTERMEDIATES_PATH%
)

IF NOT EXIST %BINARIES_PATH% (
	mkdir %BINARIES_PATH%
)

REM cl %COMPILER_OPTIONS% %SOURCE_PATH%\*.cpp %INCLUDED_LIBRARIES% %LINKER_OPTIONS%
cl %COMPILER_OPTIONS% %SOURCE_PATH%\*.cpp
lib %BINARIES_PATH%\*.obj /OUT:%BINARIES_PATH%\renderhub_Debug_x64.lib