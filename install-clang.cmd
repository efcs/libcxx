if "%CLANG_VERSION%"=="ToT" (
    set CLANG_URL="http://llvm.org/pre-releases/win-snapshots/LLVM-5.0.0-r301646-win32.exe"
    appveyor DownloadFile %CLANG_URL% -FileName clang-installer.exe
    clang-installer.exe
)
